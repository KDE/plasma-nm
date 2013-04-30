/*
    Copyright 2013 Jan Grulich <jgrulich@redhat.com>
    Copyright 2013 Lukas Tinkl <ltinkl@redhat.com>

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

#include "connectiondetaileditor.h"
#include "ui_connectiondetaileditor.h"
#include "connectionwidget.h"
#include "wiredconnectionwidget.h"
#include "wificonnectionwidget.h"
#include "ipv4widget.h"
#include "ipv6widget.h"
#include "wifisecurity.h"
#include "wiredsecurity.h"
#include "pppwidget.h"
#include "pppoewidget.h"
#include "gsmwidget.h"
#include "cdmawidget.h"
#include "btwidget.h"
#include "infinibandwidget.h"
#include "bondwidget.h"
#include "vpnuiplugin.h"

#include <QDebug>

#include <NetworkManagerQt/settings.h>
#include <NetworkManagerQt/activeconnection.h>
#include <NetworkManagerQt/connection.h>
#include <NetworkManagerQt/settings/802-11-wireless.h>
#include <NetworkManagerQt/settings/vpn.h>
#include <NetworkManagerQt/generic-types.h>

#include <KPluginFactory>
#include <KServiceTypeTrader>

using namespace NetworkManager;

ConnectionDetailEditor::ConnectionDetailEditor(NetworkManager::Settings::ConnectionSettings::ConnectionType type, QWidget* parent,
                                               const QString &vpnType, const QString &masterUuid, const QString &slaveType, Qt::WindowFlags f):
    QDialog(parent, f),
    m_ui(new Ui::ConnectionDetailEditor),
    m_connection(new NetworkManager::Settings::ConnectionSettings(type)),
    m_numSecrets(0),
    m_new(true),
    m_vpnType(vpnType),
    m_masterUuid(masterUuid),
    m_slaveType(slaveType)
{
    setAttribute(Qt::WA_DeleteOnClose);
    m_ui->setupUi(this);

    initEditor();
}

ConnectionDetailEditor::ConnectionDetailEditor(Settings::ConnectionSettings::ConnectionType type, const QVariantList &args, QWidget *parent, Qt::WindowFlags f):
    QDialog(parent, f),
    m_ui(new Ui::ConnectionDetailEditor),
    m_connection(new NetworkManager::Settings::ConnectionSettings(type)),
    m_numSecrets(0),
    m_new(true)
{
    setAttribute(Qt::WA_DeleteOnClose);
    m_ui->setupUi(this);

    // parse args given from the wizard
    qDebug() << "Editing new mobile connection, number of args:" << args.count();
    foreach(const QVariant & arg, args) {
        qDebug() << "Argument:" << arg;
    }

    if (args.count() == 2) { //GSM or CDMA
        QVariantMap tmp = qdbus_cast<QVariantMap>(args.value(1));

#if 0 // network IDs are not used yet and seem to break the setting
        if (args.count() == 3) { // gsm specific
            QStringList networkIds = args.value(1).toStringList();
            if (!networkIds.isEmpty())
                tmp.insert("network-id", networkIds.first());
        }
#endif

        m_connection->setConnectionType(type);
        m_connection->setId(args.value(0).toString());
        qDebug() << "New " << m_connection->typeAsString(m_connection->connectionType()) << "connection initializing with:" << tmp;
        if (type == Settings::ConnectionSettings::Gsm)
            m_connection->setting(Settings::Setting::Gsm)->fromMap(tmp);
        else if (type == Settings::ConnectionSettings::Cdma)
            m_connection->setting(Settings::Setting::Cdma)->fromMap(tmp);
        else
            qWarning() << Q_FUNC_INFO << "Unhandled setting type";

        qDebug() << "New connection initialized:";
        m_connection->printSetting();
    } else {
        qWarning() << Q_FUNC_INFO << "Unexpected number of args to parse";
    }

    initEditor();
}


ConnectionDetailEditor::ConnectionDetailEditor(const NetworkManager::Settings::ConnectionSettings::Ptr &connection, QWidget* parent, Qt::WindowFlags f):
    QDialog(parent, f),
    m_ui(new Ui::ConnectionDetailEditor),
    m_connection(connection),
    m_numSecrets(0),
    m_new(false)
{
    setAttribute(Qt::WA_DeleteOnClose);
    m_ui->setupUi(this);

    initEditor();
}

ConnectionDetailEditor::~ConnectionDetailEditor()
{
}

void ConnectionDetailEditor::initEditor()
{
    qDBusRegisterMetaType<NMStringMap>();
    qDBusRegisterMetaType<UIntList>();
    qDBusRegisterMetaType<UIntListList>();
    qDBusRegisterMetaType<IpV6DBusAddressList>();
    qDBusRegisterMetaType<IpV6DBusRouteList>();
    qDBusRegisterMetaType<IpV6DBusNameservers>();

    if (!m_new) {
        NetworkManager::Settings::Connection::Ptr connection = NetworkManager::Settings::findConnectionByUuid(m_connection->uuid());
        if (connection) {
            connect(connection.data(), SIGNAL(gotSecrets(QString,bool,NMVariantMapMap,QString)),
                    SLOT(gotSecrets(QString,bool,NMVariantMapMap,QString)), Qt::UniqueConnection);

            switch (m_connection->connectionType()) {
                case Settings::ConnectionSettings::Adsl:
                    connection->secrets("adsl");
                    m_numSecrets = 1;
                    break;
                case Settings::ConnectionSettings::Bluetooth:
                    connection->secrets("gsm");
                    m_numSecrets = 1;
                    break;
                case Settings::ConnectionSettings::Cdma:
                    connection->secrets("cdma");
                    m_numSecrets = 1;
                    break;
                case Settings::ConnectionSettings::Gsm:
                    connection->secrets("gsm");
                    m_numSecrets = 1;
                    break;
                case Settings::ConnectionSettings::Pppoe:
                    connection->secrets("pppoe");
                    m_numSecrets = 1;
                    break;
                case Settings::ConnectionSettings::Wired:
                    connection->secrets("802-1x");
                    m_numSecrets = 1;
                    break;
                case Settings::ConnectionSettings::Wireless:
                    connection->secrets("802-1x");
                    connection->secrets("802-11-wireless-security");
                    m_numSecrets = 2;
                    break;
                case Settings::ConnectionSettings::Vpn:
                    connection->secrets("vpn");
                    m_numSecrets = 1;
                    break;
                default:
                    initTabs();
                    break;
            }
        }
    } else {
        initTabs();
    }

    if (m_connection->id().isEmpty()) {
        setWindowTitle(i18n("New Connection (%1)", m_connection->typeAsString(m_connection->connectionType())));
        m_ui->connectionName->setText(i18n("New %1 connection", m_connection->typeAsString(m_connection->connectionType())));
    } else {
        setWindowTitle(i18n("Edit Connection '%1'", m_connection->id()));
        m_ui->connectionName->setText(m_connection->id());
    }

    connect(this, SIGNAL(accepted()), SLOT(saveSetting()));
    connect(this, SIGNAL(accepted()), SLOT(disconnectSignals()));
    connect(this, SIGNAL(rejected()), SLOT(disconnectSignals()));
}

void ConnectionDetailEditor::initTabs()
{
    ConnectionWidget * connectionWidget = new ConnectionWidget(m_connection);
    m_ui->tabWidget->addTab(connectionWidget, i18n("General"));

    // create/set UUID
    QString uuid = m_connection->uuid();
    if (QUuid(uuid).isNull()) {
        uuid = NetworkManager::Settings::ConnectionSettings::createNewUuid();
        m_connection->setUuid(uuid);
    }
    qDebug() << "Initting tabs, UUID:" << uuid;

    const NetworkManager::Settings::ConnectionSettings::ConnectionType type = m_connection->connectionType();

    /*Adsl, Cdma, Gsm, Infiniband, Pppoe, Vpn, Wired, Wireless, Bluetooth, OlpcMesh, Vlan, Wimax, Bond, Bridge */
    if (type == NetworkManager::Settings::ConnectionSettings::Wired) {
        WiredConnectionWidget * wiredWidget = new WiredConnectionWidget(m_connection->setting(NetworkManager::Settings::Setting::Wired), this);
        m_ui->tabWidget->addTab(wiredWidget, i18n("Wired"));
        WiredSecurity * wiredSecurity = new WiredSecurity(m_connection->setting(NetworkManager::Settings::Setting::Security8021x).staticCast<NetworkManager::Settings::Security8021xSetting>(), this);
        m_ui->tabWidget->addTab(wiredSecurity, i18n("802.1x Security"));
    } else if (type == NetworkManager::Settings::ConnectionSettings::Wireless) {
        WifiConnectionWidget * wifiWidget = new WifiConnectionWidget(m_connection->setting(NetworkManager::Settings::Setting::Wireless), this);
        m_ui->tabWidget->addTab(wifiWidget, i18n("Wireless"));
        WifiSecurity * wifiSecurity = new WifiSecurity(m_connection->setting(NetworkManager::Settings::Setting::WirelessSecurity),
                                                       m_connection->setting(NetworkManager::Settings::Setting::Security8021x).staticCast<NetworkManager::Settings::Security8021xSetting>(),
                                                       this);
        m_ui->tabWidget->addTab(wifiSecurity, i18n("Wi-Fi Security"));
    } else if (type == NetworkManager::Settings::ConnectionSettings::Pppoe) { // DSL
        PppoeWidget * pppoeWidget = new PppoeWidget(m_connection->setting(NetworkManager::Settings::Setting::Pppoe), this);
        m_ui->tabWidget->addTab(pppoeWidget, i18n("DSL"));
        WiredConnectionWidget * wiredWidget = new WiredConnectionWidget(m_connection->setting(NetworkManager::Settings::Setting::Wired), this);
        m_ui->tabWidget->addTab(wiredWidget, i18n("Wired"));
    } else if (type == NetworkManager::Settings::ConnectionSettings::Gsm) { // GSM
        GsmWidget * gsmWidget = new GsmWidget(m_connection->setting(NetworkManager::Settings::Setting::Gsm), this);
        m_ui->tabWidget->addTab(gsmWidget, i18n("Mobile Broadband (%1)", m_connection->typeAsString(m_connection->connectionType())));
    } else if (type == NetworkManager::Settings::ConnectionSettings::Cdma) { // CDMA
        CdmaWidget * cdmaWidget = new CdmaWidget(m_connection->setting(NetworkManager::Settings::Setting::Cdma), this);
        m_ui->tabWidget->addTab(cdmaWidget, i18n("Mobile Broadband (%1)", m_connection->typeAsString(m_connection->connectionType())));
    } else if (type == NetworkManager::Settings::ConnectionSettings::Bluetooth) {  // Bluetooth
        BtWidget * btWidget = new BtWidget(m_connection->setting(NetworkManager::Settings::Setting::Bluetooth), this);
        m_ui->tabWidget->addTab(btWidget, i18n("Bluetooth"));
        NetworkManager::Settings::BluetoothSetting::Ptr btSetting = m_connection->setting(NetworkManager::Settings::Setting::Bluetooth).staticCast<NetworkManager::Settings::BluetoothSetting>();
        if (btSetting->profileType() == NetworkManager::Settings::BluetoothSetting::Dun) {
            GsmWidget * gsmWidget = new GsmWidget(m_connection->setting(NetworkManager::Settings::Setting::Gsm), this);
            m_ui->tabWidget->addTab(gsmWidget, i18n("GSM"));
            PPPWidget * pppWidget = new PPPWidget(m_connection->setting(NetworkManager::Settings::Setting::Ppp), this);
            m_ui->tabWidget->addTab(pppWidget, i18n("PPP"));

        }
    } else if (type == NetworkManager::Settings::ConnectionSettings::Infiniband) { // Infiniband
        InfinibandWidget * infinibandWidget = new InfinibandWidget(m_connection->setting(NetworkManager::Settings::Setting::Infiniband), this);
        m_ui->tabWidget->addTab(infinibandWidget, i18n("Infiniband"));
    } else if (type == NetworkManager::Settings::ConnectionSettings::Bond) { // Bond
        BondWidget * bondWidget = new BondWidget(uuid, m_connection->setting(NetworkManager::Settings::Setting::Bond), this);
        m_ui->tabWidget->addTab(bondWidget, i18n("Bond"));
    } else if (type == NetworkManager::Settings::ConnectionSettings::Vpn) { // VPN
        QString error;
        VpnUiPlugin * vpnPlugin = 0;
        NetworkManager::Settings::VpnSetting::Ptr vpnSetting =
                m_connection->setting(NetworkManager::Settings::Setting::Vpn).staticCast<NetworkManager::Settings::VpnSetting>();
        if (!vpnSetting) {
            qDebug() << "Missing VPN setting!";
        } else {
            QString serviceType;
            if (m_new && !m_vpnType.isEmpty()) {
                serviceType = m_vpnType;
                vpnSetting->setServiceType(serviceType);
            }
            else
                serviceType = vpnSetting->serviceType();
            //qDebug() << "Editor loading VPN plugin" << serviceType;
            //vpnSetting->printSetting();
            vpnPlugin = KServiceTypeTrader::createInstanceFromQuery<VpnUiPlugin>(QString::fromLatin1("PlasmaNM/VpnUiPlugin"),
                                                                                 QString::fromLatin1("[X-NetworkManager-Services]=='%1'").arg(serviceType),
                                                                                 this, QVariantList(), &error);
            if (vpnPlugin && error.isEmpty()) {
                const QString shortName = serviceType.section('.', -1);
                SettingWidget * vpnWidget = vpnPlugin->widget(vpnSetting, this);
                m_ui->tabWidget->addTab(vpnWidget, i18n("VPN (%1)", shortName));
            } else {
                qDebug() << error << ", serviceType == " << serviceType;
            }
        }
    }

    // PPP widget
    if (type == Settings::ConnectionSettings::Pppoe || type == Settings::ConnectionSettings::Cdma || type == Settings::ConnectionSettings::Gsm) {
        PPPWidget * pppWidget = new PPPWidget(m_connection->setting(NetworkManager::Settings::Setting::Ppp), this);
        m_ui->tabWidget->addTab(pppWidget, i18n("PPP"));
    }

    // IPv4 widget
    if (!isSlave()) {
        IPv4Widget * ipv4Widget = new IPv4Widget(m_connection->setting(NetworkManager::Settings::Setting::Ipv4), this);
        m_ui->tabWidget->addTab(ipv4Widget, i18n("IPv4"));
    }

    // IPv6 widget
    if ((type == Settings::ConnectionSettings::Wired || type == Settings::ConnectionSettings::Wireless ||
            type == Settings::ConnectionSettings::Infiniband || type == Settings::ConnectionSettings::Bond) && !isSlave()) {
        IPv6Widget * ipv6Widget = new IPv6Widget(m_connection->setting(NetworkManager::Settings::Setting::Ipv6), this);
        m_ui->tabWidget->addTab(ipv6Widget, i18n("IPv6"));
    }
}

