/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>
    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "ipv4settings.h"
#include "ui_wireguardinterfacewidget.h"
#include <QHostAddress>
#include <QHostInfo>
#include <networkmanagerqt/iproute.h>
#include <networkmanagerqt/ipv4setting.h>
#include <qhostaddress.h>

IPv4Settings::IPv4Settings(QObject *parent)
    : QObject(parent)
    , m_addressModel(new RouteTableModel(this))
    , m_routeModel(new RouteTableModel(this))
{
    connect(m_routeModel, &RouteTableModel::routesChanged, this, [this] {
        Q_EMIT routesChanged();
        Q_EMIT validChanged();
    });
    connect(m_addressModel, &RouteTableModel::routesChanged, this, [this] {
        Q_EMIT addressesChanged();
        Q_EMIT validChanged();
    });
}

quint32 IPv4Settings::suggestNetmask(quint32 ip)
{
    /*
        A   0       0.0.0.0 <-->127.255.255.255  255.0.0.0 <--->/8
        B   10      128.0.0.0 <>191.255.255.255  255.255.0.0 <->/16
        C   110     192.0.0.0 <>223.255.255.255  255.255.255.0 >/24
        D   1110    224.0.0.0 <>239.255.255.255  not defined <->not defined
        E   1111    240.0.0.0 <>255.255.255.254  not defined <->not defined
    */
    quint32 netmask = 0;

    if (!(ip & 0x80000000)) {
        // test 0 leading bit
        netmask = 0xFF000000;
    } else if (!(ip & 0x40000000)) {
        // test 10 leading bits
        netmask = 0xFFFF0000;
    } else if (!(ip & 0x20000000)) {
        // test 110 leading bits
        netmask = 0xFFFFFF00;
    }

    return netmask;
}

void IPv4Settings::loadConfig(const NetworkManager::Ipv4Setting::Ptr &setting)
{
    if (!setting)
        return;

    QList<IpRouteEntry> routeEntries;
    for (const NetworkManager::IpRoute &r : setting->routes()) {
        IpRouteEntry entry;
        entry.address = r.ip().toString();
        entry.netmask = r.netmask().toString();
        entry.nextHop = r.nextHop().toString();
        entry.metric = r.metric();
        routeEntries.append(entry);
    }
    m_routeModel->setRoutes(routeEntries);
    m_neverDefault = setting->neverDefault();
    m_ignoreAutoRoutes = setting->ignoreAutoRoutes();
    m_dhcpHostname = setting->dhcpHostname();
    m_dhcpSendHostname = setting->dhcpSendHostname();
    m_dadTimeout = setting->dadTimeout();

    switch (setting->method()) {
    case NetworkManager::Ipv4Setting::Automatic:
        m_method = setting->ignoreAutoDns() ? AutomaticOnlyIP : Automatic;
        break;
    case NetworkManager::Ipv4Setting::Manual:
        m_method = Manual;
        break;
    case NetworkManager::Ipv4Setting::LinkLocal:
        m_method = LinkLocal;
        break;
    case NetworkManager::Ipv4Setting::Shared:
        m_method = Shared;
        break;
    case NetworkManager::Ipv4Setting::Disabled:
        m_method = Disabled;
        break;
    }

    QStringList tmp;
    for (const QHostAddress &addr : setting->dns()) {
        tmp.append(addr.toString());
    }

    // dns
    m_dns = tmp.join(QStringLiteral(","));
    m_dnsSearch = setting->dnsSearch().join(QLatin1Char(','));
    m_dhcpClientId = setting->dhcpClientId();

    // metric
    m_routeMetric = static_cast<double>(setting->routeMetric());

    // addresses
    QList<IpRouteEntry> addressEntries;
    for (const NetworkManager::IpAddress &addr : setting->addresses()) {
        IpRouteEntry entry;
        entry.address = addr.ip().toString();
        entry.netmask = addr.netmask().toString();
        entry.nextHop = addr.gateway().toString();
        addressEntries.append(entry);
    }
    m_addressModel->setRoutes(addressEntries);

    m_ipv4Required = !setting->mayFail();

    Q_EMIT methodChanged();
    Q_EMIT dnsChanged();
    Q_EMIT dnsSearchChanged();
    Q_EMIT dhcpClientIdChanged();
    Q_EMIT routeMetricChanged();
    Q_EMIT addressesChanged();
    Q_EMIT ipv4RequiredChanged();
    Q_EMIT routesChanged();
    Q_EMIT validChanged();
}

