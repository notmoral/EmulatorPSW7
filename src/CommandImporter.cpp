#include "CommandImporter.h"

#include <QFile>
#include <QTextStream>

CommandImporter::CommandImporter(QObject* parent)
    : QObject(parent)
{
    connect(&m_client, &TcpClient::connectedChanged,
            this, &CommandImporter::onConnectedChanged);
    connect(&m_client, &TcpClient::responseReceived,
            this, &CommandImporter::onResponseReceived);
    connect(&m_client, &TcpClient::errorOccurred,
            this, &CommandImporter::onErrorOccurred);
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

void CommandImporter::startImport(const QString& host, int port)
{
    if (m_running) {
        emit importError("Импорт уже выполняется");
        return;
    }
    if (m_commands.isEmpty()) {
        emit importError("Нет загруженных команд");
        return;
    }

    m_running = true;
    m_currentIndex = 0;
    emit runningChanged();
    emit importStarted(m_commands.size());

    m_client.connectToServer(host, port);
}

void CommandImporter::sendCurrentCommand()
{
    const QString command = m_commands.at(m_currentIndex);
    const QString prefixed =
        "[DEVICE=" + m_deviceName + "]" + command;
    m_client.sendCommand(prefixed);
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
