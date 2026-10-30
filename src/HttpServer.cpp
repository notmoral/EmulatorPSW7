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

HttpServer::HttpServer(ScpiParser* sp,
                       QObject* parent,
                       const QString& login,
                       const QString& password)
    : QObject(parent),
      parser(sp),
      m_login(login),
      m_password(password)
{
    m_authEnabled = !m_login.isEmpty();

    m_database.open();
    m_database.create_tables();

    m_server = new QTcpServer(this);
    connect(m_server, &QTcpServer::newConnection,
            this, &HttpServer::onNewConnection);

    if (m_server->listen(QHostAddress::AnyIPv4, 8080)) {
        qDebug() << "HTTP server started on 0.0.0.0:8080 (all interfaces)";
        if (m_authEnabled) {
            qDebug() << "  Basic auth enabled for user:" << m_login;
        } else {
            qDebug() << "  Basic auth disabled (open access)";
        }
    } else {
        qDebug() << "Failed to start HTTP server:" << m_server->errorString();
    }
}

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

void HttpServer::onDisconnected()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (socket) {
        m_buffers.remove(socket);
        socket->deleteLater();
    }
}

void HttpServer::onReadyRead()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    m_buffers[socket].append(socket->readAll());
    const QByteArray& data = m_buffers[socket];

    const int headerEnd = data.indexOf("\r\n\r\n");
    if (headerEnd < 0) return;

    // Проверяем Content-Length: если тело ещё не пришло целиком — ждём.
    int contentLength = 0;
    const QByteArray headers = data.left(headerEnd);
    for (const QByteArray& line : headers.split('\n')) {
        if (line.toLower().startsWith("content-length:")) {
            contentLength = line.mid(15).trimmed().toInt();
            break;
        }
    }
    if (data.size() < headerEnd + 4 + contentLength) return;

    handleRequest(socket, data);
    m_buffers[socket].clear();
}

bool HttpServer::checkAuth(const QByteArray& headers) const
{
    if (!m_authEnabled) return true;

    const QString expected =
        "Basic " + QString::fromUtf8(
            (m_login + ":" + m_password).toUtf8().toBase64());

    for (const QByteArray& line : headers.split('\n')) {
        const QByteArray trimmed = line.trimmed();
        if (trimmed.toLower().startsWith("authorization:")) {
            const QString value = QString::fromUtf8(trimmed.mid(14)).trimmed();
            return value == expected;
        }
    }
    return false;
}

void HttpServer::handleRequest(QTcpSocket* socket, const QByteArray& request)
{
    const int firstLineEnd = request.indexOf("\r\n");
    if (firstLineEnd < 0) {
        socket->write(buildResponse(400, "Bad Request",
                                    "{\"error\":\"bad request\"}"));
        socket->disconnectFromHost();
        return;
    }

    const int headerEnd = request.indexOf("\r\n\r\n");
    const QByteArray headers = request.mid(firstLineEnd + 2,
                                           headerEnd - firstLineEnd - 2);
    const QByteArray body = request.mid(headerEnd + 4);

    const QList<QByteArray> parts = request.left(firstLineEnd).split(' ');
    if (parts.size() < 2) {
        socket->write(buildResponse(400, "Bad Request",
                                    "{\"error\":\"bad request line\"}"));
        socket->disconnectFromHost();
        return;
    }

    const QByteArray method = parts[0];
    const QString path      = QString::fromUtf8(parts[1]);

    // Проверка аутентификации доступа
    if (!checkAuth(headers)) {
        QByteArray resp = buildResponse(401, "Unauthorized",
                                        "{\"error\":\"authentication required\"}");
        resp.replace("HTTP/1.1 401 Unauthorized\r\n",
                     "HTTP/1.1 401 Unauthorized\r\n"
                     "WWW-Authenticate: Basic realm=\"Emulator\"\r\n");
        socket->write(resp);
        socket->disconnectFromHost();
        return;
    }

    QUrl url("http://localhost" + path);
    if (url.path() != "/scpi") {
        socket->write(buildResponse(404, "Not Found",
                                    "{\"error\":\"use /scpi\"}"));
        socket->disconnectFromHost();
        return;
    }

    QString cmd;
    QString device = "PSW7";

    if (method == "GET") {
        QUrlQuery query(url);
        cmd    = query.queryItemValue("cmd", QUrl::FullyDecoded);
        QString dev = query.queryItemValue("device", QUrl::FullyDecoded);
        if (!dev.isEmpty()) device = dev;
    } else if (method == "POST") {
        const QJsonDocument doc = QJsonDocument::fromJson(body);
        if (!doc.isObject()) {
            socket->write(buildResponse(400, "Bad Request",
                                        "{\"error\":\"invalid JSON\"}"));
            socket->disconnectFromHost();
            return;
        }
        const QJsonObject obj = doc.object();
        cmd = obj.value("cmd").toString();
        const QString dev = obj.value("device").toString();
        if (!dev.isEmpty()) device = dev;
    } else {
        socket->write(buildResponse(405, "Method Not Allowed",
                                    "{\"error\":\"GET or POST only\"}"));
        socket->disconnectFromHost();
        return;
    }

    if (cmd.isEmpty()) {
        socket->write(buildResponse(400, "Bad Request",
                                    "{\"error\":\"cmd required\"}"));
        socket->disconnectFromHost();
        return;
    }

    const QString response = parser->parse(device, cmd);
    const QString instrumentName = parser->displayName(device);
    m_database.save_command(device, instrumentName, "HTTP", cmd, response);

    QJsonObject obj;
    obj["device"]   = device;
    obj["command"]  = cmd;
    obj["response"] = response;
    const QByteArray respBody =
        QJsonDocument(obj).toJson(QJsonDocument::Compact);

    socket->write(buildResponse(200, "OK", respBody));
    socket->disconnectFromHost();
}

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