/**
 * @file main.cpp
 * @brief Точка входа приложения EmulatorPSW7.
 */

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

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQuickStyle::setStyle("Fusion");

    PowerSupply powerSupply;
    ScpiParser  parser(powerSupply);
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
