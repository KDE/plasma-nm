/*
    Copyright 2013-2017 Jan Grulich <jgrulich@redhat.com>

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

import QtQuick 2.2
import org.kde.plasma.plasmoid 2.0
import org.kde.plasma.networkmanagement 0.2 as PlasmaNM
import org.kde.kquickcontrolsaddons 2.0
import QtQuick.Layouts 1.1

Item {
    id: mainWindow

    property bool showSections: true
    readonly property string kcm: "kcm_networkmanagement.desktop"
    readonly property bool kcmAuthorized: KCMShell.authorize(kcm).length == 1

    Plasmoid.toolTipMainText: i18n("Networks")
    Plasmoid.toolTipSubText: networkStatus.activeConnections
    Plasmoid.icon: connectionIconProvider.connectionTooltipIcon
    Plasmoid.switchWidth: units.gridUnit * 10
    Plasmoid.switchHeight: units.gridUnit * 10
    Plasmoid.compactRepresentation: CompactRepresentation { }
    Plasmoid.fullRepresentation: PopupDialog {
        id: dialogItem
        Layout.minimumWidth: units.iconSizes.medium * 10
        Layout.minimumHeight: units.gridUnit * 20
        anchors.fill: parent
        focus: true
    }

    function action_openKCM() {
        KCMShell.open(kcm)
    }

    Component.onCompleted: {
        if (kcmAuthorized) {
            plasmoid.setAction("openKCM", i18n("&Configure Network Connections..."), "preferences-system-network");
        }
    }

    PlasmaNM.NetworkStatus {
        id: networkStatus
    }

    PlasmaNM.ConnectionIcon {
        id: connectionIconProvider
    }

    PlasmaNM.Handler {
        id: handler
    }

    Timer {
        id: scanTimer
        interval: 15000
        repeat: true
        running: plasmoid.expanded
        triggeredOnStart: true

        onTriggered: {
            handler.requestScan()
        }
    }

    PlasmaNM.Configuration {
        unlockModemOnDetection: plasmoid.configuration.unlockModemOnDetection
        manageVirtualConnections: plasmoid.configuration.manageVirtualConnections
    }
}
