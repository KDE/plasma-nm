/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>
    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "stringlistmodel.h"
StringListModel::StringListModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int StringListModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_items.count();
}

QVariant StringListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_items.count()) {
        return {};
    }
    if (role == Qt::DisplayRole) {
        return m_items.at(index.row());
    }
    return {};
}

QHash<int, QByteArray> StringListModel::roleNames() const
{
    return {{Qt::DisplayRole, QByteArrayLiteral("display")}};
}

QStringList StringListModel::stringList() const
{
    return m_items;
}

void StringListModel::setStringList(const QStringList &list)
{
    if (m_items == list) {
        return;
    }
    beginResetModel();
    m_items = list;
    endResetModel();
    Q_EMIT stringListChanged();
}

void StringListModel::append(const QString &value)
{
    if (value.isEmpty() || m_items.contains(value)) {
        return;
    }

    m_items.append(value);
    std::sort(m_items.begin(), m_items.end());

    beginResetModel();
    endResetModel();

    Q_EMIT stringListChanged();
}

void StringListModel::removeAt(int index)
{
    if (index < 0 || index >= m_items.count()) {
        return;
    }
    beginRemoveRows(QModelIndex(), index, index);
    m_items.removeAt(index);
    endRemoveRows();
    Q_EMIT stringListChanged();
}

void StringListModel::moveUp(int index)
{
    if (index <= 0 || index >= m_items.count()) {
        return;
    }
    beginMoveRows(QModelIndex(), index, index, QModelIndex(), index - 1);
    m_items.move(index, index - 1);
    endMoveRows();
    Q_EMIT stringListChanged();
}

void StringListModel::moveDown(int index)
{
    if (index < 0 || index >= m_items.count() - 1) {
        return;
    }
    beginMoveRows(QModelIndex(), index, index, QModelIndex(), index + 2);
    m_items.move(index, index + 1);
    endMoveRows();
    Q_EMIT stringListChanged();
}

bool StringListModel::contains(const QString &value) const
{
    return m_items.contains(value);
}

#include "moc_stringlistmodel.cpp"
