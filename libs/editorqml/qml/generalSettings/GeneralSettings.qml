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

    required property GeneralSetting setting

    RowLayout {
        Layout.fillWidth: true

        QQC2.CheckBox {
            id: autoconnect

            text: i18n("Connect automatically with priority")
            checked: root.setting.autoconnect
            enabled: !root.setting.isVpnConnection

            onToggled: root.setting.autoconnect = checked
        }

        QQC2.SpinBox {
            Layout.fillWidth: true

            from: -100
            to: 100
            editable: true

            enabled: autoconnect.checked && autoconnect.enabled
            value: root.setting.autoconnectPriority

            onValueModified: root.setting.autoconnectPriority = value
        }
    }

    Loader {
        id: permsDialogLoader
        active: false

        sourceComponent: AdvancedPermissionsDialog {
            onAccepted: root.setting.permissions = permissions
        }
    }

    RowLayout {
        Layout.fillWidth: true

        QQC2.CheckBox {
            id: allUsers

            text: i18n("All users may connect to this network")
            checked: root.setting.allUsers

            onToggled: root.setting.allUsers = checked
        }

        QQC2.Button {
            Layout.fillWidth: true

            text: i18n("Advanced…")
            icon.name: "preferences-desktop-user"

            enabled: !allUsers.checked

            onClicked: {
                if (!permsDialogLoader.active) {
                    permsDialogLoader.active = true;
                }

                permsDialogLoader.item.permissions = root.setting.permissions;
                permsDialogLoader.item.open();
            }
        }
    }

    RowLayout {
        Layout.fillWidth: true

        QQC2.CheckBox {
            id: autoVpn

            text: i18n("Automatically connect to VPN")
            checked: root.setting.autoconnectVpn
            enabled: !root.setting.isVpnConnection

            onToggled: root.setting.autoconnectVpn = checked
        }

        QQC2.ComboBox {
            Layout.fillWidth: true

            enabled: autoVpn.checked && autoVpn.enabled

            model: root.setting.vpnConnections
            textRole: "text"
            valueRole: "uuid"

            currentIndex: indexOfValue(root.setting.vpnUuid)

            onActivated: root.setting.vpnUuid = currentValue
        }
    }

    QQC2.ComboBox {
        Kirigami.FormData.label: i18n("Firewall zone:")
        Layout.fillWidth: true

        editable: true
        model: root.setting.firewallZones
        editText: root.setting.firewallZone

        onEditTextChanged: {
            if (editText !== root.setting.firewallZone)
                root.setting.firewallZone = editText;
        }
    }

    QQC2.ComboBox {
        Kirigami.FormData.label: i18n("Metered:")
        Layout.fillWidth: true

        model: [i18nc("@item metered mode", "Automatic"), i18n("Yes"), i18n("No")]

        currentIndex: root.setting.metered
        onActivated: root.setting.metered = currentIndex
    }

    Item {
        Layout.fillHeight: true
    }
}
