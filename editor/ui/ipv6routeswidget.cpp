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

#include "ipv6routeswidget.h"

#include <KLineEdit>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QNetworkAddressEntry>

#include <KDebug>

#include "ui_ipv6routes.h"

#include "ipv6delegate.h"
#include "intdelegate.h"

class IpV6RoutesWidget::Private
{
public:
    Private() : model(0,4)
    {
        QStandardItem * headerItem = new QStandardItem(i18nc("Header text for IPv6 address", "Address"));
        model.setHorizontalHeaderItem(0, headerItem);
        headerItem = new QStandardItem(i18nc("Header text for IPv6 netmask", "Netmask"));
        model.setHorizontalHeaderItem(1, headerItem);
        headerItem = new QStandardItem(i18nc("Header text for IPv6 gateway", "Gateway"));
        model.setHorizontalHeaderItem(2, headerItem);
        headerItem = new QStandardItem(i18nc("Header text for IPv6 route metric", "Metric"));
        model.setHorizontalHeaderItem(3, headerItem);
    }
    Ui_RoutesIp6Config ui;
    QStandardItemModel model;
};

IpV6RoutesWidget::IpV6RoutesWidget(QWidget * parent)
    : QDialog(parent), d(new IpV6RoutesWidget::Private())
{
    d->ui.setupUi(this);
    d->ui.tableViewAddresses->setModel(&d->model);
    d->ui.tableViewAddresses->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);

    IpV6Delegate *ipDelegate = new IpV6Delegate(this);
    IntDelegate *netmaskDelegate = new IntDelegate(0,128,this);
    IntDelegate *metricDelegate = new IntDelegate(this);
    d->ui.tableViewAddresses->setItemDelegateForColumn(0, ipDelegate);
    d->ui.tableViewAddresses->setItemDelegateForColumn(1, netmaskDelegate);
    d->ui.tableViewAddresses->setItemDelegateForColumn(2, ipDelegate);
    d->ui.tableViewAddresses->setItemDelegateForColumn(3, metricDelegate);

    connect(d->ui.pushButtonAdd, SIGNAL(clicked()), this, SLOT(addRoute()));
    connect(d->ui.pushButtonRemove, SIGNAL(clicked()), this, SLOT(removeRoute()));

    connect(d->ui.tableViewAddresses->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this,
            SLOT(selectionChanged(QItemSelection)));

    connect(&d->model, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(tableViewItemChanged(QStandardItem*)));

    connect(d->ui.buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(d->ui.buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

IpV6RoutesWidget::~IpV6RoutesWidget()
{
    delete d;
}

void IpV6RoutesWidget::setNeverDefault(bool checked)
{
    d->ui.cbNeverDefault->setChecked(checked);
}

bool IpV6RoutesWidget::neverDefault() const
{
    return d->ui.cbNeverDefault->isChecked();
}

void IpV6RoutesWidget::setIgnoreAutoRoutes(bool checked)
{
    d->ui.cbIgnoreAutoRoutes->setChecked(checked);
}

void IpV6RoutesWidget::setIgnoreAutoRoutesCheckboxEnabled(bool enabled)
{
    d->ui.cbIgnoreAutoRoutes->setEnabled(enabled);
}

bool IpV6RoutesWidget::ignoreautoroutes() const
{
    return d->ui.cbIgnoreAutoRoutes->isChecked();
}

void IpV6RoutesWidget::setRoutes(const QList<NetworkManager::IPv6Route> &list)
{
    d->model.removeRows(0, d->model.rowCount());
    foreach (const NetworkManager::IPv6Route &addr, list) {
        QList<QStandardItem *> item;
        QNetworkAddressEntry entry;
        QNetworkAddressEntry nexthop;
        // we need to set up IP before prefix/netmask manipulation
        entry.setIp(QHostAddress(addr.route()));
        nexthop.setIp(QHostAddress(addr.nextHop()));
        kDebug()<<entry.ip().toString();
        item << new QStandardItem(entry.ip().toString())
             << new QStandardItem(QString::number(addr.prefix(),10))
             << new QStandardItem(nexthop.ip().toString())
             << new QStandardItem(QString::number(addr.metric(),10));

        d->model.appendRow(item);
    }
}

QList<NetworkManager::IPv6Route> IpV6RoutesWidget::routes()
{
    QList<NetworkManager::IPv6Route> list;

    for (int i = 0, rowCount = d->model.rowCount(); i < rowCount; i++) {
        QHostAddress ip, nhop;
        QNetworkAddressEntry entry;
        quint32 netmask, metric;

        ip.setAddress(d->model.item(i, 0)->text());
        entry.setIp(ip);
        nhop.setAddress(d->model.item(i, 2)->text());
        netmask = d->model.item(i, 1)->text().toUInt();
        metric = d->model.item(i, 3)->text().toUInt();

        list.append(NetworkManager::IPv6Route(ip.toIPv6Address(),
                                              netmask,
                                              nhop.toIPv6Address(),
                                              metric));
    }
    return list;
}

void IpV6RoutesWidget::addRoute()
{
    QList<QStandardItem *> item;
    item << new QStandardItem << new QStandardItem << new QStandardItem;
    d->model.appendRow(item);

    int rowCount = d->model.rowCount();
    if (rowCount > 0) {
        d->ui.tableViewAddresses->selectRow(rowCount - 1);

        QItemSelectionModel * selectionModel = d->ui.tableViewAddresses->selectionModel();
        QModelIndexList list = selectionModel->selectedIndexes();
        if (list.size()) {
            // QTableView is configured to select only rows.
            // So, list[0] - IP address.
            d->ui.tableViewAddresses->edit(list[0]);
        }
    }
}

void IpV6RoutesWidget::removeRoute()
{
    QItemSelectionModel * selectionModel = d->ui.tableViewAddresses->selectionModel();
    if (selectionModel->hasSelection()) {
        QModelIndexList indexes = selectionModel->selectedIndexes();
        d->model.takeRow(indexes[0].row());
    }
    d->ui.pushButtonRemove->setEnabled(false);
}

void IpV6RoutesWidget::selectionChanged(const QItemSelection & selected)
{
    kDebug() << "selectionChanged";
    d->ui.pushButtonRemove->setEnabled(!selected.isEmpty());
}

extern quint32 suggestNetmask(Q_IPV6ADDR ip);

void IpV6RoutesWidget::tableViewItemChanged(QStandardItem *item)
{
    if (item->text().isEmpty()) {
        return;
    }

    int column = item->column();
    if (column == 0) { // ip
        int row = item->row();

        QStandardItem *netmaskItem = d->model.item(row, column + 1); // netmask
        if (netmaskItem && netmaskItem->text().isEmpty()) {
            QHostAddress addr(item->text());
            quint32 netmask = suggestNetmask(addr.toIPv6Address());
            if (netmask) {
                netmaskItem->setText(QString::number(netmask,10));
            }
        }
    }
}
