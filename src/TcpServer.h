#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class TcpServer : public QObject {
    Q_OBJECT
private:
    QTcpServer* m_server;
    QTcpSocket* m_client;

    QByteArray m_buffer;
public:
    TcpServer(QObject* parent = nullptr);

private slots:
    void onNewConnection();
    void onReadyRead();
};

#endif