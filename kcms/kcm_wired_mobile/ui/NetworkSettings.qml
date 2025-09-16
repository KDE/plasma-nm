// SPDX-FileCopyrightText: 2017 Martin Kacej <m.kacej@atlas.sk>
// SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
// SPDX-FileCopyrightText: 2025 Sebastian Kügler <sebas@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls

import org.kde.kirigami as Kirigami
import org.kde.plasma.networkmanagement as PlasmaNM
import org.kde.kcmutils
import org.kde.kirigamiaddons.formcard 1 as FormCard

Kirigami.ScrollablePage {
    title: path ?  wirelessSettings["ssid"] : i18n("Add New Connection")

    property var path
    property var ipSettings: ({})
    property var ipRegex: /^(([01]?[0-9]?[0-9]|2([0-4][0-9]|5[0-5]))\.){3}([01]?[0-9]?[0-9]|2([0-4][0-9]|5[0-5]))$/
    property bool enabledSave: (ipMethodCombobox.currentIndex == 0
                                || (ipMethodCombobox.currentIndex == 1
                                    && manualIPaddress.acceptableInput
                                    && manualIPgateway.acceptableInput
                                    && manualIPprefix.acceptableInput
                                    && manualIPdns.acceptableInput))

    actions: [
        Kirigami.Action {
            icon.name: "dialog-ok"
            text: i18n("Save")
            enabled: enabledSave
            onTriggered: {
                save()
                kcm.pop()
            }
        }
    ]

    topPadding: Kirigami.Units.gridUnit
    bottomPadding: Kirigami.Units.gridUnit
    leftPadding: 0
    rightPadding: 0

    ColumnLayout {
        spacing: 0

        FormCard.FormHeader {
            title: i18nc("@title:group", "IP Settings")
        }

        FormCard.FormCard {
            FormCard.FormComboBoxDelegate {
                id: ipMethodCombobox
                text: i18n("Method")
                model: [i18n("Automatic"), i18n("Manual")]
                currentIndex: ipSettings["method"] === "manual" ? 1 : 0
                property var manualIp: currentIndex === 1
                onCurrentIndexChanged: {
                    ipSettings["method"] = currentIndex === 1 ? "manual" : "auto"
                }
            }

            FormCard.FormDelegateSeparator {
                above: ipMethodCombobox
                below: manualIPaddress
                visible: manualIPaddress.visible
            }

            FormCard.FormTextFieldDelegate {
                id: manualIPaddress
                label: i18n("IP Address")
                visible: ipMethodCombobox.manualIp
                placeholderText: "192.168.1.128"
                text: ipSettings["address"] ? ipSettings["address"] : ""
                onTextChanged: ipSettings["address"] = text
                validator: RegularExpressionValidator {
                    regularExpression: ipRegex
                }
            }

            FormCard.FormDelegateSeparator {
                above: manualIPaddress
                below: manualIPgateway
                visible: manualIPgateway.visible
            }

            FormCard.FormTextFieldDelegate {
                id: manualIPgateway
                label: i18n("Gateway")
                visible: ipMethodCombobox.manualIp
                placeholderText: "192.168.1.1"
                text: ipSettings["gateway"] ? ipSettings["gateway"] : ""
                onTextChanged: ipSettings["gateway"] = text
                validator: RegularExpressionValidator {
                    regularExpression: ipRegex
                }
            }

            FormCard.FormDelegateSeparator {
                above: manualIPgateway
                below: manualIPprefix
                visible: manualIPprefix.visible
            }

            FormCard.FormTextFieldDelegate {
                id: manualIPprefix
                label: i18n("Network prefix length")
                visible: ipMethodCombobox.manualIp
                placeholderText: "16"
                text: ipSettings["prefix"] ? ipSettings["prefix"] : ""
                onTextChanged: ipSettings["prefix"] = text
                validator: IntValidator {
                    bottom: 1
                    top: 32
                }
            }

            FormCard.FormDelegateSeparator {
                above: manualIPprefix
                below: manualIPdns
                visible: manualIPdns.visible
            }

            FormCard.FormTextFieldDelegate {
                id: manualIPdns
                label: i18n("DNS")
                visible: ipMethodCombobox.manualIp
                placeholderText: "8.8.8.8"
                text: ipSettings["dns"] ? ipSettings["dns"] : ""
                onTextChanged: ipSettings["dns"] = text
                validator: RegularExpressionValidator {
                    regularExpression: ipRegex
                }
            }
        }
    }

    Component.onCompleted: {
        ipSettings = kcm.getConnectionSettings(path, "ipv4")
    }

    function save() {
        if (path) {
            kcm.updateConnectionFromQML(path, ipSettings);
        } else {
            kcm.addConnectionFromQML(ipSettings);
        }
    }
}
