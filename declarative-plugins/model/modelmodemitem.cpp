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
#include "uiutils.h"

#include <NetworkManagerQt/modemdevice.h>
#include <NetworkManagerQt/manager.h>

#include <KLocalizedString>

#include "debug.h"

ModelModemItem::ModelModemItem(const NetworkManager::Device::Ptr & device, QObject* parent):
    ModelItem(device, parent),
    m_modemNetwork(0)
{
    addSpecificDevice(device);
}

ModelModemItem::~ModelModemItem()
{
}

void ModelModemItem::addSpecificDevice(const NetworkManager::Device::Ptr & device)
{
    //TODO add support for more modems

    NetworkManager::ModemDevice::Ptr modemDevice = device.objectCast<NetworkManager::ModemDevice>();

    if (modemDevice) {
        m_modemNetwork = modemDevice->getModemNetworkIface().objectCast<ModemManager::ModemGsmNetworkInterface>();

        if (m_modemNetwork) {
            connect(m_modemNetwork.data(), SIGNAL(destroyed(QObject*)),
                    SLOT(modemNetworkRemoved()));
            connect(m_modemNetwork.data(), SIGNAL(signalQualityChanged(uint)),
                    SLOT(onSignalQualitychanged(uint)), Qt::UniqueConnection);
            connect(m_modemNetwork.data(), SIGNAL(accessTechnologyChanged(ModemManager::ModemInterface::AccessTechnology)),
                    SLOT(onAccessTechnologyChanged(ModemManager::ModemInterface::AccessTechnology)), Qt::UniqueConnection);
            connect(m_modemNetwork.data(), SIGNAL(allowedModeChanged(ModemManager::ModemInterface::AllowedMode)),
                    SLOT(onAllowedModeChanged(ModemManager::ModemInterface::AllowedMode)), Qt::UniqueConnection);
        }
    }

    updateDetails();
}

void ModelModemItem::updateDetails()
{
    QString format = "<tr><td align=\"right\" width=\"50%\"><b>%1</b></td><td align=\"left\" width=\"50%\">&nbsp;%2</td></tr>";

    m_details = "<qt><table>";

    // Prepare objects
    if (m_type != NetworkManager::Settings::ConnectionSettings::Unknown && m_flags.testFlag(Model::ConnectionType)) {
        m_details += QString(format).arg(i18nc("type of network device", "Type:"), NetworkManager::Settings::ConnectionSettings::typeAsString(m_type));
    }
    m_details += QString(format).arg("\n", "\n");

    foreach (const QString & path, m_devicePaths) {
        if (m_connected && m_activeDevicePath != path) {
            continue;
        }

        // Prepare objects
        NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(path);

        // Set details
        if (device) {
            QString name;
            if (device->ipInterfaceName().isEmpty()) {
                name = device->interfaceName();
            } else {
                name = device->ipInterfaceName();
            }
            if (m_flags.testFlag(Model::DeviceSystemName))
                m_details += QString(format).arg(i18n("System name:"), name);

            if (device->ipV4Config().isValid() && m_connected && m_flags.testFlag(Model::DeviceIpv4Address)) {
                QHostAddress addr = device->ipV4Config().addresses().first().ip();
                m_details += QString(format).arg(i18n("IPv4 Address:"), addr.toString());
            }

            if (device->ipV6Config().isValid() && m_connected && m_flags.testFlag(Model::DeviceIpv6Address)) {
                QHostAddress addr = device->ipV6Config().addresses().first().ip();
                m_details += QString(format).arg(i18n("IPv6 Address:"), addr.toString());
            }

            m_details += QString(format).arg("\n", "\n");
        }

        if (m_modemNetwork) {
            if (m_flags.testFlag(Model::ModemOperator))
                m_details += QString(format).arg(i18n("Operator:"), m_modemNetwork->getRegistrationInfo().operatorName);
            if (m_flags.testFlag(Model::ModemSignalQuality))
                m_details += QString(format).arg(i18n("Signal quality:"), QString("%1%").arg(m_modemNetwork->getSignalQuality()));
            if (m_flags.testFlag(Model::ModemAccessTechnology))
                m_details += QString(format).arg(i18n("Access technology:"), QString("%1/%2").arg(UiUtils::convertTypeToString(m_modemNetwork->type()), UiUtils::convertAccessTechnologyToString(m_modemNetwork->getAccessTechnology())));
            if (m_flags.testFlag(Model::ModemAllowedMode))
                m_details += QString(format).arg(i18n("Allowed Mode"), UiUtils::convertAllowedModeToString(m_modemNetwork->getAllowedMode()));
        }
    }

    m_details += "</table></qt>";

    Q_EMIT itemChanged();
}

void ModelModemItem::modemNetworkRemoved()
{
    m_modemNetwork.clear();
}

void ModelModemItem::onSignalQualitychanged(uint signal)
{
    updateDetails();

    NMItemDebug() << name() << ": signal quality changed to " << signal;
}

void ModelModemItem::onAccessTechnologyChanged(ModemManager::ModemInterface::AccessTechnology tech)
{
    updateDetails();

    NMItemDebug() << name() << ": access technology changed to " << UiUtils::convertAccessTechnologyToString(tech);
}

void ModelModemItem::onAllowedModeChanged(ModemManager::ModemInterface::AllowedMode mode)
{
    updateDetails();

    NMItemDebug() << name() << ": allowed mode changed to " << UiUtils::convertAllowedModeToString(mode);
}
