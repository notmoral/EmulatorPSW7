/**
 * @file ScpiParser.h
 * @brief Диспетчер SCPI-команд между несколькими приборами.
 */

#ifndef SCPIPARSER_H
#define SCPIPARSER_H

#include <QString>
#include <QMap>

#include "IInstrument.h"

/**
 * @brief Разбирает SCPI-команды и направляет их нужному прибору.
 *
 * Хранит map<короткое_имя, IInstrument*>. Метод parse() получает
 * имя прибора и команду, находит соответствующий IInstrument и
 * делегирует обработку ему.
 */
class ScpiParser {
public:
    ScpiParser();

    /**
     * @brief Регистрирует прибор в парсере.
     * @param instrument Указатель (владение остаётся у вызывающей стороны).
     */
    void registerInstrument(IInstrument* instrument);

    /**
     * @brief Парсит команду для указанного прибора.
     * @param deviceName Короткое имя прибора ("PSW7").
     * @param command Текст SCPI-команды.
     * @return Ответ прибора или пустая строка.
     */
    QString parse(const QString& deviceName, const QString& command);

    /**
     * @brief Возвращает человекочитаемое имя прибора.
     * @param deviceName Короткое имя прибора ("PSW7").
     * @return Например, "GW Instek PSW7-800". Пустая строка,
     *         если прибор не зарегистрирован.
     */
    QString displayName(const QString& deviceName) const;

private:
    QMap<QString, IInstrument*> m_instruments;
};

#endif // SCPIPARSER_H
