/*
 * Mobile applet proxy model - model for displaying netwoks in mobile applet
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

#ifndef MOBILEAPPLETPROXYMODEL_H
#define MOBILEAPPLETPROXYMODEL_H

#include "appletproxymodel.h"

class Q_DECL_EXPORT MobileAppletProxymodel : public AppletProxyModel
{
Q_OBJECT
Q_PROPERTY(QAbstractItemModel * sourceModel READ sourceModel WRITE setSourceModel)
public:
    explicit MobileAppletProxymodel(QObject* parent = 0);
    virtual ~MobileAppletProxymodel();

protected:
    virtual bool filterAcceptsRow(int source_row, const QModelIndex& source_parent)  const Q_DECL_OVERRIDE;

};

#endif // MOBILEAPPLETPROXYMODEL_H
