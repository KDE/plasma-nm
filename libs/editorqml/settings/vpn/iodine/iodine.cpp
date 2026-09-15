/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "iodine.h"

#include "nm-iodine-service.h"

IodineSetting::IodineSetting(QObject *parent)
    : QObject(parent)
{
}

IodineSetting::~IodineSetting() = default;

void IodineSetting::loadConfig(const NetworkManager::VpnSetting::Ptr &setting)
{
    if (!setting) {
        return;
    }

    setIodinePassword(QString());

    const NMStringMap data = setting->data();

    setTopLevelDomain(data.value(QLatin1String(NM_IODINE_KEY_TOPDOMAIN)));
    setNameserver(data.value(QLatin1String(NM_IODINE_KEY_NAMESERVER)));

    const auto flags = static_cast<NetworkManager::Setting::SecretFlags>(data.value(QLatin1String(NM_IODINE_KEY_PASSWORD "-flags")).toInt());
    if (flags.testFlag(NetworkManager::Setting::None)) {
        setIodinePasswordOption(StoreForAllUsers);
    } else if (flags.testFlag(NetworkManager::Setting::AgentOwned)) {
        setIodinePasswordOption(StoreForUser);
    } else {
        setIodinePasswordOption(AlwaysAsk);
    }

    setFragmentSize(data.value(QLatin1String(NM_IODINE_KEY_FRAGSIZE)).toInt());

    loadSecrets(setting);
}

void IodineSetting::loadSecrets(const NetworkManager::VpnSetting::Ptr &setting)
{
    if (!setting) {
        return;
    }

    const QString password = setting->secrets().value(QLatin1String(NM_IODINE_KEY_PASSWORD));
    if (!password.isEmpty()) {
        setIodinePassword(password);
    }
}

QString IodineSetting::serviceType() const
{
    return QLatin1String(NM_DBUS_SERVICE_IODINE);
}

QVariantMap IodineSetting::setting() const
{
    NetworkManager::VpnSetting setting;
    setting.setServiceType(QLatin1String(NM_DBUS_SERVICE_IODINE));

    NMStringMap data;
    NMStringMap secrets;

    if (!m_topLevelDomain.isEmpty()) {
        data.insert(QLatin1String(NM_IODINE_KEY_TOPDOMAIN), m_topLevelDomain);
    }

    if (!m_nameserver.isEmpty()) {
        data.insert(QLatin1String(NM_IODINE_KEY_NAMESERVER), m_nameserver);
    }

    if (!m_iodinePassword.isEmpty()) {
        secrets.insert(QLatin1String(NM_IODINE_KEY_PASSWORD), m_iodinePassword);
    }

    NetworkManager::Setting::SecretFlagType passwordFlag = NetworkManager::Setting::AgentOwned;
    switch (m_iodinePasswordOption) {
    case StoreForAllUsers:
        passwordFlag = NetworkManager::Setting::None;
        break;
    case StoreForUser:
        passwordFlag = NetworkManager::Setting::AgentOwned;
        break;
    case AlwaysAsk:
        passwordFlag = NetworkManager::Setting::NotSaved;
        break;
    }
    data.insert(QLatin1String(NM_IODINE_KEY_PASSWORD "-flags"), QString::number(passwordFlag));

    if (m_fragmentSize > 0) {
        data.insert(QLatin1String(NM_IODINE_KEY_FRAGSIZE), QString::number(m_fragmentSize));
    }

    setting.setData(data);
    setting.setSecrets(secrets);

    return setting.toMap();
}

bool IodineSetting::isValid() const
{
    return !m_topLevelDomain.isEmpty();
}

QString IodineSetting::topLevelDomain() const
{
    return m_topLevelDomain;
}

void IodineSetting::setTopLevelDomain(const QString &topLevelDomain)
{
    if (m_topLevelDomain == topLevelDomain) {
        return;
    }
    m_topLevelDomain = topLevelDomain;
    Q_EMIT topLevelDomainChanged();
    Q_EMIT validChanged();
}

QString IodineSetting::nameserver() const
{
    return m_nameserver;
}

void IodineSetting::setNameserver(const QString &nameserver)
{
    if (m_nameserver == nameserver) {
        return;
    }
    m_nameserver = nameserver;
    Q_EMIT nameserverChanged();
    Q_EMIT validChanged();
}

QString IodineSetting::iodinePassword() const
{
    return m_iodinePassword;
}

void IodineSetting::setIodinePassword(const QString &password)
{
    if (m_iodinePassword == password) {
        return;
    }
    m_iodinePassword = password;
    Q_EMIT iodinePasswordChanged();
    Q_EMIT validChanged();
}

IodineSetting::PasswordOption IodineSetting::iodinePasswordOption() const
{
    return m_iodinePasswordOption;
}

void IodineSetting::setIodinePasswordOption(PasswordOption option)
{
    if (m_iodinePasswordOption == option) {
        return;
    }
    m_iodinePasswordOption = option;
    Q_EMIT iodinePasswordOptionChanged();
    Q_EMIT validChanged();
}

int IodineSetting::fragmentSize() const
{
    return m_fragmentSize;
}

void IodineSetting::setFragmentSize(int fragmentSize)
{
    if (m_fragmentSize == fragmentSize) {
        return;
    }
    m_fragmentSize = fragmentSize;
    Q_EMIT fragmentSizeChanged();
    Q_EMIT validChanged();
}

#include "moc_iodine.cpp"
