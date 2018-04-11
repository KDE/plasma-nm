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
import org.kde.kirigami 2.2 as Kirigami

Kirigami.ScrollablePage{
    property var details
    property var str: 0
    property var connection : ({})
    property var enabledSaving: (editorIpSection.enabledSave && editorSecuritySection.enabledSave && ssidField.text)

    title: i18n("Connection Editor")

    ColumnLayout{
        id: columnlayout
        anchors.horizontalCenter: parent.horizontalCenter

        Controls.Label {
            text: i18n("SSID")
            font.weight: Font.Bold
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Controls.TextField {
            id: ssidField
            anchors.horizontalCenter: parent.horizontalCenter
            placeholderText: i18n("None")
        }

        IPDetailsSection {
            id: editorIpSection
            width: parent.width
            anchors.horizontalCenter: parent.horizontalCenter
        }

        SecuritySection {
            id: editorSecuritySection
            anchors.topMargin: units.gridUnit
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width
        }
    }

    actions {
        left: Kirigami.Action {
            iconName: "dialog-ok"
            enabled: enabledSaving
            onTriggered: {
                save()
                applicationWindow().pageStack.pop()
            }
        }
        right: Kirigami.Action {
            iconName: "dialog-cancel"
            onTriggered: {
                applicationWindow().pageStack.pop()
            }
        }
    }



    function save() {
        connection = editorIpSection.ipmap
        connection["id"] = ssidField.text
        connection["mode"] = "infrastructure"
        connection["802-11-wireless-security"] = editorSecuritySection.securityMap
        console.info(connection)
        handler.addConnectionFromQML(connection)
        console.info('Connection saved '+ connection["id"])
    }
}
