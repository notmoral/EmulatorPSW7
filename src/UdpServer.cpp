#include "UdpServer.h"
#include <QDebug>
#include <QHostAddress>

UdpServer::UdpServer(ScpiParser* sp, QObject* parent)
    : QObject(parent), parser(sp)
{
    m_database.open();
    m_database.create_tables();

    if (!m_database.open()) {
        qDebug() << "UDP: failed to open database";
    }

    m_socket = new QUdpSocket(this);

    connect(m_socket, &QUdpSocket::readyRead,
            this, &UdpServer::onReadyRead);

    if (m_socket->bind(QHostAddress("127.0.0.1"), 5025)) {
        qDebug() << "UDP server started on 127.0.0.1:5025";
    } else {
        qDebug() << "Failed to start UDP server:"
                 << m_socket->errorString();
    }
}

void UdpServer::onReadyRead()
{
    while (m_socket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(m_socket->pendingDatagramSize());

        QHostAddress sender;
        quint16 senderPort = 0;

        m_socket->readDatagram(datagram.data(), datagram.size(),
                               &sender, &senderPort);

        QString command = QString::fromUtf8(datagram).trimmed();
        QString device  = "PSW7";

        if (command.startsWith("[DEVICE=")) {
            const int endPos = command.indexOf(']');
            if (endPos > 0) {
                device  = command.mid(8, endPos - 8);
                command = command.mid(endPos + 1).trimmed();
            }
        }

        const QString response = parser->parse(command);
        m_database.save_command(device, command, response);

        const QByteArray responseData = response.toUtf8() + '\n';
        m_socket->writeDatagram(responseData, sender, senderPort);
    }
}
