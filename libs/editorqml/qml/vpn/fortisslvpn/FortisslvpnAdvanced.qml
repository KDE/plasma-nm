/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami

Kirigami.Dialog {
    id: dialog

    required property var setting

    parent: QQC2.Overlay.overlay

    header: null

    standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel

    preferredWidth: Kirigami.Units.gridUnit * 30
    preferredHeight: Kirigami.Units.gridUnit * 18

    Kirigami.FormLayout {
        anchors.fill: parent
        anchors.margins: Kirigami.Units.largeSpacing

        wideMode: true

        Item {
            Kirigami.FormData.isSection: true
            Kirigami.FormData.label: i18n("Authentication")
        }

        QQC2.CheckBox {
            Layout.fillWidth: true

            text: i18n("Use a One-Time Password")

            QQC2.ToolTip.text: i18n("Ask for a one-time password every time this connection is activated.")
            QQC2.ToolTip.visible: hovered
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

            checked: dialog.setting.useOtp
            onToggled: dialog.setting.useOtp = checked
        }

        QQC2.CheckBox {
            Layout.fillWidth: true

            text: i18n("2FA")

            QQC2.ToolTip.text: i18n("Use two-factor authentication. This supersedes the one-time password.")
            QQC2.ToolTip.visible: hovered
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

            checked: dialog.setting.useTwoFactorAuth
            onToggled: dialog.setting.useTwoFactorAuth = checked
        }

        QQC2.TextField {
            Kirigami.FormData.label: i18n("Realm:")
            Layout.fillWidth: true

            text: dialog.setting.realm
            onTextEdited: dialog.setting.realm = text
        }

        Item {
            Kirigami.FormData.isSection: true
            Kirigami.FormData.label: i18n("Security")
        }

        QQC2.TextField {
            Kirigami.FormData.label: i18n("Trusted certificate:")
            Layout.fillWidth: true

            placeholderText: "0123456789abcdef0123456789abcdef0123456789abcdef0123456789"

            text: dialog.setting.trustedCert
            onTextEdited: dialog.setting.trustedCert = text
        }
    }
}
