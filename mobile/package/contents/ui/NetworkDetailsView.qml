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
import org.kde.kirigami 2.2 as Kirigami

Kirigami.ScrollablePage{
    property var path
    property var settings: ({})
    property var activeMap: ({})
    property bool enabledSave: true && detailsIP.enabledSave

    header : ColumnLayout {
        width: parent.width
        anchors.leftMargin: Kirigami.Units.largeSpacing * 2
        Kirigami.Separator {}
        RowLayout{
            Kirigami.Separator {}
            Controls.Label {
                id: detailsName
                anchors.leftMargin: Kirigami.Units.largeSpacing * 2
                text: "Connection Name"
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
        SecuritySection {
            id: detailsSecuritySection
            anchors.bottomMargin: 10
        }

        IPDetailsSection {
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
                RowLayout {
                    anchors.centerIn: parent
                    Kirigami.Icon {
                        width: Kirigami.Units.iconSizes.smallMedium
                        height: width
                        source: "document-save"
                    }
                    Text {
                        text: i18n("Save")
                    }
                }
                onPressed: {
                    save()
                    applicationWindow().pageStack.pop()
                }
            }
            Controls.Button {
                RowLayout {
                    anchors.centerIn: parent
                    Kirigami.Icon {
                        width: Kirigami.Units.iconSizes.smallMedium
                        height: width
                        source: "dialog-cancel"
                    }
                    Text {
                        text: i18n("Cancel")
                    }
                }
                onPressed: {
                    applicationWindow().pageStack.pop()
                }
            }
        }
    }

    function loadNetworkSettings() {
        console.info(path);
        settings = utils.getConnectionSettings(path,"connection");
        detailsName.text = settings["id"]
        detailsSecuritySection.securityMap = utils.getConnectionSettings(path,"802-11-wireless-security");
        detailsIP.ipmap = utils.getConnectionSettings(path,"ipv4");
        detailsSecuritySection.setStateFromMap();
        detailsIP.setStateFromMap();
    }

    function save() {
        settings = detailsIP.ipmap;
        if (detailsSecuritySection.password !== "") { //otherwise password is unchanged
            settings["802-11-wireless-security"] = detailsSecuritySection.securityMap;
        }
        utils.updateConnectionFromQML(path,settings);
    }
}
