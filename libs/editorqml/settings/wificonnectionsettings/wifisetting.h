/*
    SPDX-FileCopyrightText: 2013 Lukas Tinkl <ltinkl@redhat.com>
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef PLASMA_NM_WIFI_SETTING_H
#define PLASMA_NM_WIFI_SETTING_H

#include "plasmanm_editorqml_export.h"

#include <NetworkManagerQt/ConnectionSettings>
#include <NetworkManagerQt/WirelessSetting>

#include <QObject>
#include <QVariantList>

#include <qqmlregistration.h>

class PLASMANM_EDITORQML_EXPORT WifiSetting : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

    Q_PROPERTY(QString ssid READ ssid WRITE setSsid NOTIFY ssidChanged)
    Q_PROPERTY(int mode READ mode WRITE setMode NOTIFY modeChanged)
    Q_PROPERTY(QString bssid READ bssid WRITE setBssid NOTIFY bssidChanged)
    Q_PROPERTY(int band READ band WRITE setBand NOTIFY bandChanged)
    Q_PROPERTY(int channel READ channel WRITE setChannel NOTIFY channelChanged)
    Q_PROPERTY(QString macAddress READ macAddress WRITE setMacAddress NOTIFY macAddressChanged)
    Q_PROPERTY(QString clonedMacAddress READ clonedMacAddress WRITE setClonedMacAddress NOTIFY clonedMacAddressChanged)
    Q_PROPERTY(int mtu READ mtu WRITE setMtu NOTIFY mtuChanged)
    Q_PROPERTY(bool hidden READ hidden WRITE setHidden NOTIFY hiddenChanged)

    Q_PROPERTY(QStringList availableSsids READ availableSsids NOTIFY availableSsidsChanged)
    Q_PROPERTY(QVariantList availableBssids READ availableBssids NOTIFY availableBssidsChanged)
    Q_PROPERTY(QStringList macAddresses READ macAddresses CONSTANT)
    Q_PROPERTY(QVariantList channels READ channels NOTIFY channelsChanged)

    Q_PROPERTY(bool valid READ isValid NOTIFY validChanged)

public:
    explicit WifiSetting(QObject *parent = nullptr);

    QString ssid() const;
    void setSsid(const QString &ssid);

    int mode() const;
    void setMode(int mode);

    QString bssid() const;
    void setBssid(const QString &bssid);

    int band() const;
    void setBand(int band);

    int channel() const;
    void setChannel(int channel);

    QString macAddress() const;
    void setMacAddress(const QString &macAddress);

    QString clonedMacAddress() const;
    void setClonedMacAddress(const QString &clonedMacAddress);

    int mtu() const;
    void setMtu(int mtu);

    bool hidden() const;
    void setHidden(bool hidden);

    QStringList availableSsids() const;
    QVariantList availableBssids() const;
    QStringList macAddresses() const;
    QVariantList channels() const;

    bool isValid() const;

    Q_INVOKABLE QString generateRandomClonedMac() const;

    void loadConfig(const NetworkManager::ConnectionSettings::Ptr &settings);
    QVariantMap setting() const;

Q_SIGNALS:
    void ssidChanged();
    void modeChanged();
    void bssidChanged();
    void bandChanged();
    void channelChanged();
    void macAddressChanged();
    void clonedMacAddressChanged();
    void mtuChanged();
    void hiddenChanged();
    void availableSsidsChanged();
    void availableBssidsChanged();
    void channelsChanged();
    void validChanged();

private:
    QString m_ssid;
    NetworkManager::WirelessSetting::NetworkMode m_mode = NetworkManager::WirelessSetting::Infrastructure;
    QString m_bssid;
    NetworkManager::WirelessSetting::FrequencyBand m_band = NetworkManager::WirelessSetting::Automatic;
    int m_channel = 0;
    QString m_macAddress;
    QString m_clonedMacAddress;
    int m_mtu = 0;
    bool m_hidden = false;
};

#endif // PLASMA_NM_WIFI_SETTING_H
