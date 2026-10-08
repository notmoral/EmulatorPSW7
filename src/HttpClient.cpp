/**
 * @file HttpClient.cpp
 * @brief Реализация HTTP-клиента для GUI.
 */

#include "HttpClient.h"
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>

HttpClient::HttpClient(QObject* parent)
    : QObject(parent)
{
}

bool HttpClient::isConnected() const
{
    return m_connected;
}

/**
 * @brief Запоминает адрес и порт сервера, конструирует базовый URL.
 */
void HttpClient::connectToServer(const QString& host, int port)
{
    if (m_connected) return;

    m_baseUrl = QUrl(QString("http://%1:%2/scpi").arg(host).arg(port));
    m_connected = true;

    emit connectedChanged(true);
}

/**
 * @brief Сбрасывает базовый URL.
 */
void HttpClient::disconnectFromServer()
{
    if (!m_connected) return;

    m_connected = false;
    emit connectedChanged(false);
}

/**
 * @brief Отправляет команду как GET /scpi?cmd=...
 *
 * Ответ обрабатывается в лямбде, привязанной к сигналу finished().
 */
void HttpClient::sendCommand(const QString& command)
{
    if (!m_connected) {
        emit errorOccurred("Not connected");
        return;
    }

    QUrl url = m_baseUrl;
    QUrlQuery query;
    query.addQueryItem("cmd", command.trimmed());
    url.setQuery(query);

    QNetworkReply* reply = m_manager.get(QNetworkRequest(url));

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();

        if (reply->error() != QNetworkReply::NoError) {
            emit errorOccurred(reply->errorString());
            return;
        }

        const QByteArray data = reply->readAll();
        const QJsonDocument doc = QJsonDocument::fromJson(data);

        if (doc.isObject()) {
            const QString response =
                doc.object().value("response").toString();
            emit responseReceived(response);
        } else {
            // На случай, если сервер вернул не JSON.
            emit responseReceived(QString::fromUtf8(data).trimmed());
        }
    });
}