QVariantMap IPv4Settings::setting() const
{
    NetworkManager::Ipv4Setting ipv4;
    QList<NetworkManager::IpRoute> nmRoutes;

    const QList<IpRouteEntry> routeEntries = m_routeModel->routes();
    for (const IpRouteEntry &r : routeEntries) {
        const QHostAddress ip(r.address);
        const QHostAddress netmask(r.netmask);
        if (ip.isNull() || netmask.isNull())
            continue;

        NetworkManager::IpRoute route;
        route.setIp(ip);
        route.setNetmask(netmask);
        route.setNextHop(QHostAddress(r.nextHop));
        route.setMetric(r.metric);
        nmRoutes.append(route);
    }

    ipv4.setRoutes(nmRoutes);
    ipv4.setNeverDefault(m_neverDefault);
    ipv4.setIgnoreAutoRoutes(m_ignoreAutoRoutes);
    ipv4.setDhcpHostname(m_dhcpHostname);
    ipv4.setDhcpSendHostname(m_dhcpSendHostname);
    ipv4.setDadTimeout(m_dadTimeout);

    // method
    switch (m_method) {
    case Automatic:
        ipv4.setMethod(NetworkManager::Ipv4Setting::Automatic);
        break;
    case AutomaticOnlyIP:
        ipv4.setMethod(NetworkManager::Ipv4Setting::Automatic);
        ipv4.setIgnoreAutoDns(true);
        break;
    case Manual:
        ipv4.setMethod(NetworkManager::Ipv4Setting::Manual);
        break;
    case LinkLocal:
        ipv4.setMethod(NetworkManager::Ipv4Setting::LinkLocal);
        break;
    case Shared:
        ipv4.setMethod(NetworkManager::Ipv4Setting::Shared);
        break;
    case Disabled:
        ipv4.setMethod(NetworkManager::Ipv4Setting::Disabled);
        break;
    }

    // dns
    if (dnsEnabled() && !m_dns.isEmpty()) {
        QList<QHostAddress> addrs;
        for (const QString &s : m_dns.split(QLatin1Char(','))) {
            QHostAddress a(s.trimmed());
            if (!a.isNull())
                addrs.append(a);
        }
        ipv4.setDns(addrs);
    }
    if (dnsEnabled() && !m_dnsSearch.isEmpty()) {
        ipv4.setDnsSearch(m_dnsSearch.split(QLatin1Char(',')));
    }

    // dhcp id
    if (dhcpClientIdEnabled() && !m_dhcpClientId.isEmpty()) {
        ipv4.setDhcpClientId(m_dhcpClientId);
    }

    // metric
    ipv4.setRouteMetric(static_cast<int>(m_routeMetric));

    // addresses
    if (addressTableEnabled()) {
        QList<NetworkManager::IpAddress> list;
        const QList<IpRouteEntry> addressEntries = m_addressModel->routes();
        for (const IpRouteEntry &e : addressEntries) {
            const QHostAddress ip(e.address);
            const QHostAddress netmask(e.netmask);
            if (ip.isNull() || netmask.isNull())
                continue;

            NetworkManager::IpAddress addr;
            addr.setIp(ip);
            addr.setNetmask(netmask);
            if (!e.nextHop.isEmpty())
                addr.setGateway(QHostAddress(e.nextHop));
            list.append(addr);
        }

        if (!list.isEmpty()) {
            ipv4.setAddresses(list);
        }
    }

    ipv4.setMayFail(!m_ipv4Required);

    return ipv4.toMap();
}

QString IPv4Settings::dnsLabel() const
{
    return m_method == Automatic ? i18n("Other DNS Servers:") : i18n("DNS Servers:");
}

bool IPv4Settings::dnsEnabled() const
{
    if (m_method == LinkLocal || m_method == Shared || m_method == Disabled)
        return false;
    return true;
}

bool IPv4Settings::dhcpClientIdEnabled() const
{
    return m_method == Automatic || m_method == AutomaticOnlyIP;
}

bool IPv4Settings::addressTableEnabled() const
{
    return m_method == Manual;
}

bool IPv4Settings::routesEnabled() const
{
    return m_method != LinkLocal && m_method != Shared && m_method != Disabled;
}

bool IPv4Settings::isValid() const
{
    if (m_method == Manual) {
        const QList<IpRouteEntry> addressEntries = m_addressModel->routes();
        if (addressEntries.isEmpty())
            return false;
        for (const IpRouteEntry &e : addressEntries) {
            QHostAddress ip(e.address);
            QHostAddress netmask(e.netmask);
            QHostAddress gw(e.nextHop);
            if (ip.isNull() || netmask.isNull() || (gw.isNull() && !e.nextHop.isEmpty())) {
                return false;
            }
        }
    }

    if (!m_dns.isEmpty() && (m_method == Automatic || m_method == Manual || m_method == AutomaticOnlyIP)) {
        for (const QString &s : m_dns.split(QLatin1Char(','))) {
            if (QHostAddress(s.trimmed()).isNull()) {
                return false;
            }
        }
    }
    return true;
}

