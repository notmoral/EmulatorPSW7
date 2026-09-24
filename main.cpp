#include <QCoreApplication>
#include "src/TcpServer.h"

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    TcpServer server;
    return app.exec();
}