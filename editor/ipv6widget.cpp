/*
    Copyright (c) 2013 Lukas Tinkl <ltinkl@redhat.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QDialog>
#include <QStandardItemModel>
#include <QItemSelection>
#include <QNetworkAddressEntry>

#include "ipv6widget.h"
#include "ui_ipv6.h"
#include "ui/ipv6delegate.h"
#include "ui/intdelegate.h"

quint32 suggestNetmask(Q_IPV6ADDR ip)
{
    Q_UNUSED(ip);

    /*
    TODO: find out common IPv6-netmasks and make a complete function

    */
    quint32 netmask = 64;

    return netmask;
}

class IPv6Widget::Private
{
public:
    Private() : model(0,3)
    {
        QStandardItem * headerItem = new QStandardItem(i18nc("Header text for IPv6 address", "Address"));
        model.setHorizontalHeaderItem(0, headerItem);
        headerItem = new QStandardItem(i18nc("Header text for IPv6 prefix", "Prefix"));
        model.setHorizontalHeaderItem(1, headerItem);
        headerItem = new QStandardItem(i18nc("Header text for IPv6 gateway", "Gateway"));
        model.setHorizontalHeaderItem(2, headerItem);
    }
    QStandardItemModel model;
};


IPv6Widget::IPv6Widget(NetworkManager::Settings::Setting* setting, QWidget* parent, Qt::WindowFlags f):
    SettingWidget(setting, parent, f),
    m_ui(new Ui::IPv6Widget),
    m_ipv6Setting(0),
    d(new IPv6Widget::Private())
{
    m_ui->setupUi(this);

    m_ui->tableViewAddresses->setModel(&d->model);
    m_ui->tableViewAddresses->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    m_ui->tableViewAddresses->horizontalHeader()->setStretchLastSection(true);

    IpV6Delegate *ipDelegate = new IpV6Delegate(this);
    IntDelegate *prefixDelegate = new IntDelegate (0, 128, this);
    m_ui->tableViewAddresses->setItemDelegateForColumn(0, ipDelegate);
    m_ui->tableViewAddresses->setItemDelegateForColumn(1, prefixDelegate);
    m_ui->tableViewAddresses->setItemDelegateForColumn(2, ipDelegate);

    connect(m_ui->btnAdd, SIGNAL(clicked()), this, SLOT(slotAddIPAddress()));
    connect(m_ui->btnRemove, SIGNAL(clicked()), this, SLOT(slotRemoveIPAddress()));

    connect(m_ui->tableViewAddresses->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(selectionChanged(QItemSelection)));

    connect(&d->model, SIGNAL(itemChanged(QStandardItem*)),
            this, SLOT(tableViewItemChanged(QStandardItem*)));

    if (setting) {
        m_ipv6Setting = static_cast<NetworkManager::Settings::Ipv6Setting *>(setting);
        loadConfig(m_ipv6Setting);
    }

    connect(m_ui->method, SIGNAL(currentIndexChanged(int)),
            SLOT(slotModeComboChanged(int)));
    slotModeComboChanged(m_ui->method->currentIndex());

    connect(m_ui->btnRoutes, SIGNAL(clicked()),
            SLOT(slotRoutesDialog()));
}

IPv6Widget::~IPv6Widget()
{
    delete d;
}

