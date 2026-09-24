#include "ScpiParser.h"
#include <QRandomGenerator>
#include "PowerSupply.h"
#include <QStringList>
#include <QDebug>

ScpiParser::ScpiParser(PowerSupply& ps) :
    power_supply(ps)
{};

QString ScpiParser::parse(const QString& command) {
    QString clean_string = command;
    clean_string.remove("\n");
    clean_string.remove("\r");
    return recognize(clean_string);
}

QString ScpiParser::recognize(const QString& command) {
    QStringList parts = command.split(' ');
    if (command == "*IDN?") {
        return "GW-INSTEK,PSW800-4.32,TW123456,01.00.20110101";
    }
    else if (command == "*OPC?") {
        return "1";
    }
    else if (command == "MEASure:VOLTage:DC?") {
        return QString::number(power_supply.get_voltage());
    }
    else if (parts[0] == "SOURce:VOLTage:LIMit") {
        if (parts.size() == 2) {
            bool ok;
            float value = parts[1].toFloat(&ok);

            if (ok) {
                power_supply.set_voltage_limit(value);
            } else {
                return "";
            }
        } else {
            return "";
        }
    }
    else if (command == "SOURce:VOLTage:LIMit?") {
        return QString::number(power_supply.get_voltage_limit(), 'f', 3);
    }
    return "";
}