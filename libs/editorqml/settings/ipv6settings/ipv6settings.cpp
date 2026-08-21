/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>
    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "ipv6settings.h"

#include <KLocalizedString>

#include <QHostAddress>

#include <networkmanagerqt/ipconfig.h>
#include <networkmanagerqt/ipv6setting.h>

IPv6Settings::IPv6Settings(QObject *parent)
    : QObject(parent)
    , m_addressModel(new Ipv6RouteTableModel(this))
    , m_routeModel(new Ipv6RouteTableModel(this))
{
    connect(m_routeModel, &Ipv6RouteTableModel::routesChanged, this, [this] {
        Q_EMIT routesChanged();
        Q_EMIT validChanged();
    });
    connect(m_addressModel, &Ipv6RouteTableModel::routesChanged, this, [this] {
        Q_EMIT addressesChanged();
        Q_EMIT validChanged();
    });
}

uint IPv6Settings::suggestPrefix(const Q_IPV6ADDR &ip) const
{
    Q_UNUSED(ip);

    /*
       TODO: find out common IPv6 prefixes and make a complete function
    */
    return 64;
}

void IPv6Settings::loadConfig(const NetworkManager::Ipv6Setting::Ptr &setting)
{
    if (!setting)
        return;

    // routes
    QList<Ipv6RouteEntry> routeEntries;
    const QList<NetworkManager::IpRoute> nmRoutes = setting->routes();
    for (const NetworkManager::IpRoute &r : nmRoutes) {
        Ipv6RouteEntry entry;
        entry.address = r.ip().toString();
        entry.prefix = r.prefixLength();
        entry.nextHop = r.nextHop().toString();
        entry.metric = r.metric();
        routeEntries.append(entry);
    }
    m_routeModel->setRoutes(routeEntries);

    m_neverDefault = setting->neverDefault();
    m_ignoreAutoRoutes = setting->ignoreAutoRoutes();

    // method
    switch (setting->method()) {
    case NetworkManager::Ipv6Setting::Automatic:
        m_method = setting->ignoreAutoDns() ? AutomaticOnlyIP : Automatic;
        break;
    case NetworkManager::Ipv6Setting::Dhcp:
        m_method = AutomaticOnlyDHCP;
        break;
    case NetworkManager::Ipv6Setting::LinkLocal:
        m_method = LinkLocal;
        break;
    case NetworkManager::Ipv6Setting::Manual:
        m_method = Manual;
        break;
    case NetworkManager::Ipv6Setting::Ignored:
        m_method = Ignored;
        break;
    case NetworkManager::Ipv6Setting::ConfigDisabled:
        m_method = Disabled;
        break;
    }

    // dns
    QStringList tmp;
    const QList<QHostAddress> nmDns = setting->dns();
    for (const QHostAddress &addr : nmDns) {
        tmp.append(addr.toString());
    }
    m_dns = tmp.join(QLatin1Char(','));
    m_dnsSearch = setting->dnsSearch().join(QLatin1Char(','));

    // metric
    m_routeMetric = static_cast<double>(setting->routeMetric());

    // addresses
    QList<Ipv6RouteEntry> addressEntries;
    const QList<NetworkManager::IpAddress> nmAddresses = setting->addresses();
    for (const NetworkManager::IpAddress &addr : nmAddresses) {
        Ipv6RouteEntry entry;
        entry.address = addr.ip().toString();
        entry.prefix = addr.prefixLength();
        entry.nextHop = addr.gateway().toString();
        addressEntries.append(entry);
    }
    m_addressModel->setRoutes(addressEntries);

    // may-fail
    m_ipv6Required = !setting->mayFail();

    // privacy
    if (setting->privacy() != NetworkManager::Ipv6Setting::Unknown) {
        m_privacy = static_cast<PrivacyIndex>(setting->privacy() + 1);
    }

    Q_EMIT methodChanged();
    Q_EMIT dnsChanged();
    Q_EMIT dnsSearchChanged();
    Q_EMIT privacyChanged();
    Q_EMIT routeMetricChanged();
    Q_EMIT addressesChanged();
    Q_EMIT ipv6RequiredChanged();
    Q_EMIT routesChanged();
    Q_EMIT neverDefaultChanged();
    Q_EMIT ignoreAutoRoutesChanged();
    Q_EMIT validChanged();
}

