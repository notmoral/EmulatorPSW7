/**
 * @file Psw7Instrument.cpp
 * @brief Реализация SCPI-команд для GW Instek PSW7-800.
 */

#include "Psw7Instrument.h"
#include <QStringList>

Psw7Instrument::Psw7Instrument()
{
}

QString Psw7Instrument::name() const
{
    return "PSW7";
}

QString Psw7Instrument::identify() const
{
    return "GW-INSTEK,PSW800-4.32,TW123456,01.00.20110101";
}

QString Psw7Instrument::handleCommand(const QString& command)
{
    QStringList parts = command.split(' ');

    if (command == "*IDN?") {
        return identify();
    }
    else if (command == "*OPC?") {
        return "1";
    }
    else if (command == "MEASure:VOLTage:DC?") {
        return QString::number(m_powerSupply.get_voltage());
    }
    else if (parts[0] == "SOURce:VOLTage:LIMit") {
        if (parts.size() == 2) {
            bool ok;
            float value = parts[1].toFloat(&ok);
            if (ok) {
                m_powerSupply.set_voltage_limit(value);
            } else {
                return "";
            }
        } else {
            return "";
        }
    }
    else if (command == "SOURce:VOLTage:LIMit?") {
        return QString::number(m_powerSupply.get_voltage_limit(), 'f', 3);
    }

    return "";
}
