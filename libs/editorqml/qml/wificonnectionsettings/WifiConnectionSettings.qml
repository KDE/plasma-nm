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

    required property WifiSetting setting

    Item {
        Layout.fillWidth: true
        implicitHeight: connectionHeading.implicitHeight

        Kirigami.Heading {
            id: connectionHeading

            anchors.horizontalCenter: parent.horizontalCenter

            text: i18n("Connection")
            level: 2
        }
    }
    QQC2.ComboBox {
        id: ssidCombo

        Kirigami.FormData.label: i18n("SSID:")
        Layout.fillWidth: true

        editable: true
        model: root.setting.availableSsids

        function sync() {
            if (activeFocus)
                return;
            currentIndex = root.setting.availableSsids.indexOf(root.setting.ssid);
            editText = root.setting.ssid;
        }

        // Resync the SSID from selected component on evvery reload
        Component.onCompleted: sync()

        Connections {
            target: root.setting
            function onSsidChanged() {
                ssidCombo.sync();
            }
            function onAvailableSsidsChanged() {
                ssidCombo.sync();
            }
        }

        onActivated: root.setting.ssid = editText
        onEditTextChanged: {
            if (activeFocus && editText !== root.setting.ssid)
                root.setting.ssid = editText;
        }

        hoverEnabled: true
    }

    QQC2.ComboBox {
        Kirigami.FormData.label: i18n("Mode:")
        Layout.fillWidth: true

        model: [i18n("Infrastructure"), i18n("Ad-hoc"), i18n("Access Point")]

        currentIndex: root.setting.mode
        onActivated: root.setting.mode = currentIndex

        hoverEnabled: true
    }

    Item {
        Layout.fillWidth: true
        implicitHeight: advancedHeading.implicitHeight

        Kirigami.Heading {
            id: advancedHeading

            anchors.horizontalCenter: parent.horizontalCenter

            text: i18n("Advanced")
            level: 2
        }
    }
    QQC2.ComboBox {
        model: root.setting.availableBssids

        Kirigami.FormData.label: i18n("Restrict to BSSID:")
        Layout.fillWidth: true
        textRole: "bssid"
        valueRole: "bssid"

        currentIndex: indexOfValue(root.setting.bssid)

        delegate: QQC2.ItemDelegate {
            id: bssidDelegate

            required property var modelData

            width: ListView.view ? ListView.view.width : implicitWidth

            contentItem: Column {
                spacing: Kirigami.Units.smallSpacing

                QQC2.Label {
                    text: i18nc("@item BSSID (MAC address) followed by signal strength", "%1 (%2%)", bssidDelegate.modelData.bssid, bssidDelegate.modelData.signal)
                }

                QQC2.Label {
                    text: i18n("Frequency: %1 MHz", bssidDelegate.modelData.frequency)
                    font: Kirigami.Theme.smallFont
                }

                QQC2.Label {
                    text: i18n("Channel: %1", bssidDelegate.modelData.channel)
                    font: Kirigami.Theme.smallFont
                }
            }
        }

        onActivated: root.setting.bssid = currentValue
    }
    QQC2.ComboBox {
        Kirigami.FormData.label: i18n("Band:")
        Layout.fillWidth: true

        model: [i18nc("@item wireless band", "Automatic"), i18n("A (5 GHz)"), i18n("B/G (2.4 GHz)")]

        currentIndex: root.setting.band
        onActivated: root.setting.band = currentIndex

        hoverEnabled: true
    }

    QQC2.ComboBox {
        Kirigami.FormData.label: i18n("Channel:")
        Layout.fillWidth: true

        visible: root.setting.mode !== 0
        enabled: root.setting.band !== 0

        model: root.setting.channels
        textRole: "text"
        valueRole: "value"

        currentIndex: indexOfValue(root.setting.channel)
        onActivated: root.setting.channel = currentValue

        hoverEnabled: true
    }

    QQC2.ComboBox {
        id: macCombo

        Kirigami.FormData.label: i18n("Restrict to network adapter:")
        Layout.fillWidth: true

        editable: true
        model: root.setting.macAddresses

        function sync() {
            if (activeFocus)
                return;
            currentIndex = root.setting.macAddresses.indexOf(root.setting.macAddress);
            editText = root.setting.macAddress;
        }

        // Resync the Network adapter  from selected component on every reload
        Component.onCompleted: sync()

        Connections {
            target: root.setting
            function onMacAddressChanged() {
                macCombo.sync();
            }
        }

        onActivated: root.setting.macAddress = editText
        onEditTextChanged: {
            if (activeFocus && editText !== root.setting.macAddress)
                root.setting.macAddress = editText;
        }

        hoverEnabled: true
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
    }

    QQC2.CheckBox {
        Kirigami.FormData.label: i18n("Visibility:")
        text: i18n("Hidden network")

        checked: root.setting.hidden
        onToggled: root.setting.hidden = checked

        hoverEnabled: true
    }

    Item {
        Layout.fillHeight: true
    }
}
