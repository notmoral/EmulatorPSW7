#include "Database.h"

Database::Database() {
    m_database = QSqlDatabase::addDatabase("QSQLITE");
    m_database.setDatabaseName("emulator.db");
}

bool Database::open() {
    return m_database.open();
}