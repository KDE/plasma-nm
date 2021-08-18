/*
    SPDX-FileCopyrightText: 2008 Will Stephenson <wstephenson@kde.org>
    SPDX-FileCopyrightText: 2013 Lukáš Tinkl <ltinkl@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "vpnuiplugin.h"

#include <KLocalizedString>
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

KPluginFactory::Result<VpnUiPlugin> VpnUiPlugin::loadPluginForType(QObject *parent, const QString &serviceType)
{
    LoadResult result;

    auto filter = [serviceType](const KPluginMetaData &md) {
        return md.value(QStringLiteral("X-NetworkManager-Services")) == serviceType;
    };

    const QVector<KPluginMetaData> offers = KPluginMetaData::findPlugins(QStringLiteral("plasma/network/vpn"), filter);

    if (offers.isEmpty()) {
        KPluginFactory::Result<VpnUiPlugin> result;
        result.errorReason = KPluginFactory::INVALID_PLUGIN;
        result.errorText = QStringLiteral("No VPN plugin found for type %1").arg(serviceType);
        result.errorString = i18n("No VPN plugin found for type %1", serviceType);

        return result;
    }

    return KPluginFactory::instantiatePlugin<VpnUiPlugin>(offers.first(), parent);
}
