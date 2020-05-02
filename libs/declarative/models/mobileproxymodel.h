/*
 * Mobile proxy model - model for displaying netwoks in mobile kcm
 * Copyright 2017  Martin Kacej <m.kacej@atlas.sk>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
*/

#ifndef PLASMA_NM_MOBILE_PROXY_MODEL_H
#define PLASMA_NM_MOBILE_PROXY_MODEL_H

#include <QSortFilterProxyModel>

class Q_DECL_EXPORT MobileProxyModel : public QSortFilterProxyModel
{
Q_OBJECT
    Q_PROPERTY(QAbstractItemModel * sourceModel READ sourceModel WRITE setSourceModel)
    Q_PROPERTY(bool showSavedMode READ showSavedMode WRITE setShowSavedMode NOTIFY showSavedModeChanged)
public:
    explicit MobileProxyModel(QObject* parent = 0);
    virtual ~MobileProxyModel();
    void setShowSavedMode(bool mode);
    bool showSavedMode() const;
signals:
    void showSavedModeChanged(bool mode);

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const Q_DECL_OVERRIDE;
    bool lessThan(const QModelIndex& left, const QModelIndex& right) const Q_DECL_OVERRIDE;
private:
    bool m_showSavedMode;
};

#endif // PLASMA_NM_MOBILE_PROXY_MODEL_H
