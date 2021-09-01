/*
 *   Copyright 2021 Devin Lin <espidev@gmail.com>
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
import org.kde.kirigami 2.12 as Kirigami
import org.kde.kcm 1.2
import cellularnetworkkcm 1.0

Kirigami.ScrollablePage {
    id: modemPage
    title: i18n("Modem") + " " + modem.displayId
    padding: 0
    
    property Modem modem
    property bool showExtra: false
    
    ColumnLayout {
        MessagesList {
            Layout.fillWidth: true
            Layout.margins: Kirigami.Units.smallSpacing
            visible: count != 0
            model: kcm.messages
        }
        
        Kirigami.FormLayout {
            Layout.margins: Kirigami.Units.gridUnit
            ColumnLayout {
                Kirigami.FormData.label: i18n("<b>Modem Control:</b>")
                
                Controls.Button {
                    icon.name: "network-modem"
                    text: modem.details.isEnabled ? i18n("Disable Modem") : i18n("Enable Modem")
                    onClicked: modem.setEnabled(!modem.details.isEnabled)
                }
                
                Controls.Button {
                    icon.name: "system-reboot"
                    text: i18n("Force Modem Restart")
                    onClicked: modem.reset()
                }
            }
            
            Kirigami.Separator {
                Kirigami.FormData.label: "Modem Details"
                Kirigami.FormData.isSection: true
            }
            ColumnLayout {
                Kirigami.FormData.label: i18n("<b>Access Technologies:</b>")
                Repeater {
                    model: modem.details.accessTechnologies
                    Controls.Label {
                        text: modelData
                    }
                }
            }
            Controls.Label {
                Kirigami.FormData.label: i18n("<b>IMEI:</b>")
                text: modem.details.equipmentIdentifier
            }
            Controls.Label {
                Kirigami.FormData.label: i18n("<b>Enabled:</b>")
                text: modem.details.isEnabled
            }
            Controls.Label {
                Kirigami.FormData.label: i18n("<b>Manufacturer:</b>")
                text: modem.details.manufacturer
            }
            Controls.Label {
                Kirigami.FormData.label: i18n("<b>Model:</b>")
                text: modem.details.model
            }
            ColumnLayout {
                Kirigami.FormData.label: i18n("<b>Owned Numbers:</b>")
                Repeater {
                    model: modem.details.ownNumbers
                    Controls.Label {
                        text: modelData
                    }
                }
            }
            Controls.Label {
                Kirigami.FormData.label: i18n("<b>Revision:</b>")
                text: modem.details.revision
            }
            Controls.Label {
                Kirigami.FormData.label: i18n("<b>Signal Quality:</b>")
                text: modem.details.signalQuality
            }
            Controls.Label {
                Kirigami.FormData.label: i18n("<b>State:</b>")
                text: modem.details.state
            }
            Controls.Label {
                Kirigami.FormData.label: i18n("<b>Failure Reason:</b>")
                text: modem.details.stateFailedReason
            }
            ColumnLayout {
                Kirigami.FormData.label: i18n("<b>Supported Capabilities:</b>")
                Repeater {
                    model: modem.details.supportedCapabilities
                    Controls.Label {
                        text: modelData
                    }
                }
            }
            Controls.Label {
                Kirigami.FormData.label: i18n("<b>Operator Code:</b>")
                text: modem.details.operatorCode
            }
            Controls.Label {
                Kirigami.FormData.label: i18n("<b>Operator Name:</b>")
                text: modem.details.operatorName
            }
            Controls.Label {
                Kirigami.FormData.label: i18n("<b>Registration State:</b>")
                text: modem.details.registrationState
            }
            Controls.Label { 
                Kirigami.FormData.label: i18n("<b>Roaming:</b>")
                text: modem.isRoaming ? "Yes" : "No"
            }
            Controls.Label { 
                Kirigami.FormData.label: i18n("<b>Firmware Version:</b>")
                text: modem.details.firmwareVersion
            }
            Controls.Label { 
                Kirigami.FormData.label: i18n("<b>Interface Name:</b>")
                text: modem.details.interfaceName
            }
            Controls.Label { 
                Kirigami.FormData.label: i18n("<b>Metered:</b>")
                text: modem.details.metered
            }
            Controls.Label { 
                Kirigami.FormData.label: i18n("<b>Incoming data:</b>")
                text: modem.details.rxBytes
            }
            Controls.Label { 
                Kirigami.FormData.label: i18n("<b>Outgoing data:</b>")
                text: modem.details.txBytes
            }
            
            // extra info
            Controls.Button {
                text: showExtra ? i18n("Hide Detailed Information") : i18n("Show Detailed Information")
                onClicked: showExtra = !showExtra
            }
            
            Controls.Label {
                Kirigami.FormData.label: i18n("<b>Active NetworkManager Connection:</b>")
                text: modem.activeConnectionUni
                visible: showExtra
            }
            Controls.Label {
                Kirigami.FormData.label: i18n("<b>Device:</b>")
                text: modem.details.device
                visible: showExtra
            }
            Controls.Label {
                Kirigami.FormData.label: i18n("<b>Device ID:</b>")
                text: modem.details.deviceIdentifier
                visible: showExtra
            }
            ColumnLayout {
                Kirigami.FormData.label: i18n("<b>Drivers:</b>")
                visible: showExtra
                Repeater {
                    model: modem.details.drivers
                    Controls.Label {
                        text: modelData
                    }
                }
            }
            Controls.Label {
                Kirigami.FormData.label: i18n("<b>Maximum Active Bearers:</b>")
                text: modem.details.maxActiveBearers
                visible: showExtra
            }
            Controls.Label {
                Kirigami.FormData.label: i18n("<b>Maximum Total Bearers:</b>")
                text: modem.details.maxBearers
                visible: showExtra
            }
            Controls.Label {
                Kirigami.FormData.label: i18n("<b>Plugin:</b>")
                text: modem.details.plugin
                visible: showExtra
            }
            ColumnLayout {
                Kirigami.FormData.label: i18n("<b>Ports:</b>")
                visible: showExtra
                Repeater {
                    model: modem.details.ports
                    Controls.Label {
                        text: modelData
                    }
                }
            }
            Controls.Label {
                Kirigami.FormData.label: i18n("<b>Power State:</b>")
                text: modem.details.powerState
                visible: showExtra
            }
            Controls.Label {
                Kirigami.FormData.label: i18n("<b>Primary Port:</b>")
                text: modem.details.primaryPort
                visible: showExtra
            }
            Controls.Label {
                Kirigami.FormData.label: i18n("<b>SIM Path:</b>")
                text: modem.details.simPath
                visible: showExtra
            }
        }
    }
}

