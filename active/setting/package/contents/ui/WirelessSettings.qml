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
import org.kde.plasmanm 0.1 as PlasmaNm

PlasmaExtras.ScrollArea {

    flickableItem: Flickable {

        contentHeight: connectionWidget.height + wirelessWidget.height + wirelessSecurityWidget.height + ipv4Widget.height + 58; // + 48 (2 x topMargin) + 10

        ConnectionSettingWidget {
            id: connectionWidget;

            anchors {
                left: parent.left;
                right: parent.right;
                top: parent.top
            }
        }

        WirelessSettingWidget {
            id: wirelessWidget;

            anchors {
                left: parent.left;
                right: parent.right;
                top: connectionWidget.bottom;
                topMargin: 24;
            }
        }

        WirelessSecuritySettingWidget {
            id: wirelessSecurityWidget;

            anchors {
                left: parent.left;
                right: parent.right;
                top: wirelessWidget.bottom;
            }
        }

        Ipv4SettingWidget {
            id: ipv4Widget;

            anchors {
                left: parent.left;
                right: parent.right;
                top: wirelessSecurityWidget.bottom;
                topMargin: 24;
            }
        }
    }

    function resetSettings() {
        connectionWidget.resetSetting();
        ipv4Widget.resetSetting();
        wirelessWidget.resetSetting();
        wirelessSecurityWidget.resetSetting();
    }

    function loadSettings(settingsMap) {
        if (settingsMap["connection"])
            connectionWidget.loadSetting(settingsMap["connection"]);
        if (settingsMap["802-11-wireless"]) {
            wirelessWidget.loadSetting(settingsMap["802-11-wireless"]);
            if (settingsMap["802-11-wireless-security"]) {
                wirelessSecurityWidget.loadSetting(settingsMap["802-11-wireless-security"]);
            }
        }
        if (settingsMap["ipv4"])
            ipv4Widget.loadSetting(settingsMap["ipv4"]);
    }

    function loadSecrets(secretsMap) {
        if (secretsMap["802-11-wireless-security"]) {
            wirelessSecurityWidget.loadSecrets(secretsMap["802-11-wireless-security"]);
        } else if (secretsMap["802-1x"]) {
            // TODO
        }
    }

    function getSettings() {
        var resultingMap = {};
        resultingMap["connection"] = connectionWidget.getSetting();
        resultingMap["connection"]["type"] = PlasmaNm.Enums.Wireless;
        resultingMap["ipv4"] = ipv4Widget.getSetting();
        resultingMap["802-11-wireless"] = wirelessWidget.getSetting();
        resultingMap["802-11-wireless-security"] = wirelessSecurityWidget.getSetting();
        return resultingMap;
    }
}
