/**
 * @file E36312AInstrument.cpp
 * @brief Реализация SCPI-команд для Keysight E36312A.
 */

#include "E36312AInstrument.h"
#include <QStringList>
#include <QRandomGenerator>
#include <cmath>

E36312AInstrument::E36312AInstrument()
{
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

QString E36312AInstrument::handleCommand(const QString& command)
{
    const QStringList parts = command.split(' ');

    if (command == "*IDN?") {
        return identify();
    }
    else if (command == "*OPC?") {
        return "1";
    }
    else if (command == "MEASure:VOLTage:DC?") {
        // Напряжение около лимита, если выход включён; иначе ~0.
        if (m_outputOn) {
            return QString::number(m_voltageLimit - randomValue(0.0f, 0.1f), 'f', 3);
        }
        return QString::number(0.0f, 'f', 3);
    }
    else if (command == "MEASure:CURRent:DC?") {
        // Ток около лимита, если выход включён; иначе ~0.
        if (m_outputOn) {
            return QString::number(m_currentLimit - randomValue(0.0f, 0.05f), 'f', 3);
        }
        return QString::number(0.0f, 'f', 3);
    }
    else if (parts.size() == 2 && parts[0] == "SOURce:VOLTage:LIMit") {
        bool ok;
        const float value = parts[1].toFloat(&ok);
        if (ok) {
            m_voltageLimit = (value < 0.0f) ? 0.0f : value;
        }
        return "";
    }
    else if (command == "SOURce:VOLTage:LIMit?") {
        return QString::number(m_voltageLimit, 'f', 3);
    }
    else if (parts.size() == 2 && parts[0] == "SOURce:CURRent:LIMit") {
        bool ok;
        const float value = parts[1].toFloat(&ok);
        if (ok) {
            m_currentLimit = (value < 0.0f) ? 0.0f : value;
        }
        return "";
    }
    else if (command == "SOURce:CURRent:LIMit?") {
        return QString::number(m_currentLimit, 'f', 3);
    }
    else if (parts.size() == 2 && parts[0] == "OUTPut:STATe") {
        const QString arg = parts[1].toUpper();
        if (arg == "ON" || arg == "1") {
            m_outputOn = true;
        } else if (arg == "OFF" || arg == "0") {
            m_outputOn = false;
        }
        return "";
    }
    else if (command == "OUTPut:STATe?") {
        return m_outputOn ? "1" : "0";
    }

    return "";
}
