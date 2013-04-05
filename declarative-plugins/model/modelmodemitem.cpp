/*
    Copyright 2008-2010 Sebastian Kügler <sebas@kde.org>
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

#include "modelmodemitem.h"

#include <QtNetworkManager/modemdevice.h>

#include <KLocalizedString>

#include "debug.h"

ModelModemItem::ModelModemItem(NetworkManager::Device* device, QObject* parent):
    ModelItem(device, parent)
{
    setDevice(device);
}

ModelModemItem::~ModelModemItem()
{
}

/* From the old networkmanagement */

QString ModelModemItem::convertTypeToString(const ModemManager::ModemInterface::Type type)
{
    switch (type) {
        case ModemManager::ModemInterface::UnknownType: return i18nc("Unknown cellular type","Unknown");
        case ModemManager::ModemInterface::GsmType: return i18nc("Gsm cellular type","Gsm");
        case ModemManager::ModemInterface::CdmaType: return i18nc("Cdma cellular type","Cdma");
    }

    return i18nc("Unknown cellular type","Unknown");
}

QString ModelModemItem::convertBandToString(const ModemManager::ModemInterface::Band band)
{
    switch (band) {
        case ModemManager::ModemInterface::UnknownBand: return i18nc("Unknown cellular frequency band","Unknown");
        case ModemManager::ModemInterface::AnyBand: return i18nc("Any cellular frequency band","Any");
        case ModemManager::ModemInterface::Egsm: return i18nc("Cellular frequency band","GSM/GPRS/EDGE 900 MHz");
        case ModemManager::ModemInterface::Dcs: return i18nc("Cellular frequency band","GSM/GPRS/EDGE 1800 MHz");
        case ModemManager::ModemInterface::Pcs: return i18nc("Cellular frequency band","GSM/GPRS/EDGE 1900 MHz");
        case ModemManager::ModemInterface::G850: return i18nc("Cellular frequency band","GSM/GPRS/EDGE 850 MHz");
        case ModemManager::ModemInterface::U2100: return i18nc("Cellular frequency band","WCDMA 2100 MHz (Class I)");
        case ModemManager::ModemInterface::U1800: return i18nc("Cellular frequency band","WCDMA 3GPP 1800 MHz (Class III)");
        case ModemManager::ModemInterface::U17IV: return i18nc("Cellular frequency band","WCDMA 3GPP AWS 1700/2100 MHz (Class IV)");
        case ModemManager::ModemInterface::U800: return i18nc("Cellular frequency band","WCDMA 3GPP UMTS 800 MHz (Class VI)");
        case ModemManager::ModemInterface::U850: return i18nc("Cellular frequency band","WCDMA 3GPP UMTS 850 MHz (Class V)");
        case ModemManager::ModemInterface::U900: return i18nc("Cellular frequency band","WCDMA 3GPP UMTS 900 MHz (Class VIII)");
        case ModemManager::ModemInterface::U17IX: return i18nc("Cellular frequency band","WCDMA 3GPP UMTS 1700 MHz (Class IX)");
        case ModemManager::ModemInterface::U19IX: return i18nc("Cellular frequency band","WCDMA 3GPP UMTS 1900 MHz (Class II)");
    }

    return i18nc("Unknown cellular frequency band","Unknown");
}

QString ModelModemItem::convertAllowedModeToString(const ModemManager::ModemInterface::AllowedMode mode)
{
    switch (mode) {
        case ModemManager::ModemInterface::AnyModeAllowed: return i18nc("Allowed Gsm modes (2G/3G/any)","Any");
        case ModemManager::ModemInterface::Prefer2g: return i18nc("Allowed Gsm modes (2G/3G/any)","Prefer 2G");
        case ModemManager::ModemInterface::Prefer3g: return i18nc("Allowed Gsm modes (2G/3G/any)","Prefer 3G");
        case ModemManager::ModemInterface::UseOnly2g: return i18nc("Allowed Gsm modes (2G/3G/any)","Only 2G");
        case ModemManager::ModemInterface::UseOnly3g: return i18nc("Allowed Gsm modes (2G/3G/any)","Only 3G");
    }

    return i18nc("Allowed Gsm modes (2G/3G/any)","Any");
}

