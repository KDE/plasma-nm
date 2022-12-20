/*
    SPDX-FileCopyrightText: 2019 Jan Grulich <jgrulich@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

import QtQuick 2.15
import QtQuick.Dialogs 1.2
import QtQuick.Controls 2.5 as QQC2
import org.kde.kirigami 2.5 as Kirigami
import org.kde.plasma.networkmanagement 0.2 as PlasmaNM

Dialog {
    id: configurationDialog
    standardButtons: Dialog.Ok | Dialog.Cancel
    title: i18nc("@title:window", "Configuration")

    contentItem: Item {
        implicitHeight: 230
        implicitWidth: 400

        Rectangle {
            id: background
            anchors.fill: parent
            focus: true
            color: Kirigami.Theme.backgroundColor
        }

        Kirigami.FormLayout {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.topMargin: Kirigami.Units.gridUnit

            Kirigami.Heading {
                id: generalLabel
                level: 2
                text: i18n("General")
            }

            QQC2.CheckBox {
                id: unlockModem
                text: i18n("Ask for PIN on modem detection")
                onClicked: configurationChanged()
                Component.onCompleted: checked = PlasmaNM.Configuration.unlockModemOnDetection
            }

            QQC2.CheckBox {
                id: manageVirtualConnections
                text: i18n("Show virtual connections")
                onClicked: configurationChanged()
                Component.onCompleted: checked = PlasmaNM.Configuration.manageVirtualConnections
            }

            Kirigami.Heading {
                id: hotspotLabel
                level: 2
                text: i18n("Hotspot")
                Component.onCompleted: visible = handler.hotspotSupported
            }

            QQC2.TextField {
                id: hotspotName
                Kirigami.FormData.label: i18n("Hotspot name:")
                onTextChanged: configurationChanged()
                Component.onCompleted: {
                    text = PlasmaNM.Configuration.hotspotName
                    visible = handler.hotspotSupported
                }
            }

            QQC2.TextField {
                id: hotspotPassword
                Kirigami.FormData.label: i18n("Hotspot password:")
                validator: RegularExpressionValidator {
                    regularExpression: useApMode
                        ? /^$|^(?:.{8,64}){1}$/
                        : /^$|^(?:.{5}|[0-9a-fA-F]{10}|.{13}|[0-9a-fA-F]{26}){1}$/
                }

                onAcceptableInputChanged: configurationChanged()

                Component.onCompleted: {
                    text = PlasmaNM.Configuration.hotspotPassword
                    visible = handler.hotspotSupported
                }
            }
        }

        Row {
            id: buttonRow
            anchors {
                bottom: parent.bottom
                right: parent.right
                margins: Kirigami.Units.smallSpacing
            }
            spacing: Kirigami.Units.mediumSpacing

            QQC2.Button {
                id: okButton
                icon.name: "dialog-ok"
                enabled: false
                text: i18n("OK")

                onClicked: {
                    configurationDialog.accept()
                }
            }

            QQC2.Button {
                id: cancelButton
                icon.name: "dialog-cancel"
                text: i18n("Cancel")

                onClicked: {
                    configurationDialog.close()
                }
            }
        }
    }

    function configurationChanged() {
        if (handler.hotspotSupported) {
            okButton.enabled = hotspotPassword.acceptableInput && hotspotName.text
        } else {
            okButton.enabled = true
        }
    }

    onVisibleChanged: {
        if (visible) {
            unlockModem.checked = PlasmaNM.Configuration.unlockModemOnDetection
            manageVirtualConnections.checked = PlasmaNM.Configuration.manageVirtualConnections
            hotspotName.text = PlasmaNM.Configuration.hotspotName
            hotspotPassword.text = PlasmaNM.Configuration.hotspotPassword
        }
    }

    onAccepted: {
        PlasmaNM.Configuration.unlockModemOnDetection = unlockModem.checked
        PlasmaNM.Configuration.manageVirtualConnections = manageVirtualConnections.checked
        PlasmaNM.Configuration.hotspotName = hotspotName.text
        PlasmaNM.Configuration.hotspotPassword = hotspotPassword.text
    }
}

