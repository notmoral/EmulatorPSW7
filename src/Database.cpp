#include "Database.h"
#include <QSqlQuery>

Database::Database()
{
    if (QSqlDatabase::contains("emulator_connection"))
    {
        m_database = QSqlDatabase::database("emulator_connection");
    }
    else
    {
        m_database = QSqlDatabase::addDatabase("QSQLITE", "emulator_connection");
        m_database.setDatabaseName("emulator.db");
    }
}

bool Database::save_command(
    const QString &device,
    const QString &command,
    const QString &response)
{
    QSqlQuery query(m_database);

    query.prepare(
        "INSERT INTO command_history (device, command, response) "
        "VALUES (:device, :command, :response)");

    query.bindValue(":device", device);
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

    return query.exec(
        "CREATE TABLE IF NOT EXISTS command_history ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,"
        "device TEXT NOT NULL,"
        "command TEXT NOT NULL,"
        "response TEXT"
        ")");
}