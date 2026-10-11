/**
 * @file HttpServer.cpp
 * @brief Реализация HTTP-сервера эмулятора.
 */

#include "HttpServer.h"
#include <QDebug>
#include <QHostAddress>
#include <QUrl>
#include <QUrlQuery>
#include <QJsonObject>
#include <QJsonDocument>

/**
 * @brief Конструктор: открывает БД и начинает слушать порт 8080.
 */
HttpServer::HttpServer(ScpiParser* sp, QObject* parent)
    : QObject(parent), parser(sp)
{
    m_database.open();
    m_database.create_tables();

    m_server = new QTcpServer(this);

    connect(m_server, &QTcpServer::newConnection,
            this, &HttpServer::onNewConnection);

    if (m_server->listen(QHostAddress("127.0.0.1"), 8080)) {
        qDebug() << "HTTP server started on 127.0.0.1:8080";
    } else {
        qDebug() << "Failed to start HTTP server:"
                 << m_server->errorString();
    }
}

/**
 * @brief Принимает входящее TCP-соединение и подписывается на его сигналы.
 */
void HttpServer::onNewConnection()
{
    while (m_server->hasPendingConnections()) {
        QTcpSocket* socket = m_server->nextPendingConnection();

        connect(socket, &QTcpSocket::readyRead,
                this, &HttpServer::onReadyRead);
        connect(socket, &QTcpSocket::disconnected,
                this, &HttpServer::onDisconnected);

        m_buffers.insert(socket, QByteArray());
    }
}

/**
 * @brief Удаляет буфер и сокет при отключении клиента.
 */
void HttpServer::onDisconnected()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (socket) {
        m_buffers.remove(socket);
        socket->deleteLater();
    }
}

/**
 * @brief Читает данные, ждёт полный заголовок и вызывает обработку.
 */
void HttpServer::onReadyRead()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    m_buffers[socket].append(socket->readAll());

    const QByteArray& data = m_buffers[socket];

    // Заголовок HTTP завершается пустой строкой \r\n\r\n.
    const int headerEnd = data.indexOf("\r\n\r\n");
    if (headerEnd < 0) return;

    handleRequest(socket, data);
    m_buffers[socket].clear();
}

/**
 * @brief Полный разбор запроса и формирование ответа.
 */
void HttpServer::handleRequest(QTcpSocket* socket, const QByteArray& request)
{
    const int firstLineEnd = request.indexOf("\r\n");
    if (firstLineEnd < 0) {
        socket->write(buildResponse(400, "Bad Request",
                                    "{\"error\":\"bad request\"}"));
        socket->disconnectFromHost();
        return;
    }

    const QByteArray firstLine = request.left(firstLineEnd);
    const QList<QByteArray> parts = firstLine.split(' ');
    if (parts.size() < 3) {
        socket->write(buildResponse(400, "Bad Request",
                                    "{\"error\":\"bad request line\"}"));
        socket->disconnectFromHost();
        return;
    }

    const QByteArray method = parts[0];
    const QString path      = QString::fromUtf8(parts[1]);

    if (method != "GET") {
        socket->write(buildResponse(405, "Method Not Allowed",
                                    "{\"error\":\"only GET is supported\"}"));
        socket->disconnectFromHost();
        return;
    }

    QUrl url("http://localhost" + path);

    if (url.path() != "/scpi") {
        socket->write(buildResponse(404, "Not Found",
                                    "{\"error\":\"use /scpi?cmd=...\"}"));
        socket->disconnectFromHost();
        return;
    }

    QUrlQuery query(url);
    const QString cmd = query.queryItemValue("cmd", QUrl::FullyDecoded);
    QString device    = query.queryItemValue("device", QUrl::FullyDecoded);

    if (cmd.isEmpty()) {
        socket->write(buildResponse(400, "Bad Request",
                                    "{\"error\":\"cmd parameter required\"}"));
        socket->disconnectFromHost();
        return;
    }

    if (device.isEmpty()) device = "PSW7";

    const QString response = parser->parse(device, cmd);

    const QString instrumentName = parser->displayName(device);
    m_database.save_command(device, instrumentName, "HTTP", cmd, response);

    QJsonObject obj;
    obj["device"]   = device;
    obj["command"]  = cmd;
    obj["response"] = response;
    const QByteArray body =
        QJsonDocument(obj).toJson(QJsonDocument::Compact);

    socket->write(buildResponse(200, "OK", body));
    socket->disconnectFromHost();
}

/**
 * @brief Собирает HTTP-ответ с заголовками и телом.
 */
QByteArray HttpServer::buildResponse(int statusCode,
                                     const QByteArray& statusText,
                                     const QByteArray& body)
{
    QByteArray response;
    response.append("HTTP/1.1 " + QByteArray::number(statusCode)
                    + " " + statusText + "\r\n");
    response.append("Content-Type: application/json\r\n");
    response.append("Content-Length: "
                    + QByteArray::number(body.size()) + "\r\n");
    response.append("Connection: close\r\n");
    response.append("\r\n");
    response.append(body);
    return response;
}
