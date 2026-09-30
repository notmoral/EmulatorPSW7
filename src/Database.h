#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>

class Database
{
public:
    Database();
    bool open();
    bool create_tables();
    bool save_command(
        const QString& device,
        const QString& command,
        const QString& response
        );
private:
    QSqlDatabase m_database;
};

#endif // DATABASE_H
