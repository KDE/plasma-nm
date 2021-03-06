/*
    SPDX-FileCopyrightText: 2011 Ilia Kats <ilia-kats@gmx.net>
    SPDX-FileCopyrightText: 2013 Lukas Tinkl <ltinkl@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "openconnectui.h"

#include <KPluginFactory>

#include "openconnectwidget.h"
#include "openconnectauth.h"

K_PLUGIN_FACTORY(OpenconnectUiPluginFactory, registerPlugin<OpenconnectUiPlugin>();)

OpenconnectUiPlugin::OpenconnectUiPlugin(QObject * parent, const QVariantList &)
    : VpnUiPlugin(parent)
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

#include "openconnectui.moc"
