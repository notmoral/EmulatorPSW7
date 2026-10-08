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
 * Предоставляет простой REST-интерфейс поверх SCPI:
 * клиент делает GET-запрос вида
 * @c /scpi?cmd=*IDN?&device=PSW7, сервер парсит команду через
 * ScpiParser, сохраняет запись в БД и возвращает JSON-объект
 * с полями @c device, @c command и @c response.
 *
 * Поддерживается только метод GET. Запросы к другим путям и
 * методы возвращают HTTP-коды 404 и 405 соответственно.
 *
 * Порт 8080 выбран, чтобы не пересекаться с TCP/UDP на 5025.
 */
class HttpServer : public QObject {
    Q_OBJECT
public:
    /**
     * @brief Создаёт сервер и сразу начинает слушать порт 8080.
     * @param sp Указатель на парсер SCPI (не владеет).
     * @param parent Родитель QObject.
     */
    HttpServer(ScpiParser* sp, QObject* parent = nullptr);

private slots:
    /// Обрабатывает новое TCP-подключение.
    void onNewConnection();

    /// Читает данные из сокета и вызывает handleRequest().
    void onReadyRead();

    /// Освобождает ресурсы, связанные с отключившимся сокетом.
    void onDisconnected();

private:
    /**
     * @brief Разбирает HTTP-запрос, формирует ответ, пишет в БД.
     * @param socket Сокет клиента.
     * @param request Полный текст HTTP-запроса (заголовки + тело).
     */
    void handleRequest(QTcpSocket* socket, const QByteArray& request);

    /**
     * @brief Собирает HTTP-ответ с заданным кодом и телом.
     * @param statusCode Код (200, 400, 404, 405).
     * @param statusText Текстовая часть статуса ("OK", "Bad Request", ...).
     * @param body Тело ответа.
     * @return Полный HTTP-ответ, готовый к отправке.
     */
    QByteArray buildResponse(int statusCode,
                             const QByteArray& statusText,
                             const QByteArray& body);

    QTcpServer* m_server;   ///< Слушающий TCP-сокет.
    ScpiParser* parser;     ///< Парсер SCPI-команд.
    Database    m_database; ///< Хранилище истории.

    /// Накопительный буфер для каждого активного соединения.
    QHash<QTcpSocket*, QByteArray> m_buffers;
};

#endif // HTTPSERVER_H
