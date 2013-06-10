/*
Copyright 2009 Will Stephenson <wstephenson@kde.org>
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

#include "pptp.h"

#include <KPluginFactory>

#include "pptpwidget.h"
#include "pptpauth.h"

K_PLUGIN_FACTORY(PptpUiPluginFactory, registerPlugin<PptpUiPlugin>();)
K_EXPORT_PLUGIN(PptpUiPluginFactory("plasmanm_pptpui"))

PptpUiPlugin::PptpUiPlugin(QObject * parent, const QVariantList &) : VpnUiPlugin(parent)
{

}

PptpUiPlugin::~PptpUiPlugin()
{

}

SettingWidget * PptpUiPlugin::widget(const NetworkManager::VpnSetting::Ptr &setting, QWidget * parent)
{
    return new PptpSettingWidget(setting, parent);
}

SettingWidget * PptpUiPlugin::askUser(const NetworkManager::VpnSetting::Ptr &setting, QWidget * parent)
{
    return new PptpAuthWidget(setting, parent);
}

#if 0
QString PptpUiPlugin::suggestedFileName(Knm::Connection *connection) const
{
    Q_UNUSED(connection);

    // TODO : implement suggested file name
    return QString();
}

QString PptpUiPlugin::supportedFileExtensions() const
{
    // TODO : return supported file extensions
    return QString();
}

QVariantList PptpUiPlugin::importConnectionSettings(const QString &fileName)
{
    Q_UNUSED(fileName);

    // TODO : import the PPTP connection from file and return settings
    mError = VpnUiPlugin::NotImplemented;
    return QVariantList();
}

bool PptpUiPlugin::exportConnectionSettings(Knm::Connection * connection, const QString &fileName)
{
    Q_UNUSED(connection);
    Q_UNUSED(fileName);

    // TODO : export PPTP connection to file
    mError = VpnUiPlugin::NotImplemented;
    return false;
}
#endif
