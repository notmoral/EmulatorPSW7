#ifndef HISTORYMODEL_H
#define HISTORYMODEL_H

#include <QAbstractListModel>
#include <QVariantList>

#include "Database.h"

class HistoryModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles
    {
        IdRole = Qt::UserRole + 1,
        TimestampRole,
        DeviceRole,
        CommandRole,
        ResponseRole
    };

    explicit HistoryModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(
        const QModelIndex &index,
        int role = Qt::DisplayRole
        ) const override;

    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void reload();

private:
    QVariantList m_history;
    Database m_database;
};

#endif // HISTORYMODEL_H