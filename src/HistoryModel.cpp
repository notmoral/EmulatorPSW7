#include "HistoryModel.h"

HistoryModel::HistoryModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_database.open();
    m_database.create_tables();

    reload();
}

int HistoryModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_history.size();
}

QVariant HistoryModel::data(
    const QModelIndex &index,
    int role) const
{
    if (!index.isValid() || index.row() >= m_history.size())
        return {};

    const QVariantMap record =
        m_history.at(index.row()).toMap();

    switch (role)
    {
    case IdRole:
        return record.value("id");

    case TimestampRole:
        return record.value("timestamp");

    case DeviceRole:
        return record.value("device");

    case CommandRole:
        return record.value("command");

    case ResponseRole:
        return record.value("response");

    default:
        return {};
    }
}

QHash<int, QByteArray> HistoryModel::roleNames() const
{
    return {
        { IdRole, "recordId" },
        { TimestampRole, "timestamp" },
        { DeviceRole, "device" },
        { CommandRole, "command" },
        { ResponseRole, "response" }
    };
}

void HistoryModel::reload()
{
    beginResetModel();

    m_history = m_database.get_history();

    endResetModel();
}