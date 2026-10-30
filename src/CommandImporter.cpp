/**
 * @file CommandImporter.cpp
 * @brief Реализация импорта SCPI-команд из файла.
 */

#include "CommandImporter.h"
#include "TcpClient.h"
#include "UdpClient.h"
#include "HttpClient.h"

#include <QFile>
#include <QTextStream>
#include <QMetaObject>

CommandImporter::CommandImporter(QObject* parent)
    : QObject(parent)
{
}

CommandImporter::~CommandImporter()
{
    cleanupClient();
}

bool CommandImporter::createClient(const QString& protocol)
{
    cleanupClient();

    const QString p = protocol.toUpper();

    if (p == "TCP") {
        m_client = new TcpClient(this);
    } else if (p == "UDP") {
        m_client = new UdpClient(this);
    } else if (p == "HTTP") {
        m_client = new HttpClient(this);
        static_cast<HttpClient*>(m_client)->setCredentials("admin", "admin123");
    } else {
        emit importError("Неизвестный протокол: " + protocol);
        return false;
    }

    // Подключаем сигналы. Так как тип клиента выбран в рантайме,
    // используем старый строковый синтаксис connect.
    connect(m_client, SIGNAL(connectedChanged(bool)),
            this, SLOT(onConnectedChanged(bool)));
    connect(m_client, SIGNAL(responseReceived(QString)),
            this, SLOT(onResponseReceived(QString)));
    connect(m_client, SIGNAL(errorOccurred(QString)),
            this, SLOT(onErrorOccurred(QString)));

    return true;
}

void CommandImporter::cleanupClient()
{
    if (m_client) {
        m_client->disconnect(this);
        m_client->deleteLater();
        m_client = nullptr;
    }
}

bool CommandImporter::loadFile(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit importError("Не удалось открыть файл: " + filePath);
        return false;
    }

    QTextStream stream(&file);
    QStringList lines;
    while (!stream.atEnd()) {
        const QString line = stream.readLine().trimmed();
        if (!line.isEmpty() && !line.startsWith('#'))
            lines.append(line);
    }
    file.close();

    if (lines.size() < 2) {
        emit importError("Файл должен содержать имя прибора и хотя бы одну команду");
        return false;
    }

    m_deviceName = lines.takeFirst();
    m_commands   = lines;
    m_currentIndex = 0;

    return true;
}

void CommandImporter::startImport(const QString& host,
                                  int port,
                                  const QString& protocol)
{
    if (m_running) {
        emit importError("Импорт уже выполняется");
        return;
    }
    if (m_commands.isEmpty()) {
        emit importError("Нет загруженных команд");
        return;
    }
    if (!createClient(protocol)) {
        return;
    }

    m_running = true;
    m_currentIndex = 0;
    emit runningChanged();
    emit importStarted(m_commands.size());

    QMetaObject::invokeMethod(m_client, "connectToServer",
                              Q_ARG(QString, host),
                              Q_ARG(int, port));
}

void CommandImporter::sendCurrentCommand()
{
    const QString command = m_commands.at(m_currentIndex);
    const QString prefixed =
        "[DEVICE=" + m_deviceName + "]" + command;

    QMetaObject::invokeMethod(m_client, "sendCommand",
                              Q_ARG(QString, prefixed));
}

void CommandImporter::onConnectedChanged(bool connected)
{
    if (!m_running) return;

    if (connected) {
        sendCurrentCommand();
    } else {
        m_running = false;
        emit runningChanged();
        emit importError("Соединение потеряно во время импорта");
    }
}

void CommandImporter::onResponseReceived(const QString& /*response*/)
{
    if (!m_running) return;

    m_currentIndex++;
    emit importProgress(m_currentIndex, m_commands.size());

    if (m_currentIndex >= m_commands.size()) {
        m_running = false;
        emit runningChanged();
        emit importFinished(m_commands.size());
        return;
    }

    sendCurrentCommand();
}

void CommandImporter::onErrorOccurred(const QString& error)
{
    if (!m_running) return;
    m_running = false;
    emit runningChanged();
    emit importError(error);
}