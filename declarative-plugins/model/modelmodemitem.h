/*
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

#ifndef PLASMA_NM_MODEL_MODEM_ITEM_H
#define PLASMA_NM_MODEL_MODEM_ITEM_H

#include "model/modelitem.h"

#include <QtModemManager/modemgsmnetworkinterface.h>

class ModelModemItem : public ModelItem
{
Q_OBJECT
public:
    static QString convertTypeToString(const ModemManager::ModemInterface::Type type);
    static QString convertBandToString(const ModemManager::ModemInterface::Band band);
    static QString convertAllowedModeToString(const ModemManager::ModemInterface::AllowedMode mode);
    static QString convertAccessTechnologyToString(const ModemManager::ModemInterface::AccessTechnology tech);

    ModelModemItem(const NetworkManager::Device::Ptr & device, QObject * parent = 0);
    ~ModelModemItem();

    void setDevice(const NetworkManager::Device::Ptr & device);

private Q_SLOTS:
    void modemNetworkRemoved();
    void onSignalQualitychanged(uint signal);
    void onAllowedModeChanged(ModemManager::ModemInterface::AllowedMode mode);
    void onAccessTechnologyChanged(ModemManager::ModemInterface::AccessTechnology tech);
protected:
    ModemManager::ModemGsmNetworkInterface::Ptr m_modemNetwork;

    void updateDetailsContent();
};

#endif // PLASMA_NM_CONNECTION_MODEM_ITEM_H