QString ModelModemItem::convertAccessTechnologyToString(const ModemManager::ModemInterface::AccessTechnology tech)
{
    switch (tech) {
        case ModemManager::ModemInterface::UnknownTechnology: return i18nc("Unknown cellular access technology","Unknown");
        case ModemManager::ModemInterface::Gsm: return i18nc("Cellular access technology","GSM");
        case ModemManager::ModemInterface::GsmCompact: return i18nc("Cellular access technology","Compact GSM");
        case ModemManager::ModemInterface::Gprs: return i18nc("Cellular access technology","GPRS");
        case ModemManager::ModemInterface::Edge: return i18nc("Cellular access technology","EDGE");
        case ModemManager::ModemInterface::Umts: return i18nc("Cellular access technology","UMTS");
        case ModemManager::ModemInterface::Hsdpa: return i18nc("Cellular access technology","HSDPA");
        case ModemManager::ModemInterface::Hsupa: return i18nc("Cellular access technology","HSUPA");
        case ModemManager::ModemInterface::Hspa: return i18nc("Cellular access technology","HSPA");
    }

    return i18nc("Unknown cellular access technology","Unknown");
}

void ModelModemItem::setDevice(NetworkManager::Device* device)
{
    ModelItem::setDevice(device);

    NetworkManager::ModemDevice * modemDevice = qobject_cast<NetworkManager::ModemDevice*>(device);

    if (modemDevice) {
        m_modemNetwork = modemDevice->getModemNetworkIface();

        if (m_modemNetwork) {
            connect(m_modemNetwork, SIGNAL(signalQualityChanged(uint)),
                    SLOT(onSignalQualitychanged(uint)), Qt::UniqueConnection);
            connect(m_modemNetwork, SIGNAL(accessTechnologyChanged(ModemManager::ModemInterface::AccessTechnology)),
                    SLOT(onAccessTechnologyChanged(ModemManager::ModemInterface::AccessTechnology)), Qt::UniqueConnection);
            connect(m_modemNetwork, SIGNAL(allowedModeChanged(ModemManager::ModemInterface::AllowedMode)),
                    SLOT(onAllowedModeChanged(ModemManager::ModemInterface::AllowedMode)), Qt::UniqueConnection);
        }
    }
}

void ModelModemItem::updateDetailsContent()
{
    ModelItem::updateDetailsContent();

    QString format = "<tr><td align=\"right\"><b>%1</b></td><td align=\"left\">&nbsp;%2</td></tr>";

    if (m_modemNetwork) {
        m_details += QString(format).arg(i18n("Operator:"), m_modemNetwork->getRegistrationInfo().operatorName);
        m_details += QString(format).arg(i18n("Signal quality:"), QString("%1%").arg(m_modemNetwork->getSignalQuality()));
        m_details += QString(format).arg(i18n("Access technology:"), QString("%1/%2").arg(convertTypeToString(m_modemNetwork->type()), convertAccessTechnologyToString(m_modemNetwork->getAccessTechnology())));
        m_details += QString(format).arg(i18n("Allowed Mode"), convertAllowedModeToString(m_modemNetwork->getAllowedMode()));
    }
}

void ModelModemItem::onSignalQualitychanged(uint signal)
{
    updateDetails();

    emit itemChanged();

    NMItemDebug() << name() << ": signal quality changed to " << signal;
}

void ModelModemItem::onAccessTechnologyChanged(ModemManager::ModemInterface::AccessTechnology tech)
{
    updateDetails();

    emit itemChanged();

    NMItemDebug() << name() << ": access technology changed to " << convertAccessTechnologyToString(tech);
}

void ModelModemItem::onAllowedModeChanged(ModemManager::ModemInterface::AllowedMode mode)
{
    updateDetails();

    emit itemChanged();

    NMItemDebug() << name() << ": allowed mode changed to " << convertAllowedModeToString(mode);
}
