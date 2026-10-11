/**
 * @file TcpServer.h
 * @brief TCP-сервер эмулятора, принимает SCPI-команды.
 */

#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

#include "ScpiParser.h"
#include "../src/Database.h"

/**
 * @brief TCP-сервер, слушающий 127.0.0.1:5025.
 *
 * Каждую полученную команду передаёт в ScpiParser, отправляет ответ
 * обратно клиенту и сохраняет пару "команда-ответ" в базу данных.
 *
 * Поддерживает префикс @c [DEVICE=NAME], которым клиент указывает
 * имя прибора для записи в историю (используется CommandImporter).
 */
class TcpServer : public QObject {
    Q_OBJECT
private:
    QTcpServer* m_server;   ///< Слушающий сокет.
    QTcpSocket* m_client;   ///< Текущее клиентское соединение.

    QByteArray m_buffer;    ///< Буфер последнего чтения.

    ScpiParser* parser;     ///< Парсер SCPI-команд.

    Database m_database;    ///< Хранилище истории команд.
public:
    /**
     * @brief Создаёт сервер и сразу начинает слушать порт 5025.
     * @param sp Указатель на парсер (не владеет).
     * @param parent Родитель QObject.
     */
    TcpServer(ScpiParser* sp, QObject* parent = nullptr);

private slots:
    /// Обрабатывает новое TCP-подключение.
    void onNewConnection();

    /// Читает команду, отправляет ответ, пишет в БД.
    void onReadyRead();
};

#endif
