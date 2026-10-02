#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QString>
#include <QVariantList>

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

    QVariantList get_history();

private:
    QSqlDatabase m_database;
};

#endif // DATABASE_H