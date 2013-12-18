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

#include "ipv4widget.h"
#include "ui_ipv4.h"
#include "ui/ipv4delegate.h"

#include <QDialog>
#include <QStandardItemModel>
#include <QItemSelection>
#include <QNetworkAddressEntry>

#include <KEditListWidget>
#include <KDialog>

quint32 suggestNetmask(quint32 ip)
{
    /*
        A   0       0.0.0.0 <-->127.255.255.255  255.0.0.0 <--->/8
        B   10      128.0.0.0 <>191.255.255.255  255.255.0.0 <->/16
        C   110     192.0.0.0 <>223.255.255.255  255.255.255.0 >/24
        D   1110    224.0.0.0 <>239.255.255.255  not defined <->not defined
        E   1111    240.0.0.0 <>255.255.255.254  not defined <->not defined
    */
    quint32 netmask = 0;

    if (!(ip & 0x80000000)) {
        // test 0 leading bit
        netmask = 0xFF000000;
    }
    else if (!(ip & 0x40000000)) {
        // test 10 leading bits
        netmask = 0xFFFF0000;
    }
    else if (!(ip & 0x20000000)) {
        // test 110 leading bits
        netmask = 0xFFFFFF00;
    }

    return netmask;
}

class IPv4Widget::Private
{
public:
    Private() : model(0,3)
    {
        QStandardItem * headerItem = new QStandardItem(i18nc("Header text for IPv4 address", "Address"));
        model.setHorizontalHeaderItem(0, headerItem);
        headerItem = new QStandardItem(i18nc("Header text for IPv4 netmask", "Netmask"));
        model.setHorizontalHeaderItem(1, headerItem);
        headerItem = new QStandardItem(i18nc("Header text for IPv4 gateway", "Gateway"));
        model.setHorizontalHeaderItem(2, headerItem);
    }
    QStandardItemModel model;
};


IPv4Widget::IPv4Widget(const NetworkManager::Setting::Ptr &setting, QWidget* parent, Qt::WindowFlags f):
    SettingWidget(setting, parent, f),
    m_ui(new Ui::IPv4Widget),
    d(new IPv4Widget::Private())
{
    m_ui->setupUi(this);

    m_ui->tableViewAddresses->setModel(&d->model);
    m_ui->tableViewAddresses->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    m_ui->tableViewAddresses->horizontalHeader()->setStretchLastSection(true);

    IpV4Delegate *ipDelegate = new IpV4Delegate(this);
    m_ui->tableViewAddresses->setItemDelegateForColumn(0, ipDelegate);
    m_ui->tableViewAddresses->setItemDelegateForColumn(1, ipDelegate);
    m_ui->tableViewAddresses->setItemDelegateForColumn(2, ipDelegate);

    connect(m_ui->btnAdd, SIGNAL(clicked()), this, SLOT(slotAddIPAddress()));
    connect(m_ui->btnRemove, SIGNAL(clicked()), this, SLOT(slotRemoveIPAddress()));

    connect(m_ui->dnsMorePushButton, SIGNAL(clicked()), SLOT(slotDnsServers()));
    connect(m_ui->dnsSearchMorePushButton, SIGNAL(clicked()), SLOT(slotDnsDomains()));

    connect(m_ui->tableViewAddresses->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(selectionChanged(QItemSelection)));

    connect(&d->model, SIGNAL(itemChanged(QStandardItem*)),
            this, SLOT(tableViewItemChanged(QStandardItem*)));

    if (setting) {
        loadConfig(setting);
    }

    connect(m_ui->method, SIGNAL(currentIndexChanged(int)),
            SLOT(slotModeComboChanged(int)));
    slotModeComboChanged(m_ui->method->currentIndex());

    connect(m_ui->btnRoutes, SIGNAL(clicked()),
            SLOT(slotRoutesDialog()));

    // Validation
    connect(m_ui->dns, SIGNAL(textChanged(QString)), SLOT(slotWidgetChanged()));
    connect(m_ui->method, SIGNAL(currentIndexChanged(int)), SLOT(slotWidgetChanged()));
    connect(&d->model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), SLOT(slotWidgetChanged()));
    connect(&d->model, SIGNAL(rowsRemoved(QModelIndex,int,int)), SLOT(slotWidgetChanged()));

    KAcceleratorManager::manage(this);
}

IPv4Widget::~IPv4Widget()
{
    delete d;
    delete m_ui;
}

