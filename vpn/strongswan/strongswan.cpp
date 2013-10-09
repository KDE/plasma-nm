/*
Copyright 2009 Will Stephenson <wstephenson@kde.org>
Copyright 2010 Maurus Rohrer <maurus.rohrer@gmail.com>
Copyright 2013 Lukas Tinkl <ltinkl@redhat.com>

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

#include "strongswan.h"
#include "strongswanwidget.h"
#include "strongswanauth.h"

#include <KPluginFactory>

K_PLUGIN_FACTORY(StrongswanUiPluginFactory, registerPlugin<StrongswanUiPlugin>();)

StrongswanUiPlugin::StrongswanUiPlugin(QObject * parent, const QVariantList &)
    : VpnUiPlugin(parent)
{
}

StrongswanUiPlugin::~StrongswanUiPlugin()
{
}

SettingWidget * StrongswanUiPlugin::widget(const NetworkManager::VpnSetting::Ptr &setting, QWidget * parent)
{
    return new StrongswanSettingWidget(setting, parent);
}

SettingWidget * StrongswanUiPlugin::askUser(const NetworkManager::VpnSetting::Ptr &setting, QWidget * parent)
{
    return new StrongswanAuthWidget(setting, parent);
}

QString StrongswanUiPlugin::suggestedFileName(const NetworkManager::ConnectionSettings::Ptr &connection) const
{
    Q_UNUSED(connection);

    // TODO : implement suggested file name
    return QString();
}

QString StrongswanUiPlugin::supportedFileExtensions() const
{
    // TODO : return supported file extensions
    return QString();
}

NMVariantMapMap StrongswanUiPlugin::importConnectionSettings(const QString &fileName)
{
    Q_UNUSED(fileName);

    // TODO : import the StrongSwan connection from file and return settings
    mError = VpnUiPlugin::NotImplemented;
    return NMVariantMapMap();
}

bool StrongswanUiPlugin::exportConnectionSettings(const NetworkManager::ConnectionSettings::Ptr &connection, const QString &fileName)
{
    Q_UNUSED(connection);
    Q_UNUSED(fileName);

    // TODO : export StrongSwan connection to file
    mError = VpnUiPlugin::NotImplemented;
    return false;
}

#include "strongswan.moc"

// vim: sw=4 sts=4 et tw=100
