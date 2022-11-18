/*
    SPDX-FileCopyrightText: 2013 Jan Grulich <jgrulich@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "libreswan.h"

#include <KPluginFactory>

#include "libreswanauth.h"
#include "libreswanwidget.h"

K_PLUGIN_CLASS_WITH_JSON(LibreswanUiPlugin, "plasmanetworkmanagement_libreswanui.json")

LibreswanUiPlugin::LibreswanUiPlugin(QObject *parent, const QVariantList &)
    : VpnUiPlugin(parent)
{
}

LibreswanUiPlugin::~LibreswanUiPlugin() = default;

SettingWidget *LibreswanUiPlugin::widget(const NetworkManager::VpnSetting::Ptr &setting, QWidget *parent)
{
    return new LibreswanWidget(setting, parent);
}

SettingWidget *LibreswanUiPlugin::askUser(const NetworkManager::VpnSetting::Ptr &setting, const QStringList &hints, QWidget *parent)
{
    return new LibreswanAuthDialog(setting, hints, parent);
}

QString LibreswanUiPlugin::suggestedFileName(const NetworkManager::ConnectionSettings::Ptr &connection) const
{
    Q_UNUSED(connection);
    return {};
}

NMVariantMapMap LibreswanUiPlugin::importConnectionSettings(const QString &fileName)
{
    Q_UNUSED(fileName);

    // TODO : import the Libreswan connection from file and return settings
    mError = VpnUiPlugin::NotImplemented;
    return {};
}

bool LibreswanUiPlugin::exportConnectionSettings(const NetworkManager::ConnectionSettings::Ptr &connection, const QString &fileName)
{
    Q_UNUSED(connection);
    Q_UNUSED(fileName);

    // TODO : export Libreswan connection to file
    mError = VpnUiPlugin::NotImplemented;
    return false;
}

#include "libreswan.moc"
