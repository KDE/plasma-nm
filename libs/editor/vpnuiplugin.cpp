/*
    SPDX-FileCopyrightText: 2008 Will Stephenson <wstephenson@kde.org>
    SPDX-FileCopyrightText: 2013 Lukáš Tinkl <ltinkl@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "vpnuiplugin.h"

#include <KLocalizedString>

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
