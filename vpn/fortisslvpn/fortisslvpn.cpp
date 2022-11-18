/*
    SPDX-FileCopyrightText: 2016 Jan Grulich <jgrulich@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "fortisslvpn.h"
#include "fortisslvpnauth.h"
#include "fortisslvpnwidget.h"

#include <KPluginFactory>

K_PLUGIN_CLASS_WITH_JSON(FortisslvpnUiPlugin, "plasmanetworkmanagement_fortisslvpnui.json")

FortisslvpnUiPlugin::FortisslvpnUiPlugin(QObject *parent, const QVariantList &)
    : VpnUiPlugin(parent)
{
}

FortisslvpnUiPlugin::~FortisslvpnUiPlugin() = default;

SettingWidget *FortisslvpnUiPlugin::widget(const NetworkManager::VpnSetting::Ptr &setting, QWidget *parent)
{
    return new FortisslvpnWidget(setting, parent);
}

SettingWidget *FortisslvpnUiPlugin::askUser(const NetworkManager::VpnSetting::Ptr &setting, const QStringList &hints, QWidget *parent)
{
    return new FortisslvpnAuthDialog(setting, hints, parent);
}

QString FortisslvpnUiPlugin::suggestedFileName(const NetworkManager::ConnectionSettings::Ptr &connection) const
{
    Q_UNUSED(connection);
    return {};
}

NMVariantMapMap FortisslvpnUiPlugin::importConnectionSettings(const QString &fileName)
{
    Q_UNUSED(fileName);

    // TODO : import the Fortisslvpn connection from file and return settings
    mError = VpnUiPlugin::NotImplemented;
    return {};
}

bool FortisslvpnUiPlugin::exportConnectionSettings(const NetworkManager::ConnectionSettings::Ptr &connection, const QString &fileName)
{
    Q_UNUSED(connection);
    Q_UNUSED(fileName);

    // TODO : export Fortisslvpn connection to file
    mError = VpnUiPlugin::NotImplemented;
    return false;
}

#include "fortisslvpn.moc"
