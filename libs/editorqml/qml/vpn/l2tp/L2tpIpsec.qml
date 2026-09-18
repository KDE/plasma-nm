/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Dialogs
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.plasma.networkmanagement.editorqml as PlasmaNMQ

Kirigami.Dialog {
    id: dialog

    required property var setting

    readonly property int authPresharedKey: 0
    readonly property int authTls: 1

    readonly property bool usesCertificates: dialog.setting.machineAuthType === dialog.authTls

    parent: QQC2.Overlay.overlay

    header: null

    standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel

    preferredWidth: Kirigami.Units.gridUnit * 34

    readonly property list<string> revertableProperties: ["ipsecEnabled", "machineAuthType", "presharedKey", "presharedKeyOption", "machineCa", "machineCert", "machineKey", "machineKeyPassword", "machineKeyPasswordOption", "remoteId", "ipsecIke", "ipsecEsp", "useIkeLifetime", "ikeLifetime", "useSaLifetime", "saLifetime", "enforceUdpEncapsulation", "useIpCompression", "useIkev2", "disablePfs"]

    property var previousState: null

    onOpened: {
        const state = {};
        for (const name of dialog.revertableProperties) {
            state[name] = dialog.setting[name];
        }
        dialog.previousState = state;
    }

    onRejected: {
        if (!dialog.previousState) {
            return;
        }

        for (const name of dialog.revertableProperties) {
            dialog.setting[name] = dialog.previousState[name];
        }
    }

    ColumnLayout {
        spacing: Kirigami.Units.largeSpacing

        QQC2.CheckBox {
            Layout.fillWidth: true

            text: i18n("Enable IPsec tunnel to L2TP host")

            checked: dialog.setting.ipsecEnabled
            onToggled: dialog.setting.ipsecEnabled = checked
        }

        Kirigami.FormLayout {
            Layout.fillWidth: true

            enabled: dialog.setting.ipsecEnabled

            QQC2.Label {
                Kirigami.FormData.isSection: true

                text: i18n("Machine Authentication")
                horizontalAlignment: Text.AlignHCenter
            }

            QQC2.ComboBox {
                Kirigami.FormData.label: i18n("Type:")
                Layout.fillWidth: true

                QQC2.ToolTip.text: i18n("Select an authentication mode.")
                QQC2.ToolTip.visible: hovered
                QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

                model: [i18n("Pre-shared Key (PSK)"), i18n("Certificates (TLS)")]

                currentIndex: dialog.setting.machineAuthType
                onActivated: dialog.setting.machineAuthType = currentIndex
            }

            RowLayout {
                Kirigami.FormData.label: i18n("CA Certificate:")
                Layout.fillWidth: true
                spacing: Kirigami.Units.smallSpacing

                visible: dialog.usesCertificates

                QQC2.TextField {
                    Layout.fillWidth: true

                    QQC2.ToolTip.text: i18n("Certificate authority (CA) file in .pem, .der, .crt, .cer or .p12 formats.")
                    QQC2.ToolTip.visible: hovered
                    QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

                    text: dialog.setting.machineCa
                    onTextEdited: dialog.setting.machineCa = text
                }

                QQC2.Button {
                    icon.name: "document-open"
                    onClicked: machineCaDialog.open()
                }
            }

            RowLayout {
                Kirigami.FormData.label: i18n("Machine Certificate:")
                Layout.fillWidth: true
                spacing: Kirigami.Units.smallSpacing

                visible: dialog.usesCertificates

                QQC2.TextField {
                    Layout.fillWidth: true

                    QQC2.ToolTip.text: i18n("Certificate in .pem, .der or .p12 formats.")
                    QQC2.ToolTip.visible: hovered
                    QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

                    text: dialog.setting.machineCert
                    onTextEdited: dialog.setting.machineCert = text
                }

                QQC2.Button {
                    icon.name: "document-open"
                    onClicked: machineCertDialog.open()
                }
            }

            RowLayout {
                Kirigami.FormData.label: i18n("Private Key:")
                Layout.fillWidth: true
                spacing: Kirigami.Units.smallSpacing

                visible: dialog.usesCertificates

                QQC2.TextField {
                    Layout.fillWidth: true

                    QQC2.ToolTip.text: i18n("Private key in .pem, .der, .key, .pk8 or .p12 formats.")
                    QQC2.ToolTip.visible: hovered
                    QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

                    text: dialog.setting.machineKey
                    onTextEdited: dialog.setting.machineKey = text
                }

                QQC2.Button {
                    icon.name: "document-open"
                    onClicked: machineKeyDialog.open()
                }
            }
        }

        PlasmaNMQ.PasswordField {
            Layout.fillWidth: true

            enabled: dialog.setting.ipsecEnabled
            visible: !dialog.usesCertificates

            passwordLabel: i18n("Pre-shared Key:")
            showPasswordOptions: true
            showNotRequired: true

            password: dialog.setting.presharedKey
            passwordOption: dialog.setting.presharedKeyOption

            onPasswordEdited: password => dialog.setting.presharedKey = password
            onPasswordOptionEdited: option => dialog.setting.presharedKeyOption = option
        }

        PlasmaNMQ.PasswordField {
            Layout.fillWidth: true

            enabled: dialog.setting.ipsecEnabled
            visible: dialog.usesCertificates

            passwordLabel: i18n("Private Key Password:")
            text: i18n("Password for private key or PKCS#12 certificate.")
            showPasswordOptions: true
            showNotRequired: true

            password: dialog.setting.machineKeyPassword
            passwordOption: dialog.setting.machineKeyPasswordOption

            onPasswordEdited: password => dialog.setting.machineKeyPassword = password
            onPasswordOptionEdited: option => dialog.setting.machineKeyPasswordOption = option
        }

        Kirigami.FormLayout {
            Layout.fillWidth: true

            enabled: dialog.setting.ipsecEnabled

            QQC2.Label {
                Kirigami.FormData.isSection: true

                text: i18n("Advanced")
                horizontalAlignment: Text.AlignHCenter
            }

            QQC2.TextField {
                Kirigami.FormData.label: i18n("Remote ID:")
                Layout.fillWidth: true

                QQC2.ToolTip.text: i18n("Optional. How the IPsec server should be identified for authentication. Sometimes referred to as Peer ID or Gateway ID, also referred to as rightid by strongSwan, Libreswan, Openswan and FreeS/WAN. See strongSwan or Libreswan documentation for leftid/rightid syntax and identity parsing.")
                QQC2.ToolTip.visible: hovered
                QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

                text: dialog.setting.remoteId
                onTextEdited: dialog.setting.remoteId = text
            }

            QQC2.TextField {
                Kirigami.FormData.label: i18n("Phase1 Algorithms:")
                Layout.fillWidth: true

                QQC2.ToolTip.text: i18n("Optional. A list of proposals for IKE - Main Mode. The format is “enc-integ-group,enc-integ-group, …”.")
                QQC2.ToolTip.visible: hovered
                QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

                text: dialog.setting.ipsecIke
                onTextEdited: dialog.setting.ipsecIke = text
            }

            QQC2.TextField {
                Kirigami.FormData.label: i18n("Phase2 Algorithms:")
                Layout.fillWidth: true

                QQC2.ToolTip.text: i18n("Optional. A list of proposals for ESP - Quick Mode. The format is “enc-integ,enc-integ, …”.")
                QQC2.ToolTip.visible: hovered
                QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

                text: dialog.setting.ipsecEsp
                onTextEdited: dialog.setting.ipsecEsp = text
            }

            QQC2.CheckBox {
                text: i18n("Phase1 Lifetime:")

                QQC2.ToolTip.text: i18n("How long the keying channel of a connection should last before being renegotiated.")
                QQC2.ToolTip.visible: hovered
                QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

                checked: dialog.setting.useIkeLifetime
                onToggled: dialog.setting.useIkeLifetime = checked
            }

            L2tpLifetimeField {
                Layout.fillWidth: true

                enabled: dialog.setting.useIkeLifetime

                seconds: dialog.setting.ikeLifetime
                onSecondsEdited: seconds => dialog.setting.ikeLifetime = seconds
            }

            QQC2.CheckBox {
                text: i18n("Phase2 Lifetime:")

                QQC2.ToolTip.text: i18n("How long a particular instance of a connection (a set of encryption/authentication keys for user packets) should last, from successful negotiation to expiry.")
                QQC2.ToolTip.visible: hovered
                QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

                checked: dialog.setting.useSaLifetime
                onToggled: dialog.setting.useSaLifetime = checked
            }

            L2tpLifetimeField {
                Layout.fillWidth: true

                enabled: dialog.setting.useSaLifetime

                seconds: dialog.setting.saLifetime
                onSecondsEdited: seconds => dialog.setting.saLifetime = seconds
            }

            QQC2.CheckBox {
                Layout.fillWidth: true

                text: i18n("Enforce UDP encapsulation")

                QQC2.ToolTip.text: i18n("Some firewalls block ESP traffic. Enforcing UDP encapsulation even if no NAT situation is detected might help in such cases.")
                QQC2.ToolTip.visible: hovered
                QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

                checked: dialog.setting.enforceUdpEncapsulation
                onToggled: dialog.setting.enforceUdpEncapsulation = checked
            }

            QQC2.CheckBox {
                Layout.fillWidth: true

                text: i18n("Use IP compression")

                QQC2.ToolTip.text: i18n("IPComp compresses raw IP packets before they get encrypted. This saves some bandwidth, but uses more processing power.")
                QQC2.ToolTip.visible: hovered
                QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

                checked: dialog.setting.useIpCompression
                onToggled: dialog.setting.useIpCompression = checked
            }

            QQC2.CheckBox {
                Layout.fillWidth: true

                text: i18n("Use IKEv2 key exchange")

                checked: dialog.setting.useIkev2
                onToggled: dialog.setting.useIkev2 = checked
            }

            QQC2.CheckBox {
                Layout.fillWidth: true

                enabled: dialog.setting.ipsecEnabled && dialog.setting.ipsecSupportsPfs

                text: i18n("Disable PFS")

                QQC2.ToolTip.text: i18n("Disable perfect forward secrecy. Enable this option only if the server doesn’t support PFS.")
                QQC2.ToolTip.visible: hovered && enabled
                QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

                checked: dialog.setting.disablePfs
                onToggled: dialog.setting.disablePfs = checked
            }
        }
    }

    FileDialog {
        id: machineCaDialog

        onAccepted: dialog.setting.machineCa = selectedFile
    }

    FileDialog {
        id: machineCertDialog

        onAccepted: dialog.setting.machineCert = selectedFile
    }

    FileDialog {
        id: machineKeyDialog

        onAccepted: dialog.setting.machineKey = selectedFile
    }
}
