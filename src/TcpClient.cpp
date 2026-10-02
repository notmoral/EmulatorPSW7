#include "TcpClient.h"

TcpClient::TcpClient(QObject *parent)
    : QObject(parent)
{
    connect(
        &m_socket,
        &QTcpSocket::connected,
        this,
        [this]()
        {
            emit connectedChanged(true);
        }
    );

    connect(
        &m_socket,
        &QTcpSocket::readyRead,
        this,
        [this]()
        {
            const QString response =
                QString::fromUtf8(
                    m_socket.readAll()
                ).trimmed();

            emit responseReceived(response);
        }
    );

    connect(
        &m_socket,
        &QTcpSocket::disconnected,
        this,
        [this]()
        {
            emit connectedChanged(false);
        }
    );

    connect(
        &m_socket,
        &QAbstractSocket::errorOccurred,
        this,
        [this](QAbstractSocket::SocketError)
        {
            emit errorOccurred(
                m_socket.errorString()
            );
        }
    );
}

void TcpClient::connectToServer(
    const QString &host,
    int port)
{
    if (m_socket.state() !=
        QAbstractSocket::UnconnectedState)
    {
        m_socket.disconnectFromHost();
    }

    m_socket.connectToHost(host, port);
}

void TcpClient::sendCommand(
    const QString &command)
{
    if (m_socket.state() !=
        QAbstractSocket::ConnectedState)
    {
        emit errorOccurred("Not connected");
        return;
    }

    m_socket.write(
        command.trimmed().toUtf8() + '\n'
    );
}
