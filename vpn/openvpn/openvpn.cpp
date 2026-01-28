/*
    SPDX-FileCopyrightText: 2008 Will Stephenson <wstephenson@kde.org>
    SPDX-FileCopyrightText: 2011-2012 Rajeesh K Nambiar <rajeeshknambiar@gmail.com>
    SPDX-FileCopyrightText: 2011 Ilia Kats <ilia-kats@gmx.net>
    SPDX-FileCopyrightText: 2012-2016 Lamarque V. Souza <lamarque@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "openvpn.h"

#include <QLatin1Char>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QStringBuilder>

#include <KLocalizedString>
#include <KMessageBox>
#include <KPluginFactory>
#include <kwidgetsaddons_version.h>

#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/Ipv4Setting>
#include <NetworkManagerQt/VpnSetting>

#include "openvpnauth.h"
#include "openvpnwidget.h"

#include <arpa/inet.h>

K_PLUGIN_CLASS_WITH_JSON(OpenVpnUiPlugin, "plasmanetworkmanagement_openvpnui.json")

OpenVpnUiPlugin::OpenVpnUiPlugin(QObject *parent, const QVariantList &)
    : VpnUiPlugin(parent)
{
}

OpenVpnUiPlugin::~OpenVpnUiPlugin() = default;

SettingWidget *OpenVpnUiPlugin::widget(const NetworkManager::VpnSetting::Ptr &setting, QWidget *parent)
{
    auto wid = new OpenVpnSettingWidget(setting, parent);
    return wid;
}

SettingWidget *OpenVpnUiPlugin::askUser(const NetworkManager::VpnSetting::Ptr &setting, const QStringList &hints, QWidget *parent)
{
    return new OpenVpnAuthWidget(setting, hints, parent);
}

QString OpenVpnUiPlugin::suggestedFileName(const NetworkManager::ConnectionSettings::Ptr &connection) const
{
    return connection->id() + QStringLiteral("_openvpn.conf");
}

QStringList OpenVpnUiPlugin::supportedFileExtensions() const
{
    return {QStringLiteral("*.ovpn"), QStringLiteral("*.conf")};
}

VpnUiPlugin::ImportResult OpenVpnUiPlugin::importConnectionSettings(const QString &fileName)
{
    GError *error = nullptr;

    GSList *plugins = nm_vpn_plugin_info_list_load();

    NMVpnPluginInfo *plugin_info = nm_vpn_plugin_info_list_find_by_service(plugins, "org.freedesktop.NetworkManager.openvpn");

    if (!plugin_info) {
        return VpnUiPlugin::ImportResult::fail(i18n("NetworkManager is missing support for OpenVPN"));
    }

    NMVpnEditorPlugin *plugin = nm_vpn_plugin_info_load_editor_plugin(plugin_info, &error);

    NMConnection *connection = nm_vpn_editor_plugin_import(plugin, fileName.toUtf8().constData(), &error);

    if (!connection) {
        const QString errorMessage = QString::fromUtf8(error->message);
        g_error_free(error);

        return VpnUiPlugin::ImportResult::fail(errorMessage);
    }

    return VpnUiPlugin::ImportResult::pass(connection);
}

VpnUiPlugin::ExportResult OpenVpnUiPlugin::exportConnectionSettings(const NetworkManager::ConnectionSettings::Ptr &connection, const QString &fileName)
{
    QFile expFile(fileName);
    if (!expFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return VpnUiPlugin::ExportResult::fail("Could not open file for writing");
    }

    NMClient *client = nm_client_new(NULL, NULL);
    const GPtrArray *connections = nm_client_get_connections(client);
    NMConnection *found = NULL;

    for (uint i = 0; i < connections->len; i++) {
        auto conn = static_cast<NMConnection *>(g_ptr_array_index(connections, i));
        if (nm_connection_get_id(conn) == connection->id()) {
            found = conn;
            break;
        }
    }

    if (!found) {
        qWarning() << "Could not find connection to export";
        return VpnUiPlugin::ExportResult::fail(i18n("An unexpected error has occurred"));
    }

    GError *error = nullptr;
    GSList *plugins = nm_vpn_plugin_info_list_load();
    NMVpnPluginInfo *plugin_info = nm_vpn_plugin_info_list_find_by_service(plugins, "org.freedesktop.NetworkManager.openvpn");
    if (!plugin_info) {
        return VpnUiPlugin::ExportResult::fail(i18n("NetworkManager is missing support for OpenVPN"));
    }

    NMVpnEditorPlugin *plugin = nm_vpn_plugin_info_load_editor_plugin(plugin_info, &error);

    nm_vpn_editor_plugin_export(plugin, fileName.toUtf8().constData(), found, &error);
    if (!connection) {
        const QString errorMessage = QString::fromUtf8(error->message);
        g_error_free(error);

        return VpnUiPlugin::ExportResult::fail(errorMessage);
    }

    return VpnUiPlugin::ExportResult::pass();
}

#include "openvpn.moc"

#include "moc_openvpn.cpp"
