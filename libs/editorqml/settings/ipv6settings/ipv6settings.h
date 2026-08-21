/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>
    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef PLASMA_NM_IPV6_SETTINGS_NH
#define PLASMA_NM_IPV6_SETTINGS_NH

#include "ipv6routetablemodel.h"
#include "plasmanm_editorqml_export.h"

#include <QList>
#include <QObject>
#include <QString>
#include <QtQmlIntegration/QtQmlIntegration>

#include <NetworkManagerQt/IpAddress>
#include <NetworkManagerQt/IpRoute>
#include <NetworkManagerQt/Ipv6Setting>

class PLASMANM_EDITORQML_EXPORT IPv6Settings : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(IPv6Setting)
    QML_UNCREATABLE("")

    Q_PROPERTY(MethodIndex method READ method WRITE setMethod NOTIFY methodChanged)

    Q_PROPERTY(QString dns READ dns WRITE setDns NOTIFY dnsChanged)
    Q_PROPERTY(QString dnsSearch READ dnsSearch WRITE setDnsSearch NOTIFY dnsSearchChanged)
    Q_PROPERTY(PrivacyIndex privacy READ privacy WRITE setPrivacy NOTIFY privacyChanged)
    Q_PROPERTY(QString dnsLabel READ dnsLabel NOTIFY methodChanged)

    Q_PROPERTY(double routeMetric READ routeMetric WRITE setRouteMetric NOTIFY routeMetricChanged)

    Q_PROPERTY(QList<Ipv6RouteEntry> addresses READ addresses WRITE setAddresses NOTIFY addressesChanged)
    Q_PROPERTY(Ipv6RouteTableModel *addressModel READ addressModel CONSTANT)

    Q_PROPERTY(bool ipv6Required READ ipv6Required WRITE setIpv6Required NOTIFY ipv6RequiredChanged)

    Q_PROPERTY(QList<Ipv6RouteEntry> routes READ routes WRITE setRoutes NOTIFY routesChanged)
    Q_PROPERTY(Ipv6RouteTableModel *routeModel READ routeModel CONSTANT)

    // routes dialog fields
    Q_PROPERTY(bool neverDefault READ neverDefault WRITE setNeverDefault NOTIFY neverDefaultChanged)
    Q_PROPERTY(bool ignoreAutoRoutes READ ignoreAutoRoutes WRITE setIgnoreAutoRoutes NOTIFY ignoreAutoRoutesChanged)

    Q_PROPERTY(bool dnsEnabled READ dnsEnabled NOTIFY methodChanged)
    Q_PROPERTY(bool privacyEnabled READ privacyEnabled NOTIFY methodChanged)
    Q_PROPERTY(bool routeMetricEnabled READ routeMetricEnabled NOTIFY methodChanged)
    Q_PROPERTY(bool addressTableEnabled READ addressTableEnabled NOTIFY methodChanged)
    Q_PROPERTY(bool routesEnabled READ routesEnabled NOTIFY methodChanged)
    Q_PROPERTY(bool ipv6RequiredEnabled READ ipv6RequiredEnabled NOTIFY methodChanged)

    Q_PROPERTY(bool valid READ isValid NOTIFY validChanged)

public:
    enum MethodIndex {
        Automatic = 0,
        AutomaticOnlyIP,
        AutomaticOnlyDHCP,
        LinkLocal,
        Manual,
        Ignored,
        Disabled
    };
    Q_ENUM(MethodIndex)

    enum PrivacyIndex {
        Default = 0,
        PrivacyDisabled,
        EnabledPublicAddress,
        EnabledTemporaryAddress
    };
    Q_ENUM(PrivacyIndex)

    explicit IPv6Settings(QObject *parent = nullptr);

    Q_INVOKABLE void loadConfig(const NetworkManager::Ipv6Setting::Ptr &setting);

    Q_INVOKABLE QVariantMap setting() const;

    // property getters
    MethodIndex method() const;
    QString dns() const;
    QString dnsSearch() const;
    PrivacyIndex privacy() const;
    double routeMetric() const;
    QList<Ipv6RouteEntry> addresses() const;
    Ipv6RouteTableModel *addressModel() const;
    bool ipv6Required() const;
    QList<Ipv6RouteEntry> routes() const;
    Ipv6RouteTableModel *routeModel() const;
    bool neverDefault() const;
    bool ignoreAutoRoutes() const;

    bool dnsEnabled() const;
    bool privacyEnabled() const;
    bool routeMetricEnabled() const;
    bool addressTableEnabled() const;
    bool routesEnabled() const;
    bool ipv6RequiredEnabled() const;
    QString dnsLabel() const;

    bool isValid() const;

    // property setters
    void setMethod(MethodIndex method);
    void setDns(const QString &dns);
    void setDnsSearch(const QString &dnsSearch);
    void setPrivacy(PrivacyIndex privacy);
    void setRouteMetric(double metric);
    void setAddresses(const QList<Ipv6RouteEntry> &addresses);
    void setIpv6Required(bool required);
    void setRoutes(const QList<Ipv6RouteEntry> &routes);
    void setNeverDefault(bool neverDefault);
    void setIgnoreAutoRoutes(bool ignore);

    Q_INVOKABLE void suggestPrefixForAddress(int index);

    // Shares QHostAddress with setting()/isValid() so the dialogs cannot accept
    // a value this class would later drop.
    Q_INVOKABLE bool isValidAddress(const QString &address) const;

Q_SIGNALS:
    void methodChanged();
    void dnsChanged();
    void dnsSearchChanged();
    void privacyChanged();
    void routeMetricChanged();
    void addressesChanged();
    void ipv6RequiredChanged();
    void routesChanged();
    void neverDefaultChanged();
    void ignoreAutoRoutesChanged();
    void validChanged();

private:
    uint suggestPrefix(const Q_IPV6ADDR &ip) const;

    MethodIndex m_method = Automatic;
    QString m_dns;
    QString m_dnsSearch;
    PrivacyIndex m_privacy = Default;
    double m_routeMetric = -1;
    Ipv6RouteTableModel *m_addressModel = nullptr;
    bool m_ipv6Required = true;

    Ipv6RouteTableModel *m_routeModel = nullptr;
    bool m_neverDefault = false;
    bool m_ignoreAutoRoutes = false;
};

#endif
