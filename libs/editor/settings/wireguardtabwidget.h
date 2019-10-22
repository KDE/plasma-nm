/*
    Copyright 2019 Bruce Anderson <banderson19com@san.rr.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
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

    enum EndPointValid {BothValid, AddressValid, PortValid, BothInvalid};

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
