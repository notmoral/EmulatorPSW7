#include <QCoreApplication>
#include "src/TcpServer.h"
#include "src/ScpiParser.h"
#include "src/PowerSupply.h"

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    PowerSupply ps;
    ScpiParser parser(ps);
    TcpServer server(&parser);

    return app.exec();
}
