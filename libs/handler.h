/*
    SPDX-FileCopyrightText: 2013-2014 Jan Grulich <jgrulich@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef PLASMA_NM_HANDLER_H
#define PLASMA_NM_HANDLER_H

#include <QDBusInterface>
#include <QTimer>

#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/ConnectionSettings>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/Utils>
#if WITH_MODEMMANAGER_SUPPORT
#include <ModemManagerQt/GenericTypes>
#endif

class Q_DECL_EXPORT Handler : public QObject
{
    Q_OBJECT

public:
    enum HandlerAction {
        ActivateConnection,
        AddAndActivateConnection,
        AddConnection,
        DeactivateConnection,
        RemoveConnection,
        RequestScan,
        UpdateConnection,
        CreateHotspot,
    };

    explicit Handler(QObject *parent = nullptr);
    ~Handler() override;

    Q_PROPERTY(bool hotspotSupported READ hotspotSupported NOTIFY hotspotSupportedChanged);

public:
    bool hotspotSupported() const
    {
        return m_hotspotSupported;
    };

public Q_SLOTS:
    /**
     * Activates given connection
     * @connection - d-bus path of the connection you want to activate
     * @device - d-bus path of the device where the connection should be activated
     * @specificParameter - d-bus path of the specific object you want to use for this activation, i.e access point
     */
    void activateConnection(const QString &connection, const QString &device, const QString &specificParameter);
    /**
     * Adds and activates a new wireless connection
     * @device - d-bus path of the wireless device where the connection should be activated
     * @specificParameter - d-bus path of the accesspoint you want to connect to
     * @password - pre-filled password which should be used for the new wireless connection
     * @autoConnect - boolean value whether this connection should be activated automatically when it's available
     *
     * Works automatically for wireless connections with WEP/WPA security, for wireless connections with WPA/WPA
     * it will open the connection editor for advanced configuration.
     * */
    void addAndActivateConnection(const QString &device, const QString &specificParameter, const QString &password = QString());

    /**
     * Returns a code that includes the credentials to a said wifi connection
     * Here's some information on how this information is created, it's generally used to put in QR codes to share.
     * https://github.com/zxing/zxing/wiki/Barcode-Contents#wi-fi-network-config-android-ios-11
     *
     * @param connectionPath the d-bus path to the connection we want to read
     * @param ssid the name of the network being displayed
     * @param securityType the authentication protocol used for this specific ssid
     */
    QString wifiCode(const QString &connectionPath, const QString &ssid, /*NetworkManager::WirelessSecurityType*/ int securityType) const;

    /**
     * Adds a new connection
     * @map - NMVariantMapMap with connection settings
     */
    void addConnection(const NMVariantMapMap &map);
    /**
     * Deactivates given connection
     * @connection - d-bus path of the connection you want to deactivate
     * @device - d-bus path of the connection where the connection is activated
     */
    void deactivateConnection(const QString &connection, const QString &device);
    /**
     * Disconnects all connections
     */
    void disconnectAll();
    void enableAirplaneMode(bool enable);
    void enableNetworking(bool enable);
    void enableWireless(bool enable);

    void enableWwan(bool enable);

    /**
     * Removes given connection
     * @connection - d-bus path of the connection you want to edit
     */
    void removeConnection(const QString &connection);
    /**
     * Updates given connection
     * @connection - connection which should be updated
     * @map - NMVariantMapMap with new connection settings
     */
    void updateConnection(const NetworkManager::Connection::Ptr &connection, const NMVariantMapMap &map);
    void requestScan(const QString &interface = QString());

    void createHotspot();
    void stopHotspot();

private Q_SLOTS:
    void secretAgentError(const QString &connectionPath, const QString &message);
    void replyFinished(QDBusPendingCallWatcher *watcher);
    void hotspotCreated(QDBusPendingCallWatcher *watcher);
    void primaryConnectionTypeChanged(NetworkManager::ConnectionSettings::ConnectionType type);
#if WITH_MODEMMANAGER_SUPPORT
    void unlockRequiredChanged(MMModemLock modemLock);
#endif

Q_SIGNALS:
    void connectionActivationFailed(const QString &connectionPath, const QString &message);
    void hotspotCreated();
    void hotspotDisabled();
    void hotspotSupportedChanged(bool hotspotSupported);

private:
    bool m_hotspotSupported;
    bool m_runningLiveImage;
    bool m_tmpWirelessEnabled;
    bool m_tmpWwanEnabled;
#if WITH_MODEMMANAGER_SUPPORT
    QString m_tmpConnectionPath;
#endif
    QString m_tmpConnectionUuid;
    QString m_tmpDevicePath;
    QString m_tmpSpecificPath;
    QMap<QString, bool> m_bluetoothAdapters;
    QMap<QString, QTimer *> m_wirelessScanRetryTimer;

    void enableBluetooth(bool enable);
    void scanRequestFailed(const QString &interface);
    bool checkRequestScanRateLimit(const NetworkManager::WirelessDevice::Ptr &wifiDevice);
    bool checkHotspotSupported();
    bool checkRunningLiveImage();
    void scheduleRequestScan(const QString &interface, int timeout);
};

#endif // PLASMA_NM_HANDLER_H
