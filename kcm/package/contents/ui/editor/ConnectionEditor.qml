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

import org.kde.kcm 1.2
import org.kde.kirigami 2.0 as Kirigami
import org.kde.plasma.networkmanagement 0.2 as PlasmaNM

SimpleKCM {
    id: connectionEditorPage

    title: connectionNameTextField.text

    PlasmaNM.Utils {
        id: nmUtils
    }

    ColumnLayout {
        spacing: Kirigami.Units.smallSpacing

        QtControls.TextField  {
            id: connectionNameTextField
            Layout.alignment: Qt.AlignHCenter
            Layout.fillWidth: true
            Layout.minimumWidth: Kirigami.Units.gridUnit * 25
            Layout.maximumWidth: Kirigami.Units.gridUnit * 40
            hoverEnabled: true
        }

        StackLayout {
            Layout.alignment: Qt.AlignHCenter
            Layout.fillWidth: true
            Layout.minimumWidth: Kirigami.Units.gridUnit * 25
            Layout.maximumWidth: Kirigami.Units.gridUnit * 40

            currentIndex: expertModeCheckbox.checked ? 1 : 0

            ColumnLayout {
                id: simpleLayout
                anchors.fill: parent

                ConnectionSetting {
                    id: connectionSetting
                    Layout.fillWidth: true
                }

                Loader {
                    id: connectionSpecificSetting
                    Layout.fillWidth: true
                }
            }

            ColumnLayout {
                anchors.fill: parent

                QtControls.TabBar {
                    id: tabBar
                    Layout.fillWidth: true

                    QtControls.TabButton {
                        text: connectionSetting.settingName
                    }

                    QtControls.TabButton {
                        text: connectionSpecificSetting.item.settingName
                    }

                    // FIXME just placeholders for now
                    QtControls.TabButton {
                        text: i18n("IP")
                    }
                }

                StackLayout {
                    id: expertLayout

                    Layout.fillWidth: true

                    currentIndex: tabBar.currentIndex
                    // Items will be re-parented
                }
            }

            // FIXME this is probably not the best solution
            onCurrentIndexChanged: {
                tabBar.currentIndex = 0
                connectionSetting.parent = currentIndex ? expertLayout : simpleLayout
                connectionSpecificSetting.parent =  currentIndex ? expertLayout : simpleLayout
            }
        }
    }

    function loadConnectionSettings() {
        connectionNameTextField.text = connectionSettingsObject.id
        // Load general connection setting
        connectionSetting.loadSettings()

        // Load connection specific setting
        if (connectionSettingsObject.connectionType == PlasmaNM.Enums.Wired) {
            connectionSpecificSetting.source = "WiredSetting.qml"
        } else if (connectionSettingsObject.connectionType == PlasmaNM.Enums.Wireless) {
            connectionSpecificSetting.source = "WirelessSetting.qml"
        }

        connectionSpecificSetting.item.loadSettings()
    }
}

