/*
    SPDX-FileCopyrightText: 2013 Jan Grulich <jgrulich@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "openswan.h"

#include <KPluginFactory>

#include "openswanauth.h"
#include "openswanwidget.h"

K_PLUGIN_CLASS_WITH_JSON(OpenswanUiPlugin, "plasmanetworkmanagement_openswanui.json")

OpenswanUiPlugin::OpenswanUiPlugin(QObject *parent, const QVariantList &)
    : VpnUiPlugin(parent)
{
}

OpenswanUiPlugin::~OpenswanUiPlugin() = default;

SettingWidget *OpenswanUiPlugin::widget(const NetworkManager::VpnSetting::Ptr &setting, QWidget *parent)
{
    return new OpenswanWidget(setting, parent);
}

SettingWidget *OpenswanUiPlugin::askUser(const NetworkManager::VpnSetting::Ptr &setting, const QStringList &hints, QWidget *parent)
{
    return new OpenswanAuthDialog(setting, hints, parent);
}

QString OpenswanUiPlugin::suggestedFileName(const NetworkManager::ConnectionSettings::Ptr &connection) const
{
    Q_UNUSED(connection);
    return {};
}

QString OpenswanUiPlugin::supportedFileExtensions() const
{
    return {};
}

NMVariantMapMap OpenswanUiPlugin::importConnectionSettings(const QString &fileName)
{
    Q_UNUSED(fileName);

    // TODO : import the Openswan connection from file and return settings
    mError = VpnUiPlugin::NotImplemented;
    return {};
}

bool OpenswanUiPlugin::exportConnectionSettings(const NetworkManager::ConnectionSettings::Ptr &connection, const QString &fileName)
{
    Q_UNUSED(connection);
    Q_UNUSED(fileName);

    // TODO : export Openswan connection to file
    mError = VpnUiPlugin::NotImplemented;
    return false;
}

#include "openswan.moc"
