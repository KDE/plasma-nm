/*
    SPDX-FileCopyrightText: 2013-2017 Jan Grulich <jgrulich@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
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
        anchors.topMargin: PlasmaCore.Units.smallSpacing * 2

        PlasmaExtras.ScrollArea {
            id: scrollView
            anchors.fill: parent
            frameVisible: false

            PlasmaExtras.PlaceholderMessage {
                anchors.centerIn: parent
                width: parent.width - (PlasmaCore.Units.largeSpacing * 4)
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

                spacing: PlasmaCore.Units.smallSpacing
                clip: true
                model: appletProxyModel
                currentIndex: -1
                boundsBehavior: Flickable.StopAtBounds
                section.property: showSeparator ? "Section" : ""
                section.delegate: ListItem { separator: true }
                highlight: PlasmaComponents.Highlight { }
                highlightMoveDuration: PlasmaCore.Units.longDuration
                highlightResizeDuration: PlasmaCore.Units.longDuration
                delegate: ConnectionItem {
                    width: connectionView.width
                }
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
                }
            }
        }
    }
}
