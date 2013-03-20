/*
Copyright 2011 Ilia Kats <ilia-kats@gmx.net>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of
the License or (at your option) version 3 or any later version
accepted by the membership of KDE e.V. (or its successor approved
by the membership of KDE e.V.), which shall act as a proxy
defined in Section 14 of version 3 of the license.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef IPV4ROUTESWIDGET_H
#define IPV4ROUTESWIDGET_H

#include <QDialog>
#include <QStyledItemDelegate>

#include <QtNetworkManager/ipv4config.h>

class QStandardItem;
class QItemSelection;

class IpV4RoutesWidget : public QDialog
{
    Q_OBJECT
public:
    IpV4RoutesWidget(QWidget * parent = 0);
    virtual ~IpV4RoutesWidget();

    void setRoutes(const QList<NetworkManager::IPv4Route> &list);
    QList<NetworkManager::IPv4Route> routes();
    void setNeverDefault(bool checked);
    bool neverDefault() const;
    void setIgnoreAutoRoutes(bool checked);
    void setIgnoreAutoRoutesCheckboxEnabled(bool enabled);
    bool ignoreautoroutes() const;

protected slots:
    void addRoute();
    void removeRoute();
    /**
     * Update remove IP button depending on if there is a selection
     */
    void selectionChanged(const QItemSelection &);
    void tableViewItemChanged(QStandardItem *);

private:
    class Private;
    Private *d;
};

#endif // IPV4ROUTESWIDGET_H
