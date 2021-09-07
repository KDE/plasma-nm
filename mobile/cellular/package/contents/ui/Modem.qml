/*
 * SPDX-FileCopyrightText: 2021 Devin Lin <espidev@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.12
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.12 as Controls
import org.kde.kirigami 2.12 as Kirigami
import org.kde.kcm 1.2
import cellularnetworkkcm 1.0

Kirigami.ScrollablePage {
    id: modemPage
    title: i18n("Modem %1", modem.displayId)
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
                Kirigami.FormData.label: i18n("Modem Control:")
                
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
                Kirigami.FormData.label: i18n("Modem Details")
                Kirigami.FormData.isSection: true
            }
            ColumnLayout {
                Kirigami.FormData.label: i18n("Access Technologies:")
                Repeater {
                    model: modem.details.accessTechnologies
                    Controls.Label {
                        text: modelData
                    }
                }
            }
            Controls.Label {
                Kirigami.FormData.label: i18n("IMEI:")
                text: modem.details.equipmentIdentifier
            }
            Controls.Label {
                Kirigami.FormData.label: i18n("Enabled:")
                text: modem.details.isEnabled
            }
            Controls.Label {
                Kirigami.FormData.label: i18n("Manufacturer:")
                text: modem.details.manufacturer
            }
            Controls.Label {
                Kirigami.FormData.label: i18n("Model:")
                text: modem.details.model
            }
            ColumnLayout {
                Kirigami.FormData.label: i18n("Owned Numbers:")
                Repeater {
                    model: modem.details.ownNumbers
                    Controls.Label {
                        text: modelData
                    }
                }
            }
            Controls.Label {
                Kirigami.FormData.label: "<b>" + i18n("Revision:") + "</b>"
                text: modem.details.revision
            }
            Controls.Label {
                Kirigami.FormData.label: "<b>" + i18n("Signal Quality:") + "</b>"
                text: modem.details.signalQuality
            }
            Controls.Label {
                Kirigami.FormData.label: "<b>" + i18n("State:") + "</b>"
                text: modem.details.state
            }
            Controls.Label {
                Kirigami.FormData.label: "<b>" + i18n("Failure Reason:") + "</b>"
                text: modem.details.stateFailedReason
            }
            ColumnLayout {
                Kirigami.FormData.label: "<b>" + i18n("Supported Capabilities:") + "</b>"
                Repeater {
                    model: modem.details.supportedCapabilities
                    Controls.Label {
                        text: modelData
                    }
                }
            }
            Controls.Label {
                Kirigami.FormData.label: "<b>" + i18n("Operator Code:") + "</b>"
                text: modem.details.operatorCode
            }
            Controls.Label {
                Kirigami.FormData.label: "<b>" + i18n("Operator Name:") + "</b>"
                text: modem.details.operatorName
            }
            Controls.Label {
                Kirigami.FormData.label: "<b>" + i18n("Registration State:") + "</b>"
                text: modem.details.registrationState
            }
            Controls.Label { 
                Kirigami.FormData.label: "<b>" + i18n("Roaming:") + "</b>"
                text: modem.isRoaming ? i18n("Yes") : i18n("No")
            }
            Controls.Label { 
                Kirigami.FormData.label: "<b>" + i18n("Firmware Version:") + "</b>"
                text: modem.details.firmwareVersion
            }
            Controls.Label { 
                Kirigami.FormData.label: "<b>" + i18n("Interface Name:") + "</b>"
                text: modem.details.interfaceName
            }
            Controls.Label { 
                Kirigami.FormData.label: "<b>" + i18n("Metered:") + "</b>"
                text: modem.details.metered
            }
            
            // extra info
            Controls.Button {
                text: showExtra ? i18n("Hide Detailed Information") : i18n("Show Detailed Information")
                onClicked: showExtra = !showExtra
            }
            
            Controls.Label {
                Kirigami.FormData.label: "<b>" + i18n("Active NetworkManager Connection:") + "</b>"
                text: modem.activeConnectionUni
                visible: showExtra
            }
            Controls.Label {
                Kirigami.FormData.label: "<b>" + i18n("Device:") + "</b>"
                text: modem.details.device
                visible: showExtra
            }
            Controls.Label {
                Kirigami.FormData.label: "<b>" + i18n("Device ID:") + "</b>"
                text: modem.details.deviceIdentifier
                visible: showExtra
            }
            ColumnLayout {
                Kirigami.FormData.label: "<b>" + i18n("Drivers:") + "</b>"
                visible: showExtra
                Repeater {
                    model: modem.details.drivers
                    Controls.Label {
                        text: modelData
                    }
                }
            }
            Controls.Label {
                Kirigami.FormData.label: "<b>" + i18n("Plugin:") + "</b>"
                text: modem.details.plugin
                visible: showExtra
            }
            Controls.Label {
                Kirigami.FormData.label: "<b>" + i18n("Power State:") + "</b>"
                text: modem.details.powerState
                visible: showExtra
            }
            Controls.Label {
                Kirigami.FormData.label: "<b>" + i18n("SIM Path:") + "</b>"
                text: modem.details.simPath
                visible: showExtra
            }
        }
    }
}

