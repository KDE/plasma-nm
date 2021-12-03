/*
    SPDX-FileCopyrightText: 2015 Jan Grulich <jgrulich@redhat.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "ssh.h"
#include "sshauth.h"
#include "sshwidget.h"

#include <KPluginFactory>

K_PLUGIN_CLASS_WITH_JSON(SshUiPlugin, "plasmanetworkmanagement_sshui.json")

SshUiPlugin::SshUiPlugin(QObject *parent, const QVariantList &)
    : VpnUiPlugin(parent)
{
}

SshUiPlugin::~SshUiPlugin() = default;

SettingWidget *SshUiPlugin::widget(const NetworkManager::VpnSetting::Ptr &setting, QWidget *parent)
{
    return new SshSettingWidget(setting, parent);
}

SettingWidget *SshUiPlugin::askUser(const NetworkManager::VpnSetting::Ptr &setting, const QStringList &hints, QWidget *parent)
{
    return new SshAuthWidget(setting, hints, parent);
}

QString SshUiPlugin::suggestedFileName(const NetworkManager::ConnectionSettings::Ptr &connection) const
{
    Q_UNUSED(connection);

    // TODO : implement suggested file name
    return {};
}

QString SshUiPlugin::supportedFileExtensions() const
{
    // TODO : return supported file extensions
    return {};
}

NMVariantMapMap SshUiPlugin::importConnectionSettings(const QString &fileName)
{
    Q_UNUSED(fileName);

    // TODO : import the SSH connection from file and return settings
    mError = VpnUiPlugin::NotImplemented;
    return {};
}

bool SshUiPlugin::exportConnectionSettings(const NetworkManager::ConnectionSettings::Ptr &connection, const QString &fileName)
{
    Q_UNUSED(connection);
    Q_UNUSED(fileName);

    // TODO : export SSH connection to file
    mError = VpnUiPlugin::NotImplemented;
    return false;
}

#include "ssh.moc"
