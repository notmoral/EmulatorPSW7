/**
 * @file ScpiParser.cpp
 * @brief Реализация диспетчера SCPI-команд.
 */

#include "ScpiParser.h"

ScpiParser::ScpiParser()
{
}

void ScpiParser::registerInstrument(IInstrument* instrument)
{
    if (instrument) {
        m_instruments.insert(instrument->name(), instrument);
    }
}

QString ScpiParser::parse(const QString& deviceName, const QString& command)
{
    QString cleanString = command;
    cleanString.remove("\n");
    cleanString.remove("\r");
    cleanString = cleanString.trimmed();

    IInstrument* instrument = m_instruments.value(deviceName, nullptr);
    if (!instrument) {
        if (m_instruments.isEmpty()) return "";
        instrument = m_instruments.first();
    }

    return instrument->handleCommand(cleanString);
}
