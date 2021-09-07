/*
 * SPDX-FileCopyrightText: 2018 Martin Kacej <m.kacej@atlas.sk>
 * SPDX-FileCopyrightText: 2020 Dimitris Kardarakos <dimkard@posteo.net>
 * SPDX-FileCopyrightText: 2021 Devin Lin <espidev@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
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
        id: nmHandler
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
                
                Controls.CheckBox {
                    id: mobileDataCheckbox
                    Kirigami.FormData.label: i18n("Mobile data:")
                    text: checked ? i18n("On") : i18n("Off")
                    enabled: enabledConnections.wwanHwEnabled && availableDevices.modemDeviceAvailable
                    
                    checked: enabledConnections.wwanEnabled
                    onCheckedChanged: {
                        if (enabledConnections.wwanEnabled != checked) {
                            nmHandler.enableWwan(checked)
                        }
                    }
                }
                
                Controls.Button {
                    Kirigami.FormData.label: i18n("Data Usage:")
                    text: i18n("View Data Usage")
                    icon.name: "office-chart-bar"
                    enabled: false // TODO
                }
                
                Kirigami.Separator {
                    Kirigami.FormData.label: kcm.sims.count == 1 ? i18n("SIM") : i18n("SIMs")
                    Kirigami.FormData.isSection: true
                }
                
                Repeater {
                    model: kcm.sims
                    
                    delegate: Kirigami.BasicListItem {
                        label: i18n("SIM %1", modelData.displayId)
                        icon: "auth-sim-symbolic"
                        onClicked: kcm.push("Sim.qml", { "sim": modelData })
                    }
                }
            }
        }
    }
}
