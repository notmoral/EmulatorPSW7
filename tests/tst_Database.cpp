#include <QtTest/QTest>
#include <QSqlDatabase>
#include <QSqlQuery>

#include "../src/Database.h"

class TestDatabase : public QObject
{
    Q_OBJECT

private slots:
    void database_opens();
    void database_creates_tables();
    void database_table_exists();
    void database_table_columns();
    void database_saves_command();
    void database_reads_saved_command();
};

void TestDatabase::database_reads_saved_command()
{
    Database database;

    QVERIFY(database.open());
    QVERIFY(database.create_tables());

    QVERIFY(database.save_command(
        "PSW7",
        "*IDN?",
        "GW-INSTEK,PSW800-4.32,TW123456,01.00.20110101"
        ));

    QSqlQuery query(QSqlDatabase::database("emulator_connection"));

    QVERIFY(query.exec(
        "SELECT device, command, response "
        "FROM command_history "
        "ORDER BY id DESC "
        "LIMIT 1"
        ));

    QVERIFY(query.next());

    QCOMPARE(query.value(0).toString(), QString("PSW7"));
    QCOMPARE(query.value(1).toString(), QString("*IDN?"));
    QCOMPARE(
        query.value(2).toString(),
        QString("GW-INSTEK,PSW800-4.32,TW123456,01.00.20110101")
        );
}

void TestDatabase::database_saves_command()
{
    Database database;

    QVERIFY(database.open());
    QVERIFY(database.create_tables());

    QVERIFY(database.save_command(
        "PSW7",
        "*IDN?",
        "GW-INSTEK,PSW800-4.32,TW123456,01.00.20110101"
        ));
}

void TestDatabase::database_table_columns()
{
    Database database;

    QVERIFY(database.open());
    QVERIFY(database.create_tables());

    QSqlQuery query(QSqlDatabase::database("emulator_connection"));

    QVERIFY(query.exec("PRAGMA table_info(command_history)"));

    QStringList columns;

    while (query.next()) {
        columns.append(query.value(1).toString());
    }

    QCOMPARE(columns.size(), 5);
    QVERIFY(columns.contains("id"));
    QVERIFY(columns.contains("timestamp"));
    QVERIFY(columns.contains("device"));
    QVERIFY(columns.contains("command"));
    QVERIFY(columns.contains("response"));
}

void TestDatabase::database_opens()
{
    Database database;

    QVERIFY(database.open());
}

void TestDatabase::database_creates_tables()
{
    Database database;

    QVERIFY(database.open());
    QVERIFY(database.create_tables());
}

void TestDatabase::database_table_exists()
{
    Database database;

    QVERIFY(database.open());
    QVERIFY(database.create_tables());

    QSqlQuery query(QSqlDatabase::database("emulator_connection"));

    QVERIFY(query.exec(
        "SELECT name FROM sqlite_master "
        "WHERE type = 'table' "
        "AND name = 'command_history'"
        ));

    QVERIFY(query.next());
}

QTEST_MAIN(TestDatabase)

#include "tst_Database.moc"