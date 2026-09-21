/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

import QtQuick
import QtQuick.Controls as QQC2
import org.kde.kirigami as Kirigami

QQC2.ComboBox {
    id: root

    required property var ciphers

    property string cipher: ""

    signal cipherEdited(string cipher)

    readonly property var entries: {
        const items = [i18nc("@item::inlist Default openvpn cipher item", "Default")];

        if (root.cipher !== "" && !root.ciphers.includes(root.cipher)) {
            items.push(root.cipher);
        }

        return items.concat(root.ciphers);
    }

    QQC2.ToolTip.text: root.ciphers.length === 0 ? i18n("No ciphers were reported by OpenVPN.") : i18n("Cipher to use for the data channel.")
    QQC2.ToolTip.visible: hovered
    QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

    model: root.entries

    currentIndex: root.cipher === "" ? 0 : root.entries.indexOf(root.cipher)
    onActivated: root.cipherEdited(currentIndex === 0 ? "" : currentText)

    Binding {
        target: root.popup
        property: "height"
        value: Math.min(root.popup.implicitHeight, Kirigami.Units.gridUnit * 16)
        restoreMode: Binding.RestoreNone
    }
}
