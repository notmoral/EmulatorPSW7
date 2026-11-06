/**
 * @file E36312AInstrument.cpp
 * @brief Реализация SCPI-команд для Keysight E36312A.
 */

#include "E36312AInstrument.h"
#include <QStringList>
#include <QRegularExpression>
#include <QRandomGenerator>
#include <cmath>

E36312AInstrument::E36312AInstrument()
{
    m_channels[0].voltageLimit = 6.0f;
    m_channels[0].currentLimit = 5.0f;
    m_channels[1].voltageLimit = 25.0f;
    m_channels[1].currentLimit = 1.0f;
    m_channels[2].voltageLimit = 25.0f;
    m_channels[2].currentLimit = 1.0f;
}

QString E36312AInstrument::name() const
{
    return "E36312A";
}

QString E36312AInstrument::displayName() const
{
    return "Keysight E36312A";
}

QString E36312AInstrument::identify() const
{
    return "Keysight Technologies,E36312A,MY12345678,1.0.0";
}

float E36312AInstrument::randomValue(float min, float max)
{
    const double r = QRandomGenerator::global()->generateDouble();
    const float value = min + static_cast<float>(r) * (max - min);
    return std::round(value * 1000.0f) / 1000.0f;
}

bool E36312AInstrument::validChannel(int index)
{
    return index >= 0 && index < 3;
}

QString E36312AInstrument::popError()
{
    if (m_errorQueue.isEmpty()) {
        return "0, No error";
    }
    return m_errorQueue.dequeue();
}

QString E36312AInstrument::handleCommand(const QString& command)
{
    const QString clean = command.trimmed();

    if (clean == "*IDN?") return identify();
    if (clean == "*OPC?")  return "1";
    if (clean == "SYSTem:ERRor?") return popError(); // <-- ДОБАВЛЕНО

    if (clean == "INSTrument:NSELect?")
        return QString::number(m_selectedChannel + 1);

    if (clean.startsWith("INSTrument:NSELect ")) {
        bool ok;
        const int ch = clean.mid(QString("INSTrument:NSELect ").size()).trimmed().toInt(&ok);
        if (ok && validChannel(ch - 1)) {
            m_selectedChannel = ch - 1;
        } else {
            m_errorQueue.enqueue("-222, Data out of range"); // <-- ОШИБКА
        }
        return "";
    }

    static const QRegularExpression re(
        R"(^(OUTPut|SOURce|MEASure)(\d)?(:[^\s]+)(?:\s+(.*))?$)"
    );

    const QRegularExpressionMatch match = re.match(clean);
    if (!match.hasMatch()) {
        m_errorQueue.enqueue("-113, Undefined header"); // <-- ОШИБКА
        return "";
    }

    const QString head    = match.captured(1);
    const QString chStr   = match.captured(2);
    const QString tail    = match.captured(3);
    const QString arg     = match.captured(4).trimmed();

    int channel = m_selectedChannel;
    if (!chStr.isEmpty()) {
        const int ch = chStr.toInt();
        if (validChannel(ch - 1)) channel = ch - 1;
        else {
            m_errorQueue.enqueue("-222, Data out of range"); // <-- ОШИБКА
            return "";
        }
    }

    // --- OUTPut:STATe ---
    if (head == "OUTPut") {
        if (tail == ":STATe?") {
            return m_channels[channel].outputOn ? "1" : "0";
        }
        if (tail == ":STATe") {
            const QString a = arg.toUpper();
            if (a == "ON"  || a == "1") m_channels[channel].outputOn = true;
            else if (a == "OFF" || a == "0") m_channels[channel].outputOn = false;
            else m_errorQueue.enqueue("-224, Illegal parameter value"); // <-- ОШИБКА
            return "";
        }
    }

    // --- SOURce:VOLTage|CURRent:LIMit ---
    if (head == "SOURce") {
        if (tail == ":VOLTage:LIMit?") {
            return QString::number(m_channels[channel].voltageLimit, 'f', 3);
        }
        if (tail == ":VOLTage:LIMit") {
            bool ok;
            const float v = arg.toFloat(&ok);
            if (ok) m_channels[channel].voltageLimit = (v < 0) ? 0.0f : v;
            else m_errorQueue.enqueue("-104, Data type error"); // <-- ОШИБКА
            return "";
        }
        if (tail == ":CURRent:LIMit?") {
            return QString::number(m_channels[channel].currentLimit, 'f', 3);
        }
        if (tail == ":CURRent:LIMit") {
            bool ok;
            const float v = arg.toFloat(&ok);
            if (ok) m_channels[channel].currentLimit = (v < 0) ? 0.0f : v;
            else m_errorQueue.enqueue("-104, Data type error"); // <-- ОШИБКА
            return "";
        }
    }

    // --- MEASure:VOLTage|CURRent:DC? ---
    if (head == "MEASure") {
        if (tail == ":VOLTage:DC?") {
            if (!m_channels[channel].outputOn) return QString::number(0.0f, 'f', 3);
            return QString::number(m_channels[channel].voltageLimit - randomValue(0.0f, 0.1f), 'f', 3);
        }
        if (tail == ":CURRent:DC?") {
            if (!m_channels[channel].outputOn) return QString::number(0.0f, 'f', 3);
            return QString::number(m_channels[channel].currentLimit - randomValue(0.0f, 0.05f), 'f', 3);
        }
    }

    m_errorQueue.enqueue("-113, Undefined header"); // <-- ОШИБКА (если дошли до сюда)
    return "";
}