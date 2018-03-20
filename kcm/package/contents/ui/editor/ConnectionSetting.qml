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

        QtControls.Button {
            id: advancedPermissionsButton

            Layout.alignment: Qt.AlignRight

            enabled: !allUsersAllowedCheckbox.checked
            text: i18n("Advanced...")
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

        model: connectionSettingObject.vpnConnections
    }

    RowLayout {
        ColumnLayout {
            QtControls.Label {
                Layout.fillHeight: true
                Layout.alignment: Qt.AlignRight

                text: i18n("Firewall zone:")
            }

            QtControls.Label {
                Layout.fillHeight: true
                Layout.alignment: Qt.AlignRight

                text: i18n("Priority:")
            }
        }

        ColumnLayout {
            QtControls.ComboBox {
                id: firewallZoneCombobox

                Layout.fillWidth: true
            }

            QtControls.SpinBox {
                id: prioritySpinBox

                Layout.fillWidth: true

                value: 0
            }
        }
    }

    function loadSetting() {
        if (connectionSettingObject.connectionType == PlasmaNM.Enums.Vpn) {
            autoconnectCheckbox.enabled = false
            autoconnectVpnCheckbox.enabled = false
            prioritySpinBox.enabled = false
            vpnListCombobox.enabled = false
        } else {
            autoconnectCheckbox.enabled = true
            autoconnectVpnCheckbox.enabled = true
            prioritySpinBox.enabled = true
            vpnListCombobox.enabled = true
        }

        autoconnectCheckbox.checked = connectionSettingObject.autoconnect
        allUsersAllowedCheckbox.checked = !connectionSettingObject.permissions.length
        prioritySpinBox.value = connectionSettingObject.priority

        // TODO set firewall zone and vpn
    }
}
