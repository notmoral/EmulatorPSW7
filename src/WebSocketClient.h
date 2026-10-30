/**
 * @file WebSocketClient.h
 * @brief WebSocket-клиент для GUI.
 */

#ifndef WEBSOCKETCLIENT_H
#define WEBSOCKETCLIENT_H

#include <QObject>
#include <QWebSocket>

/**
 * @brief Клиент WebSocket, доступный из QML.
 *
 * Подключается к серверу (по умолчанию ws://127.0.0.1:8081)
 * и испускает сигнал reloadRequested() при получении
 * сообщения "reload".
 */
class WebSocketClient : public QObject
{
    Q_OBJECT
public:
    explicit WebSocketClient(QObject* parent = nullptr);

    Q_INVOKABLE void connectToServer(const QString& url);

signals:
    /// Сигнал для QML: обновить историю.
    void reloadRequested();

private:
    QWebSocket m_socket;
};

#endif // WEBSOCKETCLIENT_H