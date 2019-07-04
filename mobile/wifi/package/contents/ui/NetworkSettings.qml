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
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.2 as Controls
import org.kde.kirigami 2.2 as Kirigami

Kirigami.ScrollablePage{
    property var path
    property var settings: ({})
    property var activeMap: ({})
    property bool enabledSave: true && detailsIP.enabledSave

    header: ColumnLayout {
        width: parent.width
        anchors.leftMargin: Kirigami.Units.largeSpacing * 2
        Kirigami.Separator {}
        RowLayout{
            Kirigami.Separator {}
            Controls.Label {
                id: detailsName
                anchors.leftMargin: Kirigami.Units.largeSpacing * 2
                text: i18n("Connection Name")
                font.weight: Font.Bold
            }
        }
        Kirigami.Separator {}
        Rectangle{
            height: 1
            Layout.fillWidth: true
            color: "black"
        }
    }

    Column {
        id: detailsView
        spacing: Kirigami.Units.gridUnit
        WirelessSecuritySetting {
            id: detailsSecuritySection
            anchors.bottomMargin: 10
        }

        IPAddressSetting {
            id: detailsIP
        }
    }

    footer: Item {
        height: Kirigami.Units.gridUnit * 4

        RowLayout {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: Kirigami.Units.gridUnit

            Controls.Button {
                enabled: enabledSave

                icon.name: "document-save"
                text: i18n("Save")

                onPressed: {
                    save()
                    applicationWindow().pageStack.pop()
                }
            }
            Controls.Button {
                icon.name: "dialog-cancel"
                text: i18n("Cancel")

                onPressed: {
                    applicationWindow().pageStack.pop()
                }
            }
        }
    }

    function loadNetworkSettings() {
        console.info(path);
        settings = kcm.getConnectionSettings(path,"connection");
        detailsName.text = settings["id"]
        detailsSecuritySection.securityMap = kcm.getConnectionSettings(path,"802-11-wireless-security");
        detailsIP.ipmap = kcm.getConnectionSettings(path,"ipv4");
        detailsSecuritySection.setStateFromMap();
        detailsIP.setStateFromMap();
    }

    function save() {
        settings = detailsIP.ipmap;
        if (detailsSecuritySection.password !== "") { //otherwise password is unchanged
            settings["802-11-wireless-security"] = detailsSecuritySection.securityMap;
        }
        kcm.updateConnectionFromQML(path,settings);
    }
}
