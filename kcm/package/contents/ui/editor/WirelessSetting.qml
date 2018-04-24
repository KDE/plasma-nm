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

import org.kde.kirigami 2.0 as Kirigami // for units

import org.kde.plasma.networkmanagement 0.2 as PlasmaNM

ColumnLayout {
    id: connectionSetting

    property string settingName: i18n("Wireless")

    spacing: Math.round(Kirigami.Units.gridUnit / 2)

    GridLayout {
        columns: 2

        QtControls.Label {
            Layout.alignment: Qt.AlignRight

            text: i18n("SSID:")
        }

        QtControls.ComboBox {
            id: ssidCombobox
            Layout.fillWidth: true

            delegate: QtControls.ItemDelegate {
                id: ssidComboDelegate

                property bool apVisible: typeof(modelData.signal) !== "undefined"

                height: apVisible ? (Kirigami.Units.gridUnit * 2) + Kirigami.Units.largeSpacing : Kirigami.Units.gridUnit
                width: parent.width

                RowLayout {
                    anchors.fill: parent
                    Kirigami.Icon {
                        Layout.alignment: Qt.AlignVCenter
                        height: Kirigami.Units.iconSizes.small
                        width: Kirigami.Units.iconSizes.small
                        source: modelData.security === i18n("Insecure") ? "object-unlocked" : "object-locked"
                        visible: ssidComboDelegate.apVisible
                    }

                    ColumnLayout {
                        spacing: 0
                        QtControls.Label {
                            Layout.alignment: Qt.AlignLeft
                            Layout.fillWidth: true
                            Layout.leftMargin: apVisible ? 0 : Kirigami.Units.iconSizes.small + Kirigami.Units.smallSpacing
                            text: ssidComboDelegate.apVisible ? i18n("%1 (%2%)", modelData.ssid, modelData.signal) : modelData.ssid
                        }

                        QtControls.Label {
                            Layout.alignment: Qt.AlignLeft
                            Layout.fillWidth: true
                            text: i18n("Security: %1", modelData.security)
                            visible: ssidComboDelegate.apVisible
                        }
                    }
                }
            }

            editable: true
            model: nmUtils.availableSsids()
            textRole: "ssid"

            onCurrentTextChanged: {
                var currentBssid = bssidCombobox.currentText
                bssidCombobox.model = nmUtils.availableBssids(ssidCombobox.currentText, currentBssid)
                bssidCombobox.currentIndex = bssidCombobox.find(currentBssid)
            }
        }

        QtControls.Label {
            Layout.alignment: Qt.AlignRight

            text: i18n("Mode:")
        }

        QtControls.ComboBox {
            id: modeCombobox
            Layout.fillWidth: true

            model: [i18n("Infrastructure"), i18n("Ad-hoc"), i18n("Access Point")]
        }

        QtControls.Label {
            Layout.alignment: Qt.AlignRight

            opacity: visible ? 1 : 0
            visible: !modeCombobox.currentIndex
            text: i18n("BSSID:")
        }

        QtControls.ComboBox {
            id: bssidCombobox
            Layout.fillWidth: true

            delegate: QtControls.ItemDelegate {
                id: bssidComboDelegate

                property bool apVisible: typeof(modelData.signal) !== "undefined"

                height: apVisible ? (Kirigami.Units.gridUnit * 3) + Kirigami.Units.largeSpacing : Kirigami.Units.gridUnit
                width: parent.width

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 0

                    QtControls.Label {
                        Layout.alignment: Qt.AlignLeft
                        Layout.fillWidth: true
                        text: bssidComboDelegate.apVisible ? i18n("%1 (%2%)", modelData.bssid, modelData.signal) : modelData.bssid
                    }

                    QtControls.Label {
                        Layout.alignment: Qt.AlignLeft
                        Layout.fillWidth: true
                        text: i18n("Frequency: %1", modelData.frequency)
                        visible: bssidComboDelegate.apVisible
                    }

                    QtControls.Label {
                        Layout.alignment: Qt.AlignLeft
                        Layout.fillWidth: true
                        text: i18n("Channel: %1", modelData.channel)
                        visible: bssidComboDelegate.apVisible
                    }
                }
            }

            model: nmUtils.availableBssids(ssidCombobox.currentText)
            editable: true
            opacity: visible ? 1 : 0
            visible: !modeCombobox.currentIndex
            textRole: "bssid"
            validator: RegExpValidator { regExp: /([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2}/ }
        }

        QtControls.Label {
            Layout.alignment: Qt.AlignRight

            opacity: visible ? 1 : 0
            visible: modeCombobox.currentIndex
            text: i18n("Band:")
        }

        QtControls.ComboBox {
            id: bandCombobox
            Layout.fillWidth: true

            model: [i18n("Automatic"), i18n("A (5 GHz)"), i18n("B/G (2.4 GHz)")]
            opacity: visible ? 1 : 0
            visible: modeCombobox.currentIndex
        }

        QtControls.Label {
            Layout.alignment: Qt.AlignRight

            opacity: visible ? 1 : 0
            visible: modeCombobox.currentIndex
            text: i18n("Channel:")
        }

        QtControls.ComboBox {
            id: channelCombobox
            Layout.fillWidth: true

            enabled: bandCombobox.currentIndex
            model: nmUtils.wirelessChannels(bandCombobox.currentIndex)
            opacity: visible ? 1 : 0
            visible: modeCombobox.currentIndex
        }

        QtControls.Label {
            Layout.alignment: Qt.AlignRight

            text: i18n("Restrict to device:")
        }

        QtControls.ComboBox {
            id: restrictToDeviceCombobox
            Layout.fillWidth: true

            delegate: QtControls.ItemDelegate {
                height: Kirigami.Units.gridUnit + Kirigami.Units.largeSpacing
                width: parent.width

                QtControls.Label {
                    id: label
                    anchors {
                        left: parent.left
                        leftMargin: Kirigami.Units.smallSpacing
                        verticalCenter: parent.verticalCenter
                    }
                    text: typeof(modelData.device) !==  "undefined" ? i18n("%1 (%2)", modelData.device, modelData.mac) : modelData.mac
                }
            }

            editable: true
            model: nmUtils.deviceHwAddresses(2) // Wireless device enum is NM_DEVICE_TYPE_WIFI which is 2
            validator: RegExpValidator { regExp: /([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2}/ }
            textRole: "mac"
        }

        QtControls.Label {
            Layout.alignment: Qt.AlignRight

            text: i18n("Cloned MAC address:")
        }

        QtControls.ComboBox {
            id: assignedMacAddressCombobox
            Layout.fillWidth: true

            editable: true
            model: ["", i18n("Preserve"), i18n("Permanent"), i18n("Random"), i18n("Stable")]
            validator: RegExpValidator { regExp: /(([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2})|Preserve|Permanent|Random|Stable/ }
        }

        QtControls.Label {
            Layout.alignment: Qt.AlignRight

            text: i18n("MTU:")
        }

        QtControls.SpinBox {
            id: mtuSpinBox
            Layout.fillWidth: true

            from: 0
            value: 0
            to: 1000
            valueFromText: function(text, locale) { return text === "Automatic" ? 0 : Number.fromLocaleString(locale, text); }
            textFromValue: function(value, locale) { return value ? Number(value).toLocaleString(locale, 'f', 0) : i18n("Automatic")}
        }

        QtControls.Label {
            Layout.alignment: Qt.AlignRight

            text: i18n("Visibility:")
        }

        QtControls.CheckBox {
            id: visibilityCheckbox
            Layout.fillWidth: true
            text: i18n("Hidden network")
        }
    }

    function loadSettings() {
        // TODO: maybe make this enum exported to QML as well
        var wirelessSettingObject = connectionSettingsObject.setting(12) // 12 == Wireless

        // SSID
        ssidCombobox.model = nmUtils.availableSsids(wirelessSettingObject.ssid)
        ssidCombobox.currentIndex = ssidCombobox.find(wirelessSettingObject.ssid)

        // Mode
        modeCombobox.currentIndex = wirelessSettingObject.mode

        // BSSID
        bssidCombobox.model = nmUtils.availableBssids(ssidCombobox.currentText, wirelessSettingObject.bssid)
        bssidCombobox.currentIndex = bssidCombobox.find(wirelessSettingObject.bssid)

        // Band
        bandCombobox.currentIndex = wirelessSettingObject.band

        // Channel
        channelCombobox.currentIndex = channelCombobox.find(wirelessSettingObject.channel + " (", Qt.MatchStartsWith)

        // Mac Address
        restrictToDeviceCombobox.model = nmUtils.deviceHwAddresses(2, wirelessSettingObject.macAddress)
        restrictToDeviceCombobox.currentIndex = restrictToDeviceCombobox.find(wirelessSettingObject.macAddress)

        // Assigned MAC address
        var assignedMacHwAddress = assignedMacAddressCombobox.find(wirelessSettingObject.assignedMacAddress, Qt.MatchFixedString)
        if (assignedMacHwAddress > 0) {
            assignedMacAddressCombobox.currentIndex = assignedMacHwAddress
        } else {
            assignedMacAddressCombobox.editText = wirelessSettingObject.assignedMacAddress
        }

        // MTU
        mtuSpinBox.value = wirelessSettingObject.mtu

        // Hidden
        visibilityCheckbox.checked = wirelessSettingObject.hidden
    }
}
