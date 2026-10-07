#include "TcpServer.h"
#include <QDebug>

TcpServer::TcpServer(ScpiParser* sp, QObject* parent)
    : QObject(parent), parser(sp) {
    m_database.open();
    m_database.create_tables();

    if (!m_database.open()) {
        qDebug() << "Failed to open database";
    }

    m_server = new QTcpServer(this);
    connect(m_server, &QTcpServer::newConnection,
            this, &TcpServer::onNewConnection);

    if (m_server->listen(QHostAddress("127.0.0.1"), 5025)) {
        qDebug() << "Server started!";
    }
    else {
        qDebug() << "Failed to start server: " <<
            m_server->errorString().toStdString();
    }
}

void TcpServer::onNewConnection() {
    m_client = m_server->nextPendingConnection();
    connect(m_client, &QTcpSocket::readyRead,
            this, &TcpServer::onReadyRead);

    qDebug() << "Client connect!";

}

void TcpServer::onReadyRead() {
    m_buffer = m_client->readAll();
    QString command = QString::fromUtf8(m_buffer).trimmed();

    QString device = "PSW7";

    if (command.startsWith("[DEVICE=")) {
        const int endPos = command.indexOf(']');
        if (endPos > 0) {
            device  = command.mid(8, endPos - 8);
            command = command.mid(endPos + 1).trimmed();
        }
    }

    const QString response = parser->parse(command);
    m_client->write(response.toUtf8() + '\n');

    m_database.save_command(device, command, response);
}
