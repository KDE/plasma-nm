/*
    Copyright 2013 Jan Grulich <jgrulich@redhat.com>

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

#include <QtNetworkManager/settings.h>
#include <QtNetworkManager/activeconnection.h>
#include <QtNetworkManager/connection.h>

using namespace NetworkManager;

ConnectionDetailEditor::ConnectionDetailEditor(Settings::ConnectionSettings::ConnectionType type, QWidget* parent, Qt::WindowFlags f):
    QDialog(parent, f),
    m_detailEditor(new Ui::ConnectionDetailEditor),
    m_connection(new NetworkManager::Settings::ConnectionSettings(type)),
    m_new(true)
{
    m_detailEditor->setupUi(this);

    initEditor();
}


ConnectionDetailEditor::ConnectionDetailEditor(Settings::ConnectionSettings* connection, QWidget* parent, Qt::WindowFlags f):
    QDialog(parent, f),
    m_detailEditor(new Ui::ConnectionDetailEditor),
    m_connection(connection),
    m_new(false)
{
    m_detailEditor->setupUi(this);

    initEditor();
}

ConnectionDetailEditor::~ConnectionDetailEditor()
{
}

void ConnectionDetailEditor::initEditor()
{
    initTabs();

    if (m_connection->id().isEmpty()) {
        setWindowTitle(i18n("New Connection (%1)", m_connection->typeAsString(m_connection->connectionType())));
        m_detailEditor->connectionName->setText(i18n("New %1 connection", m_connection->typeAsString(m_connection->connectionType())));
    } else {
        setWindowTitle(i18n("Edit Connection '%1'", m_connection->id()));
        m_detailEditor->connectionName->setText(m_connection->id());
    }

    connect(this, SIGNAL(accepted()), SLOT(saveSetting()));
}

void ConnectionDetailEditor::initTabs()
{
    ConnectionWidget * connectionWidget = new ConnectionWidget(m_connection);
    m_detailEditor->tabWidget->addTab(connectionWidget, i18n("General"));

    const NetworkManager::Settings::ConnectionSettings::ConnectionType type = m_connection->connectionType();

    /*Adsl, Cdma, Gsm, Infiniband, Ipv4, Ipv6, Ppp, Pppoe, Security8021x, Serial,
      Vpn, Wired, Wireless, WirelessSecurity, Bluetooth, OlpcMesh, Vlan, Wimax, Bond, Bridge, BridgePort;*/
    if (type == NetworkManager::Settings::ConnectionSettings::Wired) {
        WiredConnectionWidget * wiredWidget = new WiredConnectionWidget(m_connection->setting(NetworkManager::Settings::Setting::Wired), this);
        m_detailEditor->tabWidget->addTab(wiredWidget, i18n("Wired"));
        WiredSecurity * wiredSecurity = new WiredSecurity(static_cast<NetworkManager::Settings::Security8021xSetting *>(m_connection->setting(NetworkManager::Settings::Setting::Security8021x)), this);
        m_detailEditor->tabWidget->addTab(wiredSecurity, i18n("802.1x Security"));
        IPv4Widget * ipv4Widget = new IPv4Widget(m_connection->setting(NetworkManager::Settings::Setting::Ipv4), this);
        m_detailEditor->tabWidget->addTab(ipv4Widget, i18n("IPv4"));
        IPv6Widget * ipv6Widget = new IPv6Widget(m_connection->setting(NetworkManager::Settings::Setting::Ipv6), this);
        m_detailEditor->tabWidget->addTab(ipv6Widget, i18n("IPv6"));
    } else if (type == NetworkManager::Settings::ConnectionSettings::Wireless) {
        WifiConnectionWidget * wifiWidget = new WifiConnectionWidget(m_connection->setting(NetworkManager::Settings::Setting::Wireless), this);
        m_detailEditor->tabWidget->addTab(wifiWidget, i18n("Wireless"));
        WifiSecurity * wifiSecurity = new WifiSecurity(m_connection->setting(NetworkManager::Settings::Setting::WirelessSecurity),
                                                       static_cast<NetworkManager::Settings::Security8021xSetting *>(m_connection->setting(NetworkManager::Settings::Setting::Security8021x)),
                                                       this);
        m_detailEditor->tabWidget->addTab(wifiSecurity, i18n("Wi-Fi Security"));
        IPv4Widget * ipv4Widget = new IPv4Widget(m_connection->setting(NetworkManager::Settings::Setting::Ipv4), this);
        m_detailEditor->tabWidget->addTab(ipv4Widget, i18n("IPv4"));
        IPv6Widget * ipv6Widget = new IPv6Widget(m_connection->setting(NetworkManager::Settings::Setting::Ipv6), this);
        m_detailEditor->tabWidget->addTab(ipv6Widget, i18n("IPv6"));
    } else if (type == NetworkManager::Settings::ConnectionSettings::Pppoe) { // DSL
        PppoeWidget * pppoeWidget = new PppoeWidget(m_connection->setting(NetworkManager::Settings::Setting::Pppoe), this);
        m_detailEditor->tabWidget->addTab(pppoeWidget, i18n("DSL"));
        WiredConnectionWidget * wiredWidget = new WiredConnectionWidget(m_connection->setting(NetworkManager::Settings::Setting::Wired), this);
        m_detailEditor->tabWidget->addTab(wiredWidget, i18n("Wired"));
        PPPWidget * pppWidget = new PPPWidget(m_connection->setting(NetworkManager::Settings::Setting::Ppp), this);
        m_detailEditor->tabWidget->addTab(pppWidget, i18n("PPP"));
        IPv4Widget * ipv4Widget = new IPv4Widget(m_connection->setting(NetworkManager::Settings::Setting::Ipv4), this);
        m_detailEditor->tabWidget->addTab(ipv4Widget, i18n("IPv4"));
    } else if (type == NetworkManager::Settings::ConnectionSettings::Gsm) { // GSM
        GsmWidget * gsmWidget = new GsmWidget(m_connection->setting(NetworkManager::Settings::Setting::Gsm), this);
        m_detailEditor->tabWidget->addTab(gsmWidget, i18n("Mobile Broadband"));
        PPPWidget * pppWidget = new PPPWidget(m_connection->setting(NetworkManager::Settings::Setting::Ppp), this);
        m_detailEditor->tabWidget->addTab(pppWidget, i18n("PPP"));
        IPv4Widget * ipv4Widget = new IPv4Widget(m_connection->setting(NetworkManager::Settings::Setting::Ipv4), this);
        m_detailEditor->tabWidget->addTab(ipv4Widget, i18n("IPv4"));
    } else if (type == NetworkManager::Settings::ConnectionSettings::Cdma) { // CDMA
        CdmaWidget * cdmaWidget = new CdmaWidget(m_connection->setting(NetworkManager::Settings::Setting::Cdma), this);
        m_detailEditor->tabWidget->addTab(cdmaWidget, i18n("Mobile Broadband"));
        PPPWidget * pppWidget = new PPPWidget(m_connection->setting(NetworkManager::Settings::Setting::Ppp), this);
        m_detailEditor->tabWidget->addTab(pppWidget, i18n("PPP"));
        IPv4Widget * ipv4Widget = new IPv4Widget(m_connection->setting(NetworkManager::Settings::Setting::Ipv4), this);
        m_detailEditor->tabWidget->addTab(ipv4Widget, i18n("IPv4"));
    }
}

