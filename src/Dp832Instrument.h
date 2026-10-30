/**
 * @file Dp832Instrument.h
 * @brief SCPI-реализация для Rigol DP832.
 */

#ifndef DP832INSTRUMENT_H
#define DP832INSTRUMENT_H

#include "IInstrument.h"

/**
 * @brief Прибор Rigol DP832 (трёхканальный источник питания).
 *
 * Каналы:
 *  - 1: 30V / 3A
 *  - 2: 30V / 3A
 *  - 3: 5V / 3A
 *
 * SCPI-команды Rigol используют суффикс канала в квадратных скобках,
 * например:
 *  - `:SOURce1:VOLTage 10`
 *  - `:OUTPut2:STATe ON`
 *  - `:MEASure:VOLTage? CH1`
 *
 * Наш парсер принимает как формы с суффиксом (`SOURce1:VOLTage`),
 * так и формы с явным аргументом канала (`MEASure:VOLTage? CH1`).
 */
class Dp832Instrument : public IInstrument {
public:
    Dp832Instrument();

    QString name() const override;
    QString displayName() const override;
    QString identify() const override;
    QString handleCommand(const QString& command) override;

private:
    struct Channel {
        float voltageLimit = 30.0f; ///< Лимит напряжения, В.
        float currentLimit = 3.0f;  ///< Лимит тока, А.
        bool  outputOn     = false; ///< Состояние выхода.
    };

    Channel m_channels[3];
    int     m_selectedChannel = 0;

    static float randomValue(float min, float max);
    static bool validChannel(int index);
};

#endif // DP832INSTRUMENT_H