void IPv6Widget::loadConfig(NetworkManager::Settings::Setting * setting)
{
    Q_UNUSED(setting)

    // method
    m_ui->method->setCurrentIndex(static_cast<int>(m_ipv6Setting->method()));

    // dns
    QStringList tmp;
    foreach (const QHostAddress & addr, m_ipv6Setting->dns()) {
        tmp.append(addr.toString());
    }
    m_ui->dns->setText(tmp.join(","));
    m_ui->dnsSearch->setText(m_ipv6Setting->dnsSearch().join(","));

    // addresses
    foreach (const NetworkManager::IPv6Address &addr, m_ipv6Setting->addresses()) {
        QList<QStandardItem *> item;
        QNetworkAddressEntry entry;
        // we need to set up IP before prefix/netmask manipulation
        entry.setIp(QHostAddress(addr.address()));

        item << new QStandardItem(entry.ip().toString())
             << new QStandardItem(QString::number(addr.netMask(),10));

        QString gateway;
        if (!QHostAddress(addr.gateway()).isNull()) {
            gateway = QHostAddress(addr.gateway()).toString();
        }
        item << new QStandardItem(gateway);

        d->model.appendRow(item);
    }

    // may-fail
    m_ui->ipv6RequiredCB->setChecked(!m_ipv6Setting->mayFail());

    // privacy
    if (m_ipv6Setting->privacy() != NetworkManager::Settings::Ipv6Setting::Unknown) {
        m_ui->privacyCombo->setCurrentIndex(static_cast<int>(m_ipv6Setting->privacy()));
    }
}

QVariantMap IPv6Widget::setting() const
{
    // method
    m_ipv6Setting->setMethod(static_cast<NetworkManager::Settings::Ipv6Setting::ConfigMethod>(m_ui->method->currentIndex()));

    // dns
    if (m_ui->dns->isEnabled() && !m_ui->dns->text().isEmpty()) {
        QStringList tmp = m_ui->dns->text().split(',');
        QList<QHostAddress> tmpAddrList;
        foreach (const QString & str, tmp) {
            QHostAddress addr(str);
            if (!addr.isNull())
                tmpAddrList.append(addr);
        }
        m_ipv6Setting->setDns(tmpAddrList);
    }
    if (m_ui->dnsSearch->isEnabled() && !m_ui->dnsSearch->text().isEmpty()) {
        m_ipv6Setting->setDnsSearch(m_ui->dnsSearch->text().split(','));
    }

    // addresses
    if (m_ui->tableViewAddresses->isEnabled()) {
        QList<NetworkManager::IPv6Address> list;
        for (int i = 0, rowCount = d->model.rowCount(); i < rowCount; i++) {
            QHostAddress ip, gw;
            QNetworkAddressEntry entry;

            ip.setAddress(d->model.item(i, 0)->text());
            entry.setIp(ip);
            entry.setPrefixLength(d->model.item(i, 1)->text().toInt());
            gw.setAddress(d->model.item(i, 2)->text());

            list.append(NetworkManager::IPv6Address(ip.toIPv6Address(),
                                                    entry.prefixLength(),
                                                    gw.toIPv6Address()));
        }
        m_ipv6Setting->setAddresses(list);
    }

    // may-fail
    if (m_ui->ipv6RequiredCB->isEnabled()) {
        m_ipv6Setting->setMayFail(!m_ui->ipv6RequiredCB->isChecked());
    }

    // privacy
    if (m_ui->privacyCombo->isEnabled()) {
        m_ipv6Setting->setPrivacy(static_cast<NetworkManager::Settings::Ipv6Setting::IPv6Privacy>(m_ui->privacyCombo->currentIndex()));
    }

    return m_ipv6Setting->toMap();
}