void IPv4Widget::loadConfig(const NetworkManager::Setting::Ptr &setting)
{
    NetworkManager::Ipv4Setting::Ptr ipv4Setting = setting.staticCast<NetworkManager::Ipv4Setting>();

    m_tmpIpv4Setting.setRoutes(ipv4Setting->routes());
    m_tmpIpv4Setting.setNeverDefault(ipv4Setting->neverDefault());
    m_tmpIpv4Setting.setIgnoreAutoRoutes(ipv4Setting->ignoreAutoRoutes());

    // method
    switch (ipv4Setting->method()) {
        case NetworkManager::Ipv4Setting::Automatic:
            if (ipv4Setting->ignoreAutoDns()) {
                m_ui->method->setCurrentIndex(AutomaticOnlyIP);
            } else {
                m_ui->method->setCurrentIndex(Automatic);
            }
            break;
        case NetworkManager::Ipv4Setting::Manual:
            m_ui->method->setCurrentIndex(Manual);
            break;
        case NetworkManager::Ipv4Setting::LinkLocal:
            m_ui->method->setCurrentIndex(LinkLocal);
            break;
        case NetworkManager::Ipv4Setting::Shared:
            m_ui->method->setCurrentIndex(Shared);
            break;
        case NetworkManager::Ipv4Setting::Disabled:
            m_ui->method->setCurrentIndex(Disabled);
            break;
    }

    // dns
    QStringList tmp;
    foreach (const QHostAddress & addr, ipv4Setting->dns()) {
        tmp.append(addr.toString());
    }
    m_ui->dns->setText(tmp.join(","));
    m_ui->dnsSearch->setText(ipv4Setting->dnsSearch().join(","));

    m_ui->dhcpClientId->setText(ipv4Setting->dhcpClientId());

    // addresses
    foreach (const NetworkManager::IpAddress &addr, ipv4Setting->addresses()) {
        QList<QStandardItem *> item;
        item << new QStandardItem(addr.ip().toString())
             << new QStandardItem(addr.netmask().toString())
             << new QStandardItem(addr.gateway().toString());

        d->model.appendRow(item);
    }

    // may-fail
    m_ui->ipv4RequiredCB->setChecked(!ipv4Setting->mayFail());
}

QVariantMap IPv4Widget::setting(bool agentOwned) const
{
    Q_UNUSED(agentOwned);

    NetworkManager::Ipv4Setting ipv4Setting;

    ipv4Setting.setRoutes(m_tmpIpv4Setting.routes());
    ipv4Setting.setNeverDefault(m_tmpIpv4Setting.neverDefault());
    ipv4Setting.setIgnoreAutoRoutes(m_tmpIpv4Setting.ignoreAutoRoutes());

    // method
    switch ((MethodIndex)m_ui->method->currentIndex()) {
        case Automatic:
            ipv4Setting.setMethod(NetworkManager::Ipv4Setting::Automatic);
            break;
        case IPv4Widget::AutomaticOnlyIP:
            ipv4Setting.setMethod(NetworkManager::Ipv4Setting::Automatic);
            ipv4Setting.setIgnoreAutoDns(true);
            break;
        case Manual:
            ipv4Setting.setMethod(NetworkManager::Ipv4Setting::Manual);
            break;
        case LinkLocal:
            ipv4Setting.setMethod(NetworkManager::Ipv4Setting::LinkLocal);
            break;
        case Shared:
            ipv4Setting.setMethod(NetworkManager::Ipv4Setting::Shared);
            break;
        case Disabled:
            ipv4Setting.setMethod(NetworkManager::Ipv4Setting::Disabled);
            break;
    }

    // dns
    if (m_ui->dns->isEnabled() && !m_ui->dns->text().isEmpty()) {
        QStringList tmp = m_ui->dns->text().split(',');
        QList<QHostAddress> tmpAddrList;
        foreach (const QString & str, tmp) {
            QHostAddress addr(str);
            if (!addr.isNull())
                tmpAddrList.append(addr);
        }
        ipv4Setting.setDns(tmpAddrList);
    }
    if (m_ui->dnsSearch->isEnabled() && !m_ui->dnsSearch->text().isEmpty()) {
        ipv4Setting.setDnsSearch(m_ui->dnsSearch->text().split(','));
    }

    // dhcp id
    if (m_ui->dhcpClientId->isEnabled() && !m_ui->dhcpClientId->text().isEmpty()) {
        ipv4Setting.setDhcpClientId(m_ui->dhcpClientId->text());
    }

    // addresses
    if (m_ui->tableViewAddresses->isEnabled()) {
        QList<NetworkManager::IpAddress> list;
        for (int i = 0, rowCount = d->model.rowCount(); i < rowCount; i++) {
            NetworkManager::IpAddress address;
            address.setIp(QHostAddress(d->model.item(i, 0)->text()));
            address.setNetmask(QHostAddress(d->model.item(i, 1)->text()));
            address.setGateway(QHostAddress(d->model.item(i, 2)->text()));
            list << address;
        }
        if (!list.isEmpty()) {
            ipv4Setting.setAddresses(list);
        }
    }

    // may-fail
    if (m_ui->ipv4RequiredCB->isEnabled()) {
        ipv4Setting.setMayFail(!m_ui->ipv4RequiredCB->isChecked());
    }

    return ipv4Setting.toMap();
}

