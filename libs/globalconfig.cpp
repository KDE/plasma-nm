/*
    Copyright 2013 Dan Vratil <dvratil@redhat.com>
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

#include "globalconfig.h"

// #include <QDebug>

static GlobalConfig *s_instance = 0;

GlobalConfig* GlobalConfig::instance()
{
    if (s_instance == 0) {
        s_instance = new GlobalConfig(0);
    }

    return s_instance;
}

// This is called from instance()
GlobalConfig::GlobalConfig(void *dummy)
    : QObject()
    , m_airplaneMode(false)
    , m_unit(KBytes)
{
    Q_UNUSED(dummy);
}

// This one is called when instantiated in QML
GlobalConfig::GlobalConfig()
    : QObject()
    , m_airplaneMode(false)
    , m_unit(KBytes)
{
    GlobalConfig *singleton = instance();

    connect(singleton, SIGNAL(airplaneModeEnabledChanged()),
            this, SIGNAL(airplaneModeEnabledChanged()));
    connect(singleton, SIGNAL(detailKeysChanged()),
            this, SIGNAL(detailKeysChanged()));
    connect(singleton, SIGNAL(networkSpeedUnitChanged()),
            this, SIGNAL(networkSpeedUnitChanged()));
}

GlobalConfig::~GlobalConfig()
{
}

bool GlobalConfig::airplaneModeEnabled() const
{
    if (this != s_instance) {
        return s_instance->airplaneModeEnabled();
    }

    return m_airplaneMode;
}

void GlobalConfig::setAirplaneModeEnabled(bool enabled)
{
    if (this != s_instance) {
        s_instance->setAirplaneModeEnabled(enabled);
        return;
    }

    if (m_airplaneMode != enabled) {
        m_airplaneMode = enabled;
        Q_EMIT airplaneModeEnabledChanged();
    }
}

QStringList GlobalConfig::detailKeys() const
{
    if (this != s_instance) {
        return s_instance->detailKeys();
    }

    return m_keys;
}

void GlobalConfig::setDetailKeys(const QStringList& keys)
{
    if (this != s_instance) {
        s_instance->setDetailKeys(keys);
        return;
    }

    if (m_keys != keys) {
        m_keys = keys;
        Q_EMIT detailKeysChanged();
    }
}

GlobalConfig::NetworkSpeedUnit GlobalConfig::networkSpeedUnit() const
{
    if (this != s_instance) {
        return s_instance->m_unit;
    }
    return m_unit;
}

void GlobalConfig::setNetworkSpeedUnit(GlobalConfig::NetworkSpeedUnit unit)
{
    if (this != s_instance) {
        s_instance->setNetworkSpeedUnit(unit);
        return;
    }

    if (m_unit != unit) {
        m_unit = unit;
        Q_EMIT networkSpeedUnitChanged();
    }
}
