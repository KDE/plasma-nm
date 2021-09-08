/*
 * SPDX-FileCopyrightText: 2021 Devin Lin <espidev@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.12
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.12 as Controls
import org.kde.kirigami 2.12 as Kirigami
import org.kde.plasma.networkmanagement 0.2 as PlasmaNM
import org.kde.kcm 1.2
import cellularnetworkkcm 1.0

Kirigami.ScrollablePage {
    id: simPage
    title: i18n("SIM") + " " + sim.displayId
    
    property Sim sim
    
    padding: 0
    
    PlasmaNM.EnabledConnections {
        id: enabledConnections
    }
    
    ColumnLayout {
        spacing: 0
        
        Kirigami.InlineMessage {
            Layout.fillWidth: true
            Layout.margins: Kirigami.Units.largeSpacing
            Layout.bottomMargin: visible && !messagesList.visible ? Kirigami.Units.largeSpacing : 0
            visible: !sim.enabled
            type: Kirigami.MessageType.Error
            text: qsTr("This SIM slot is empty, a SIM card needs to be inserted in order for it to be used.")
        }
        
        MessagesList {
            id: messagesList
            Layout.fillWidth: true
            Layout.margins: Kirigami.Units.largeSpacing
            visible: count != 0
            model: kcm.messages
        }
        
        Kirigami.FormLayout {
            Layout.margins: Kirigami.Units.gridUnit
            Layout.fillWidth: true
            wideMode: false
            
            Controls.CheckBox {
                Kirigami.FormData.label: i18n("Data roaming:")
                text: checked ? i18n("On") : i18n("Off")
                enabled: sim.enabled
                checked: sim.modem.isRoaming
                onCheckedChanged: sim.modem.isRoaming = checked
            }
            
            Controls.Button {
                Kirigami.FormData.label: i18n("APNs:")
                icon.name: "globe"
                text: i18n("Modify Access Point Names")
                enabled: sim.enabled && enabledConnections.wwanEnabled
                onClicked: {
                    kcm.push("ProfileList.qml", {"modem": sim.modem});
                }
            }
            
            Controls.Button {
                Kirigami.FormData.label: i18n("Networks:")
                icon.name: "network-mobile-available"
                text: i18n("Select Network Operator")
                enabled: sim.enabled
                onClicked: {
                    kcm.push("AvailableNetworks.qml", { "modem": sim.modem, "sim": sim });
                }
            }
            
            Controls.Button {
                Kirigami.FormData.label: i18n("SIM Lock:")
                icon.name: "unlock"
                text: i18n("Modify SIM Lock")
                enabled: sim.enabled
                onClicked: kcm.push("SimLock.qml", { "sim": sim });
            }
            
            Controls.Button {
                Kirigami.FormData.label: i18n("Modem:")
                icon.name: "network-modem"
                text: i18n("View Modem Details")
                onClicked: kcm.push("Modem.qml", { "modem": sim.modem })
            }
            
            Kirigami.Separator {
                Kirigami.FormData.label: i18n("SIM Details")
                Kirigami.FormData.isSection: true
            }
            Controls.Label {
                Kirigami.FormData.label: i18n("Locked:")
                text: sim.locked ? i18n("Yes") : i18n("No")
            }
            Controls.Label {
                Kirigami.FormData.label: i18n("Locked Reason:")
                text: sim.lockedReason
            }
            Controls.Label {
                Kirigami.FormData.label: i18n("IMSI:")
                text: sim.imsi
            }
            Controls.Label {
                Kirigami.FormData.label: i18n("EID:")
                text: sim.eid
            }
            Controls.Label {
                Kirigami.FormData.label: i18n("Operator Code (modem):")
                text: sim.modem.details.operatorCode
            }
            Controls.Label {
                Kirigami.FormData.label: i18n("Operator Name (modem):")
                text: sim.modem.details.operatorName
            }
            Controls.Label {
                Kirigami.FormData.label: i18n("Operator Code (provided by SIM):")
                text: sim.operatorIdentifier
            }
            Controls.Label {
                Kirigami.FormData.label: i18n("Operator Name (provided by SIM):")
                text: sim.operatorName
            }
            Controls.Label {
                Kirigami.FormData.label: i18n("SIM ID:")
                text: sim.simIdentifier
            }
            ColumnLayout {
                Kirigami.FormData.label: i18n("Emergency Numbers:")
                Repeater {
                    model: sim.emergencyNumbers
                    Controls.Label {
                        text: modelData
                    }
                }
            }
        }
    }
}
