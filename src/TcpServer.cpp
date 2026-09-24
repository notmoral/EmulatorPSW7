#include "TcpServer.h"
#include <QDebug>

TcpServer::TcpServer(QObject* parent) : QObject(parent) {
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
    QString messageText = QString::fromUtf8(m_buffer);
    qDebug() << messageText;
}