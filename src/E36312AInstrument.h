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
 * Реализует SCPI-команды с поддержкой трёх независимых каналов.
 *
 * Каналы:
 *  - 1: 6V / 5A
 *  - 2: 25V / 1A
 *  - 3: 25V / 1A
 *
 * Выбор канала:
 *  - явный префикс номера в команде (`OUTPut2:STATe ON`)
 *  - либо через `INSTrument:NSELect <1|2|3>` — устанавливает канал по умолчанию
 *
 * Команды без номера канала (`OUTPut:STATe?`) применяются к текущему
 * выбранному каналу.
 */
class E36312AInstrument : public IInstrument {
public:
    E36312AInstrument();

    QString name() const override;
    QString displayName() const override;
    QString identify() const override;
    QString handleCommand(const QString& command) override;

private:
    /// Состояние одного канала.
    struct Channel {
        float voltageLimit = 5.0f;  ///< Лимит напряжения, В.
        float currentLimit = 1.0f;  ///< Лимит тока, А.
        bool  outputOn     = false; ///< Состояние выхода.
    };

    Channel m_channels[3];      ///< Три канала (индексы 0, 1, 2).
    int     m_selectedChannel = 0; ///< Текущий канал (0..2).

    static float randomValue(float min, float max);

    /// Проверяет, что индекс канала в диапазоне 0..2.
    static bool validChannel(int index);
};

#endif // E36312AINSTRUMENT_H