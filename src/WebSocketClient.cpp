/**
 * @file WebSocketClient.cpp
 * @brief Реализация WebSocket-клиента для GUI.
 */

#include "WebSocketClient.h"
#include <QDebug>
#include <QUrl>

WebSocketClient::WebSocketClient(QObject* parent)
    : QObject(parent)
{
    // Когда приходит текстовое сообщение от сервера
    connect(&m_socket, &QWebSocket::textMessageReceived,
            this, [this](const QString& message) {
        if (message == "reload") {
            emit reloadRequested();
        }
    });

    connect(&m_socket, &QWebSocket::connected, this, []() {
        qDebug() << "WebSocket client connected";
    });

    connect(&m_socket, &QWebSocket::disconnected, this, []() {
        qDebug() << "WebSocket client disconnected";
    });
}

void WebSocketClient::connectToServer(const QString& url)
{
    if (m_socket.state() == QAbstractSocket::ConnectedState) {
        m_socket.close();
    }
    m_socket.open(QUrl(url));
}