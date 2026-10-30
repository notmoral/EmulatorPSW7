/**
 * @file HttpClient.h
 * @brief HTTP-клиент для GUI.
 */

#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QUrl>

/**
 * @brief Клиент HTTP-сервера, доступный из QML.
 *
 * Имеет тот же API, что TcpClient и UdpClient.
 * Если заданы логин и пароль через setCredentials(), клиент отправляет
 * заголовок Authorization: Basic ... в каждом запросе.
 */
class HttpClient : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool connected READ isConnected NOTIFY connectedChanged)

public:
    explicit HttpClient(QObject* parent = nullptr);

    bool isConnected() const;

    Q_INVOKABLE void connectToServer(const QString& host, int port);
    Q_INVOKABLE void disconnectFromServer();
    Q_INVOKABLE void sendCommand(const QString& command);

    /**
     * @brief Устанавливает логин и пароль для Basic Auth.
     * @param login Имя пользователя. Пусто — без аутентификации.
     * @param password Пароль.
     */
    Q_INVOKABLE void setCredentials(const QString& login, const QString& password);

signals:
    void connectedChanged(bool connected);
    void responseReceived(const QString& response);
    void errorOccurred(const QString& error);

private:
    QNetworkAccessManager m_manager;
    QUrl                  m_baseUrl;
    bool                  m_connected = false;
    QString               m_login;
    QString               m_password;
};

#endif // HTTPCLIENT_H