void IPv6Widget::slotModeComboChanged(int index)
{
    if (index == 0) {  // Automatic
        m_ui->dns->setEnabled(true);
        m_ui->dnsMorePushButton->setEnabled(true);
        m_ui->dnsSearch->setEnabled(true);
        m_ui->dnsSearchMorePushButton->setEnabled(true);
        m_ui->ipv6RequiredCB->setEnabled(true);
        m_ui->privacyCombo->setEnabled(true);
        m_ui->btnRoutes->setEnabled(true);

        m_ui->tableViewAddresses->setVisible(false);
        m_ui->btnAdd->setVisible(false);
        m_ui->btnRemove->setVisible(false);
    } else if (index == 3) {  // Manual
        m_ui->dns->setEnabled(true);
        m_ui->dnsMorePushButton->setEnabled(true);
        m_ui->dnsSearch->setEnabled(true);
        m_ui->dnsSearchMorePushButton->setEnabled(true);
        m_ui->ipv6RequiredCB->setEnabled(true);
        m_ui->privacyCombo->setEnabled(true);
        m_ui->btnRoutes->setEnabled(true);

        m_ui->tableViewAddresses->setVisible(true);
        m_ui->btnAdd->setVisible(true);
        m_ui->btnRemove->setVisible(true);
    } else if (index == 1 || index == 2) {  // Link-local or DHCP
        m_ui->dns->setEnabled(false);
        m_ui->dnsMorePushButton->setEnabled(false);
        m_ui->dnsSearch->setEnabled(false);
        m_ui->dnsSearchMorePushButton->setEnabled(false);
        m_ui->ipv6RequiredCB->setEnabled(true);
        m_ui->privacyCombo->setEnabled(true);
        m_ui->btnRoutes->setEnabled(false);

        m_ui->tableViewAddresses->setVisible(false);
        m_ui->btnAdd->setVisible(false);
        m_ui->btnRemove->setVisible(false);
    } else if (index == 4) {  // Ignored
        m_ui->dns->setEnabled(false);
        m_ui->dnsMorePushButton->setEnabled(false);
        m_ui->dnsSearch->setEnabled(false);
        m_ui->dnsSearchMorePushButton->setEnabled(false);
        m_ui->ipv6RequiredCB->setEnabled(false);
        m_ui->privacyCombo->setEnabled(false);
        m_ui->btnRoutes->setEnabled(false);

        m_ui->tableViewAddresses->setVisible(false);
        m_ui->btnAdd->setVisible(false);
        m_ui->btnRemove->setVisible(false);
    }
}

void IPv6Widget::slotAddIPAddress()
{
    QList<QStandardItem *> item;
    item << new QStandardItem << new QStandardItem << new QStandardItem;
    d->model.appendRow(item);

    int rowCount = d->model.rowCount();
    if (rowCount > 0) {
        m_ui->tableViewAddresses->selectRow(rowCount - 1);

        QItemSelectionModel * selectionModel = m_ui->tableViewAddresses->selectionModel();
        QModelIndexList list = selectionModel->selectedIndexes();
        if (list.size()) {
            // QTableView is configured to select only rows.
            // So, list[0] - IP address.
            m_ui->tableViewAddresses->edit(list[0]);
        }
    }
}

void IPv6Widget::slotRemoveIPAddress()
{
    QItemSelectionModel * selectionModel = m_ui->tableViewAddresses->selectionModel();
    if (selectionModel->hasSelection()) {
        QModelIndexList indexes = selectionModel->selectedIndexes();
        d->model.takeRow(indexes[0].row());
    }
    m_ui->btnRemove->setEnabled(m_ui->tableViewAddresses->selectionModel()->hasSelection());
}

void IPv6Widget::selectionChanged(const QItemSelection & selected)
{
    m_ui->btnRemove->setEnabled(!selected.isEmpty());
}

void IPv6Widget::tableViewItemChanged(QStandardItem *item)
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

void IPv6Widget::slotRoutesDialog()
{
    IpV6RoutesWidget * dlg = new IpV6RoutesWidget(this);
    dlg->setRoutes(m_ipv6Setting->routes());
    dlg->setNeverDefault(m_ipv6Setting->neverDefault());
    if (m_ui->method->currentIndex() == 3) {  // manual
        dlg->setIgnoreAutoRoutesCheckboxEnabled(false);
    } else {
        dlg->setIgnoreAutoRoutes(m_ipv6Setting->ignoreAutoRoutes());
    }
    if (dlg->exec() == QDialog::Accepted) {
        m_ipv6Setting->setRoutes(dlg->routes());
        m_ipv6Setting->setNeverDefault(dlg->neverDefault());
        m_ipv6Setting->setIgnoreAutoRoutes(dlg->ignoreautoroutes());
    }
    delete dlg;
}
