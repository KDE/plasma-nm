/*
 *   Copyright 2017 Martin Kacej <m.kacej@atlas.sk>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

import QtQuick 2.6
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.2 as Controls
import org.kde.plasma.networkmanagement 0.2 as PlasmaNM
import org.kde.kirigami 2.2 as Kirigami

Kirigami.ApplicationItem {
    id: main
    objectName: "wifiMain"

    pageStack.defaultColumnWidth: Kirigami.Units.gridUnit * 25
    pageStack.initialPage: networkListview
    Kirigami.Theme.colorSet: Kirigami.Theme.Window

    anchors.fill: parent

    PlasmaNM.Handler {
        id: handler
        onWirelessScanTimerEnabled: {
            if (enabled) {
                scanTimer.restart()
            } else {
                scanTimer.stop()
            }
        }
    }

    PlasmaNM.EnabledConnections {
        id: enabledConnections

        onWirelessEnabledChanged: {
            wifiSwitchButton.checked = wifiSwitchButton.enabled && enabled
        }
    }

    contextDrawer: Kirigami.ContextDrawer {
        id: contextDrawer
    }

    PlasmaNM.NetworkModel {
        id: connectionModel
    }

    PlasmaNM.MobileProxyModel {
        id: mobileProxyModel
        sourceModel: connectionModel
        showSavedMode: false
    }

    Component.onCompleted: handler.requestScan()

    Timer {
        id: scanTimer
        interval: 15000
        repeat: true
        running: parent.visible

        onTriggered: handler.requestScan()
    }

    NetworkListView {
        id: networkListview
        anchors.fill: parent
    }

    Component {
        id: connectionEditorDialogComponent
        ConnectionEditor { }
    }

    Component {
        id: networkDetailsViewComponent
        NetworkSettings { }
    }

    Component {
        id: tetheringComponent
        TetheringSetting { }
    }
}