QVariantMap IPv6Settings::setting() const
{
    NetworkManager::Ipv6Setting ipv6;
    QList<NetworkManager::IpRoute> nmRoutes;

    const QList<Ipv6RouteEntry> routeEntries = m_routeModel->routes();
    for (const Ipv6RouteEntry &r : routeEntries) {
        const QHostAddress ip(r.address);
        if (ip.protocol() != QAbstractSocket::IPv6Protocol)
            continue;

        NetworkManager::IpRoute route;
        route.setIp(ip);
        route.setPrefixLength(r.prefix);
        route.setNextHop(QHostAddress(r.nextHop));
        route.setMetric(r.metric);
        nmRoutes.append(route);
    }

    ipv6.setRoutes(nmRoutes);
    ipv6.setNeverDefault(m_neverDefault);
    ipv6.setIgnoreAutoRoutes(m_ignoreAutoRoutes);

    // method
    switch (m_method) {
    case Automatic:
        ipv6.setMethod(NetworkManager::Ipv6Setting::Automatic);
        break;
    case AutomaticOnlyIP:
        ipv6.setMethod(NetworkManager::Ipv6Setting::Automatic);
        ipv6.setIgnoreAutoDns(true);
        break;
    case AutomaticOnlyDHCP:
        ipv6.setMethod(NetworkManager::Ipv6Setting::Dhcp);
        break;
    case LinkLocal:
        ipv6.setMethod(NetworkManager::Ipv6Setting::LinkLocal);
        break;
    case Manual:
        ipv6.setMethod(NetworkManager::Ipv6Setting::Manual);
        break;
    case Ignored:
        ipv6.setMethod(NetworkManager::Ipv6Setting::Ignored);
        break;
    case Disabled:
        ipv6.setMethod(NetworkManager::Ipv6Setting::ConfigDisabled);
        break;
    }

    // dns
    if (dnsEnabled() && !m_dns.isEmpty()) {
        QList<QHostAddress> addrs;
        const QStringList servers = m_dns.split(QLatin1Char(','));
        for (const QString &s : servers) {
            QHostAddress a(s.trimmed());
            if (!a.isNull())
                addrs.append(a);
        }
        ipv6.setDns(addrs);
    }
    if (dnsEnabled() && !m_dnsSearch.isEmpty()) {
        ipv6.setDnsSearch(m_dnsSearch.split(QLatin1Char(',')));
    }

    // metric
    ipv6.setRouteMetric(static_cast<int>(m_routeMetric));

    // addresses
    if (addressTableEnabled()) {
        QList<NetworkManager::IpAddress> list;
        const QList<Ipv6RouteEntry> addressEntries = m_addressModel->routes();
        for (const Ipv6RouteEntry &e : addressEntries) {
            const QHostAddress ip(e.address);
            if (ip.protocol() != QAbstractSocket::IPv6Protocol)
                continue;

            NetworkManager::IpAddress addr;
            addr.setIp(ip);
            addr.setPrefixLength(e.prefix);
            if (!e.nextHop.isEmpty())
                addr.setGateway(QHostAddress(e.nextHop));
            list.append(addr);
        }

        if (!list.isEmpty()) {
            ipv6.setAddresses(list);
        }
    }

    // may-fail
    ipv6.setMayFail(!m_ipv6Required);

    if (privacyEnabled() && m_privacy != Default) {
        ipv6.setPrivacy(static_cast<NetworkManager::Ipv6Setting::IPv6Privacy>(m_privacy - 1));
    }

    return ipv6.toMap();
}

QString IPv6Settings::dnsLabel() const
{
    return m_method == Automatic ? i18n("Other DNS Servers:") : i18n("DNS Servers:");
}

bool IPv6Settings::dnsEnabled() const
{
    return m_method == Automatic || m_method == AutomaticOnlyIP || m_method == Manual;
}

bool IPv6Settings::privacyEnabled() const
{
    return m_method != Ignored && m_method != Disabled;
}

bool IPv6Settings::routeMetricEnabled() const
{
    return m_method != Ignored && m_method != Disabled;
}

bool IPv6Settings::addressTableEnabled() const
{
    return m_method == Manual;
}

bool IPv6Settings::routesEnabled() const
{
    return m_method == Automatic || m_method == AutomaticOnlyIP || m_method == Manual;
}

bool IPv6Settings::ipv6RequiredEnabled() const
{
    return m_method != Ignored && m_method != Disabled;
}

bool IPv6Settings::isValid() const
{
    if (m_method == Manual) {
        const QList<Ipv6RouteEntry> addressEntries = m_addressModel->routes();
        if (addressEntries.isEmpty())
            return false;

        for (const Ipv6RouteEntry &e : addressEntries) {
            const QHostAddress ip(e.address);
            const QHostAddress gateway(e.nextHop);

            if (ip.protocol() != QAbstractSocket::IPv6Protocol)
                return false;
            if (e.prefix < 1 || e.prefix > Ipv6RouteTableModel::MaxPrefixLength)
                return false;
            if (gateway.isNull() && !e.nextHop.isEmpty())
                return false;
        }
    }

    if (!m_dns.isEmpty() && (m_method == Automatic || m_method == AutomaticOnlyIP || m_method == Manual)) {
        const QStringList servers = m_dns.split(QLatin1Char(','));
        for (const QString &s : servers) {
            if (QHostAddress(s.trimmed()).isNull()) {
                return false;
            }
        }
    }

    return true;
}

