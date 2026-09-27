#include <QtTest/QTest>
#include <QTcpSocket>

class TestEmulator : public QObject
{
    Q_OBJECT

private:
    QString sendCommand(QTcpSocket& socket, const QString& command);

private slots:
    void emulator_measurements();
    void reconnect_stress();
};

QString TestEmulator::sendCommand(QTcpSocket& socket, const QString& command)
{
    socket.write(command.toUtf8() + '\n');

    if (!socket.waitForBytesWritten(1000)) {
        return {};
    }

    if (!socket.waitForReadyRead(1000)) {
        return {};
    }

    QByteArray response = socket.readAll();

    return QString::fromUtf8(response).trimmed();
}

void TestEmulator::emulator_measurements()
{
    QTcpSocket socket;

    socket.connectToHost("127.0.0.1", 5025);

    QVERIFY2(socket.waitForConnected(1000),
             "Failed to connect to emulator");

    // *IDN?
    QString idn = sendCommand(socket, "*IDN?");

    QVERIFY(!idn.isEmpty());

    qDebug() << "IDN:" << idn;

    // Set voltage limit
    socket.write("SOURce:VOLTage:LIMit 10\n");

    QVERIFY2(socket.waitForBytesWritten(1000),
             "Failed to send voltage limit");

    QVERIFY2(socket.waitForReadyRead(1000),
             "No response after setting voltage limit");

    // Read and discard response from the write command
    socket.readAll();

    // Collect 100 measurements
    for (int i = 0; i < 100; ++i) {
        QString response =
            sendCommand(socket, "MEASure:VOLTage:DC?");

        bool ok = false;
        float voltage = response.toFloat(&ok);

        QVERIFY2(ok, "Invalid voltage value");

        QVERIFY2(voltage >= 9.0f && voltage <= 10.0f,
                 "Voltage is outside expected range");

        qDebug() << "Measurement" << i + 1 << ":" << voltage;
    }

    socket.disconnectFromHost();
    socket.close();
}

void TestEmulator::reconnect_stress()
{
    for (int i = 0; i < 100; ++i) {
        QTcpSocket socket;

        socket.connectToHost("127.0.0.1", 5025);

        QVERIFY2(socket.waitForConnected(1000),
                 "Failed to reconnect to emulator");

        QString idn = sendCommand(socket, "*IDN?");

        QVERIFY2(!idn.isEmpty(),
                 "Emulator stopped responding");

        socket.disconnectFromHost();
        socket.close();

        qDebug() << "Reconnect cycle:" << i + 1;
    }
}

QTEST_MAIN(TestEmulator)

#include "tst_Emulator.moc"