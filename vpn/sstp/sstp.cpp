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

#include "sstp.h"
#include "sstpwidget.h"
#include "sstpauth.h"

#include <KPluginFactory>

K_PLUGIN_FACTORY_WITH_JSON(SstpUiPluginFactory, "plasmanetworkmanagement_sstpui.json", registerPlugin<SstpUiPlugin>();)

SstpUiPlugin::SstpUiPlugin(QObject *parent, const QVariantList &)
    : VpnUiPlugin(parent)
{
}

SstpUiPlugin::~SstpUiPlugin()
{
}

SettingWidget * SstpUiPlugin::widget(const NetworkManager::VpnSetting::Ptr &setting, QWidget *parent)
{
    return new SstpSettingWidget(setting, parent);
}

SettingWidget * SstpUiPlugin::askUser(const NetworkManager::VpnSetting::Ptr &setting, QWidget *parent)
{
    return new SstpAuthWidget(setting, parent);
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
