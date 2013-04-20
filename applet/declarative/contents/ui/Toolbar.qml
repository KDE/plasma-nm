/*
    Copyright 2013 Jan Grulich <jgrulich@redhat.com>

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

import QtQuick 1.1
import org.kde.qtextracomponents 0.1
import org.kde.plasma.graphicswidgets 0.1 as PlasmaWidgets
import org.kde.plasma.components 0.1 as PlasmaComponents
import org.kde.plasma.extras 0.1 as PlasmaExtras
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.plasmanm 0.1 as PlasmaNM

Item {
    id: toolBar;

    property bool expanded: false;

    signal toolbarExpanded();
    signal enableNetworking(bool enable);
    signal enableWireless(bool enable);
    signal enableWwan(bool enable);
    signal openEditorToolbar();

    height: 30;

    PlasmaNM.GlobalStatus {
        id: globalStatus;

        onSetGlobalStatus: {
            statusLabel.text = status;
            progressIndicator.running = inProgress;
            if (connected) {
                statusIcon.icon = QIcon("user-online");
            } else {
                statusIcon.icon = QIcon("user-offline");
            }
        }
    }

    QIconItem {
        id: statusIcon

        height: 25; width: 25;
        anchors { left: parent.left; bottom: parent.bottom; top: statusLabel.top}

        PlasmaComponents.BusyIndicator {
            id: progressIndicator;

            anchors.fill: parent;
            running: false;
            visible: running;
        }
    }

    PlasmaComponents.Label {
        id: statusLabel;

        height: 30;
        anchors { left: statusIcon.right; right: toolButton.left; bottom: parent.bottom; leftMargin: 5 }
    }

    PlasmaComponents.ToolButton {
        id: toolButton;

        height: 30; width: 30;
        anchors { right: parent.right; bottom: parent.bottom }
        iconSource: "configure";

        onClicked: {
            if (!expanded) {
                toolbarExpanded();
                expanded = !expanded;
            // Toolbar may be set as expanded, but was closed from the item
            } else if (expanded && connectionView.itemExpandable == true && toolbar.toolbarExpandable == false) {
                toolbarExpanded();
            } else {
                expanded = !expanded;
            }
        }
    }

    PlasmaNM.EnabledConnections {
        id: enabledConnections;

        onNetworkingEnabled: {
            options.networkingEnabled = enabled;
        }

        onWirelessEnabled: {
            options.wirelessEnabled = enabled;
        }

        onWirelessHwEnabled: {
            options.wirelessHwEnabled = enabled;
        }

        onWwanEnabled: {
            options.wwanEnabled = enabled;
        }

        onWwanHwEnabled: {
            options.wwanHwEnabled = enabled;
        }
    }

    OptionsWidget {
        id: options;

        anchors { left: parent.left; right: parent.right }
        visible: false;

        onNetworkingEnabledChanged: enableNetworking(enabled);
        onWirelessEnabledChanged: enableWireless(enabled);
        onWwanEnabledChanged: enableWwan(enabled);
        onOpenEditor: {
            expanded = false;
            openEditorToolbar();
        }

        Component.onCompleted: {
            enabledConnections.init();
        }
    }

    states: [
        State {
            name: "Options";
            when: expanded && toolbar.toolbarExpandable;
            PropertyChanges { target: toolBar; height: 150 }
            PropertyChanges { target: options; visible: true }
        }
    ]

    transitions: Transition {
        NumberAnimation { duration: 300; properties: "height, visible" }
    }

    Component.onCompleted: {
        globalStatus.init();
    }
}
