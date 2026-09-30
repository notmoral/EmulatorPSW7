#include <QtTest/QTest>
#include "../src/Database.h"

class TestDatabase : public QObject
{
    Q_OBJECT

private slots:
    void database_opens();
};

void TestDatabase::database_opens()
{
    Database database;

    QVERIFY(database.open());
}

QTEST_MAIN(TestDatabase)

#include "tst_Database.moc"