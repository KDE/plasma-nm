/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.plasma.networkmanagement.editorqml as PlasmaNMQ

ColumnLayout {
    id: root

    required property var setting

    spacing: Kirigami.Units.largeSpacing

    Kirigami.FormLayout {
        Layout.fillWidth: true

        QQC2.TextField {
            Kirigami.FormData.label: i18n("Toplevel Domain:")
            Layout.fillWidth: true

            QQC2.ToolTip.text: i18n("The domain the iodine server is authoritative for.")
            QQC2.ToolTip.visible: hovered
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

            text: root.setting.topLevelDomain
            onTextEdited: root.setting.topLevelDomain = text
        }

        QQC2.TextField {
            Kirigami.FormData.label: i18n("Nameserver:")
            Layout.fillWidth: true

            QQC2.ToolTip.text: i18n("Nameserver to use for the tunnel. Leave empty to use the system default.")
            QQC2.ToolTip.visible: hovered
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

            text: root.setting.nameserver
            onTextEdited: root.setting.nameserver = text
        }
    }

    PlasmaNMQ.PasswordField {
        Layout.fillWidth: true

        passwordLabel: i18n("Password:")
        showPasswordOptions: true

        password: root.setting.iodinePassword
        passwordOption: root.setting.iodinePasswordOption

        onPasswordEdited: password => root.setting.iodinePassword = password
        onPasswordOptionEdited: option => root.setting.iodinePasswordOption = option
    }

    Kirigami.FormLayout {
        Layout.fillWidth: true

        QQC2.SpinBox {
            Kirigami.FormData.label: i18n("Fragment Size:")

            QQC2.ToolTip.text: i18n("Maximum size of the upstream DNS fragments, in bytes. Automatic lets iodine probe for it.")
            QQC2.ToolTip.visible: hovered
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

            from: 0
            to: 10000

            textFromValue: (value, locale) => value === 0 ? i18n("Automatic") : i18np("%1 byte", "%1 bytes", Number(value).toLocaleString(locale, 'f', 0))
            valueFromText: (text, locale) => text === i18n("Automatic") ? 0 : Number.fromLocaleString(locale, text.replace(/[^0-9]/g, ""))

            value: root.setting.fragmentSize
            onValueModified: root.setting.fragmentSize = value
        }
    }

    Item {
        Layout.fillHeight: true
    }
}
