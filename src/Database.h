/**
 * @file Database.h
 * @brief Обёртка над SQLite-хранилищем истории команд.
 */

#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QString>
#include <QVariantList>

/**
 * @brief Хранит историю SCPI-команд в SQLite.
 *
 * Файл базы — emulator.db рядом с исполняемым файлом.
 * Таблица command_history: id, timestamp, device, instrument_name,
 * connection_type, command, response.
 */
class Database
{
public:
    Database();

    /// Открывает соединение с базой.
    bool open();

    /// Создаёт таблицу command_history и выполняет миграции.
    bool create_tables();

    /**
     * @brief Сохраняет одну запись в историю.
     * @param device Короткое имя прибора ("PSW7").
     * @param instrumentName Полное имя ("GW Instek PSW7-800").
     * @param connectionType Тип соединения ("TCP"/"UDP"/"HTTP").
     * @param command Текст SCPI-команды.
     * @param response Ответ прибора.
     * @return true при успешной вставке.
     */
    bool save_command(
        const QString& device,
        const QString& instrumentName,
        const QString& connectionType,
        const QString& command,
        const QString& response
        );

    /**
     * @brief Возвращает всю историю в порядке убывания id.
     * @return Список QVariantMap с ключами id, timestamp, device,
     *         instrument_name, connection_type, command, response.
     */
    QVariantList get_history();

private:
    QSqlDatabase m_database;    ///< Соединение Qt SQL.
};

#endif // DATABASE_H