void IPv4Widget::slotModeComboChanged(int index)
{
    if (index == Automatic) {  // Automatic
        m_ui->dnsLabel->setText(i18n("Other DNS Servers:"));
        m_ui->dns->setEnabled(true);
        m_ui->dnsMorePushButton->setEnabled(true);
        m_ui->dnsSearch->setEnabled(true);
        m_ui->dnsSearchMorePushButton->setEnabled(true);
        m_ui->dhcpClientId->setEnabled(true);
        m_ui->ipv4RequiredCB->setEnabled(true);
        m_ui->btnRoutes->setEnabled(true);
        m_ui->tableViewAddresses->setEnabled(false);
        m_ui->tableViewAddresses->setVisible(false);
        m_ui->btnAdd->setVisible(false);
        m_ui->btnRemove->setVisible(false);
    } else if (index == AutomaticOnlyIP) {
        m_ui->dnsLabel->setText(i18n("DNS Servers:"));
        m_ui->dns->setEnabled(true);
        m_ui->dnsMorePushButton->setEnabled(true);
        m_ui->dnsSearch->setEnabled(true);
        m_ui->dnsSearchMorePushButton->setEnabled(true);
        m_ui->dhcpClientId->setEnabled(true);
        m_ui->ipv4RequiredCB->setEnabled(true);
        m_ui->btnRoutes->setEnabled(true);
        m_ui->tableViewAddresses->setEnabled(false);
        m_ui->tableViewAddresses->setVisible(false);
        m_ui->btnAdd->setVisible(false);
        m_ui->btnRemove->setVisible(false);
    } else if (index == Manual) {  // Manual
        m_ui->dnsLabel->setText(i18n("DNS Servers:"));
        m_ui->dns->setEnabled(true);
        m_ui->dnsMorePushButton->setEnabled(true);
        m_ui->dnsSearch->setEnabled(true);
        m_ui->dnsSearchMorePushButton->setEnabled(true);
        m_ui->dhcpClientId->setEnabled(false);
        m_ui->ipv4RequiredCB->setEnabled(true);
        m_ui->btnRoutes->setEnabled(true);
        m_ui->tableViewAddresses->setEnabled(true);
        m_ui->tableViewAddresses->setVisible(true);
        m_ui->btnAdd->setVisible(true);
        m_ui->btnRemove->setVisible(true);
    } else if (index == LinkLocal || index == Shared) {  // Link-local or Shared
        m_ui->dnsLabel->setText(i18n("DNS Servers:"));
        m_ui->dns->setEnabled(false);
        m_ui->dnsMorePushButton->setEnabled(false);
        m_ui->dnsSearch->setEnabled(false);
        m_ui->dnsSearchMorePushButton->setEnabled(false);
        m_ui->dhcpClientId->setEnabled(false);
        m_ui->ipv4RequiredCB->setEnabled(true);
        m_ui->btnRoutes->setEnabled(false);
        m_ui->tableViewAddresses->setEnabled(false);
        m_ui->tableViewAddresses->setVisible(false);
        m_ui->btnAdd->setVisible(false);
        m_ui->btnRemove->setVisible(false);
    } else if (index == Disabled) {  // Disabled
        m_ui->dnsLabel->setText(i18n("DNS Servers:"));
        m_ui->dns->setEnabled(false);
        m_ui->dnsMorePushButton->setEnabled(false);
        m_ui->dnsSearch->setEnabled(false);
        m_ui->dnsSearchMorePushButton->setEnabled(false);
        m_ui->dhcpClientId->setEnabled(false);
        m_ui->ipv4RequiredCB->setEnabled(false);
        m_ui->btnRoutes->setEnabled(false);
        m_ui->tableViewAddresses->setEnabled(false);
        m_ui->tableViewAddresses->setVisible(false);
        m_ui->btnAdd->setVisible(false);
        m_ui->btnRemove->setVisible(false);
    }
}

void IPv4Widget::slotAddIPAddress()
{
    QList<QStandardItem *> item;
    item << new QStandardItem << new QStandardItem << new QStandardItem;
    d->model.appendRow(item);

    const int rowCount = d->model.rowCount();
    if (rowCount > 0) {
        m_ui->tableViewAddresses->selectRow(rowCount - 1);

        QItemSelectionModel * selectionModel = m_ui->tableViewAddresses->selectionModel();
        QModelIndexList list = selectionModel->selectedIndexes();
        if (!list.isEmpty()) {
            // QTableView is configured to select only rows.
            // So, list[0] - IP address.
            m_ui->tableViewAddresses->edit(list[0]);
        }
    }
}

