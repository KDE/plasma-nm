/*
    SPDX-FileCopyrightText: 2016 Jan Grulich <jgrulich@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

import QtQuick 2.1
import QtQuick.Controls 1.4 as QQC
import org.kde.kirigami 2.5 as Kirigami
import org.kde.kquickcontrolsaddons 2.0 as KQuickControlsAddons
import org.kde.plasma.components 3.0 as PlasmaComponents3
import org.kde.plasma.networkmanagement 0.2 as PlasmaNM

ListItem {
    id: connectionItem

    checked: mouseArea.containsMouse || ConnectionPath === connectionView.currentConnectionPath
    height: connectionItemBase.height

    signal aboutToChangeConnection(bool exportable, string name, string path)
    signal aboutToExportConnection(string path)
    signal aboutToRemoveConnection(string name, string path)

    Item {
        id: connectionItemBase

        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            topMargin: -Math.round(Kirigami.Units.gridUnit / 3)
        }
        height: Math.max(Kirigami.Units.iconSizes.medium, connectionNameLabel.height + connectionStatusLabel.height) + Math.round(Kirigami.Units.gridUnit / 2)

        KQuickControlsAddons.QIconItem {
            id: connectionIcon

            anchors {
                left: parent.left
                verticalCenter: parent.verticalCenter
            }
            height: Kirigami.Units.iconSizes.medium; width: height
            icon: KcmConnectionIcon
        }

        Text {
            id: connectionNameLabel

            anchors {
                bottom: connectionIcon.verticalCenter
                left: connectionIcon.right
                leftMargin: Math.round(Kirigami.Units.gridUnit / 2)
                right: connectingIndicator.visible ? connectingIndicator.left : parent.right
            }
            color: textColor
            height: paintedHeight
            elide: Text.ElideRight
            font.weight: ConnectionState == PlasmaNM.Enums.Activated ? Font.DemiBold : Font.Normal
            font.italic: ConnectionState == PlasmaNM.Enums.Activating ? true : false
            text: Name
            textFormat: Text.PlainText
        }

        Text {
            id: connectionStatusLabel

            anchors {
                left: connectionIcon.right
                leftMargin: Math.round(Kirigami.Units.gridUnit / 2)
                right: connectingIndicator.visible ? connectingIndicator.left : parent.right
                top: connectionNameLabel.bottom
            }
            color: textColor
            height: paintedHeight
            elide: Text.ElideRight
            font.pointSize: Kirigami.Theme.smallFont.pointSize
            opacity: 0.6
            text: itemText()
        }

        PlasmaComponents3.BusyIndicator {
            id: connectingIndicator

            anchors {
                right: parent.right
                rightMargin: Math.round(Kirigami.Units.gridUnit / 2)
                verticalCenter: connectionIcon.verticalCenter
            }
            height: Kirigami.Units.iconSizes.medium; width: height
            running: ConnectionState == PlasmaNM.Enums.Activating
            visible: running
        }
    }

    QQC.Menu {
        id: connectionItemMenu

        QQC.MenuItem {
            text: ConnectionState == PlasmaNM.Enums.Deactivated ? i18n("Connect") : i18n("Disconnect")
            visible: ItemType == 1
            onTriggered: {
                if (ConnectionState == PlasmaNM.Enums.Deactivated) {
                    handler.activateConnection(ConnectionPath, DevicePath, SpecificPath);
                } else {
                    handler.deactivateConnection(ConnectionPath, DevicePath);
                }
            }
        }

        QQC.MenuItem {
            iconName: "list-remove"
            text: i18n("Delete");

            onTriggered: {
                aboutToRemoveConnection(Name, ConnectionPath)
            }
        }

        QQC.MenuItem {
            iconName: "document-export"
            visible: KcmVpnConnectionExportable
            text: i18n("Export");

            onTriggered: aboutToExportConnection(ConnectionPath)
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        acceptedButtons: Qt.AllButtons;
        hoverEnabled: true

        onClicked: {
            if (mouse.button === Qt.LeftButton) {
                aboutToChangeConnection(KcmVpnConnectionExportable, Name, ConnectionPath)
            } else if (mouse.button == Qt.RightButton) {
                connectionItemMenu.popup()
            }
        }
    }

    /* This generates the status description under each connection
       in the list at the left side of the applet. */
    function itemText() {
        if (ConnectionState == PlasmaNM.Enums.Activated) {
            return i18n("Connected")
        } else if (ConnectionState == PlasmaNM.Enums.Activating) {
            return i18n("Connecting")
        } else {
            return LastUsed
        }
    }
}
