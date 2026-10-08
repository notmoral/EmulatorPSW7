/**
 * @file UdpServer.h
 * @brief UDP-сервер эмулятора, принимает SCPI-команды.
 */

#ifndef UDPSERVER_H
#define UDPSERVER_H

#include <QObject>
#include <QUdpSocket>

#include "ScpiParser.h"
#include "Database.h"

/**
 * @brief UDP-сервер, слушающий 127.0.0.1:5025.
 *
 * Аналог TcpServer, но работает через датаграммы. Для каждой
 * принятой команды формирует ответ и отправляет его обратно
 * по адресу и порту отправителя. Пишет команду и ответ в БД.
 *
 * Поддерживает префикс @c [DEVICE=NAME] для указания имени прибора.
 */
class UdpServer : public QObject {
    Q_OBJECT
private:
    QUdpSocket* m_socket;   ///< UDP-сокет.
    ScpiParser* parser;     ///< Парсер SCPI-команд.
    Database    m_database; ///< Хранилище истории.
public:
    /**
     * @brief Создаёт сервер и сразу начинает слушать порт 5025.
     * @param sp Указатель на парсер (не владеет).
     * @param parent Родитель QObject.
     */
    UdpServer(ScpiParser* sp, QObject* parent = nullptr);

private slots:
    /// Читает все поступившие датаграммы и обрабатывает их.
    void onReadyRead();
};

#endif // UDPSERVER_H
