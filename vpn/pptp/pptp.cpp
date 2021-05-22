/*
    SPDX-FileCopyrightText: 2009 Will Stephenson <wstephenson@kde.org>
    SPDX-FileCopyrightText: 2013 Lukas Tinkl <ltinkl@redhat.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "pptp.h"

#include <KPluginFactory>

#include "pptpwidget.h"
#include "pptpauth.h"

K_PLUGIN_CLASS_WITH_JSON(PptpUiPlugin, "plasmanetworkmanagement_pptpui.json")

PptpUiPlugin::PptpUiPlugin(QObject * parent, const QVariantList &)
    : VpnUiPlugin(parent)
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

QString PptpUiPlugin::suggestedFileName(const NetworkManager::ConnectionSettings::Ptr &connection) const
{
    Q_UNUSED(connection);
    return QString();
}

QString PptpUiPlugin::supportedFileExtensions() const
{
    return QString();
}

NMVariantMapMap PptpUiPlugin::importConnectionSettings(const QString &fileName)
{
    Q_UNUSED(fileName);

    // TODO : import the Openconnect connection from file and return settings
    mError = VpnUiPlugin::NotImplemented;
    return NMVariantMapMap();
}

bool PptpUiPlugin::exportConnectionSettings(const NetworkManager::ConnectionSettings::Ptr &connection, const QString &fileName)
{
    Q_UNUSED(connection);
    Q_UNUSED(fileName);

    // TODO : export Openconnect connection to file
    mError = VpnUiPlugin::NotImplemented;
    return false;
}

#include "pptp.moc"
