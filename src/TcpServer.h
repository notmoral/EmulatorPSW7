#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

#include "ScpiParser.h"
#include "../src/Database.h"

class TcpServer : public QObject {
    Q_OBJECT
private:
    QTcpServer* m_server;
    QTcpSocket* m_client;

    QByteArray m_buffer;

    ScpiParser* parser;

    Database m_database;
public:
    TcpServer(ScpiParser* sp, QObject* parent = nullptr);

private slots:
    void onNewConnection();
    void onReadyRead();
};

#endif