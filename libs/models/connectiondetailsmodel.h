/*
    SPDX-FileCopyrightText: 2025 Alexander Wilms <f.alexander.wilms@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef PLASMA_NM_MODEL_CONNECTION_DETAILS_MODEL_H
#define PLASMA_NM_MODEL_CONNECTION_DETAILS_MODEL_H

#include "plasmanm_internal_export.h"

#include <QAbstractListModel>
#include <QString>

#include <qqmlregistration.h>

// Forward declaration
namespace ConnectionDetails
{
struct ConnectionDetailSection;
}

class PLASMANM_INTERNAL_EXPORT ConnectionDetailsModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

public:
    enum ConnectionDetailsRoles {
        IsSectionRole = Qt::UserRole + 1,
        SectionTitleRole,
        DetailLabelRole,
        DetailValueRole
    };
    Q_ENUM(ConnectionDetailsRoles)

    explicit ConnectionDetailsModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void setDetailsList(const QList<ConnectionDetails::ConnectionDetailSection> &detailsList);

private:
    struct Item {
        bool isSection;
        QString sectionTitle; // Only valid if isSection is true
        QString label;        // Only valid if isSection is false
        QString value;        // Only valid if isSection is false
    };

    QList<Item> m_items;
};

#endif // PLASMA_NM_MODEL_CONNECTION_DETAILS_MODEL_H
