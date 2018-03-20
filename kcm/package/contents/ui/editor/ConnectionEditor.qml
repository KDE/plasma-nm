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

    title: connectionSetting.connectionNameTextField.text

    PlasmaNM.Utils {
        id: nmUtils
    }

    ColumnLayout {
        id: simpleLayout

        ConnectionSetting {
            id: connectionSetting
            twinFormLayouts: connectionSpecificSetting.item

            Layout.fillWidth: true
        }

        Loader {
            id: connectionSpecificSetting

            Layout.fillWidth: true
        }
    }

    function loadConnectionSettings() {
        connectionSetting.connectionNameTextField.text = connectionSettingsObject.id
        // Load general connection setting
        connectionSetting.loadSettings()

        // Load connection specific setting
        if (connectionSettingsObject.connectionType == PlasmaNM.Enums.Wired) {
            connectionSpecificSetting.source = "WiredSetting.qml"
        } else if (connectionSettingsObject.connectionType == PlasmaNM.Enums.Wireless) {
            connectionSpecificSetting.source = "WirelessSetting.qml"
        }

        connectionSpecificSetting.item.twinFormLayouts = connectionSetting;
        connectionSpecificSetting.item.loadSettings()
    }
}

