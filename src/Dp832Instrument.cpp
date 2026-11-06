/**
 * @file Dp832Instrument.cpp
 * @brief Реализация SCPI-команд для Rigol DP832.
 */

#include "Dp832Instrument.h"
#include <QStringList>
#include <QRegularExpression>
#include <QRandomGenerator>
#include <cmath>

Dp832Instrument::Dp832Instrument()
{
    // Каналы 1 и 2: 30V / 3A. Канал 3: 5V / 3A.
    m_channels[0].voltageLimit = 30.0f;
    m_channels[1].voltageLimit = 30.0f;
    m_channels[2].voltageLimit = 5.0f;
    m_channels[0].currentLimit = 3.0f;
    m_channels[1].currentLimit = 3.0f;
    m_channels[2].currentLimit = 3.0f;
}

QString Dp832Instrument::name() const
{
    return "DP832";
}

QString Dp832Instrument::displayName() const
{
    return "Rigol DP832";
}

QString Dp832Instrument::identify() const
{
    return "RIGOL TECHNOLOGIES,DP832,DP8A000000000,00.01.15";
}

float Dp832Instrument::randomValue(float min, float max)
{
    const double r = QRandomGenerator::global()->generateDouble();
    const float v = min + static_cast<float>(r) * (max - min);
    return std::round(v * 1000.0f) / 1000.0f;
}

bool Dp832Instrument::validChannel(int index)
{
    return index >= 0 && index < 3;
}

QString Dp832Instrument::popError()
{
    if (m_errorQueue.isEmpty()) {
        return "0, No error";
    }
    return m_errorQueue.dequeue();
}

QString Dp832Instrument::handleCommand(const QString& command)
{
    const QString clean = command.trimmed();

    if (clean == "*IDN?") return identify();
    if (clean == "*OPC?")  return "1";
    if (clean == "SYSTem:ERRor?") return popError();

    // --- Защита от перенапряжения (OVP) ---
    if (clean.startsWith("VOLTage:PROTection:STATe")) {
        const QStringList parts = clean.split(' ', Qt::SkipEmptyParts);
        if (parts.size() == 2) {
            const QString arg = parts[1].toUpper();
            if (arg == "ON" || arg == "1") m_ovpEnabled = true;
            else if (arg == "OFF" || arg == "0") m_ovpEnabled = false;
            else m_errorQueue.enqueue("-224, Illegal parameter value");
            return "";
        }
        return "";
    }
    if (clean == "VOLTage:PROTection:STATe?") {
        return m_ovpEnabled ? "1" : "0";
    }
    if (clean == "VOLTage:PROTection:TRIPped?") {
        return m_ovpTripped ? "1" : "0";
    }

    static const QRegularExpression re(
        R"(^:?(OUTPut|SOURce|MEASure)(\d)?(:[A-Za-z]+)(\??)(?:\s+(.+))?$)"
    );

    const QRegularExpressionMatch match = re.match(clean);
    if (!match.hasMatch()) {
        m_errorQueue.enqueue("-113, Undefined header");
        return "";
    }

    const QString head    = match.captured(1);
    const QString chStr   = match.captured(2);
    const QString tail    = match.captured(3);
    const bool    isQuery = match.captured(4) == "?";
    const QString arg     = match.captured(5).trimmed();

    int channel = m_selectedChannel;
    if (!chStr.isEmpty()) {
        const int ch = chStr.toInt();
        if (validChannel(ch - 1)) channel = ch - 1;
        else {
            m_errorQueue.enqueue("-222, Data out of range");
            return "";
        }
    }

    QString value = arg;
    if (arg.startsWith("CH", Qt::CaseInsensitive) && arg.size() >= 3) {
        const int ch = arg.mid(2).toInt();
        if (validChannel(ch - 1)) {
            channel = ch - 1;
            value.clear();
        } else {
            m_errorQueue.enqueue("-222, Data out of range");
            return "";
        }
    }

    // OUTPut:STATe
    if (head == "OUTPut" && tail == ":STATe") {
        if (isQuery) {
            return m_channels[channel].outputOn ? "ON" : "OFF";
        }
        const QString up = value.toUpper();
        if (up == "ON"  || up == "1") m_channels[channel].outputOn = true;
        else if (up == "OFF" || up == "0") m_channels[channel].outputOn = false;
        else m_errorQueue.enqueue("-224, Illegal parameter value");
        return "";
    }

    // SOURce:VOLTage / CURRent
    if (head == "SOURce") {
        if (tail == ":VOLTage") {
            if (isQuery) return QString::number(m_channels[channel].voltageLimit, 'f', 3);
            bool ok;
            const float v = value.toFloat(&ok);
            if (ok) {
                // Максимальное напряжение: 5В для 3-го канала, 30В для остальных
                float maxVoltage = (channel == 2) ? 5.0f : 30.0f;
                
                if (m_ovpEnabled && v > maxVoltage) {
                    m_ovpTripped = true;
                    m_channels[channel].voltageLimit = 0.0f; // Сбрасываем
                    m_errorQueue.enqueue("-222, Data out of range (OVP triggered)");
                } else {
                    m_ovpTripped = false;
                    m_channels[channel].voltageLimit = (v < 0) ? 0.0f : v;
                }
            } else {
                m_errorQueue.enqueue("-104, Data type error");
            }
            return "";
        }
        if (tail == ":CURRent") {
            if (isQuery) return QString::number(m_channels[channel].currentLimit, 'f', 3);
            bool ok;
            const float v = value.toFloat(&ok);
            if (ok) m_channels[channel].currentLimit = (v < 0) ? 0.0f : v;
            else m_errorQueue.enqueue("-104, Data type error");
            return "";
        }
    }

    // MEASure:VOLTage? / CURRent?
    if (head == "MEASure" && isQuery) {
        if (tail == ":VOLTage") {
            if (!m_channels[channel].outputOn) return QString::number(0.0f, 'f', 3);
            return QString::number(m_channels[channel].voltageLimit - randomValue(0.0f, 0.1f), 'f', 3);
        }
        if (tail == ":CURRent") {
            if (!m_channels[channel].outputOn) return QString::number(0.0f, 'f', 3);
            return QString::number(m_channels[channel].currentLimit - randomValue(0.0f, 0.05f), 'f', 3);
        }
    }

    m_errorQueue.enqueue("-113, Undefined header");
    return "";
}