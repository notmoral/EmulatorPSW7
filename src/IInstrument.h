/**
 * @file IInstrument.h
 * @brief Абстрактный интерфейс SCPI-прибора.
 */

#ifndef IINSTRUMENT_H
#define IINSTRUMENT_H

#include <QString>

/**
 * @brief Интерфейс прибора, поддерживающего SCPI.
 *
 * Каждый конкретный прибор (PSW7, E36312A, ...) реализует этот
 * интерфейс и сам решает, как реагировать на ту или иную команду.
 * ScpiParser хранит набор таких приборов и делегирует вызовы.
 */
class IInstrument {
public:
    virtual ~IInstrument() = default;

    /// Короткое имя для сопоставления (например, "PSW7").
    virtual QString name() const = 0;

    /// Ответ на команду *IDN?.
    virtual QString identify() const = 0;

    /**
     * @brief Обрабатывает одну SCPI-команду и возвращает ответ.
     * @param command Текст команды без префикса [DEVICE=...].
     * @return Текстовый ответ. Пустая строка, если команда не распознана
     *         или не предполагает ответа.
     */
    virtual QString handleCommand(const QString& command) = 0;
};

#endif // IINSTRUMENT_H
