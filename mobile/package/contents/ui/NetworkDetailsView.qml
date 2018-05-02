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
    property alias signal_strength: signalStrengthLabel.text
    property alias signal_speed: linkSpeedLabel.text
    property alias ip_address: ipAddressLabel.text
    property alias security: securityLabel.text
    property var settings: ({})
    property var activeMap: ({})
    property bool enabledSave: true && detailsIP.enabledSave


    Column {
        id: detailsView
        spacing: Kirigami.Units.gridUnit
        Column {
            id: staticInfo
            anchors.bottomMargin: units.gridUnit

            Row {
                spacing: units.gridUnit / 2

                Controls.Label {
                    font.weight: Font.Bold
                    text: i18n("Strength:")
                }

                Controls.Label {
                    id: signalStrengthLabel
                }
            }

            Row {
                spacing: units.gridUnit / 2

                Controls.Label {
                    font.weight: Font.Bold
                    text: i18n("Link Speed:")
                }

                Controls.Label {
                    id: linkSpeedLabel
                }
            }

            Row {
                spacing: units.gridUnit / 2

                Controls.Label {
                    font.weight: Font.Bold
                    text: i18n("Security:")
                }

                Controls.Label {
                    id: securityLabel
                }
            }

            Row {
                spacing: units.gridUnit / 2

                Controls.Label {
                    font.weight: Font.Bold
                    text: i18n("IP Address:")
                }

                Controls.Label {
                    id: ipAddressLabel
                }
            }
        }

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
                enabled: enabledSaving
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

    function fillDetails() {
       /* var d = {}
        for (var i = 0; i < (details.length / 2); i++){
            d[details[(i * 2)]] = details[(i * 2) + 1]
        }

        if (d['Access point (SSID)'])
            detailsDialog.titleText = d['Access point (SSID)']
        signal_strength = d['Signal strength']
        if (d['IPv4 Address'])
            ip_address = detailsIP.address = d['IPv4 Address']
        if (d['Security type'])
            security = d['Security type']
        if (d['Connection speed'])
            signal_speed = d['Connection speed']
        if (activeMap && ipMethodComb.currentIndex == 1) {
            IPDetailsSection.address = activeMap["address"]
            IPDetailsSection.dns = activeMap["dns"]
            IPDetailsSection.prefix = activeMap["prefix"]
            IPDetailsSection.gateway = activeMap["gateway"]
        }*/
    }

    function loadNetworkSettings() {
        console.info(path);
        settings = utils.getConnectionSettings(path,"connection");
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
