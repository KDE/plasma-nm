/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "libreswan.h"

#include "nm-libreswan-service.h"

namespace
{
LibreswanSetting::PasswordOption optionFromFlags(const QString &rawFlags)
{
    const auto flags = static_cast<NetworkManager::Setting::SecretFlags>(rawFlags.toInt());

    if (flags.testFlag(NetworkManager::Setting::None)) {
        return LibreswanSetting::StoreForAllUsers;
    }
    if (flags.testFlag(NetworkManager::Setting::AgentOwned)) {
        return LibreswanSetting::StoreForUser;
    }
    return LibreswanSetting::AlwaysAsk;
}

void insertPasswordKeys(NMStringMap &data, const char *flagsKey, const char *inputModesKey, LibreswanSetting::PasswordOption option)
{
    switch (option) {
    case LibreswanSetting::StoreForAllUsers:
        data.insert(QLatin1String(inputModesKey), QLatin1String(NM_LIBRESWAN_PW_TYPE_SAVE));
        data.insert(QLatin1String(flagsKey), QString::number(NetworkManager::Setting::None));
        break;
    case LibreswanSetting::StoreForUser:
        data.insert(QLatin1String(flagsKey), QString::number(NetworkManager::Setting::AgentOwned));
        break;
    case LibreswanSetting::AlwaysAsk:
        data.insert(QLatin1String(inputModesKey), QLatin1String(NM_LIBRESWAN_PW_TYPE_ASK));
        data.insert(QLatin1String(flagsKey), QString::number(NetworkManager::Setting::NotSaved));
        break;
    }
}
}

LibreswanSetting::LibreswanSetting(QObject *parent)
    : QObject(parent)
{
}

LibreswanSetting::~LibreswanSetting() = default;

void LibreswanSetting::loadConfig(const NetworkManager::VpnSetting::Ptr &setting)
{
    if (!setting) {
        return;
    }

    setUserPassword(QString());
    setGroupPassword(QString());

    const NMStringMap data = setting->data();

    setGateway(data.value(QLatin1String(NM_LIBRESWAN_RIGHT)));
    setGroupName(data.value(QLatin1String(NM_LIBRESWAN_LEFTID)));

    setUserPasswordOption(optionFromFlags(data.value(QLatin1String(NM_LIBRESWAN_XAUTH_PASSWORD "-flags"))));
    setGroupPasswordOption(optionFromFlags(data.value(QLatin1String(NM_LIBRESWAN_PSK_VALUE "-flags"))));

    setUsername(data.value(QLatin1String(NM_LIBRESWAN_LEFTXAUTHUSER)));
    setPhase1Algorithms(data.value(QLatin1String(NM_LIBRESWAN_IKE)));
    setPhase2Algorithms(data.value(QLatin1String(NM_LIBRESWAN_ESP)));
    setDomain(data.value(QLatin1String(NM_LIBRESWAN_DOMAIN)));

    loadSecrets(setting);
}

void LibreswanSetting::loadSecrets(const NetworkManager::VpnSetting::Ptr &setting)
{
    if (!setting) {
        return;
    }

    const NMStringMap secrets = setting->secrets();

    const QString userPassword = secrets.value(QLatin1String(NM_LIBRESWAN_XAUTH_PASSWORD));
    if (!userPassword.isEmpty()) {
        setUserPassword(userPassword);
    }

    const QString groupPassword = secrets.value(QLatin1String(NM_LIBRESWAN_PSK_VALUE));
    if (!groupPassword.isEmpty()) {
        setGroupPassword(groupPassword);
    }
}

QString LibreswanSetting::serviceType() const
{
    return QLatin1String(NM_DBUS_SERVICE_LIBRESWAN);
}

