/*
      SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>
      SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef PLASMA_NM_WIRED_SETTING_NH
#define PLASMA_NM_WIRED_SETTING_NH

#include "plasmanm_editorqml_export.h"

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantList>
#include <QtQmlIntegration/QtQmlIntegration>

#include <NetworkManagerQt/WiredSetting>
#include <networkmanagerqt/connectionsettings.h>

class PLASMANM_EDITORQML_EXPORT WiredSetting : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

    Q_PROPERTY(QString macAddress READ macAddress WRITE setMacAddress NOTIFY macAddressChanged)
    Q_PROPERTY(QStringList macAddresses READ macAddresses NOTIFY macAddressesChanged)

    Q_PROPERTY(QString clonedMacAddress READ clonedMacAddress WRITE setClonedMacAddress NOTIFY clonedMacAddressChanged)

    Q_PROPERTY(int mtu READ mtu WRITE setMtu NOTIFY mtuChanged)

    Q_PROPERTY(LinkNegotiation linkNegotiation READ linkNegotiation WRITE setLinkNegotiation NOTIFY linkNegotiationChanged)

    Q_PROPERTY(QVariantList speeds READ speeds CONSTANT)
    Q_PROPERTY(int speed READ speed WRITE setSpeed NOTIFY speedChanged)
    Q_PROPERTY(DuplexType duplex READ duplex WRITE setDuplex NOTIFY duplexChanged)

    Q_PROPERTY(bool manualLinkEnabled READ manualLinkEnabled NOTIFY linkNegotiationChanged)

    Q_PROPERTY(bool valid READ isValid NOTIFY validChanged)

public:
    enum LinkNegotiation {
        Ignore = 0,
        Automatic,
        Manual
    };
    Q_ENUM(LinkNegotiation)

    enum DuplexType {
        Half = 0,
        Full
    };
    Q_ENUM(DuplexType)

    explicit WiredSetting(QObject *parent = nullptr);

    [[nodiscard]] Q_INVOKABLE QVariantMap setting() const;

    Q_INVOKABLE QString generateRandomClonedMac() const;

    Q_INVOKABLE void loadConfig(const NetworkManager::ConnectionSettings::Ptr &settings);

    // property getters
    [[nodiscard]] QString macAddress() const;
    [[nodiscard]] QStringList macAddresses() const;
    [[nodiscard]] QString clonedMacAddress() const;
    [[nodiscard]] int mtu() const;
    [[nodiscard]] LinkNegotiation linkNegotiation() const;
    [[nodiscard]] QVariantList speeds() const;
    [[nodiscard]] int speed() const;
    [[nodiscard]] DuplexType duplex() const;
    [[nodiscard]] bool manualLinkEnabled() const;
    [[nodiscard]] bool isValid() const;

    // property setters
    void setMacAddress(const QString &macAddress);
    void setClonedMacAddress(const QString &clonedMacAddress);
    void setMtu(int mtu);
    void setLinkNegotiation(LinkNegotiation negotiation);
    void setSpeed(int speed);
    void setDuplex(DuplexType duplex);

Q_SIGNALS:
    void macAddressChanged();
    void macAddressesChanged();
    void clonedMacAddressChanged();
    void mtuChanged();
    void linkNegotiationChanged();
    void speedChanged();
    void duplexChanged();
    void validChanged();

private:
    QString m_macAddress;
    QString m_clonedMacAddress;
    int m_mtu = 0;
    LinkNegotiation m_linkNegotiation = Ignore;
    int m_speed = 100;
    DuplexType m_duplex = Full;
};

#endif // PLASMA_NM_WIRED_SETTING_NH
