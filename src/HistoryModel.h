/**
 * @file HistoryModel.h
 * @brief Модель списка истории команд для QML ListView.
 */

#ifndef HISTORYMODEL_H
#define HISTORYMODEL_H

#include <QAbstractListModel>
#include <QVariantList>

#include "Database.h"

/**
 * @brief QAbstractListModel для отображения истории в QML.
 *
 * Роли: recordId, timestamp, device, instrumentName,
 * connectionType, command, response.
 * Метод reload() перечитывает данные из базы.
 */
class HistoryModel : public QAbstractListModel
{
    Q_OBJECT

public:
    /// Роли модели, доступные из QML-делегатов.
    enum Roles
    {
        IdRole = Qt::UserRole + 1,  ///< Первичный ключ записи.
        TimestampRole,              ///< Дата и время вставки.
        DeviceRole,                 ///< Короткое имя прибора.
        InstrumentNameRole,         ///< Полное имя прибора.
        ConnectionTypeRole,         ///< Тип соединения (TCP/UDP/HTTP).
        CommandRole,                ///< Текст SCPI-команды.
        ResponseRole                ///< Ответ прибора.
    };

    explicit HistoryModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(
        const QModelIndex &index,
        int role = Qt::DisplayRole
        ) const override;

    QHash<int, QByteArray> roleNames() const override;

    /// Перечитывает всю историю из БД и сбрасывает модель.
    Q_INVOKABLE void reload();

private:
    QVariantList m_history;     ///< Кеш записей.
    Database m_database;        ///< Источник данных.
};

#endif // HISTORYMODEL_H
