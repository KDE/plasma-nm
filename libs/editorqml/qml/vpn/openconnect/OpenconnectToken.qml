/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.plasma.networkmanagement.editorqml as PlasmaNMQ

Kirigami.Dialog {
    id: dialog

    required property var setting

    readonly property int tokenDisabled: 0
    readonly property int tokenStokenrc: 1

    readonly property bool needsSecret: dialog.setting.tokenMode !== dialog.tokenDisabled && dialog.setting.tokenMode !== dialog.tokenStokenrc

    parent: QQC2.Overlay.overlay

    title: i18nc("@title:window", "OpenConnect OTP Tokens")

    standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel

    preferredWidth: Kirigami.Units.gridUnit * 28

    readonly property list<string> revertableProperties: ["tokenMode", "tokenSecret", "tokenSecretOption"]

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

        Kirigami.FormLayout {
            Layout.fillWidth: true

            QQC2.Label {
                Kirigami.FormData.isSection: true

                text: i18n("Software Token Authentication")
                horizontalAlignment: Text.AlignHCenter
            }

            QQC2.ComboBox {
                Kirigami.FormData.label: i18n("Token Mode:")
                Layout.fillWidth: true

                model: [i18nc("@item:inlistbox no software token", "Disabled"), i18n("RSA SecurID — read from ~/.stokenrc"), i18n("RSA SecurID — manually entered"), i18n("TOTP — manually entered"), i18n("HOTP — manually entered"), i18n("Yubikey")]

                currentIndex: dialog.setting.tokenMode
                onActivated: dialog.setting.tokenMode = currentIndex
            }
        }

        PlasmaNMQ.PasswordField {
            Layout.fillWidth: true

            enabled: dialog.needsSecret

            passwordLabel: i18n("Token Secret:")
            showPasswordOptions: true

            password: dialog.setting.tokenSecret
            passwordOption: dialog.setting.tokenSecretOption

            onPasswordEdited: password => dialog.setting.tokenSecret = password
            onPasswordOptionEdited: option => dialog.setting.tokenSecretOption = option
        }
    }
}