void ConnectionDetailEditor::saveSetting()
{
    ConnectionWidget * connectionWidget = static_cast<ConnectionWidget*>(m_ui->tabWidget->widget(0));

    NMVariantMapMap settings = connectionWidget->setting();

    bool agentOwned = false;
    if (!settings.value("connection").value("permissions").toStringList().isEmpty()) {
        agentOwned = true;
    }

    qDebug() << "agent owned - " << agentOwned;
    for (int i = 1; i < m_ui->tabWidget->count(); ++i) {
        SettingWidget * widget = static_cast<SettingWidget*>(m_ui->tabWidget->widget(i));
        const QString type = widget->type();
        if (type != NetworkManager::Settings::Setting::typeAsString(NetworkManager::Settings::Setting::Security8021x) &&
            type != NetworkManager::Settings::Setting::typeAsString(NetworkManager::Settings::Setting::WirelessSecurity)) {
            settings.insert(type, widget->setting(agentOwned));
        }

        // add 802.1x security if needed
        QVariantMap security8021x;
        if (type == NetworkManager::Settings::Setting::typeAsString(NetworkManager::Settings::Setting::WirelessSecurity)) {
            WifiSecurity * wifiSecurity = static_cast<WifiSecurity*>(widget);
            if (wifiSecurity->enabled()) {
                settings.insert(type, wifiSecurity->setting(agentOwned));
            }
            if (wifiSecurity->enabled8021x()) {
                security8021x = static_cast<WifiSecurity *>(widget)->setting8021x(agentOwned);
                settings.insert(NetworkManager::Settings::Setting::typeAsString(NetworkManager::Settings::Setting::Security8021x), security8021x);
            }
        } else if (type == NetworkManager::Settings::Setting::typeAsString(NetworkManager::Settings::Setting::Security8021x)) {
            WiredSecurity * wiredSecurity = static_cast<WiredSecurity*>(widget);
            if (wiredSecurity->enabled8021x()) {
                security8021x = static_cast<WiredSecurity *>(widget)->setting(agentOwned);
                settings.insert(NetworkManager::Settings::Setting::typeAsString(NetworkManager::Settings::Setting::Security8021x), security8021x);
            }
        }
    }

    NetworkManager::Settings::ConnectionSettings * connectionSettings = new NetworkManager::Settings::ConnectionSettings(m_connection->connectionType());

    connectionSettings->fromMap(settings);
    connectionSettings->setId(m_ui->connectionName->text());

    if (connectionSettings->connectionType() == Settings::ConnectionSettings::Wireless) {
        NetworkManager::Settings::WirelessSecuritySetting::Ptr securitySetting = connectionSettings->setting(Settings::Setting::WirelessSecurity).staticCast<NetworkManager::Settings::WirelessSecuritySetting>();
        NetworkManager::Settings::WirelessSetting::Ptr wirelessSetting = connectionSettings->setting(Settings::Setting::Wireless).staticCast<NetworkManager::Settings::WirelessSetting>();

        if (securitySetting && wirelessSetting) {
            if (securitySetting->keyMgmt() != NetworkManager::Settings::WirelessSecuritySetting::WirelessSecuritySetting::Unknown) {
                wirelessSetting->setSecurity("802-11-wireless-security");
            }
        }
    }

    // set UUID
    connectionSettings->setUuid(m_connection->uuid());

    // set master & slave type
    if (isSlave()) {
        connectionSettings->setMaster(m_masterUuid);
        connectionSettings->setSlaveType(m_slaveType);
    }

    connectionSettings->printSetting();  // debug

    if (m_new) { // create new connection
        connect(NetworkManager::Settings::notifier(), SIGNAL(connectionAddComplete(QString,bool,QString)),
                SLOT(connectionAddComplete(QString,bool,QString)));
        NetworkManager::Settings::addConnection(connectionSettings->toMap());
    } else {  // update existing connection
        NetworkManager::Settings::Connection::Ptr connection = NetworkManager::Settings::findConnectionByUuid(connectionSettings->uuid());

        if (connection) {
            connection->update(connectionSettings->toMap());
        }
    }
}