QVariantMap LibreswanSetting::setting() const
{
    NetworkManager::VpnSetting setting;
    setting.setServiceType(QLatin1String(NM_DBUS_SERVICE_LIBRESWAN));

    NMStringMap data;
    NMStringMap secrets;

    if (!m_gateway.isEmpty()) {
        data.insert(QLatin1String(NM_LIBRESWAN_RIGHT), m_gateway);
    }

    if (!m_groupName.isEmpty()) {
        data.insert(QLatin1String(NM_LIBRESWAN_LEFTID), m_groupName);
    }

    if (!m_userPassword.isEmpty()) {
        secrets.insert(QLatin1String(NM_LIBRESWAN_XAUTH_PASSWORD), m_userPassword);
    }
    insertPasswordKeys(data, NM_LIBRESWAN_XAUTH_PASSWORD "-flags", NM_LIBRESWAN_XAUTH_PASSWORD_INPUT_MODES, m_userPasswordOption);

    if (!m_groupPassword.isEmpty()) {
        secrets.insert(QLatin1String(NM_LIBRESWAN_PSK_VALUE), m_groupPassword);
    }
    insertPasswordKeys(data, NM_LIBRESWAN_PSK_VALUE "-flags", NM_LIBRESWAN_PSK_INPUT_MODES, m_groupPasswordOption);

    if (!m_username.isEmpty()) {
        data.insert(QLatin1String(NM_LIBRESWAN_LEFTXAUTHUSER), m_username);
    }

    if (!m_phase1Algorithms.isEmpty()) {
        data.insert(QLatin1String(NM_LIBRESWAN_IKE), m_phase1Algorithms);
    }

    if (!m_phase2Algorithms.isEmpty()) {
        data.insert(QLatin1String(NM_LIBRESWAN_ESP), m_phase2Algorithms);
    }

    if (!m_domain.isEmpty()) {
        data.insert(QLatin1String(NM_LIBRESWAN_DOMAIN), m_domain);
    }

    setting.setData(data);
    setting.setSecrets(secrets);

    return setting.toMap();
}

bool LibreswanSetting::isValid() const
{
    return !m_gateway.isEmpty();
}

QString LibreswanSetting::gateway() const
{
    return m_gateway;
}

void LibreswanSetting::setGateway(const QString &gateway)
{
    if (m_gateway == gateway) {
        return;
    }
    m_gateway = gateway;
    Q_EMIT gatewayChanged();
    Q_EMIT validChanged();
}

QString LibreswanSetting::groupName() const
{
    return m_groupName;
}

void LibreswanSetting::setGroupName(const QString &groupName)
{
    if (m_groupName == groupName) {
        return;
    }
    m_groupName = groupName;
    Q_EMIT groupNameChanged();
    Q_EMIT validChanged();
}

QString LibreswanSetting::userPassword() const
{
    return m_userPassword;
}

void LibreswanSetting::setUserPassword(const QString &password)
{
    if (m_userPassword == password) {
        return;
    }
    m_userPassword = password;
    Q_EMIT userPasswordChanged();
    Q_EMIT validChanged();
}

LibreswanSetting::PasswordOption LibreswanSetting::userPasswordOption() const
{
    return m_userPasswordOption;
}

void LibreswanSetting::setUserPasswordOption(PasswordOption option)
{
    if (m_userPasswordOption == option) {
        return;
    }
    m_userPasswordOption = option;
    Q_EMIT userPasswordOptionChanged();
    Q_EMIT validChanged();
}

QString LibreswanSetting::groupPassword() const
{
    return m_groupPassword;
}

void LibreswanSetting::setGroupPassword(const QString &password)
{
    if (m_groupPassword == password) {
        return;
    }
    m_groupPassword = password;
    Q_EMIT groupPasswordChanged();
    Q_EMIT validChanged();
}

LibreswanSetting::PasswordOption LibreswanSetting::groupPasswordOption() const
{
    return m_groupPasswordOption;
}

void LibreswanSetting::setGroupPasswordOption(PasswordOption option)
{
    if (m_groupPasswordOption == option) {
        return;
    }
    m_groupPasswordOption = option;
    Q_EMIT groupPasswordOptionChanged();
    Q_EMIT validChanged();
}

QString LibreswanSetting::username() const
{
    return m_username;
}

void LibreswanSetting::setUsername(const QString &username)
{
    if (m_username == username) {
        return;
    }
    m_username = username;
    Q_EMIT usernameChanged();
    Q_EMIT validChanged();
}

QString LibreswanSetting::phase1Algorithms() const
{
    return m_phase1Algorithms;
}

void LibreswanSetting::setPhase1Algorithms(const QString &algorithms)
{
    if (m_phase1Algorithms == algorithms) {
        return;
    }
    m_phase1Algorithms = algorithms;
    Q_EMIT phase1AlgorithmsChanged();
    Q_EMIT validChanged();
}

QString LibreswanSetting::phase2Algorithms() const
{
    return m_phase2Algorithms;
}

void LibreswanSetting::setPhase2Algorithms(const QString &algorithms)
{
    if (m_phase2Algorithms == algorithms) {
        return;
    }
    m_phase2Algorithms = algorithms;
    Q_EMIT phase2AlgorithmsChanged();
    Q_EMIT validChanged();
}

QString LibreswanSetting::domain() const
{
    return m_domain;
}

void LibreswanSetting::setDomain(const QString &domain)
{
    if (m_domain == domain) {
        return;
    }
    m_domain = domain;
    Q_EMIT domainChanged();
    Q_EMIT validChanged();
}

#include "moc_libreswan.cpp"
