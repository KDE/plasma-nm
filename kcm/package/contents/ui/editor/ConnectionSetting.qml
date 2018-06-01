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
import QtQuick.Controls 2.2 as QtControls

import org.kde.kirigami 2.0 // for units

import org.kde.plasma.networkmanagement 0.2 as PlasmaNM

ColumnLayout {
    id: connectionSetting

    property string settingName: i18n("Connection")

    spacing: Math.round(Units.gridUnit / 2)

    QtControls.CheckBox {
        id: autoconnectCheckbox

        checked: true
        Layout.fillWidth: true

        text: i18n("Automatically connect to this network when it is available")
    }

    RowLayout {
        Layout.fillWidth: true

        QtControls.CheckBox {
            id: allUsersAllowedCheckbox

            Layout.fillWidth: true

            text: i18n("All users may connect to this network")
        }

        // TODO implement advanced configuration
        QtControls.Button {
            id: advancedPermissionsButton

            Layout.alignment: Qt.AlignRight

            enabled: !allUsersAllowedCheckbox.checked
            text: i18n("Advanced...")

            QtControls.ToolTip.text: i18n("Edit advanced permissions for this connection")
            QtControls.ToolTip.visible: advancedPermissionsButton.hovered

            visible: expertModeCheckbox.checked
        }
    }

    QtControls.CheckBox {
        id: autoconnectVpnCheckbox

        Layout.fillWidth: true

        text: i18n("Automatically connect to VPN when using this connection")
    }

    QtControls.ComboBox {
        id: vpnListCombobox

        Layout.fillWidth: true

        enabled: autoconnectVpnCheckbox.checked
        model: nmUtils.vpnConnections()
    }

    GridLayout {
        columns: 2

        QtControls.Label {
            Layout.alignment: Qt.AlignRight
            text: i18n("Firewall zone:")
            visible: expertModeCheckbox.checked
        }

        QtControls.ComboBox {
            id: firewallZoneCombobox
            Layout.fillWidth: true
            model: nmUtils.firewallZones()
            visible: expertModeCheckbox.checked
        }

        QtControls.Label {
            Layout.alignment: Qt.AlignRight
            text: i18n("Priority:")
        }

        QtControls.SpinBox {
            id: prioritySpinBox

            Layout.fillWidth: true

            value: 0

            QtControls.ToolTip.text: i18n("If the connection is set to autoconnect, connections with higher priority will be preferred.\nDefaults to 0. The higher number means higher priority. An negative number can be used to \nindicate priority lower than the default.")
            QtControls.ToolTip.visible: prioritySpinBox.hovered
        }
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

        if (connectionSettingsObject.secondaryConnection.length && connectionSettingsObject.vpnConnections.length) {
            autoconnectVpnCheckbox.checked = true
            vpnListCombobox.currentIndex = vpnListCombobox.find(connectionSettingsObject.secondaryConnection)
        } else {
            autoconnectVpnCheckbox.checked = false
        }
    }
}
