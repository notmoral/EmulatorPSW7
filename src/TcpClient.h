#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QTcpSocket>

class TcpClient : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool connected READ isConnected NOTIFY connectedChanged)

public:
    explicit TcpClient(QObject *parent = nullptr);

    bool isConnected() const;

    Q_INVOKABLE void connectToServer(
        const QString &host,
        int port
        );

    Q_INVOKABLE void disconnectFromServer();

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
