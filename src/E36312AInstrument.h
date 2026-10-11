/**
 * @file E36312AInstrument.h
 * @brief SCPI-реализация для Keysight E36312A.
 */

#ifndef E36312AINSTRUMENT_H
#define E36312AINSTRUMENT_H

#include "IInstrument.h"

/**
 * @brief Прибор Keysight E36312A (трёхканальный источник питания).
 *
 * Реализует упрощённый набор SCPI-команд:
 * *IDN?, *OPC?, MEASure:VOLTage:DC?, MEASure:CURRent:DC?,
 * SOURce:VOLTage:LIMit [value]?, OUTPut:STATe [ON|OFF]?.
 *
 * Каналы пока не разделяются — все команды относятся к первому каналу.
 * Это сделано осознанно: цель этого этапа — показать, что архитектура
 * поддерживает несколько разных приборов одновременно.
 */
class E36312AInstrument : public IInstrument {
public:
    E36312AInstrument();

    QString name() const override;
    QString displayName() const override;
    QString identify() const override;
    QString handleCommand(const QString& command) override;

private:
    /// Текущий лимит напряжения, В.
    float m_voltageLimit = 5.000f;

    /// Текущий лимит тока, А.
    float m_currentLimit = 1.000f;

    /// Состояние выхода (true = ON).
    bool m_outputOn = false;

    /// Случайное значение с 3 знаками после запятой в диапазоне [min, max].
    static float randomValue(float min, float max);
};

#endif // E36312AINSTRUMENT_H
