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
import org.kde.plasma.components 0.1 as PlasmaComponents
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.networkmanagement 0.1 as PlasmaNM

Item {
    id: toolBar;

    property bool expanded: false;

    height: theme.defaultFont.mSize.height * 1.8;

    PlasmaNM.NetworkStatus {
        id: networkStatus;
    }

    Item {
        id: toolbarLine;

        height: theme.defaultFont.mSize.height * 2;
        anchors {
            left: parent.left;
            right: parent.right;
            bottom: parent.bottom;
        }

        PlasmaCore.IconItem {
            id: statusIcon

            height: theme.smallMediumIconSize;
            width: height;
            anchors {
                left: parent.left;
                verticalCenter: parent.verticalCenter;
                leftMargin: padding.margins.left;
            }
            source: (networkStatus.networkStatus == i18n("Connected") || networkStatus.networkStatus == "Connected") ? "user-online" : "user-offline";
        }

        PlasmaComponents.Label {
            id: statusLabel;

            height: theme.defaultFont.mSize.height * 2;
            anchors {
                left: statusIcon.right;
                right: toolButton.left;
                verticalCenter: parent.verticalCenter;
                leftMargin: padding.margins.left;
            }
            elide: Text.ElideRight;
            text: networkStatus.networkStatus;
        }

        PlasmaCore.IconItem {
            id: toolButton;

            height: theme.smallMediumIconSize;
            width: height;
            anchors {
                right: parent.right;
                verticalCenter: parent.verticalCenter;
                rightMargin: padding.margins.right;
            }
            source: "configure";
        }

        MouseArea {
            id: toolbarMouseArea;

            anchors { fill: parent }

            onClicked: {
                hideOrShowOptions();
            }
        }
    }

    OptionsWidget {
        id: options;

        anchors {
            left: parent.left;
            right: parent.right;
            top: parent.top;
            bottomMargin: padding.margins.bottom;
        }
        visible: false;

        onOpenEditor: {
            if (mainWindow.autoHideOptions) {
                expanded = false;
            }
        }
    }

    states: [
        State {
            name: "Hidden";
            when: !expanded;
        },

        State {
            name: "Expanded";
            when: expanded;
            PropertyChanges { target: toolBar; height: options.childrenRect.height + theme.defaultFont.mSize.height * 2 + padding.margins.top }
            PropertyChanges { target: options; visible: true }
        }
    ]

    transitions: Transition {
        NumberAnimation { duration: 300; properties: "height, visible" }
    }

    function hideOrShowOptions() {
        if (!expanded) {
            expanded = true;
            plasmoid.writeConfig("optionsExpanded", "expanded");
        } else {
            expanded = false;
            plasmoid.writeConfig("optionsExpanded", "hidden");
        }
    }

    Component.onCompleted: {
        if (plasmoid.readConfig("optionsExpanded") == "expanded") {
            expanded = true;
        }
    }
}
