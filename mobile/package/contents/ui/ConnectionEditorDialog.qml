/*
 *   Copyright 2017 Martin Kacej <m.kacej@atlas.sk>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

import QtQuick 2.2
import QtQuick.Controls 1.4 as Controls
import QtQuick.Layouts 1.2
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.extras 2.0 as PlasmaExtras

PlasmaExtras.ScrollArea{

    frameVisible: true

    anchors.fill: parent
    property var details
    property var str: 0

    ColumnLayout{
        id: columnlayout

        PlasmaComponents.Label {
            text: i18n("SSID")
        }

        PlasmaComponents.TextField {
            Layout.fillWidth: true
            placeholderText: i18n("None")
        }

        RowLayout {
            PlasmaComponents.Label {
                anchors.left: parent.left
                text: i18n("Advanced options")
            }
            PlasmaComponents.Switch {
                id: advancedOptionsSwitch
                checked: false
            }
        }

        IPDetailsSection {
            id: ipsec
            //anchors.top: columnlayout.bottom
            enabled: advancedOptionsSwitch.checked
            visible: advancedOptionsSwitch.checked
        }

        SecuritySection{
            anchors.top: ipsec.bottom
            //anchors.topMargin: 30
            enabled: advancedOptionsSwitch.checked
            visible: advancedOptionsSwitch.checked
        }

    }
    function save() {
        console.info('Connection saved')
    }
}
