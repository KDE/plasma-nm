/*
    SPDX-FileCopyrightText: 2015 Jan Grulich <jgrulich@redhat.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "sstp.h"
#include "sstpauth.h"
#include "sstpwidget.h"

#include <KPluginFactory>

K_PLUGIN_CLASS_WITH_JSON(SstpUiPlugin, "plasmanetworkmanagement_sstpui.json")

SstpUiPlugin::SstpUiPlugin(QObject *parent, const QVariantList &)
    : VpnUiPlugin(parent)
{
}

SstpUiPlugin::~SstpUiPlugin()
{
}

SettingWidget *SstpUiPlugin::widget(const NetworkManager::VpnSetting::Ptr &setting, QWidget *parent)
{
    return new SstpSettingWidget(setting, parent);
}

SettingWidget *SstpUiPlugin::askUser(const NetworkManager::VpnSetting::Ptr &setting, const QStringList &hints, QWidget *parent)
{
    return new SstpAuthWidget(setting, hints, parent);
}

QString SstpUiPlugin::suggestedFileName(const NetworkManager::ConnectionSettings::Ptr &connection) const
{
    Q_UNUSED(connection);

    // TODO : implement suggested file name
    return QString();
}

QString SstpUiPlugin::supportedFileExtensions() const
{
    // TODO : return supported file extensions
    return QString();
}

NMVariantMapMap SstpUiPlugin::importConnectionSettings(const QString &fileName)
{
    Q_UNUSED(fileName);

    // TODO : import the SSTP connection from file and return settings
    mError = VpnUiPlugin::NotImplemented;
    return NMVariantMapMap();
}

bool SstpUiPlugin::exportConnectionSettings(const NetworkManager::ConnectionSettings::Ptr &connection, const QString &fileName)
{
    Q_UNUSED(connection);
    Q_UNUSED(fileName);

    // TODO : export SSTP connection to file
    mError = VpnUiPlugin::NotImplemented;
    return false;
}

#include "sstp.moc"
