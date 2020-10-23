/*
    Copyright 2019 Bruce Anderson <banderson19com@san.rr.com>

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
#include "debug.h"
#include "wireguardtabwidget.h"
#include "wireguardpeerwidget.h"
#include "ui_wireguardtabwidget.h"
#include "ui_wireguardpeerwidget.h"
#include "uiutils.h"
#include "simpleipv4addressvalidator.h"
#include "simpleiplistvalidator.h"
#include "wireguardkeyvalidator.h"

#include <QStandardItemModel>

#include <NetworkManagerQt/Utils>
#include <NetworkManagerQt/Ipv4Setting>
#include <NetworkManagerQt/Ipv6Setting>
#include <KColorScheme>
#include <KConfig>
#include <KConfigGroup>

// Keys for the NetworkManager configuration
#define PNM_SETTING_WIREGUARD_SETTING_NAME "wireguard"

#define PNM_WG_KEY_PEERS             "peers"
#define PNM_WG_KEY_MTU               "mtu"
#define PNM_WG_KEY_PEER_ROUTES       "peer-routes"
#define PNM_WG_PEER_KEY_ALLOWED_IPS          "allowed-ips"
#define PNM_WG_PEER_KEY_ENDPOINT             "endpoint"
#define PNM_WG_PEER_KEY_PERSISTENT_KEEPALIVE "persistent-keepalive"
#define PNM_WG_PEER_KEY_PRESHARED_KEY        "preshared-key"
#define PNM_WG_PEER_KEY_PRESHARED_KEY_FLAGS  "preshared-key-flags"
#define PNM_WG_PEER_KEY_PUBLIC_KEY           "public-key"

static WireGuardKeyValidator keyValidator();
static SimpleIpListValidator allowedIPsValidator(SimpleIpListValidator::WithCidr, SimpleIpListValidator::Both);

class WireGuardTabWidget::Private
{
public:
    Private();
    ~Private();

    Ui_WireGuardTabWidget ui;
    NetworkManager::WireGuardSetting::Ptr setting;
    KSharedConfigPtr config;
    NMVariantMapList peers;
    int currentIndex;
    bool currentPeerValid;
    bool otherPeersValid;
};

WireGuardTabWidget::Private::Private(void)
{
}

WireGuardTabWidget::Private::~Private()
{
}

WireGuardTabWidget::WireGuardTabWidget(const NMVariantMapList &peerData, QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
    , d(new Private)
{
    d->ui.setupUi(this);

    d->config = KSharedConfig::openConfig();
    setWindowTitle(i18nc("@title: window wireguard peers properties",
                         "WireGuard peers properties"));
    connect(d->ui.btnAdd, &QPushButton::clicked, this, &WireGuardTabWidget::slotAddPeer);
    connect(d->ui.btnRemove, &QPushButton::clicked, this, &WireGuardTabWidget::slotRemovePeer);
    connect(d->ui.buttonBox, &QDialogButtonBox::accepted, this, &WireGuardTabWidget::accept);
    connect(d->ui.buttonBox, &QDialogButtonBox::rejected, this, &WireGuardTabWidget::reject);

    KAcceleratorManager::manage(this);

    loadConfig(peerData);

    if (peerData.isEmpty())
        slotAddPeer();
}

WireGuardTabWidget::~WireGuardTabWidget()
{
    delete d;
}

void WireGuardTabWidget::loadConfig(const NMVariantMapList &peerData)
{
    d->peers = peerData;
    int numIncomingPeers = d->peers.size();

    // If there weren't any peers in the incoming setting, create
    // the required first element
    if (d->peers.isEmpty())
        d->peers.append(*(new QVariantMap()));

    for (int i = 0; i < numIncomingPeers; i++) {
        slotAddPeerWithData(peerData[i]);
    }
    d->ui.tabWidget->setCurrentIndex(0);
}

NMVariantMapList WireGuardTabWidget::setting() const
{
    d->peers.clear();
    for (int i = 0; i < d->ui.tabWidget->count(); i++)
        d->peers.append(static_cast<WireGuardPeerWidget*>(d->ui.tabWidget->widget(i))->setting());
    return d->peers;
}

void WireGuardTabWidget::slotAddPeer()
{
    QVariantMap *newItem = new QVariantMap; 
    int numPeers = d->ui.tabWidget->count() + 1;
    WireGuardPeerWidget *newTab = new WireGuardPeerWidget(*newItem);
    connect(newTab, &WireGuardPeerWidget::notifyValid, this, &WireGuardTabWidget::slotWidgetChanged);
    d->ui.tabWidget->addTab(newTab, QString("Peer %1").arg(QString::number(numPeers)));
    d->peers.append(*newItem);
    d->ui.tabWidget->setCurrentIndex(numPeers - 1);
    slotWidgetChanged();
}

void WireGuardTabWidget::slotAddPeerWithData(const QVariantMap &peerData)
{
    int numPeers = d->ui.tabWidget->count() + 1;
    WireGuardPeerWidget *newTab = new WireGuardPeerWidget(peerData);
    d->ui.tabWidget->addTab(newTab, QString("Peer %1").arg(QString::number(numPeers)));
    connect(newTab, &WireGuardPeerWidget::notifyValid, this, &WireGuardTabWidget::slotWidgetChanged);
    d->peers.append(peerData);
    d->ui.tabWidget->setCurrentIndex(numPeers - 1);
    slotWidgetChanged();
}

void WireGuardTabWidget::slotRemovePeer()
{
    int numPeers = d->ui.tabWidget->count() - 1;
    d->ui.tabWidget->removeTab(d->ui.tabWidget->currentIndex());
    if (numPeers == 0) {
        slotAddPeer();
        numPeers = 1;
    }

    // Retitle the tabs to reflect the current numbers
    for (int i = 0; i < numPeers; i++)
        d->ui.tabWidget->setTabText(i, QString("Peer %1").arg(QString::number(i + 1)));
}

void WireGuardTabWidget::slotWidgetChanged()
{
    bool valid = true;
    for (int i = 0; i < d->ui.tabWidget->count(); i++) {
        if (!static_cast<WireGuardPeerWidget*>(d->ui.tabWidget->widget(i))->isValid()) {
            valid = false;
            break;
        }
    }
    d->ui.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(valid);
}