void ConnectionDetailEditor::connectionAddComplete(const QString& id, bool success, const QString& msg)
{
    qDebug() << id << " - " << success << " - " << msg;
    emit connectionAdded(id, success, msg); // for slave dialogs
}

void ConnectionDetailEditor::gotSecrets(const QString& id, bool success, const NMVariantMapMap& secrets, const QString& msg)
{
    if (id == m_connection->uuid()) {
        m_numSecrets--;
    } else {
        return;
    }

    if (success) {
        foreach (const QString & key, secrets.keys()) {
            NetworkManager::Settings::Setting::Ptr setting = m_connection->setting(NetworkManager::Settings::Setting::typeFromString(key));
            setting->secretsFromMap(secrets.value(key));
        }
    } else {
        qDebug() << msg;
    }

    if (!m_numSecrets) {
        initTabs();
    }
}

void ConnectionDetailEditor::disconnectSignals()
{
    NetworkManager::Settings::Connection::Ptr connection = NetworkManager::Settings::findConnectionByUuid(m_connection->uuid());

    if (connection) {
        disconnect(connection.data(), SIGNAL(gotSecrets(QString,bool,NMVariantMapMap,QString)),
                   this, SLOT(gotSecrets(QString,bool,NMVariantMapMap,QString)));
    }
}
