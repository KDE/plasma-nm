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
import QtQuick.Layouts 1.2
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.extras 2.0 as PlasmaExtras
import org.kde.plasma.networkmanagement 0.2 as PlasmaNM

FocusScope {
    id: full

    property alias toolbarValues: toolbar

    PlasmaNM.AvailableDevices {
        id: availableDevices
    }

    Component {
        id: networkModelComponent
        PlasmaNM.NetworkModel {}
    }

    property PlasmaNM.NetworkModel connectionModel: null

    PlasmaNM.AppletProxyModel {
        id: appletProxyModel

        sourceModel: full.connectionModel
    }

    ColumnLayout {
        anchors.fill: parent

        Toolbar {
            id: toolbar
            Layout.fillWidth: true
        }

        PlasmaExtras.ScrollArea {
            id: scrollView
            Layout.fillWidth: true
            Layout.fillHeight: true
            frameVisible: false

            PlasmaExtras.Heading {
                id: disabledMessage
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                level: 3
                visible: connectionView.count === 0 && text != ""
                enabled: false
                text: {
                    if (toolbarValues.displayplaneModeMessage) {
                        return i18n("Airplane mode is enabled")
                    }
                    if (toolbarValues.displayWifiMessage) {
                        if (toolbarValues.displayWwanMessage) {
                            return i18n("Wireless and mobile networks are deactivated")
                        }
                        return i18n("Wireless is deactivated")
                    }
                    if (toolbarValues.displayWwanMessage) {
                        return i18n("Mobile network is deactivated")
                    }
                    return ""
                }
            }

            PlasmaExtras.Heading {
                id: message
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                level: 3
                opacity: connectionView.count === 0 && !disabledMessage.visible ? 0.6 : 0
                // Check connectionView.count again, to avoid a small delay.
                visible: opacity >= 0.6 && connectionView.count === 0
                Behavior on opacity { NumberAnimation { duration: 5000 } }
                text: i18n("No available connections")
            }

            ListView {
                id: connectionView

                property int currentVisibleButtonIndex: -1
                property bool showSeparator: false

                spacing: units.smallSpacing
                clip: true
                model: appletProxyModel
                currentIndex: -1
                boundsBehavior: Flickable.StopAtBounds
                section.property: showSeparator ? "Section" : ""
                section.delegate: ListItem { separator: true }
                highlight: PlasmaComponents.Highlight { }
                highlightMoveDuration: 0
                highlightResizeDuration: 0
                delegate: ConnectionItem { }
            }
        }
    }

    Connections {
        target: plasmoid
        onExpandedChanged: {
            connectionView.currentVisibleButtonIndex = -1;

            if (expanded) {
                handler.requestScan();
                full.connectionModel = networkModelComponent.createObject(full)
            } else {
                full.connectionModel.destroy()
                toolbar.closeSearch();
            }
        }
    }
}
