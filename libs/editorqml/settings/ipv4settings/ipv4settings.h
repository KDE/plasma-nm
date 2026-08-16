/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>
    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef PLASMA_NM_IPV4_WIDGET_NH
#define PLASMA_NM_IPV4_WIDGET_NH

#include "ipv4routetablemodel.h"
#include "plasmanm_editorqml_export.h"

#include <QList>
#include <QObject>
#include <QString>
#include <QtQmlIntegration/QtQmlIntegration>

#include <NetworkManagerQt/IpAddress>
#include <NetworkManagerQt/IpRoute>
#include <NetworkManagerQt/Ipv4Setting>

class PLASMANM_EDITORQML_EXPORT IPv4Settings : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(IPv4Setting)
    QML_UNCREATABLE("")

    Q_PROPERTY(MethodIndex method READ method WRITE setMethod NOTIFY methodChanged)

    Q_PROPERTY(QString dns READ dns WRITE setDns NOTIFY dnsChanged)
    Q_PROPERTY(QString dnsSearch READ dnsSearch WRITE setDnsSearch NOTIFY dnsSearchChanged)
    Q_PROPERTY(QString dhcpClientId READ dhcpClientId WRITE setDhcpClientId NOTIFY dhcpClientIdChanged)
    Q_PROPERTY(QString dnsLabel READ dnsLabel NOTIFY methodChanged)

    Q_PROPERTY(double routeMetric READ routeMetric WRITE setRouteMetric NOTIFY routeMetricChanged)

    Q_PROPERTY(QList<IpRouteEntry> addresses READ addresses WRITE setAddresses NOTIFY addressesChanged)
    Q_PROPERTY(RouteTableModel *addressModel READ addressModel CONSTANT)

    Q_PROPERTY(bool ipv4Required READ ipv4Required WRITE setIpv4Required NOTIFY ipv4RequiredChanged)

    Q_PROPERTY(QList<IpRouteEntry> routes READ routes WRITE setRoutes NOTIFY routesChanged)

    Q_PROPERTY(RouteTableModel *routeModel READ routeModel CONSTANT)

    // advanced dialog fields
    Q_PROPERTY(bool dhcpSendHostname READ dhcpSendHostname WRITE setDhcpSendHostname NOTIFY dhcpSendHostnameChanged)
    Q_PROPERTY(QString dhcpHostname READ dhcpHostname WRITE setDhcpHostname NOTIFY dhcpHostnameChanged)

    Q_PROPERTY(QString systemHostname READ systemHostname CONSTANT)
    Q_PROPERTY(int dadTimeout READ dadTimeout WRITE setDadTimeout NOTIFY dadTimeoutChanged)
    Q_PROPERTY(bool neverDefault READ neverDefault WRITE setNeverDefault NOTIFY neverDefaultChanged)
    Q_PROPERTY(bool ignoreAutoRoutes READ ignoreAutoRoutes WRITE setIgnoreAutoRoutes NOTIFY ignoreAutoRoutesChanged)

    Q_PROPERTY(bool dnsEnabled READ dnsEnabled NOTIFY methodChanged)
    Q_PROPERTY(bool dhcpClientIdEnabled READ dhcpClientIdEnabled NOTIFY methodChanged)
    Q_PROPERTY(bool addressTableEnabled READ addressTableEnabled NOTIFY methodChanged)
    Q_PROPERTY(bool routesEnabled READ routesEnabled NOTIFY methodChanged)

    Q_PROPERTY(bool valid READ isValid NOTIFY validChanged)

public:
    enum MethodIndex {
        Automatic = 0,
        AutomaticOnlyIP,
        LinkLocal,
        Manual,
        Shared,
        Disabled
    };
    Q_ENUM(MethodIndex)

    explicit IPv4Settings(QObject *parent = nullptr);

    Q_INVOKABLE void loadConfig(const NetworkManager::Ipv4Setting::Ptr &setting);

    Q_INVOKABLE QVariantMap setting() const;

    // property getters
    MethodIndex method() const;
    QString dns() const;
    QString dnsSearch() const;
    QString dhcpClientId() const;
    double routeMetric() const;
    QList<IpRouteEntry> addresses() const;
    RouteTableModel *addressModel() const;
    bool ipv4Required() const;
    QList<IpRouteEntry> routes() const;
    RouteTableModel *routeModel() const;
    bool dhcpSendHostname() const;
    QString dhcpHostname() const;
    QString systemHostname() const;
    int dadTimeout() const;
    bool neverDefault() const;
    bool ignoreAutoRoutes() const;

    bool dnsEnabled() const;
    bool dhcpClientIdEnabled() const;
    bool addressTableEnabled() const;
    bool routesEnabled() const;
    QString dnsLabel() const;

    bool isValid() const;

    // property setters
    void setMethod(MethodIndex method);
    void setDns(const QString &dns);
    void setDnsSearch(const QString &dnsSearch);
    void setDhcpClientId(const QString &id);
    void setRouteMetric(double metric);
    void setAddresses(const QList<IpRouteEntry> &addresses);
    void setIpv4Required(bool required);
    void setRoutes(const QList<IpRouteEntry> &routes);
    void setDhcpSendHostname(bool send);
    void setDhcpHostname(const QString &hostname);
    void setDadTimeout(int timeout);
    void setNeverDefault(bool neverDefault);
    void setIgnoreAutoRoutes(bool ignore);

    Q_INVOKABLE void suggestNetmaskForAddress(int index);

Q_SIGNALS:
    void methodChanged();
    void dnsChanged();
    void dnsSearchChanged();
    void dhcpClientIdChanged();
    void routeMetricChanged();
    void addressesChanged();
    void ipv4RequiredChanged();
    void routesChanged();
    void dhcpSendHostnameChanged();
    void dhcpHostnameChanged();
    void dadTimeoutChanged();
    void neverDefaultChanged();
    void ignoreAutoRoutesChanged();
    void validChanged();

private:
    quint32 suggestNetmask(quint32 ip);

    MethodIndex m_method = Automatic;
    QString m_dns;
    QString m_dnsSearch;
    QString m_dhcpClientId;
    double m_routeMetric = -1;
    RouteTableModel *m_addressModel = nullptr;
    bool m_ipv4Required = true;

    RouteTableModel *m_routeModel = nullptr;
    bool m_dhcpSendHostname = true;
    QString m_dhcpHostname;
    int m_dadTimeout = -1;
    bool m_neverDefault = false;
    bool m_ignoreAutoRoutes = false;
};

#endif
