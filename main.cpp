/**
 * @file main.cpp
 * @brief Точка входа приложения EmulatorPSW7.
 */

#include <QNetworkInterface>
#include <QNetworkAddressEntry>
#include <QDebug>

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>

#include "src/PowerSupply.h"
#include "src/ScpiParser.h"
#include "src/TcpServer.h"
#include "src/UdpServer.h"
#include "src/HttpServer.h"
#include "src/TcpClient.h"
#include "src/UdpClient.h"
#include "src/HttpClient.h"
#include "src/HistoryModel.h"
#include "src/CommandImporter.h"
#include "src/Psw7Instrument.h"
#include "src/E36312AInstrument.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQuickStyle::setStyle("Fusion");

    Psw7Instrument   psw7;
    E36312AInstrument e36312a;
    ScpiParser       parser;

    // Вывод доступных IP-адресов для удаленного подключения.
    qDebug() << "--------------------------------------------------";
    qDebug() << "Servers are listening on all interfaces (0.0.0.0).";
    qDebug() << "To connect from another machine, use one of these IPs:";
    const QList<QHostAddress> allAddresses = QNetworkInterface::allAddresses();
    bool foundIp = false;
    for (const QHostAddress &address : allAddresses) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol &&
            !address.isLoopback()) {
            qDebug().noquote() << "  - " << address.toString();
            foundIp = true;
        }
    }
    if (!foundIp) {
        qDebug() << "  (No external IPv4 addresses found. Are you offline?)";
    }
    qDebug() << "--------------------------------------------------";

    parser.registerInstrument(&psw7);
    parser.registerInstrument(&e36312a);

    TcpServer   tcpServer(&parser);
    UdpServer   udpServer(&parser);
    HttpServer  httpServer(&parser);

    qmlRegisterType<TcpClient>("EmulatorPSW7", 1, 0, "TcpClient");
    qmlRegisterType<UdpClient>("EmulatorPSW7", 1, 0, "UdpClient");
    qmlRegisterType<HttpClient>("EmulatorPSW7", 1, 0, "HttpClient");
    qmlRegisterType<HistoryModel>("EmulatorPSW7", 1, 0, "HistoryModel");
    qmlRegisterType<CommandImporter>("EmulatorPSW7", 1, 0, "CommandImporter");

    QQmlApplicationEngine engine;

    const QUrl url(QStringLiteral("qrc:/qml/Main.qml"));

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection
        );

    engine.load(url);

    return app.exec();
}
