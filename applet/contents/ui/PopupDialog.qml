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
import org.kde.plasma.components 2.0 as PlasmaComponents // for Highlight
import org.kde.plasma.components 3.0 as PlasmaComponents3
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.extras 2.0 as PlasmaExtras
import org.kde.plasma.networkmanagement 0.2 as PlasmaNM

PlasmaComponents3.Page {
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

    header: PlasmaExtras.PlasmoidHeading {
        Toolbar {
            id: toolbar
            width: parent.width
        }
    }

    FocusScope {

        anchors.fill: parent
        anchors.topMargin: units.smallSpacing * 2

        PlasmaExtras.ScrollArea {
            id: scrollView
            anchors.fill: parent
            frameVisible: false

            PlasmaExtras.PlaceholderMessage {
                anchors.centerIn: parent
                width: parent.width - (units.largeSpacing * 4)
                visible: connectionView.count === 0
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
                    return i18n("No available connections")
                }
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
                highlightMoveDuration: units.longDuration
                highlightResizeDuration: units.longDuration
                delegate: ConnectionItem { }
            }
        }

        Connections {
            target: plasmoid
            function onExpandedChanged(expanded) {
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
}