void IPv4Widget::slotRemoveIPAddress()
{
    QItemSelectionModel * selectionModel = m_ui->tableViewAddresses->selectionModel();
    if (selectionModel->hasSelection()) {
        QModelIndexList indexes = selectionModel->selectedIndexes();
        d->model.takeRow(indexes[0].row());
    }
    m_ui->btnRemove->setEnabled(m_ui->tableViewAddresses->selectionModel()->hasSelection());
}

void IPv4Widget::selectionChanged(const QItemSelection & selected)
{
    m_ui->btnRemove->setEnabled(!selected.isEmpty());
}

void IPv4Widget::tableViewItemChanged(QStandardItem *item)
{
    if (item->text().isEmpty()) {
         return;
     }

     const int column = item->column();
     if (column == 0) { // ip
         int row = item->row();

         QStandardItem *netmaskItem = d->model.item(row, column + 1); // netmask
         if (netmaskItem && netmaskItem->text().isEmpty()) {
             QHostAddress addr(item->text());
             const quint32 netmask = suggestNetmask(addr.toIPv4Address());
             if (netmask) {
                 QHostAddress v(netmask);
                 netmaskItem->setText(v.toString());
             }
         }
     }
}

void IPv4Widget::slotRoutesDialog()
{
    QPointer<IpV4RoutesWidget> dlg = new IpV4RoutesWidget(this);

    dlg->setRoutes(m_tmpIpv4Setting.routes());
    dlg->setNeverDefault(m_tmpIpv4Setting.neverDefault());
    if (m_ui->method->currentIndex() == 2) {  // manual
        dlg->setIgnoreAutoRoutesCheckboxEnabled(false);
    } else {
        dlg->setIgnoreAutoRoutes(m_tmpIpv4Setting.ignoreAutoRoutes());
    }

    if (dlg->exec() == QDialog::Accepted) {
        m_tmpIpv4Setting.setRoutes(dlg->routes());
        m_tmpIpv4Setting.setNeverDefault(dlg->neverDefault());
        m_tmpIpv4Setting.setIgnoreAutoRoutes(dlg->ignoreautoroutes());
    }

    if (dlg) {
        dlg->deleteLater();
    }
}

void IPv4Widget::slotDnsServers()
{
    QPointer<KDialog> dlg = new KDialog(this);
    dlg->setCaption(i18n("Edit DNS servers"));
    dlg->setButtons(KDialog::Ok | KDialog::Cancel);
    KEditListWidget * listWidget = new KEditListWidget(dlg);
    dlg->setMainWidget(listWidget);
    listWidget->setItems(m_ui->dns->text().split(','));

    if (dlg->exec() == KDialog::Accepted) {
        QString text = listWidget->items().join(",");
        if (text.endsWith(',')) {
            text.chop(1);
        }
        m_ui->dns->setText(text);
    }

    if (dlg) {
        dlg->deleteLater();
    }
}

void IPv4Widget::slotDnsDomains()
{
    QPointer<KDialog> dlg = new KDialog(this);
    dlg->setCaption(i18n("Edit DNS search domains"));
    dlg->setButtons(KDialog::Ok | KDialog::Cancel);
    KEditListWidget * listWidget = new KEditListWidget(dlg);
    dlg->setMainWidget(listWidget);
    listWidget->setItems(m_ui->dnsSearch->text().split(','));

    if (dlg->exec() == KDialog::Accepted) {
        QString text = listWidget->items().join(",");
        if (text.endsWith(',')) {
            text.chop(1);
        }
        m_ui->dnsSearch->setText(text);
    }

    if (dlg) {
        dlg->deleteLater();
    }
}

bool IPv4Widget::isValid() const
{
    if (m_ui->method->currentIndex() == Manual) {
        if (!d->model.rowCount()) {
            return false;
        }

        for (int i = 0, rowCount = d->model.rowCount(); i < rowCount; i++) {
            QHostAddress ip = QHostAddress(d->model.item(i, 0)->text());
            QHostAddress netmask = QHostAddress(d->model.item(i, 1)->text());
            QHostAddress gateway = QHostAddress(d->model.item(i, 2)->text());

            if (ip.isNull() || netmask.isNull() || (gateway.isNull() && !d->model.item(i, 2)->text().isEmpty())) {
                return false;
            }
        }
    }

    if (!m_ui->dns->text().isEmpty() && (m_ui->method->currentIndex() == Automatic || m_ui->method->currentIndex() == Manual || m_ui->method->currentIndex() == AutomaticOnlyIP)) {
        const QStringList tmp = m_ui->dns->text().split(',');
        foreach (const QString & str, tmp) {
            QHostAddress addr(str);
            if (addr.isNull()) {
                return false;
            }
        }
    }

    return true;
}
