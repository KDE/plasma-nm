/*
    Mobile proxy model - model for displaying netwoks in mobile kcm
    SPDX-FileCopyrightText: 2017 Martin Kacej <m.kacej@atlas.sk>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

*/

#ifndef PLASMA_NM_MOBILE_PROXY_MODEL_H
#define PLASMA_NM_MOBILE_PROXY_MODEL_H

#include <QSortFilterProxyModel>

class Q_DECL_EXPORT MobileProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(QAbstractItemModel *sourceModel READ sourceModel WRITE setSourceModel)
    Q_PROPERTY(bool showSavedMode READ showSavedMode WRITE setShowSavedMode NOTIFY showSavedModeChanged)
public:
    explicit MobileProxyModel(QObject *parent = 0);
    virtual ~MobileProxyModel();
    void setShowSavedMode(bool mode);
    bool showSavedMode() const;
signals:
    void showSavedModeChanged(bool mode);

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const Q_DECL_OVERRIDE;
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const Q_DECL_OVERRIDE;

private:
    bool m_showSavedMode;
};

#endif // PLASMA_NM_MOBILE_PROXY_MODEL_H
