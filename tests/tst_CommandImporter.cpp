/**
 * @file tst_CommandImporter.cpp
 * @brief Интеграционные тесты универсального импорта команд.
 *
 * Требует, чтобы `EmulatorPSW7.exe` был запущен заранее.
 */

#include <QtTest/QTest>
#include <QSignalSpy>
#include <QTemporaryFile>
#include <QTextStream>

#include "../src/CommandImporter.h"

class TestCommandImporter : public QObject
{
    Q_OBJECT

private:
    QString createTempFile(const QString& content);

private slots:
    void import_via_tcp();
    void import_via_udp();
    void import_via_http();
    void import_invalid_file();
    void import_unknown_protocol();
};

QString TestCommandImporter::createTempFile(const QString& content)
{
    QTemporaryFile* file = new QTemporaryFile(this);
    file->setAutoRemove(false);
    file->open();
    QTextStream out(file);
    out << content;
    out.flush();
    const QString path = file->fileName();
    file->close();
    return path;
}

void TestCommandImporter::import_via_tcp()
{
    const QString path = createTempFile(
        "PSW7\n"
        "*IDN?\n"
        "SOURce:VOLTage:LIMit 10\n"
        "MEASure:VOLTage:DC?\n"
    );

    CommandImporter importer;
    QVERIFY(importer.loadFile(path));
    QCOMPARE(importer.commandCount(), 3);

    QSignalSpy finishSpy(&importer, &CommandImporter::importFinished);
    QSignalSpy errorSpy(&importer, &CommandImporter::importError);

    importer.startImport("127.0.0.1", 5025, "TCP");

    QVERIFY2(finishSpy.wait(5000), "Import via TCP did not finish");
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(finishSpy.first().at(0).toInt(), 3);

    QFile::remove(path);
}

void TestCommandImporter::import_via_udp()
{
    const QString path = createTempFile(
        "PSW7\n"
        "*IDN?\n"
        "MEASure:VOLTage:DC?\n"
    );

    CommandImporter importer;
    QVERIFY(importer.loadFile(path));
    QCOMPARE(importer.commandCount(), 2);

    QSignalSpy finishSpy(&importer, &CommandImporter::importFinished);
    QSignalSpy errorSpy(&importer, &CommandImporter::importError);

    importer.startImport("127.0.0.1", 5025, "UDP");

    QVERIFY2(finishSpy.wait(5000), "Import via UDP did not finish");
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(finishSpy.first().at(0).toInt(), 2);

    QFile::remove(path);
}

void TestCommandImporter::import_via_http()
{
    const QString path = createTempFile(
        "E36312A\n"
        "*IDN?\n"
        "OUTPut:STATe ON\n"
        "MEASure:VOLTage:DC?\n"
    );

    CommandImporter importer;
    QVERIFY(importer.loadFile(path));
    QCOMPARE(importer.commandCount(), 3);

    QSignalSpy finishSpy(&importer, &CommandImporter::importFinished);
    QSignalSpy errorSpy(&importer, &CommandImporter::importError);

    importer.startImport("127.0.0.1", 8080, "HTTP");

    QVERIFY2(finishSpy.wait(5000), "Import via HTTP did not finish");
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(finishSpy.first().at(0).toInt(), 3);

    QFile::remove(path);
}

void TestCommandImporter::import_invalid_file()
{
    const QString path = createTempFile("PSW7\n");

    CommandImporter importer;
    QSignalSpy errorSpy(&importer, &CommandImporter::importError);

    // Файл с одной строкой (без команд) не должен загрузиться
    QVERIFY(!importer.loadFile(path));
    QVERIFY(errorSpy.count() > 0);

    QFile::remove(path);
}

void TestCommandImporter::import_unknown_protocol()
{
    const QString path = createTempFile(
        "PSW7\n"
        "*IDN?\n"
    );

    CommandImporter importer;
    QVERIFY(importer.loadFile(path));

    QSignalSpy errorSpy(&importer, &CommandImporter::importError);

    importer.startImport("127.0.0.1", 5025, "SSH");

    QVERIFY2(errorSpy.count() > 0,
             "Unknown protocol should raise importError");

    QFile::remove(path);
}

QTEST_MAIN(TestCommandImporter)

#include "tst_CommandImporter.moc"