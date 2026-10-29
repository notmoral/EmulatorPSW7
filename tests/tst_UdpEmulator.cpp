/**
 * @file tst_UdpEmulator.cpp
 * @brief Интеграционные тесты UDP-транспорта эмулятора.
 *
 * Требует, чтобы `EmulatorPSW7.exe` был запущен заранее.
 */

#include <QtTest/QTest>
#include <QUdpSocket>
#include <QNetworkDatagram>

class TestUdpEmulator : public QObject
{
    Q_OBJECT

private:
    QString sendCommand(QUdpSocket& socket,
                        const QString& command,
                        const QHostAddress& host = QHostAddress::LocalHost,
                        quint16 port = 5025);

private slots:
    void udp_idn();
    void udp_measurements();
    void udp_unknown_command();
    void udp_e36312a_channels();
};

QString TestUdpEmulator::sendCommand(QUdpSocket& socket,
                                     const QString& command,
                                     const QHostAddress& host,
                                     quint16 port)
{
    socket.writeDatagram(command.toUtf8() + '\n', host, port);

    if (!socket.waitForReadyRead(1000))
        return {};

    QByteArray datagram;
    datagram.resize(socket.pendingDatagramSize());
    socket.readDatagram(datagram.data(), datagram.size());

    return QString::fromUtf8(datagram).trimmed();
}

void TestUdpEmulator::udp_idn()
{
    QUdpSocket socket;

    QVERIFY(socket.bind(QHostAddress::LocalHost, 0));

    const QString response = sendCommand(socket, "[DEVICE=PSW7]*IDN?");

    QVERIFY2(!response.isEmpty(), "Empty UDP response to *IDN?");
    QVERIFY(response.contains("GW-INSTEK"));
}

void TestUdpEmulator::udp_measurements()
{
    QUdpSocket socket;
    QVERIFY(socket.bind(QHostAddress::LocalHost, 0));

    // Устанавливаем лимит 10V
    sendCommand(socket, "[DEVICE=PSW7]SOURce:VOLTage:LIMit 10");

    for (int i = 0; i < 10; ++i) {
        const QString response =
            sendCommand(socket, "[DEVICE=PSW7]MEASure:VOLTage:DC?");

        bool ok = false;
        const float value = response.toFloat(&ok);

        QVERIFY2(ok, "Invalid voltage value over UDP");
        QVERIFY2(value >= 9.0f && value <= 10.0f,
                 "Voltage outside [9.0, 10.0] range over UDP");
    }
}

void TestUdpEmulator::udp_unknown_command()
{
    QUdpSocket socket;
    QVERIFY(socket.bind(QHostAddress::LocalHost, 0));

    const QString response =
        sendCommand(socket, "[DEVICE=PSW7]NOT:A:REAL:COMMAND");

    QVERIFY2(response.isEmpty(),
             "Unknown command should return empty response");
}

void TestUdpEmulator::udp_e36312a_channels()
{
    QUdpSocket socket;
    QVERIFY(socket.bind(QHostAddress::LocalHost, 0));

    // Переключаемся на канал 2
    sendCommand(socket, "[DEVICE=E36312A]INSTrument:NSELect 2");

    const QString selResponse =
        sendCommand(socket, "[DEVICE=E36312A]INSTrument:NSELect?");
    QCOMPARE(selResponse, QString("2"));

    // Ставим лимит и включаем выход
    sendCommand(socket, "[DEVICE=E36312A]SOURce2:VOLTage:LIMit 20");
    sendCommand(socket, "[DEVICE=E36312A]OUTPut2:STATe ON");

    const QString limitResponse =
        sendCommand(socket, "[DEVICE=E36312A]SOURce2:VOLTage:LIMit?");
    QCOMPARE(limitResponse, QString("20.000"));

    const QString measResponse =
        sendCommand(socket, "[DEVICE=E36312A]MEASure2:VOLTage:DC?");
    bool ok = false;
    const float value = measResponse.toFloat(&ok);
    QVERIFY2(ok, "Invalid channel 2 voltage");
    QVERIFY2(value >= 19.0f && value <= 20.0f,
             "Channel 2 voltage outside expected range");

    // Канал 1 не тронут
    const QString ch1State =
        sendCommand(socket, "[DEVICE=E36312A]OUTPut1:STATe?");
    QCOMPARE(ch1State, QString("0"));
}

QTEST_MAIN(TestUdpEmulator)

#include "tst_UdpEmulator.moc"