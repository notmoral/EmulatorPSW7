#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>

class Database
{
public:
    Database();
    bool open();
private:
    QSqlDatabase m_database;
};

#endif // DATABASE_H
