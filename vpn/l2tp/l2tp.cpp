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

#include "l2tp.h"

#include <KPluginFactory>
#include <KLocale>
#include "nm-l2tp-service.h"

#include <NetworkManagerQt/VpnSetting>

#include "l2tpwidget.h"
#include "l2tpauth.h"

K_PLUGIN_FACTORY(L2tpUiPluginFactory, registerPlugin<L2tpUiPlugin>();)
K_EXPORT_PLUGIN(L2tpUiPluginFactory("plasmanetworkmanagement_l2tpui"))

L2tpUiPlugin::L2tpUiPlugin(QObject * parent, const QVariantList &):
    VpnUiPlugin(parent)
{
}

L2tpUiPlugin::~L2tpUiPlugin()
{
}

SettingWidget * L2tpUiPlugin::widget(const NetworkManager::VpnSetting::Ptr &setting, QWidget * parent)
{
    return new L2tpWidget(setting, parent);
}

SettingWidget *L2tpUiPlugin::askUser(const NetworkManager::VpnSetting::Ptr &setting, QWidget * parent)
{
    return new L2tpAuthDialog(setting, parent);
}

QString L2tpUiPlugin::suggestedFileName(const NetworkManager::ConnectionSettings::Ptr &connection) const
{
    Q_UNUSED(connection);
    return QString();
}

QString L2tpUiPlugin::supportedFileExtensions() const
{
    return QString();
}

NMVariantMapMap L2tpUiPlugin::importConnectionSettings(const QString &fileName)
{
    Q_UNUSED(fileName);
    mError = VpnUiPlugin::NotImplemented;
    return NMVariantMapMap();
}

bool L2tpUiPlugin::exportConnectionSettings(const NetworkManager::ConnectionSettings::Ptr &connection, const QString &fileName)
{
    Q_UNUSED(connection);
    Q_UNUSED(fileName);
    mError = VpnUiPlugin::NotImplemented;
    return false;
}
