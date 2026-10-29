/**
 * @file HistoryModel.cpp
 * @brief Реализация модели истории команд для QML.
 */

#include <QFile>
#include <QTextStream>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

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

    case InstrumentNameRole:
        return record.value("instrument_name");

    case ConnectionTypeRole:
        return record.value("connection_type");

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
        { InstrumentNameRole, "instrumentName" },
        { ConnectionTypeRole, "connectionType" },
        { CommandRole, "command" },
        { ResponseRole, "response" }
    };
}

void HistoryModel::reload()
{
    beginResetModel();

    m_history = m_database.get_history(m_filter);

    endResetModel();
}

void HistoryModel::setFilter(const QString& filter)
{
    m_filter = filter;
    reload();
}

bool HistoryModel::exportTo(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    // Формат определяется по расширению.
    if (filePath.endsWith(".json", Qt::CaseInsensitive)) {
        QJsonArray arr;
        for (const QVariant& v : m_history) {
            const QVariantMap row = v.toMap();
            QJsonObject obj;
            obj["id"]              = row.value("id").toInt();
            obj["timestamp"]       = row.value("timestamp").toString();
            obj["device"]          = row.value("device").toString();
            obj["instrument_name"] = row.value("instrument_name").toString();
            obj["connection_type"] = row.value("connection_type").toString();
            obj["command"]         = row.value("command").toString();
            obj["response"]        = row.value("response").toString();
            arr.append(obj);
        }
        file.write(QJsonDocument(arr).toJson(QJsonDocument::Indented));
    } else if (filePath.endsWith(".csv", Qt::CaseInsensitive)) {
        QTextStream out(&file);
        out.setEncoding(QStringConverter::Utf8);
        out << "timestamp,device,instrument_name,connection_type,command,response\n";

        auto escapeCsv = [](QString s) {
            s.replace('"', "\"\"");
            return "\"" + s + "\"";
        };

        for (const QVariant& v : m_history) {
            const QVariantMap row = v.toMap();
            out << escapeCsv(row.value("timestamp").toString())        << ","
                << escapeCsv(row.value("device").toString())           << ","
                << escapeCsv(row.value("instrument_name").toString())  << ","
                << escapeCsv(row.value("connection_type").toString())  << ","
                << escapeCsv(row.value("command").toString())          << ","
                << escapeCsv(row.value("response").toString())         << "\n";
        }
    } else {
        file.close();
        return false;
    }

    file.close();
    return true;
}
