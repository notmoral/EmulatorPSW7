#ifndef UDPCLIENT_H
#define UDPCLIENT_H

#include <QObject>
#include <QUdpSocket>
#include <QHostAddress>

class UdpClient : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool connected READ isConnected NOTIFY connectedChanged)

public:
    explicit UdpClient(QObject *parent = nullptr);

    bool isConnected() const;

    Q_INVOKABLE void connectToServer(const QString &host, int port);
    Q_INVOKABLE void disconnectFromServer();
    Q_INVOKABLE void sendCommand(const QString &command);

signals:
    void connectedChanged(bool connected);
    void responseReceived(const QString &response);
    void errorOccurred(const QString &error);

private slots:
    void onReadyRead();

private:
    QUdpSocket   m_socket;
    QHostAddress m_serverAddress;
    quint16      m_serverPort = 0;
    bool         m_connected  = false;
};

#endif // UDPCLIENT_H
