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

QString Psw7Instrument::displayName() const
{
    return "GW Instek PSW7-800";
}

QString Psw7Instrument::identify() const
{
    return "GW-INSTEK,PSW800-4.32,TW123456,01.00.20110101";
}

QString Psw7Instrument::popError()
{
    if (m_errorQueue.isEmpty()) {
        return "0, No error";
    }
    return m_errorQueue.dequeue();
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
    else if (command == "SYSTem:ERRor?") {
        return popError();
    }
    else if (command == "MEASure:VOLTage:DC?") {
        return QString::number(m_powerSupply.get_voltage());
    }
    else if (parts.size() == 2 && parts[0] == "SOURce:VOLTage:LIMit") {
        bool ok;
        float value = parts[1].toFloat(&ok);
        if (ok) {
            m_powerSupply.set_voltage_limit(value);
        } else {
            // Ошибка: неверный тип данных
            m_errorQueue.enqueue("-104, Data type error");
        }
        return "";
    }
    else if (command == "SOURce:VOLTage:LIMit?") {
        return QString::number(m_powerSupply.get_voltage_limit(), 'f', 3);
    }

    // Если команда не распознана (дошли до сюда)
    if (!command.isEmpty() && command != "SOURce:VOLTage:LIMit") {
        m_errorQueue.enqueue("-113, Undefined header");
    }

    return "";
}