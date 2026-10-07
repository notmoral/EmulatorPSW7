/**
 * @file CommandImporter.h
 * @brief Импорт SCPI-команд из текстового файла.
 */

#ifndef COMMANDIMPORTER_H
#define COMMANDIMPORTER_H

#include <QObject>
#include <QStringList>
#include "TcpClient.h"

/**
 * @brief Читает файл с командами и последовательно отправляет их серверу.
 *
 * Формат файла: первая непустая строка — имя прибора, остальные —
 * SCPI-команды. Строки, начинающиеся с '#', игнорируются.
 *
 * Перед каждой командой клиент посылает префикс @c [DEVICE=NAME],
 * чтобы TcpServer сохранил запись в БД под правильным именем.
 *
 * Прогресс передаётся через сигналы importStarted/importProgress/importFinished.
 */
class CommandImporter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool running READ isRunning NOTIFY runningChanged)

public:
    explicit CommandImporter(QObject* parent = nullptr);

    /**
     * @brief Загружает файл и парсит его.
     * @param filePath Путь к файлу.
     * @return true, если файл прочитан и содержит имя + хотя бы одну команду.
     */
    Q_INVOKABLE bool loadFile(const QString& filePath);

    /**
     * @brief Запускает процесс импорта.
     * @param host Адрес сервера.
     * @param port Порт сервера.
     */
    Q_INVOKABLE void startImport(const QString& host = "127.0.0.1",
                                 int port = 5025);

    /// @return Имя прибора, прочитанное из первой строки файла.
    Q_INVOKABLE QString deviceName() const { return m_deviceName; }

    /// @return Количество загруженных команд.
    Q_INVOKABLE int commandCount() const { return m_commands.size(); }

    /// @return true, если импорт сейчас выполняется.
    bool isRunning() const { return m_running; }

signals:
    /// Импорт стартовал, всего @p total команд.
    void importStarted(int total);

    /// Обработано @p current команд из @p total.
    void importProgress(int current, int total);

    /// Импорт завершён, обработано @p total команд.
    void importFinished(int total);

    /// Ошибка импорта.
    void importError(const QString& error);

    /// Изменилось значение свойства running.
    void runningChanged();

private slots:
    void onConnectedChanged(bool connected);
    void onResponseReceived(const QString& response);
    void onErrorOccurred(const QString& error);

private:
    void sendCurrentCommand();  ///< Отправляет команду с текущим индексом.

    TcpClient   m_client;       ///< Отдельный TCP-клиент для импорта.
    QString     m_deviceName;   ///< Имя прибора из файла.
    QStringList m_commands;     ///< Список SCPI-команд.
    int         m_currentIndex = 0;
    bool        m_running = false;
};

#endif // COMMANDIMPORTER_H
