/*
    Copyright 2018 Jan Grulich <jgrulich@redhat.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

import QtQuick 2.6
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2 as QQC2

import org.kde.kirigami 2.5 as Kirigami

import org.kde.plasma.networkmanagement 0.2 as PlasmaNM

Kirigami.FormLayout {
    id: connectionSetting

    property string settingName: i18n("Connection")
    property alias connectionNameTextField: connectionNameTextField

    Item {
        Kirigami.FormData.label: i18n("General Settings")
        Kirigami.FormData.isSection: true
    }

    QQC2.TextField  {
        id: connectionNameTextField
        focus: true
        Kirigami.FormData.label: i18n("Network Name:")

        onTextEdited: connectionSettingsObject.id = text
    }

    QQC2.SpinBox {
        id: prioritySpinBox
        Kirigami.FormData.label: i18n("Priority:")
        value: 0

        QQC2.ToolTip {
            text: i18n("If the connection is set to autoconnect, connections with higher priority will be preferred.\nDefaults to 0. The higher number means higher priority. An negative number can be used to \nindicate priority lower than the default.")
        }

        onValueModified: connectionSettingsObject.priority = value
    }

    QQC2.CheckBox {
        id: autoconnectCheckbox
        checked: true
        text: i18n("Automatically connect")

        onToggled: connectionSettingsObject.autoconnect = checked
    }

    QQC2.CheckBox {
        id: allUsersAllowedCheckbox
        Layout.fillWidth: true
        text: i18n("Allow all users to connect to this network")

        onToggled: checked ? connectionSettingsObject.permissions = [] : connectionSettingsObject.permissions = ["replace_current_user"]
    }

    RowLayout {
        Layout.fillWidth: true
        Kirigami.FormData.label: i18n("Connect to this VPN when active:")

        QQC2.CheckBox {
            id: autoconnectVpnCheckbox

            onToggled: checked ? connectionSettingsObject.secondaryConnection = vpnListCombobox.currentText : connectionSettingsObject.secondaryConnection = ""
        }
        QQC2.ComboBox {
            id: vpnListCombobox
            enabled: autoconnectVpnCheckbox.checked
            model: nmUtils.vpnConnections()

            onActivated: connectionSettingsObject.secondaryConnection = currentText
        }
    }

    QQC2.ComboBox {
        id: connectionMeteredCombobox
        model: [ i18n("Automatically"), i18n("Always"), i18n("Never") ]

        onActivated: connectionSettingsObject.metered = currentIndex

        Kirigami.FormData.label: i18n("Consider this a metered connection:")
    }

    QQC2.ComboBox {
        id: firewallZoneCombobox
        model: nmUtils.firewallZones()

        onActivated: connectionSettingsObject.zone = currentText

        Kirigami.FormData.label: i18n("Put this network in the firewall zone:")
    }

    function loadSettings() {
        if (connectionSettingsObject.connectionType == PlasmaNM.Enums.Vpn) {
            autoconnectCheckbox.enabled = false
            autoconnectVpnCheckbox.enabled = false
            prioritySpinBox.enabled = false
        } else {
            autoconnectCheckbox.enabled = true
            autoconnectVpnCheckbox.enabled = true
            prioritySpinBox.enabled = true
        }

        autoconnectCheckbox.checked = connectionSettingsObject.autoconnect
        allUsersAllowedCheckbox.checked = !connectionSettingsObject.permissions.length
        prioritySpinBox.value = connectionSettingsObject.priority

        if (connectionSettingsObject.zone.length) {
            firewallZoneCombobox.currentIndex = firewallZoneCombobox.find(connectionSettingsObject.zone)
        } else {
            firewallZoneCombobox.currentIndex = 0 // Default
        }

        if (connectionSettingsObject.secondaryConnection.length) {
            autoconnectVpnCheckbox.checked = true
            vpnListCombobox.currentIndex = vpnListCombobox.find(connectionSettingsObject.secondaryConnection)
        } else {
            autoconnectVpnCheckbox.checked = false
        }

        connectionMeteredCombobox.currentIndex = connectionSettingsObject.metered
    }
}
