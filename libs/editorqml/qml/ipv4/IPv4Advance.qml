/*
        SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

        SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.plasma.networkmanagement.editorqml

Kirigami.Dialog {
    id: dialog

    required property IPv4Setting setting

    title: i18n("Advanced IPv4 Settings")
    standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel

    preferredWidth: Kirigami.Units.gridUnit * 26

    property var previousState: null

    onOpened: previousState = {
        dhcpSendHostname: dialog.setting.dhcpSendHostname,
        dhcpHostname: dialog.setting.dhcpHostname,
        dadTimeout: dialog.setting.dadTimeout
    }

    onRejected: {
        if (!previousState)
            return;
        dialog.setting.dhcpSendHostname = previousState.dhcpSendHostname;
        dialog.setting.dhcpHostname = previousState.dhcpHostname;
        dialog.setting.dadTimeout = previousState.dadTimeout;
    }

    Kirigami.FormLayout {
        id: form

        QQC2.CheckBox {
            id: sendHostnameCheck

            Kirigami.FormData.label: i18n("DHCP:")
            text: i18n("Send hostname to DHCP server")

            checked: dialog.setting.dhcpSendHostname
            onToggled: dialog.setting.dhcpSendHostname = checked
        }

        QQC2.TextField {
            Kirigami.FormData.label: i18n("DHCP hostname:")
            Layout.fillWidth: true

            placeholderText: dialog.setting.systemHostname

            text: dialog.setting.dhcpHostname
            onTextEdited: dialog.setting.dhcpHostname = text
        }

        QQC2.SpinBox {
            Kirigami.FormData.label: i18n("DAD timeout:")
            Layout.fillWidth: true

            from: -1
            to: 30000
            stepSize: 100
            editable: true

            value: dialog.setting.dadTimeout
            onValueModified: dialog.setting.dadTimeout = value

            textFromValue: (value, locale) => {
                if (value < 0)
                    return i18nc("@item DAD timeout left to NetworkManager", "Default");
                if (value === 0)
                    return i18nc("@item duplicate address detection turned off", "Disabled");
                return i18nc("@item:intext duplicate address detection timeout in milliseconds", "%1 ms", Number(value).toLocaleString(locale, 'f', 0));
            }

            valueFromText: (text, locale) => {
                if (text === i18nc("@item DAD timeout left to NetworkManager", "Default"))
                    return -1;
                if (text === i18nc("@item duplicate address detection turned off", "Disabled"))
                    return 0;
                const digits = text.replace(/[^0-9]/g, "");
                return digits.length > 0 ? parseInt(digits, 10) : -1;
            }

            hoverEnabled: true
            QQC2.ToolTip.visible: hovered
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
            QQC2.ToolTip.text: i18n("Timeout in milliseconds used to check for the presence of duplicate IP addresses on the network.\n\n0 disables the check, -1 lets NetworkManager decide.")
        }
    }
}
