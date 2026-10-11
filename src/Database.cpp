/**
 * @file Database.cpp
 * @brief Реализация SQLite-хранилища истории команд.
 */

#include "Database.h"

#include <QSqlQuery>
#include <QVariantMap>

Database::Database()
{
    if (QSqlDatabase::contains("emulator_connection"))
    {
        m_database = QSqlDatabase::database("emulator_connection");
    }
    else
    {
        m_database = QSqlDatabase::addDatabase(
            "QSQLITE",
            "emulator_connection"
            );

        m_database.setDatabaseName("emulator.db");
    }
}

bool Database::save_command(
    const QString &device,
    const QString &instrumentName,
    const QString &connectionType,
    const QString &command,
    const QString &response)
{
    QSqlQuery query(m_database);

    query.prepare(
        "INSERT INTO command_history "
        "(device, instrument_name, connection_type, command, response) "
        "VALUES (:device, :instrument_name, :connection_type, "
        ":command, :response)"
        );

    query.bindValue(":device", device);
    query.bindValue(":instrument_name", instrumentName);
    query.bindValue(":connection_type", connectionType);
    query.bindValue(":command", command);
    query.bindValue(":response", response);

    return query.exec();
}

bool Database::open()
{
    return m_database.open();
}

bool Database::create_tables()
{
    QSqlQuery query(m_database);

    if (!query.exec(
            "CREATE TABLE IF NOT EXISTS command_history ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,"
            "device TEXT NOT NULL,"
            "instrument_name TEXT,"
            "connection_type TEXT,"
            "command TEXT NOT NULL,"
            "response TEXT"
            ")"))
    {
        return false;
    }

    QStringList existing;
    QSqlQuery check(m_database);
    if (check.exec("PRAGMA table_info(command_history)"))
    {
        while (check.next())
            existing << check.value(1).toString();
    }

    if (!existing.contains("instrument_name"))
    {
        m_database.exec(
            "ALTER TABLE command_history "
            "ADD COLUMN instrument_name TEXT"
            );
    }

    if (!existing.contains("connection_type"))
    {
        m_database.exec(
            "ALTER TABLE command_history "
            "ADD COLUMN connection_type TEXT"
            );
    }

    return true;
}

QVariantList Database::get_history()
{
    QVariantList history;

    QSqlQuery query(m_database);

    if (!query.exec(
            "SELECT id, timestamp, device, instrument_name, "
            "connection_type, command, response "
            "FROM command_history "
            "ORDER BY id DESC"
            ))
    {
        return history;
    }

    while (query.next())
    {
        QVariantMap record;

        record["id"]              = query.value("id");
        record["timestamp"]       = query.value("timestamp");
        record["device"]          = query.value("device");
        record["instrument_name"] = query.value("instrument_name");
        record["connection_type"] = query.value("connection_type");
        record["command"]         = query.value("command");
        record["response"]        = query.value("response");

        history.append(record);
    }

    return history;
}