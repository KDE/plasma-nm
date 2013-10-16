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
import org.kde.plasma.extras 0.1 as PlasmaExtras
import org.kde.networkmanagement 0.1 as PlasmaNm

PlasmaExtras.ScrollArea {

    flickableItem: Flickable {

        contentHeight: connectionWidget.height + ipv4Widget.height; // + 24 (topMargin) + 10

        ConnectionSettingWidget {
            id: connectionWidget;

            anchors {
                left: parent.left;
                right: parent.right;
                top: parent.top
            }
        }

        Ipv4SettingWidget {
            id: ipv4Widget;

            anchors {
                left: parent.left;
                right: parent.right;
                top: connectionWidget.bottom;
                topMargin: 24;
            }
        }
    }

    function resetSettings() {
        connectionWidget.resetSetting();
        ipv4Widget.resetSetting();
    }

    function loadSettings(settingsMap) {
        if (settingsMap["connection"])
            connectionWidget.loadSetting(settingsMap["connection"]);
        if (settingsMap["ipv4"])
            ipv4Widget.loadSetting(settingsMap["ipv4"]);
    }

    function getSettings() {
        var resultingMap = {};
        resultingMap["connection"] = connectionWidget.getSetting();
        resultingMap["connection"]["type"] = PlasmaNm.Enums.Wired;
        resultingMap["ipv4"] = ipv4Widget.getSetting();
        return resultingMap;
    }
}
