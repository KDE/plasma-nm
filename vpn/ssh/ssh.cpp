/*
    Copyright 2015 Jan Grulich <jgrulich@redhat.com>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of
    the License or (at your option) version 3 or any later version
    accepted by the membership of KDE e.V. (or its successor approved
    by the membership of KDE e.V.), which shall act as a proxy
    defined in Section 14 of version 3 of the license.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "ssh.h"
#include "sshwidget.h"
#include "sshauth.h"

#include <KPluginFactory>

K_PLUGIN_CLASS_WITH_JSON(SshUiPlugin, "plasmanetworkmanagement_sshui.json")

SshUiPlugin::SshUiPlugin(QObject * parent, const QVariantList &)
    : VpnUiPlugin(parent)
{
}

SshUiPlugin::~SshUiPlugin()
{
}

SettingWidget * SshUiPlugin::widget(const NetworkManager::VpnSetting::Ptr &setting, QWidget * parent)
{
    return new SshSettingWidget(setting, parent);
}

SettingWidget * SshUiPlugin::askUser(const NetworkManager::VpnSetting::Ptr &setting, QWidget * parent)
{
    return new SshAuthWidget(setting, parent);
}

QString SshUiPlugin::suggestedFileName(const NetworkManager::ConnectionSettings::Ptr &connection) const
{
    Q_UNUSED(connection);

    // TODO : implement suggested file name
    return QString();
}

QString SshUiPlugin::supportedFileExtensions() const
{
    // TODO : return supported file extensions
    return QString();
}

NMVariantMapMap SshUiPlugin::importConnectionSettings(const QString &fileName)
{
    Q_UNUSED(fileName);

    // TODO : import the SSH connection from file and return settings
    mError = VpnUiPlugin::NotImplemented;
    return NMVariantMapMap();
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
