/*
    SPDX-FileCopyrightText: 2013-2014 Jan Grulich <jgrulich@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef PLASMA_NM_APPLET_PROXY_MODEL_H
#define PLASMA_NM_APPLET_PROXY_MODEL_H

#include <QSortFilterProxyModel>

#include "networkmodelitem.h"

class Q_DECL_EXPORT AppletProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(QAbstractItemModel *sourceModel READ sourceModel WRITE setSourceModel)
public:
    explicit AppletProxyModel(QObject *parent = nullptr);
    ~AppletProxyModel() override;

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
};

#endif // PLASMA_NM_APPLET_PROXY_MODEL_H
