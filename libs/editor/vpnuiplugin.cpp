/*
    SPDX-FileCopyrightText: 2008 Will Stephenson <wstephenson@kde.org>
    SPDX-FileCopyrightText: 2013 Lukáš Tinkl <ltinkl@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "vpnuiplugin.h"

#include <KLocalizedString>
#include <KPluginFactory>
#include <KPluginLoader>
#include <KPluginMetaData>

VpnUiPlugin::VpnUiPlugin(QObject * parent, const QVariantList & /*args*/):
    QObject(parent)
{
    mError = NoError;
}

VpnUiPlugin::~VpnUiPlugin()
{
}

QMessageBox::StandardButtons VpnUiPlugin::suggestedAuthDialogButtons() const
{
    return QMessageBox::Ok | QMessageBox::Cancel;
}

VpnUiPlugin::ErrorType VpnUiPlugin::lastError() const
{
    return mError;
}

QString VpnUiPlugin::lastErrorMessage()
{
    switch (mError) {
        case NoError:
            mErrorMessage.clear();
            break;
        case NotImplemented:
            return i18nc("Error message in VPN import/export dialog", "Operation not supported for this VPN type.");
            break;
        case Error:
            break;
    }
    return mErrorMessage;
}

VpnUiPlugin::LoadResult VpnUiPlugin::loadPluginForType(QObject *parent, const QString &serviceType)
{
    LoadResult result;

    auto filter = [serviceType](const KPluginMetaData &md) {
        return md.value(QStringLiteral("X-NetworkManager-Services")) == serviceType;
    };

    const QVector<KPluginMetaData> offers = KPluginLoader::findPlugins(QStringLiteral("plasma/network/vpn"), filter);

    if (offers.isEmpty()) {
        result.error = QStringLiteral("No plugin found");
        return result;
    }

    KPluginLoader loader(offers.first().fileName());
    KPluginFactory *factory = loader.factory();
    result.plugin = factory->create<VpnUiPlugin>(parent);

    if (!result.plugin) {
        result.error = loader.errorString();
    }
    return result;
}
