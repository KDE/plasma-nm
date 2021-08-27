/*
    SPDX-FileCopyrightText: 2019 Bruce Anderson <banderson19com@san.rr.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef PLASMA_NM_WIREGUARD_TAB_WIDGET_H
#define PLASMA_NM_WIREGUARD_TAB_WIDGET_H

#include <QDialog>

#include "settingwidget.h"
#include <NetworkManagerQt/WireguardSetting>

class Q_DECL_EXPORT WireGuardTabWidget : public QDialog
{
    Q_OBJECT

public:
    explicit WireGuardTabWidget(const NMVariantMapList &peerData, QWidget *parent = nullptr, Qt::WindowFlags f = {});
    ~WireGuardTabWidget() override;

    enum EndPointValid { BothValid, AddressValid, PortValid, BothInvalid };

    void loadConfig(const NMVariantMapList &peerData);

    NMVariantMapList setting() const;

    void slotAddPeer();
    void slotAddPeerWithData(const QVariantMap &peerData);
    void slotRemovePeer();

private:
    void slotWidgetChanged();

    class Private;
    Private *d;
};

#endif // PLASMA_NM_WIREGUARD_TAB_WIDGET_H
