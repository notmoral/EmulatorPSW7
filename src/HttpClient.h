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
 * Имеет тот же API, что TcpClient и UdpClient:
 * connectToServer(), sendCommand(), disconnectFromServer() и
 * сигналы connectedChanged, responseReceived, errorOccurred.
 *
 * Поскольку HTTP не поддерживает постоянного соединения,
 * connectToServer() лишь сохраняет базовый URL, а sendCommand()
 * выполняет отдельный GET-запрос /scpi?cmd=...
 */
class HttpClient : public QObject
{
    Q_OBJECT
    /// @c true, если базовый URL задан и клиент готов отправлять запросы.
    Q_PROPERTY(bool connected READ isConnected NOTIFY connectedChanged)

public:
    explicit HttpClient(QObject* parent = nullptr);

    /// @return true, если клиент сконфигурирован.
    bool isConnected() const;

    /**
     * @brief Сохраняет базовый URL сервера.
     * @param host IP или имя хоста.
     * @param port HTTP-порт (по умолчанию 8080).
     */
    Q_INVOKABLE void connectToServer(const QString& host, int port);

    /// Сбрасывает базовый URL.
    Q_INVOKABLE void disconnectFromServer();

    /**
     * @brief Отправляет SCPI-команду GET-запросом.
     * @param command Текст команды без завершающего \n.
     *
     * Ответ приходит асинхронно через сигнал responseReceived().
     */
    Q_INVOKABLE void sendCommand(const QString& command);

signals:
    /// Изменилось состояние свойства connected.
    void connectedChanged(bool connected);

    /// Пришёл ответ сервера (значение поля @c response из JSON).
    void responseReceived(const QString& response);

    /// Ошибка сети или попытка отправки без настроенного URL.
    void errorOccurred(const QString& error);

private:
    QNetworkAccessManager m_manager;    ///< Менеджер HTTP-запросов.
    QUrl                  m_baseUrl;    ///< Базовый URL /scpi.
    bool                  m_connected = false; ///< Флаг готовности клиента.
};

#endif // HTTPCLIENT_H
