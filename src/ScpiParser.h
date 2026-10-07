/**
 * @file ScpiParser.h
 * @brief Разбор SCPI-команд, приходящих от клиента.
 */

#ifndef SCPIPARSER_H
#define SCPIPARSER_H

#include <QString>
#include "PowerSupply.h"

/**
 * @brief Парсер SCPI-команд.
 *
 * Принимает строку команды, возвращает строку-ответ. Работает
 * поверх объекта PowerSupply, который хранит состояние прибора.
 */
class ScpiParser {
private:
    /// Ссылка на источник питания, состояние которого меняем.
    PowerSupply& power_supply;

    /**
     * @brief Определяет тип команды и формирует ответ.
     * @param command Очищенная от \n и \r SCPI-команда.
     * @return Текстовый ответ. Пустая строка, если команда не распознана.
     */
    QString recognize(const QString& command);

public:
    /**
     * @brief Конструктор.
     * @param ps Ссылка на источник питания.
     */
    ScpiParser(PowerSupply& ps);

    /**
     * @brief Парсит команду и возвращает ответ.
     * @param command Сырая команда, возможно с \r\n.
     * @return Ответ прибора.
     */
    QString parse(const QString& command);
};

#endif // SCPIPARSER_H