void IPv4Settings::suggestNetmaskForAddress(int index)
{
    const QList<IpRouteEntry> addressEntries = m_addressModel->routes();
    if (index < 0 || index >= addressEntries.size())
        return;

    const IpRouteEntry &entry = addressEntries.at(index);
    if (!entry.netmask.isEmpty())
        return; // don't overwrite existing

    QHostAddress addr(entry.address);
    const quint32 netmask = suggestNetmask(addr.toIPv4Address());
    if (netmask) {
        m_addressModel->setRouteField(index, RouteTableModel::NetmaskColumn, QHostAddress(netmask).toString());
    }
}

void IPv4Settings::setMethod(MethodIndex m)
{
    if (m_method == m)
        return;
    m_method = m;
    Q_EMIT methodChanged();
    Q_EMIT validChanged();
}

IPv4Settings::MethodIndex IPv4Settings::method() const
{
    return m_method;
}

QString IPv4Settings::dns() const
{
    return m_dns;
}
void IPv4Settings::setDns(const QString &v)
{
    if (m_dns == v)
        return;
    m_dns = v;
    Q_EMIT dnsChanged();
    Q_EMIT validChanged();
}

QString IPv4Settings::dnsSearch() const
{
    return m_dnsSearch;
}
void IPv4Settings::setDnsSearch(const QString &v)
{
    if (m_dnsSearch == v)
        return;
    m_dnsSearch = v;
    Q_EMIT dnsSearchChanged();
    Q_EMIT validChanged();
}

QString IPv4Settings::dhcpClientId() const
{
    return m_dhcpClientId;
}
void IPv4Settings::setDhcpClientId(const QString &v)
{
    if (m_dhcpClientId == v)
        return;
    m_dhcpClientId = v;
    Q_EMIT dhcpClientIdChanged();
    Q_EMIT validChanged();
}

double IPv4Settings::routeMetric() const
{
    return m_routeMetric;
}
void IPv4Settings::setRouteMetric(double v)
{
    if (m_routeMetric == v)
        return;
    m_routeMetric = v;
    Q_EMIT routeMetricChanged();
    Q_EMIT validChanged();
}

QList<IpRouteEntry> IPv4Settings::addresses() const
{
    return m_addressModel->routes();
}
void IPv4Settings::setAddresses(const QList<IpRouteEntry> &v)
{
    m_addressModel->setRoutes(v);
}

RouteTableModel *IPv4Settings::addressModel() const
{
    return m_addressModel;
}

bool IPv4Settings::ipv4Required() const
{
    return m_ipv4Required;
}
void IPv4Settings::setIpv4Required(bool v)
{
    if (m_ipv4Required == v)
        return;
    m_ipv4Required = v;
    Q_EMIT ipv4RequiredChanged();
    Q_EMIT validChanged();
}

QList<IpRouteEntry> IPv4Settings::routes() const
{
    return m_routeModel->routes();
}
void IPv4Settings::setRoutes(const QList<IpRouteEntry> &v)
{
    // The model emits routesChanged() for us.
    m_routeModel->setRoutes(v);
}

RouteTableModel *IPv4Settings::routeModel() const
{
    return m_routeModel;
}

bool IPv4Settings::dhcpSendHostname() const
{
    return m_dhcpSendHostname;
}
void IPv4Settings::setDhcpSendHostname(bool v)
{
    if (m_dhcpSendHostname == v)
        return;
    m_dhcpSendHostname = v;
    Q_EMIT dhcpSendHostnameChanged();
    Q_EMIT validChanged();
}

QString IPv4Settings::dhcpHostname() const
{
    return m_dhcpHostname;
}
void IPv4Settings::setDhcpHostname(const QString &v)
{
    if (m_dhcpHostname == v)
        return;
    m_dhcpHostname = v;
    Q_EMIT dhcpHostnameChanged();
    Q_EMIT validChanged();
}

QString IPv4Settings::systemHostname() const
{
    return QHostInfo::localHostName();
}

int IPv4Settings::dadTimeout() const
{
    return m_dadTimeout;
}
void IPv4Settings::setDadTimeout(int v)
{
    if (m_dadTimeout == v)
        return;
    m_dadTimeout = v;
    Q_EMIT dadTimeoutChanged();
    Q_EMIT validChanged();
}

bool IPv4Settings::neverDefault() const
{
    return m_neverDefault;
}
void IPv4Settings::setNeverDefault(bool v)
{
    if (m_neverDefault == v)
        return;
    m_neverDefault = v;
    Q_EMIT neverDefaultChanged();
    Q_EMIT validChanged();
}

bool IPv4Settings::ignoreAutoRoutes() const
{
    return m_ignoreAutoRoutes;
}
void IPv4Settings::setIgnoreAutoRoutes(bool v)
{
    if (m_ignoreAutoRoutes == v)
        return;
    m_ignoreAutoRoutes = v;
    Q_EMIT ignoreAutoRoutesChanged();
    Q_EMIT validChanged();
}

#include "moc_ipv4settings.cpp"
