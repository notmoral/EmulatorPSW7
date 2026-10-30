/**
 * @file WebSocketServer.h
 * @brief WebSocket-сервер для мгновенных уведомлений.
 */

#ifndef WEBSOCKETSERVER_H
#define WEBSOCKETSERVER_H

#include <QObject>
#include <QWebSocketServer>
#include <QWebSocket>
#include <QList>

/**
 * @brief WebSocket-сервер, слушающий 0.0.0.0:8081.
 *
 * Рассылает всем подключенным клиентам короткое сообщение
 * при каждом сохранении новой команды в базу данных.
 * Это позволяет GUI обновлять историю без таймера.
 */
class WebSocketServer : public QObject {
    Q_OBJECT
public:
    explicit WebSocketServer(QObject* parent = nullptr);

public slots:
    /// Рассылает сообщение "reload" всем активным клиентам.
    void notifyClients();

private slots:
    void onNewConnection();

private:
    QWebSocketServer*       m_server;
    QList<QWebSocket*>      m_clients;
};

#endif // WEBSOCKETSERVER_H