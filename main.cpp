#define TESTING

#ifndef TESTING
#include <QCoreApplication>
#include "src/TcpServer.h"

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    TcpServer server;
    return app.exec();
}
#else
#include <QCoreApplication>
#include <QDebug>
#include "src/ScpiParser.h"

int main(int argc, char* argv[])
{
    PowerSupply ps;
    ScpiParser parser(ps);
    parser.parse("SOURce:VOLTage:LIMit 10");
    for (int i{}; i < 10; ++i) {

        qDebug() << parser.parse("MEASure:VOLTage:DC?");

    };
}
#endif