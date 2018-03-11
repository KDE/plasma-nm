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

import QtQuick 2.6
import QtQuick.Controls 2.2 as Controls
import QtQuick.Layouts 1.2
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.extras 2.0 as PlasmaExtras

PlasmaExtras.ScrollArea{
    property var details
    property var str: 0
    property var connection : ({})
    property var wirelessSettings: ({})
    property var enabledSaving: (editorIpSection.enabledSave && editorSecuritySection.enabledSave && ssidField.text)

    ColumnLayout{
        id: columnlayout

        PlasmaComponents.Label {
            text: i18n("SSID")
            font.weight: Font.Bold
        }

        PlasmaComponents.TextField {
            id: ssidField
            Layout.fillWidth: true
            placeholderText: i18n("None")
        }

        IPDetailsSection {
            id: editorIpSection
        }

        SecuritySection{
            id: editorSecuritySection
            //anchors.top: editorIpSection.bottom
            anchors.topMargin: units.gridUnit
        }

    }
    function save() {
        var m = ({});
        connection["id"] = ssidField.text
        connection["type"] = "802-11-wireless"
        wirelessSettings["mode"] = "infrastructure"
        m["connection"] = connection
        m["ipv4"] = editorIpSection.ipmap
        m["802-11-wireless"] = wirelessSettings
        handler.addConnectionFromQML(m)
        console.info('Connection saved '+ connection["id"])
    }
}
