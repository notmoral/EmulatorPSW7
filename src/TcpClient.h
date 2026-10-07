/**
 * @file TcpClient.h
 * @brief TCP-клиент для GUI.
 */

#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QTcpSocket>

/**
 * @brief Клиент SCPI-сервера, доступный из QML.
 *
 * Предоставляет QML-слою простой интерфейс: @c connectToServer(),
 * @c sendCommand(), @c disconnectFromServer(). Результат приходит
 * через сигнал @c responseReceived().
 */
class TcpClient : public QObject
{
    Q_OBJECT
    /// @c true, если сокет находится в состоянии ConnectedState.
    Q_PROPERTY(bool connected READ isConnected NOTIFY connectedChanged)

public:
    explicit TcpClient(QObject *parent = nullptr);

    /// @return true при активном соединении.
    bool isConnected() const;

    /**
     * @brief Подключается к серверу.
     * @param host IP или имя хоста.
     * @param port TCP-порт.
     *
     * Если сокет уже подключается или подключён — вызов игнорируется.
     */
    Q_INVOKABLE void connectToServer(const QString &host, int port);

    /// Разрывает текущее соединение, если оно есть.
    Q_INVOKABLE void disconnectFromServer();

    /**
     * @brief Отправляет SCPI-команду.
     * @param command Команда без завершающего перевода строки (добавится сам).
     */
    Q_INVOKABLE void sendCommand(const QString &command);

signals:
    /// Испускается при смене состояния соединения.
    void connectedChanged(bool connected);

    /// Пришёл ответ от сервера (без завершающих пробелов и \n).
    void responseReceived(const QString &response);

    /// Ошибка сокета или попытка отправки без соединения.
    void errorOccurred(const QString &error);

private:
    QTcpSocket m_socket;    ///< Сокет соединения.
};

#endif // TCPCLIENT_H
