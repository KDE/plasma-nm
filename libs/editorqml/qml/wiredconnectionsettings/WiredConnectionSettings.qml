/*
      SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

      SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.plasma.networkmanagement.editorqml

Kirigami.FormLayout {
    id: root

    required property WiredSetting setting

    QQC2.ComboBox {
        id: macCombo

        Kirigami.FormData.label: i18n("Restrict to device:")
        Layout.fillWidth: true

        editable: true
        model: root.setting.macAddresses

        function sync() {
            if (activeFocus)
                return;
            currentIndex = root.setting.macAddresses.indexOf(root.setting.macAddress);
            editText = root.setting.macAddress;
        }

        // Resync the network device from selected component on every reload
        Component.onCompleted: sync()

        Connections {
            target: root.setting
            function onMacAddressChanged() {
                macCombo.sync();
            }
            function onMacAddressesChanged() {
                macCombo.sync();
            }
        }

        onActivated: root.setting.macAddress = editText
        onEditTextChanged: {
            if (activeFocus && editText !== root.setting.macAddress)
                root.setting.macAddress = editText;
        }

        hoverEnabled: true
        QQC2.ToolTip.visible: hovered
        QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
        QQC2.ToolTip.text: i18n("This option locks this connection to the network device specified by its permanent MAC address entered here. Example: 00:11:22:33:44:55")
    }

    RowLayout {
        Kirigami.FormData.label: i18n("Cloned MAC address:")
        Layout.fillWidth: true

        QQC2.TextField {
            Layout.fillWidth: true

            inputMask: "HH:HH:HH:HH:HH:HH;_"
            text: root.setting.clonedMacAddress

            onTextEdited: root.setting.clonedMacAddress = text

            hoverEnabled: true
            QQC2.ToolTip.visible: hovered
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
            QQC2.ToolTip.text: i18n("The MAC address entered here will be used as hardware address for the network device this connection is activated on. This feature is known as MAC cloning or spoofing. Example: 11:22:33:44:55:66")
        }

        QQC2.Button {
            text: i18n("Random…")

            onClicked: root.setting.clonedMacAddress = root.setting.generateRandomClonedMac()
        }
    }

    QQC2.SpinBox {
        Kirigami.FormData.label: i18n("MTU:")
        Layout.fillWidth: true

        from: 0
        to: 10000
        editable: true

        value: root.setting.mtu
        onValueModified: root.setting.mtu = value

        textFromValue: (value, locale) => value === 0 ? i18nc("@item MTU is automatically determined", "Automatic") : i18nc("@item:intext MTU size, %1 is the number of bytes", "%1 bytes", Number(value).toLocaleString(locale, 'f', 0))

        valueFromText: (text, locale) => {
            if (text === i18nc("@item MTU is automatically determined", "Automatic"))
                return 0;
            const digits = text.replace(/[^0-9]/g, "");
            return digits.length > 0 ? parseInt(digits, 10) : 0;
        }

        hoverEnabled: true
        QQC2.ToolTip.visible: hovered
        QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
        QQC2.ToolTip.text: i18n("Only transmit packets of the specified size or smaller, breaking larger packets up into multiple Ethernet frames")
    }

    QQC2.ComboBox {
        Kirigami.FormData.label: i18n("Link negotiation:")
        Layout.fillWidth: true

        model: [i18nc("@item:inlistbox link negotiation", "Ignore"), i18nc("@item:inlistbox link negotiation", "Automatic"), i18nc("@item:inlistbox link negotiation", "Manual")]

        currentIndex: root.setting.linkNegotiation
        onActivated: root.setting.linkNegotiation = currentIndex

        hoverEnabled: true
        QQC2.ToolTip.visible: hovered
        QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
        QQC2.ToolTip.text: i18n("Device link negotiation. If “Manual” is chosen, “Speed” and “Duplex” values will be forced without checking the device compatibility. If unsure, leave here “Ignore” or pick “Automatic”.")
    }

    QQC2.ComboBox {
        id: speedCombo

        Kirigami.FormData.label: i18n("Speed:")
        Layout.fillWidth: true

        enabled: root.setting.manualLinkEnabled

        model: root.setting.speeds
        textRole: "text"
        valueRole: "value"

        currentIndex: {
            const speed = Number(root.setting.speed);

            for (let i = 0; i < model.length; ++i) {
                if (Number(model[i].value) === speed)
                    return i;
            }

            return -1;
        }

        onActivated: {
            root.setting.speed = Number(currentValue);
        }
    }

    QQC2.ComboBox {
        Kirigami.FormData.label: i18n("Duplex:")
        Layout.fillWidth: true

        enabled: root.setting.manualLinkEnabled

        model: [i18nc("@item:inlistbox duplex mode", "Half"), i18nc("@item:inlistbox duplex mode", "Full")]

        currentIndex: root.setting.duplex
        onActivated: root.setting.duplex = currentIndex

        hoverEnabled: true
        QQC2.ToolTip.visible: hovered
        QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
        QQC2.ToolTip.text: i18n("Request that the device use the specified duplex mode. Either “Half” or “Full”")
    }

    Item {
        Layout.fillHeight: true
    }
}
