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

#include <QtNetworkManager/settings.h>
#include <QtNetworkManager/connection.h>
#include <QtNetworkManager/settings/connection.h>

#include "connectionwidget.h"
#include "ui_connectionwidget.h"
#include <KUser>

ConnectionWidget::ConnectionWidget(const NetworkManager::Settings::ConnectionSettings::Ptr &settings, QWidget* parent, Qt::WindowFlags f):
    QWidget(parent, f),
    m_widget(new Ui::ConnectionWidget),
    m_type(settings->connectionType())
{
    m_widget->setupUi(this);

    m_widget->firewallZone->addItems(firewallZones());

    // VPN combo
    populateVpnConnections();
    if (settings->connectionType() == NetworkManager::Settings::ConnectionSettings::Vpn) {
        m_widget->autoconnectVpn->setEnabled(false);
        m_widget->vpnCombobox->setEnabled(false);
        m_widget->autoconnect->setEnabled(false);
    } else {
        m_widget->autoconnectVpn->setEnabled(true);
        m_widget->vpnCombobox->setEnabled(true);
        m_widget->autoconnect->setEnabled(true);
    }

    if (settings)
        loadConfig(settings);
}

ConnectionWidget::~ConnectionWidget()
{
}

void ConnectionWidget::loadConfig(const NetworkManager::Settings::ConnectionSettings::Ptr &settings)
{
    if (settings->permissions().isEmpty()) {
        m_widget->allUsers->setChecked(true);
    } else {
        m_widget->allUsers->setChecked(false);
    }

    const QString zone = settings->zone();
    m_widget->firewallZone->setCurrentIndex(m_widget->firewallZone->findText(zone));

    const QStringList secondaries = settings->secondaries();
    const QStringList vpnKeys = vpnConnections().keys();
    if (!secondaries.isEmpty() && !vpnKeys.isEmpty()) {
        foreach (const QString & vpnKey, vpnKeys) {
            if (secondaries.contains(vpnKey)) {
                m_widget->vpnCombobox->setCurrentIndex(m_widget->vpnCombobox->findData(vpnKey));
                m_widget->autoconnectVpn->setChecked(true);
                break;
            }
        }
    } else {
        m_widget->autoconnectVpn->setChecked(false);
    }

    m_widget->autoconnect->setChecked(settings->autoconnect());
}

NMVariantMapMap ConnectionWidget::setting() const
{
    NetworkManager::Settings::ConnectionSettings settings;

    settings.setConnectionType(m_type);
    settings.setAutoconnect(m_widget->autoconnect->isChecked());

    if (m_widget->allUsers->isChecked()) {
        settings.setPermissions(QHash<QString, QString>());
    } else {
        settings.addToPermissions(KUser().loginName(), QString());
    }

    if (m_widget->autoconnectVpn->isChecked() && m_widget->vpnCombobox->count() > 0) {
        settings.setSecondaries(QStringList() << m_widget->vpnCombobox->itemData(m_widget->vpnCombobox->currentIndex()).toString());
    }

    const QString zone = m_widget->firewallZone->currentText();
    if (!zone.isEmpty()) {
        settings.setZone(zone);
    }

    return settings.toMap();
}

NMStringMap ConnectionWidget::vpnConnections() const
{
    NetworkManager::Settings::Connection::List list = NetworkManager::Settings::listConnections();
    NMStringMap result;

    foreach (const NetworkManager::Settings::Connection::Ptr & conn, list) {
        NetworkManager::Settings::ConnectionSettings::Ptr conSet = conn->settings();
        if (conSet->connectionType() == NetworkManager::Settings::ConnectionSettings::Vpn) {
            //qDebug() << "Found VPN" << conSet->id() << conSet->uuid();
            result.insert(conSet->uuid(), conSet->id());
        }
    }

    return result;
}

QStringList ConnectionWidget::firewallZones() const
{
    QDBusMessage msg = QDBusMessage::createMethodCall("org.fedoraproject.FirewallD1", "/org/fedoraproject/FirewallD1", "org.fedoraproject.FirewallD1.zone",
                                                      "getZones");
    QDBusPendingReply<QStringList> reply = QDBusConnection::systemBus().asyncCall(msg);
    reply.waitForFinished();
    if (reply.isValid())
        return reply.value();

    return QStringList();
}

void ConnectionWidget::populateVpnConnections()
{
    QMapIterator<QString,QString> it(vpnConnections());
    while (it.hasNext()) {
        it.next();
        m_widget->vpnCombobox->addItem(it.value(), it.key());
    }
}
