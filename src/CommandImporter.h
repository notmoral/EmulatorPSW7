#ifndef COMMANDIMPORTER_H
#define COMMANDIMPORTER_H

#include <QObject>
#include <QStringList>
#include "TcpClient.h"

class CommandImporter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool running READ isRunning NOTIFY runningChanged)

public:
    explicit CommandImporter(QObject* parent = nullptr);

    Q_INVOKABLE bool loadFile(const QString& filePath);
    Q_INVOKABLE void startImport(const QString& host = "127.0.0.1",
                                 int port = 5025);
    Q_INVOKABLE QString deviceName() const { return m_deviceName; }
    Q_INVOKABLE int commandCount() const { return m_commands.size(); }

    bool isRunning() const { return m_running; }

signals:
    void importStarted(int total);
    void importProgress(int current, int total);
    void importFinished(int total);
    void importError(const QString& error);
    void runningChanged();

private slots:
    void onConnectedChanged(bool connected);
    void onResponseReceived(const QString& response);
    void onErrorOccurred(const QString& error);

private:
    void sendCurrentCommand();

    TcpClient m_client;
    QString   m_deviceName;
    QStringList m_commands;
    int m_currentIndex = 0;
    bool m_running = false;
};

#endif // COMMANDIMPORTER_H