void ConnectionDetailEditor::saveSetting()
{
    ConnectionWidget * connectionWidget = static_cast<ConnectionWidget*>(m_detailEditor->tabWidget->widget(0));

    QVariantMapMap settings = connectionWidget->setting();

    for (int i = 1; i < m_detailEditor->tabWidget->count(); ++i) {
        SettingWidget * widget = static_cast<SettingWidget*>(m_detailEditor->tabWidget->widget(i));
        settings.insert(widget->type(), widget->setting());
    }

    m_connection->fromMap(settings);
    m_connection->setId(m_detailEditor->connectionName->text());

    if (m_new) {
        m_connection->setUuid(QUuid::createUuid().toString().mid(1, QUuid::createUuid().toString().length() - 2));
    }

    m_connection->printSetting();
    if (m_new) {
        connect(NetworkManager::Settings::notifier(), SIGNAL(connectionAddComplete(QString,bool,QString)),
                SLOT(connectionAddComplete(QString,bool,QString)));
        qDebug() << NetworkManager::Settings::addConnection(m_connection->toMap());
    } else {
        NetworkManager::Settings::Connection * connection = NetworkManager::Settings::findConnectionByUuid(m_connection->uuid());

        if (connection) {
            connection->update(m_connection->toMap());
        }
    }
}

void ConnectionDetailEditor::connectionAddComplete(const QString& id, bool success, const QString& msg)
{
    qDebug() << id << " - " << success << " - " << msg;
}
