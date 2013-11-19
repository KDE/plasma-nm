/*
    Copyright 2011 Ilia Kats <ilia-kats@gmx.net>
    Copyright 2013 Lukas Tinkl <ltinkl@redhat.com>

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

#include "openconnectui.h"

#include <KPluginFactory>

#include "openconnectwidget.h"
#include "openconnectauth.h"

K_PLUGIN_FACTORY(OpenconnectUiPluginFactory, registerPlugin<OpenconnectUiPlugin>();)
K_EXPORT_PLUGIN(OpenconnectUiPluginFactory("plasmanetworkmanagement_openconnectui"))

OpenconnectUiPlugin::OpenconnectUiPlugin(QObject * parent, const QVariantList &) : VpnUiPlugin(parent)
{

}

OpenconnectUiPlugin::~OpenconnectUiPlugin()
{

}

SettingWidget * OpenconnectUiPlugin::widget(const NetworkManager::VpnSetting::Ptr &setting, QWidget * parent)
{
    return new OpenconnectSettingWidget(setting, parent);
}

SettingWidget * OpenconnectUiPlugin::askUser(const NetworkManager::VpnSetting::Ptr &setting, QWidget * parent)
{
    return new OpenconnectAuthWidget(setting, parent);
}

QString OpenconnectUiPlugin::suggestedFileName(const NetworkManager::ConnectionSettings::Ptr &connection) const
{
    Q_UNUSED(connection);
    return QString();
}

QString OpenconnectUiPlugin::supportedFileExtensions() const
{
    return QString();
}

QMessageBox::StandardButtons OpenconnectUiPlugin::suggestedAuthDialogButtons() const
{
    return QMessageBox::Close;
}

NMVariantMapMap OpenconnectUiPlugin::importConnectionSettings(const QString &fileName)
{
    Q_UNUSED(fileName);

    // TODO : import the Openconnect connection from file and return settings
    mError = VpnUiPlugin::NotImplemented;
    return NMVariantMapMap();
}

bool OpenconnectUiPlugin::exportConnectionSettings(const NetworkManager::ConnectionSettings::Ptr &connection, const QString &fileName)
{
    Q_UNUSED(connection);
    Q_UNUSED(fileName);

    // TODO : export Openconnect connection to file
    mError = VpnUiPlugin::NotImplemented;
    return false;
}
