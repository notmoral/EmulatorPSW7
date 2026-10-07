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
 * Таблица command_history: id, timestamp, device, command, response.
 */
class Database
{
public:
    Database();

    /// Открывает соединение с базой.
    bool open();

    /// Создаёт таблицу command_history, если её ещё нет.
    bool create_tables();

    /**
     * @brief Сохраняет одну запись в историю.
     * @param device Имя прибора.
     * @param command Текст SCPI-команды.
     * @param response Ответ прибора.
     * @return true при успешной вставке.
     */
    bool save_command(
        const QString& device,
        const QString& command,
        const QString& response
        );

    /**
     * @brief Возвращает всю историю в порядке убывания id.
     * @return Список QVariantMap, каждый с ключами id, timestamp, device, command, response.
     */
    QVariantList get_history();

private:
    QSqlDatabase m_database;    ///< Соединение Qt SQL.
};

#endif // DATABASE_H
