/*
 *   Copyright 2018 Martin Kacej <m.kacej@atlas.sk>
 *             2020 Dimitris Kardarakos <dimkard@posteo.net>
 *             2020-2021 Devin Lin <espidev@gmail.com>
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

import QtQuick 2.12
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.12 as Controls
import org.kde.plasma.networkmanagement 0.2 as PlasmaNM
import org.kde.kirigami 2.12 as Kirigami
import org.kde.kcm 1.2
import cellularnetworkkcm 1.0

SimpleKCM {
    id: main
    objectName: "mobileDataMain"

    PlasmaNM.Handler {
        id: handler
    }

    PlasmaNM.AvailableDevices {
        id: availableDevices
    }

    PlasmaNM.EnabledConnections {
        id: enabledConnections

        onWwanEnabledChanged: {
            mobileDataCheckbox.checked = mobileDataCheckbox.enabled && enabled
        }

        onWwanHwEnabledChanged: {
            mobileDataCheckbox.enabled = enabled && availableDevices.modemDeviceAvailable
        }
    }
    
    Kirigami.PlaceholderMessage {
        id: noModem
        anchors.centerIn: parent
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: Kirigami.Units.largeSpacing
        
        visible: !enabledConnections.wwanHwEnabled || !availableDevices.modemDeviceAvailable || !kcm.modemFound
        icon.name: "auth-sim-missing"
        text: i18n("Modem not available")
    }
    
    Flickable {
        anchors.left: parent.left
        anchors.right: parent.right
        ColumnLayout {
            anchors.left: parent.left
            anchors.right: parent.right
            spacing: 0
            visible: !noModem.visible
            
            MessagesList {
                Layout.fillWidth: true
                Layout.margins: Kirigami.Units.largeSpacing
                model: kcm.messages
            }
            
            Kirigami.FormLayout {
                Layout.leftMargin: Kirigami.Units.gridUnit
                Layout.rightMargin: Kirigami.Units.gridUnit
                Layout.fillWidth: true
                wideMode: false
                
                Controls.Switch {
                    id: mobileDataCheckbox
                    Kirigami.FormData.label: i18n("<b>Mobile data:</b>")
                    text: checked ? i18n("On") : i18n("Off")
                    enabled: enabledConnections.wwanHwEnabled && availableDevices.modemDeviceAvailable
                    
                    checked: kcm.selectedModem.mobileDataActive
                    onCheckedChanged: {
                        kcm.selectedModem.mobileDataActive = checked;
                    }
                }
                
                Controls.Button {
                    Kirigami.FormData.label: i18n("<b>Data Usage:</b>")
                    text: i18n("View Data Usage")
                    icon.name: "office-chart-bar"
                    enabled: false // TODO
                }
                
                Kirigami.Separator {
                    Kirigami.FormData.label: kcm.sims.count == 1 ? "SIM" : "SIMs"
                    Kirigami.FormData.isSection: true
                }
                
                Repeater {
                    model: kcm.sims
                    
                    delegate: Kirigami.BasicListItem {
                        label: "SIM " + modelData.displayId
                        icon: "auth-sim-symbolic"
                        onClicked: kcm.push("Sim.qml", { "sim": modelData })
                    }
                }
            }
        }
    }
}
