/*
    SPDX-FileCopyrightText: 2025 Alexander Wilms <f.alexander.wilms@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "connectiondetailsmodel.h"
#include "connectiondetails.h"

#include <QStringList>

ConnectionDetailsModel::ConnectionDetailsModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int ConnectionDetailsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return m_items.count();
}

QVariant ConnectionDetailsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_items.count()) {
        return {};
    }

    const Item &item = m_items.at(index.row());

    switch (role) {
    case IsSectionRole:
        return item.isSection;
    case SectionTitleRole:
        return item.sectionTitle;
    case DetailLabelRole:
        return item.label;
    case DetailValueRole:
        return item.value;
    default:
        return {};
    }
}

QHash<int, QByteArray> ConnectionDetailsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[IsSectionRole] = "isSection";
    roles[SectionTitleRole] = "sectionTitle";
    roles[DetailLabelRole] = "detailLabel";
    roles[DetailValueRole] = "detailValue";
    return roles;
}

void ConnectionDetailsModel::setDetailsList(const QList<ConnectionDetails::ConnectionDetailSection> &detailsList)
{
    QList<Item> items;
    for (const auto &section : detailsList) {
        // Add section header
        items.append({true, section.title, {}, {}}); // isSection, sectionTitle, label, value

        // Add detail rows
        for (const auto &[label, value] : section.details) {
            items.append({false, {}, label, value}); // isSection, sectionTitle, label, value
        }
    }

    // Same number of items, signal dataChanged for the ones that changed.
    if (items.size() == m_items.size()) {
        for (qsizetype i = 0; i < items.size(); ++i) {
            auto &currentItem = m_items[i];
            const auto &newItem = items.at(i);
            if (currentItem != newItem) {
                currentItem = newItem;
                const QModelIndex itemIndex = index(i, 0);
                Q_EMIT dataChanged(itemIndex, itemIndex);
            }
        }
        // Just reset the model (could optimized with insert/removeRows...)
    } else {
        beginResetModel();
        m_items = items;
        endResetModel();
    }
}

QString ConnectionDetailsModel::accessibilityDescription() const
{
    QStringList parts;

    for (const Item &item : m_items) {
        if (item.isSection) {
            // Skip section headers for accessibility - they're just organizational
            continue;
        } else if (!item.label.isEmpty() && !item.value.isEmpty()) {
            // Format as "Label: Value"
            parts.append(QString("%1: %2").arg(item.label, item.value));
        }
    }

    return parts.join(", ");
}
