/**
 * @file Psw7Instrument.h
 * @brief SCPI-реализация для GW Instek PSW7-800.
 */

#ifndef PSW7INSTRUMENT_H
#define PSW7INSTRUMENT_H

#include "IInstrument.h"
#include "PowerSupply.h"

/**
 * @brief Прибор GW Instek PSW7-800.
 *
 * Инкапсулирует логику SCPI-команд, ранее жившую в ScpiParser::recognize.
 * Внутри использует PowerSupply как модель состояния.
 */
class Psw7Instrument : public IInstrument {
public:
    Psw7Instrument();

    QString name() const override;
    QString displayName() const override;
    QString identify() const override;
    QString handleCommand(const QString& command) override;
    QString popError() override;

private:
    PowerSupply m_powerSupply;  ///< Модель напряжения и лимита.
    bool m_ovpEnabled = false; ///< Флаг включения защиты от перенапряжения.
    bool m_ovpTripped = false; ///< Флаг срабатывания защиты.
};

#endif // PSW7INSTRUMENT_H
