#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml>

#include "src/PowerSupply.h"
#include "src/ScpiParser.h"
#include "src/TcpServer.h"
#include "src/TcpClient.h"
#include "src/HistoryModel.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    PowerSupply powerSupply;
    ScpiParser parser(powerSupply);
    TcpServer server(&parser);

    qmlRegisterType<TcpClient>(
        "EmulatorPSW7",
        1,
        0,
        "TcpClient"
        );

    qmlRegisterType<HistoryModel>(
        "EmulatorPSW7",
        1,
        0,
        "HistoryModel"
        );

    QQmlApplicationEngine engine;

    engine.load(
        QUrl(QStringLiteral("qrc:/qml/Main.qml"))
        );

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}