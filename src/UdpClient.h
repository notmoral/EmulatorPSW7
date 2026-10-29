/**
 * @file UdpClient.h
 * @brief UDP-клиент для GUI.
 */

#ifndef UDPCLIENT_H
#define UDPCLIENT_H

#include <QObject>
#include <QUdpSocket>
#include <QHostAddress>

/**
 * @brief Клиент UDP-сервера, доступный из QML.
 *
 * Имеет тот же API, что TcpClient и HttpClient:
 * connectToServer(), sendCommand(), disconnectFromServer() и
 * сигналы connectedChanged, responseReceived, errorOccurred.
 *
 * UDP не устанавливает соединение: connectToServer() открывает
 * локальный сокет и запоминает адрес сервера, после чего эмулирует
 * состояние "подключён" для единообразия с TCP.
 */
class UdpClient : public QObject
{
    Q_OBJECT

    /// @c true, если сокет открыт и адрес сервера задан.
    Q_PROPERTY(bool connected READ isConnected NOTIFY connectedChanged)

public:
    /**
     * @brief Конструктор.
     * @param parent Родитель QObject.
     */
    explicit UdpClient(QObject *parent = nullptr);

    /// @return true, если клиент готов отправлять датаграммы.
    bool isConnected() const;

    /**
     * @brief Открывает локальный UDP-сокет и запоминает адрес сервера.
     * @param host IP-адрес сервера.
     * @param port UDP-порт сервера (обычно 5025).
     */
    Q_INVOKABLE void connectToServer(const QString &host, int port);

    /// Закрывает локальный сокет.
    Q_INVOKABLE void disconnectFromServer();

    /**
     * @brief Отправляет SCPI-команду датаграммой.
     * @param command Текст команды. К нему автоматически добавляется \n.
     */
    Q_INVOKABLE void sendCommand(const QString &command);

signals:
    /// Изменилось значение свойства connected.
    void connectedChanged(bool connected);

    /// Пришёл ответ от сервера (без завершающих пробелов и \n).
    void responseReceived(const QString &response);

    /// Ошибка сети или попытка отправки без открытого сокета.
    void errorOccurred(const QString &error);

private slots:
    /// Читает все поступившие датаграммы и испускает responseReceived.
    void onReadyRead();

private:
    QUdpSocket   m_socket;        ///< Сокет для отправки и приёма датаграмм.
    QHostAddress m_serverAddress; ///< Адрес сервера, заданный в connectToServer.
    quint16      m_serverPort = 0;///< Порт сервера.
    bool         m_connected  = false; ///< Флаг готовности клиента.
};

#endif // UDPCLIENT_H
