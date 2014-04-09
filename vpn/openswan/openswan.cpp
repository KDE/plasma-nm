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

#include "openswan.h"

#include <KPluginFactory>

#include "openswanwidget.h"
#include "openswanauth.h"

#define TRANSLATION_DOMAIN "plasmanetworkmanagement_openswanui"
#include <KLocalizedString>

K_PLUGIN_FACTORY_WITH_JSON(OpenswanUiPluginFactory, "plasmanetworkmanagement_openswanui.json", registerPlugin<OpenswanUiPlugin>(); )

OpenswanUiPlugin::OpenswanUiPlugin(QObject * parent, const QVariantList &):
    VpnUiPlugin(parent)
{
}

OpenswanUiPlugin::~OpenswanUiPlugin()
{
}

SettingWidget * OpenswanUiPlugin::widget(const NetworkManager::VpnSetting::Ptr &setting, QWidget * parent)
{
    return new OpenswanWidget(setting, parent);
}

SettingWidget * OpenswanUiPlugin::askUser(const NetworkManager::VpnSetting::Ptr &setting, QWidget * parent)
{
    return new OpenswanAuthDialog(setting, parent);
}

QString OpenswanUiPlugin::suggestedFileName(const NetworkManager::ConnectionSettings::Ptr &connection) const
{
    Q_UNUSED(connection);
    return QString();
}

QString OpenswanUiPlugin::supportedFileExtensions() const
{
    return QString();
}

NMVariantMapMap OpenswanUiPlugin::importConnectionSettings(const QString &fileName)
{
    Q_UNUSED(fileName);

    // TODO : import the Openconnect connection from file and return settings
    mError = VpnUiPlugin::NotImplemented;
    return NMVariantMapMap();
}

bool OpenswanUiPlugin::exportConnectionSettings(const NetworkManager::ConnectionSettings::Ptr &connection, const QString &fileName)
{
    Q_UNUSED(connection);
    Q_UNUSED(fileName);

    // TODO : export Openconnect connection to file
    mError = VpnUiPlugin::NotImplemented;
    return false;
}

#include "openswan.moc"
