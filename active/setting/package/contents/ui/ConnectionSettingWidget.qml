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

Item {
    id: connectionSettingWidget;

    height: childrenRect.height;

    Item {
        id: connectionName;

        height: childrenRect.height;
        anchors {
            left: parent.left;
            right: parent.right;
            top: parent.top;
            topMargin: 12;
        }

        PlasmaComponents.Label {
            id: connectionNameLabel;

            anchors {
                top: parent.top;
                right: parent.horizontalCenter;
                rightMargin: 12;
            }
            text: i18n("Connection name:");
        }

        PlasmaComponents.TextField {
            id: connectionNameInput;

            width: 200;
            anchors {
                top: parent.top;
                left: parent.horizontalCenter;
            }
        }
    }

    Item {
        id: automaticallyConnect;

        height: childrenRect.height;

        anchors {
            left: parent.left;
            right: parent.right;
            top: connectionName.bottom;
            topMargin: 12;
        }

        PlasmaComponents.Label {
            id: automaticallyConnectLabel;

            anchors {
                top: parent.top;
                right: parent.horizontalCenter;
                rightMargin: 12;
            }
            text: i18n("Automatically connect:");
        }

        PlasmaComponents.CheckBox {
            id: automaticallyConnectCheckbox;

            anchors {
                top: parent.top;
                left: parent.horizontalCenter;
            }
            checked: true;
        }
    }

    function resetSetting() {
        connectionNameInput.text = "";
        automaticallyConnectCheckbox.checked = true;
    }

    function loadSetting(settingMap) {
        resetSetting();

        connectionNameInput.text = settingMap["id"];
        if (settingMap["autoconnect"]) {
            automaticallyConnectCheckbox.checked = true;
        } else {
            automaticallyConnectCheckbox.checked = false;
        }
    }

    function getSetting() {
        var settingMap = {};

        settingMap["id"] = connectionNameInput.text;
        if (automaticallyConnectCheckbox.checked) {
            settingMap["autoconnect"] = true;
        } else {
            settingMap["autoconnect"] = false;
        }

        return settingMap;
    }
}
