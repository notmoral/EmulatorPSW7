#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QTcpSocket>

class TcpClient : public QObject
{
    Q_OBJECT

public:
    explicit TcpClient(QObject *parent = nullptr);

    Q_INVOKABLE void connectToServer(
        const QString &host,
        int port
    );

    Q_INVOKABLE void sendCommand(
        const QString &command
    );

signals:
    void connectedChanged(bool connected);
    void responseReceived(const QString &response);
    void errorOccurred(const QString &error);

private:
    QTcpSocket m_socket;
};

#endif // TCPCLIENT_H
