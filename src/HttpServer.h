/**
 * @file HttpServer.h
 * @brief HTTP-сервер эмулятора для управления по REST.
 */

#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHash>

#include "ScpiParser.h"
#include "Database.h"

/**
 * @brief HTTP-сервер, слушающий 127.0.0.1:8080.
 *
 * Поддерживает REST-интерфейс для отправки SCPI-команд:
 *
 * GET  /scpi?cmd=*IDN?&device=PSW7
 * POST /scpi  { "cmd": "*IDN?", "device": "PSW7" }
 *
 * Ответ — JSON:
 * { "device": "PSW7", "command": "*IDN?", "response": "..." }
 *
 * Опционально защищён базовой аутентификацией: если задан
 * login и password, клиент должен передавать заголовок
 * Authorization: Basic base64(login:password).
 * Без него сервер отвечает 401 Unauthorized.
 */
class HttpServer : public QObject {
    Q_OBJECT
public:
    /**
     * @brief Создаёт сервер и начинает слушать порт 8080.
     * @param sp Указатель на парсер SCPI.
     * @param parent Родитель QObject.
     * @param login Логин для Basic Auth. Пусто = без аутентификации.
     * @param password Пароль для Basic Auth.
     */
    HttpServer(ScpiParser* sp,
               QObject* parent = nullptr,
               const QString& login = QString(),
               const QString& password = QString());

private slots:
    void onNewConnection();
    void onReadyRead();
    void onDisconnected();

private:
    void handleRequest(QTcpSocket* socket, const QByteArray& request);
    QByteArray buildResponse(int statusCode,
                             const QByteArray& statusText,
                             const QByteArray& body);
    bool checkAuth(const QByteArray& headers) const;

    QTcpServer* m_server;
    ScpiParser* parser;
    Database    m_database;
    QHash<QTcpSocket*, QByteArray> m_buffers;

    QString m_login;
    QString m_password;
    bool    m_authEnabled = false;
};

#endif // HTTPSERVER_H