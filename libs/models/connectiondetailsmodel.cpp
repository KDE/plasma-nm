/*
    SPDX-FileCopyrightText: 2025 Alexander Wilms <f.alexander.wilms@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "connectiondetailsmodel.h"
#include "connectiondetails.h"

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
    beginResetModel();
    m_items.clear();

    for (const auto &section : detailsList) {
        // Add section header
        m_items.append({true, section.title, {}, {}}); // isSection, sectionTitle, label, value

        // Add detail rows
        for (const auto &[label, value] : section.details) {
            m_items.append({false, {}, label, value}); // isSection, sectionTitle, label, value
        }
    }
    endResetModel();
}
