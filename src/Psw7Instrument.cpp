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

    if (command == "*IDN?") return identify();
    if (command == "*OPC?") return "1";
    if (command == "SYSTem:ERRor?") return popError();

    // --- Защита от перенапряжения (OVP) ---
    if (parts.size() == 2 && parts[0] == "VOLTage:PROTection:STATe") {
        const QString arg = parts[1].toUpper();
        if (arg == "ON" || arg == "1") m_ovpEnabled = true;
        else if (arg == "OFF" || arg == "0") m_ovpEnabled = false;
        else m_errorQueue.enqueue("-224, Illegal parameter value");
        return "";
    }
    if (command == "VOLTage:PROTection:STATe?") {
        return m_ovpEnabled ? "1" : "0";
    }
    if (command == "VOLTage:PROTection:TRIPped?") {
        return m_ovpTripped ? "1" : "0";
    }

    // --- Защита от перегрузки по току (OCP) ---
    if (parts.size() == 2 && parts[0] == "CURRent:PROTection:STATe") {
        const QString arg = parts[1].toUpper();
        if (arg == "ON" || arg == "1") m_ocpEnabled = true;
        else if (arg == "OFF" || arg == "0") m_ocpEnabled = false;
        else m_errorQueue.enqueue("-224, Illegal parameter value");
        return "";
    }
    if (command == "CURRent:PROTection:STATe?") {
        return m_ocpEnabled ? "1" : "0";
    }
    if (command == "CURRent:PROTection:TRIPped?") {
        return m_ocpTripped ? "1" : "0";
    }

    // --- Измерения ---
    if (command == "MEASure:VOLTage:DC?") {
        // Проверка OCP перед выдачей показаний
        if (m_ocpEnabled && m_powerSupply.is_overcurrent()) {
            m_ocpTripped = true;
            m_powerSupply.set_voltage_limit(0.0f); // Отключаем выход
            m_errorQueue.enqueue("-222, Data out of range (OCP triggered)");
            return "0.000";
        }
        return QString::number(m_powerSupply.get_voltage());
    }
    else if (command == "MEASure:CURRent:DC?") {
        return QString::number(m_powerSupply.get_current());
    }
    
    // --- Лимит напряжения ---
    else if (parts.size() == 2 && parts[0] == "SOURce:VOLTage:LIMit") {
        bool ok;
        float value = parts[1].toFloat(&ok);
        if (ok) {
            // Проверка срабатывания OVP (максимум 12 В для PSW7)
            if (m_ovpEnabled && value > 12.0f) {
                m_ovpTripped = true;
                m_powerSupply.set_voltage_limit(0.0f); // Сбрасываем напряжение
                m_errorQueue.enqueue("-222, Data out of range (OVP triggered)");
            } else {
                m_ovpTripped = false; // Сбрасываем флаг, если значение в норме
                m_powerSupply.set_voltage_limit(value);
            }
        } else {
            m_errorQueue.enqueue("-104, Data type error");
        }
        return "";
    }
    else if (command == "SOURce:VOLTage:LIMit?") {
        return QString::number(m_powerSupply.get_voltage_limit(), 'f', 3);
    }

    // --- Лимит тока ---
    else if (parts.size() == 2 && parts[0] == "SOURce:CURRent:LIMit") {
        bool ok;
        float value = parts[1].toFloat(&ok);
        if (ok) {
            m_powerSupply.set_current_limit(value);
        } else {
            m_errorQueue.enqueue("-104, Data type error");
        }
        return "";
    }
    else if (command == "SOURce:CURRent:LIMit?") {
        return QString::number(m_powerSupply.get_current_limit(), 'f', 3);
    }

    // --- Сопротивление нагрузки (кастомная команда для тестов) ---
    else if (parts.size() == 2 && parts[0] == "SYSTem:LOAD") {
        bool ok;
        float value = parts[1].toFloat(&ok);
        if (ok) {
            m_powerSupply.set_load_resistance(value);
        } else {
            m_errorQueue.enqueue("-104, Data type error");
        }
        return "";
    }
    else if (command == "SYSTem:LOAD?") {
        return QString::number(m_powerSupply.get_load_resistance(), 'f', 3);
    }

    // Если команда не распознана
    m_errorQueue.enqueue("-113, Undefined header");
    return "";
}