bool IPv6Settings::isValidAddress(const QString &address) const
{
    QHostAddress parsed;
    return parsed.setAddress(address) && parsed.protocol() == QAbstractSocket::IPv6Protocol;
}

void IPv6Settings::suggestPrefixForAddress(int index)
{
    const QList<Ipv6RouteEntry> addressEntries = m_addressModel->routes();
    if (index < 0 || index >= addressEntries.size())
        return;

    const Ipv6RouteEntry &entry = addressEntries.at(index);
    if (entry.prefix != 0)
        return; // don't overwrite an existing prefix

    const QHostAddress addr(entry.address);
    if (addr.protocol() != QAbstractSocket::IPv6Protocol)
        return;

    const uint prefix = suggestPrefix(addr.toIPv6Address());
    if (prefix) {
        m_addressModel->setRouteField(index, Ipv6RouteTableModel::PrefixColumn, prefix);
    }
}

IPv6Settings::MethodIndex IPv6Settings::method() const
{
    return m_method;
}
void IPv6Settings::setMethod(MethodIndex m)
{
    if (m_method == m)
        return;
    m_method = m;
    Q_EMIT methodChanged();
    Q_EMIT validChanged();
}

QString IPv6Settings::dns() const
{
    return m_dns;
}
void IPv6Settings::setDns(const QString &v)
{
    if (m_dns == v)
        return;
    m_dns = v;
    Q_EMIT dnsChanged();
    Q_EMIT validChanged();
}

QString IPv6Settings::dnsSearch() const
{
    return m_dnsSearch;
}
void IPv6Settings::setDnsSearch(const QString &v)
{
    if (m_dnsSearch == v)
        return;
    m_dnsSearch = v;
    Q_EMIT dnsSearchChanged();
    Q_EMIT validChanged();
}

IPv6Settings::PrivacyIndex IPv6Settings::privacy() const
{
    return m_privacy;
}
void IPv6Settings::setPrivacy(PrivacyIndex v)
{
    if (m_privacy == v)
        return;
    m_privacy = v;
    Q_EMIT privacyChanged();
    Q_EMIT validChanged();
}

double IPv6Settings::routeMetric() const
{
    return m_routeMetric;
}
void IPv6Settings::setRouteMetric(double v)
{
    if (m_routeMetric == v)
        return;
    m_routeMetric = v;
    Q_EMIT routeMetricChanged();
    Q_EMIT validChanged();
}

QList<Ipv6RouteEntry> IPv6Settings::addresses() const
{
    return m_addressModel->routes();
}
void IPv6Settings::setAddresses(const QList<Ipv6RouteEntry> &v)
{
    m_addressModel->setRoutes(v);
}

Ipv6RouteTableModel *IPv6Settings::addressModel() const
{
    return m_addressModel;
}

bool IPv6Settings::ipv6Required() const
{
    return m_ipv6Required;
}
void IPv6Settings::setIpv6Required(bool v)
{
    if (m_ipv6Required == v)
        return;
    m_ipv6Required = v;
    Q_EMIT ipv6RequiredChanged();
    Q_EMIT validChanged();
}

QList<Ipv6RouteEntry> IPv6Settings::routes() const
{
    return m_routeModel->routes();
}
void IPv6Settings::setRoutes(const QList<Ipv6RouteEntry> &v)
{
    m_routeModel->setRoutes(v);
}

Ipv6RouteTableModel *IPv6Settings::routeModel() const
{
    return m_routeModel;
}

bool IPv6Settings::neverDefault() const
{
    return m_neverDefault;
}
void IPv6Settings::setNeverDefault(bool v)
{
    if (m_neverDefault == v)
        return;
    m_neverDefault = v;
    Q_EMIT neverDefaultChanged();
    Q_EMIT validChanged();
}

bool IPv6Settings::ignoreAutoRoutes() const
{
    return m_ignoreAutoRoutes;
}
void IPv6Settings::setIgnoreAutoRoutes(bool v)
{
    if (m_ignoreAutoRoutes == v)
        return;
    m_ignoreAutoRoutes = v;
    Q_EMIT ignoreAutoRoutesChanged();
    Q_EMIT validChanged();
}

#include "moc_ipv6settings.cpp"
