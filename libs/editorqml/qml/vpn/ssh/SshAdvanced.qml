/*
    SPDX-FileCopyrightText: 2015 Jan Grulich <jgrulich@redhat.com>
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami

Kirigami.Dialog {
    id: dialog

    required property var setting

    title: i18nc("@title:window", "Advanced")
    standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel

    preferredWidth: Kirigami.Units.gridUnit * 32
    preferredHeight: Kirigami.Units.gridUnit * 20

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Kirigami.Units.largeSpacing
        spacing: Kirigami.Units.smallSpacing

        RowLayout {
            Layout.fillWidth: true
            spacing: Kirigami.Units.largeSpacing

            QQC2.CheckBox {
                Layout.fillWidth: true

                text: i18n("Use custom gateway port:")

                checked: dialog.setting.useCustomPort
                onToggled: dialog.setting.useCustomPort = checked
            }

            QQC2.SpinBox {
                enabled: dialog.setting.useCustomPort

                from: 1
                to: 65535

                value: dialog.setting.port
                onValueModified: dialog.setting.port = value
            }
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: Kirigami.Units.largeSpacing

            QQC2.CheckBox {
                Layout.fillWidth: true

                text: i18n("Use custom tunnel Maximum Transmission Unit (MTU):")

                checked: dialog.setting.useCustomMtu
                onToggled: dialog.setting.useCustomMtu = checked
            }

            QQC2.SpinBox {
                enabled: dialog.setting.useCustomMtu

                from: 1
                to: 9000

                value: dialog.setting.mtu
                onValueModified: dialog.setting.mtu = value
            }
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: Kirigami.Units.largeSpacing

            QQC2.CheckBox {
                Layout.fillWidth: true

                text: i18n("Extra SSH options:")

                checked: dialog.setting.useExtraOptions
                onToggled: dialog.setting.useExtraOptions = checked
            }

            QQC2.TextField {
                Layout.fillWidth: true
                Layout.preferredWidth: Kirigami.Units.gridUnit * 14

                enabled: dialog.setting.useExtraOptions

                text: dialog.setting.extraOptions
                onTextEdited: dialog.setting.extraOptions = text
            }
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: Kirigami.Units.largeSpacing

            QQC2.CheckBox {
                Layout.fillWidth: true

                text: i18n("Remote device number:")

                checked: dialog.setting.useRemoteDevice
                onToggled: dialog.setting.useRemoteDevice = checked
            }

            QQC2.SpinBox {
                enabled: dialog.setting.useRemoteDevice

                from: 0
                to: 255

                value: dialog.setting.remoteDevice
                onValueModified: dialog.setting.remoteDevice = value
            }
        }

        QQC2.CheckBox {
            Layout.fillWidth: true

            text: i18n("Use a TAP device")

            checked: dialog.setting.useTapDevice
            onToggled: dialog.setting.useTapDevice = checked
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: Kirigami.Units.largeSpacing

            QQC2.CheckBox {
                Layout.fillWidth: true

                text: i18n("Remote username")

                checked: dialog.setting.useRemoteUsername
                onToggled: dialog.setting.useRemoteUsername = checked
            }

            QQC2.TextField {
                Layout.fillWidth: true
                Layout.preferredWidth: Kirigami.Units.gridUnit * 14

                enabled: dialog.setting.useRemoteUsername

                text: dialog.setting.remoteUsername
                onTextEdited: dialog.setting.remoteUsername = text
            }
        }

        QQC2.CheckBox {
            Layout.fillWidth: true

            text: i18n("Do not replace default route")

            checked: dialog.setting.doNotReplaceDefaultRoute
            onToggled: dialog.setting.doNotReplaceDefaultRoute = checked
        }
    }
}
