/**
 * @file WebSocketServer.cpp
 * @brief Реализация WebSocket-сервера для уведомлений.
 */

#include "WebSocketServer.h"
#include <QDebug>
#include <QHostAddress>

WebSocketServer::WebSocketServer(QObject* parent)
    : QObject(parent)
{
    m_server = new QWebSocketServer("EmulatorWS",
                                    QWebSocketServer::NonSecureMode,
                                    this);
    if (m_server->listen(QHostAddress::AnyIPv4, 8081)) {
        qDebug() << "WebSocket server started on 0.0.0.0:8081";
    } else {
        qDebug() << "Failed to start WebSocket server:" << m_server->errorString();
    }

    connect(m_server, &QWebSocketServer::newConnection,
            this, &WebSocketServer::onNewConnection);
}

void WebSocketServer::onNewConnection()
{
    while (m_server->hasPendingConnections()) {
        QWebSocket* client = m_server->nextPendingConnection();
        m_clients.append(client);
        qDebug() << "WebSocket client connected. Total:" << m_clients.size();

        // Обработка отключения клиента
        connect(client, &QWebSocket::disconnected, this, [this, client]() {
            m_clients.removeAll(client);
            client->deleteLater();
            qDebug() << "WebSocket client disconnected. Total:" << m_clients.size();
        });
    }
}

void WebSocketServer::notifyClients()
{
    if (m_clients.isEmpty()) return;

    // Рассылаем короткое сообщение "reload"
    for (QWebSocket* client : std::as_const(m_clients)) {
        client->sendTextMessage("reload");
    }
}