#include "UdpClient.h"
#include <QDebug>

UdpClient::UdpClient(QObject *parent)
    : QObject(parent)
{
    connect(&m_socket, &QUdpSocket::readyRead,
            this, &UdpClient::onReadyRead);
}

bool UdpClient::isConnected() const
{
    return m_connected;
}

void UdpClient::connectToServer(const QString &host, int port)
{
    if (m_connected) {
        return;
    }

    if (!m_socket.bind(QHostAddress::AnyIPv4, 0)) {
        emit errorOccurred("Не удалось открыть UDP-сокет: "
                           + m_socket.errorString());
        return;
    }

    m_serverAddress = QHostAddress(host);
    m_serverPort    = static_cast<quint16>(port);
    m_connected     = true;

    emit connectedChanged(true);
}

void UdpClient::disconnectFromServer()
{
    if (!m_connected) return;

    m_socket.close();
    m_connected = false;
    emit connectedChanged(false);
}

void UdpClient::sendCommand(const QString &command)
{
    if (!m_connected) {
        emit errorOccurred("Not connected");
        return;
    }

    const QByteArray data = command.trimmed().toUtf8() + '\n';
    m_socket.writeDatagram(data, m_serverAddress, m_serverPort);
}

void UdpClient::onReadyRead()
{
    while (m_socket.hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(m_socket.pendingDatagramSize());
        m_socket.readDatagram(datagram.data(), datagram.size());

        const QString response = QString::fromUtf8(datagram).trimmed();
        emit responseReceived(response);
    }
}
