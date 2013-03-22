/*
    Copyright 2012-2013  Jan Grulich <jgrulich@redhat.com>

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
    property alias enabled: toolButton.enabled;

    signal toolbarExpanded();
    signal enableNetworking(bool enable);
    signal enableWireless(bool enable);
    signal enableWwan(bool enable);

    function hideOptions()
    {
        toolBar.state = '';
        expanded = false;
    }

    height: 30;

    PlasmaNM.AppletInfo {
        id: statusInfo;

        onSetConnectedStatus: {
            statusIcon.icon = QIcon("user-online");
            statusLabel.text = i18n("Connected");
        }

        onSetDisconnectedStatus: {
            statusIcon.icon = QIcon("user-offline");
            statusLabel.text = i18n("Not connected");
        }
    }

    QIconItem {
        id: statusIcon

        height: 20; width: 20;
        anchors { left: parent.left; bottom: parent.bottom; top: statusLabel.top}
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
                toolBar.state = 'Options';
                toolbarExpanded();
            } else {
                toolBar.state = '';
            }
            expanded = !expanded;
        }
    }

    PlasmaNM.AppletInfo {
        id: networkInfo;

        onNetworkingEnabled: {
            options.networkingEnabled = enabled;
        }

        onWirelessEnabled: {
            options.wirelessEnabled = enabled;
        }

        onWwanEnabled: {
            options.wwanEnabled = enabled;
        }
    }

    OptionsWidget {
        id: options;

        anchors { left: parent.left; right: parent.right }
        visible: false;

        onNetworkingEnabledChanged: enableNetworking(enabled);
        onWirelessEnabledChanged: enableWireless(enabled);
        onWwanEnabledChanged: enableWwan(enabled);

        Component.onCompleted: {
            networkInfo.initNetworkInfo();
        }
    }

    states: [
        State {
            name: "Options";
            PropertyChanges { target: toolBar; height: 150 }
            PropertyChanges { target: options; visible: true }
        }
    ]

    transitions: Transition {
        NumberAnimation { duration: 200; properties: "height, visible" }
    }

    Component.onCompleted: {
        statusInfo.initStatusInfo();
    }
}
