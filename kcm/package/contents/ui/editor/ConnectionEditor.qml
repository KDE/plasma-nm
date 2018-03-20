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

// FIXME add horizontal scrollbar?
Item {
    id: connectionEditorTab

    QtControls.TextField  {
        id: connectionNameTextField

        anchors.top: parent.top

        hoverEnabled: true
    }

    QtControls.TabBar {
        id: tabBar

        width: parent.width
        anchors {
            top: connectionNameTextField.bottom
            topMargin: Math.round(Units.gridUnit / 2)
        }

        QtControls.TabButton {
            text: i18n("Connection")
        }

        // FIXME just placeholders for now
        QtControls.TabButton {
            text: i18n("Wireless")
        }

        QtControls.TabButton {
            text: i18n("Wireless security")
        }

        QtControls.TabButton {
            text: i18n("IPv4")
        }

        QtControls.TabButton {
            text: i18n("IPv6")
        }
    }

    StackLayout {
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: tabBar.bottom
            topMargin: Math.round(Units.gridUnit / 2)
        }

        currentIndex: tabBar.currentIndex

        ConnectionSetting {
            id: connectionSetting
        }
    }

    function loadConnectionSetting() {
        connectionNameTextField.text = connectionSettingObject.id
        // Load general connection setting
        connectionSetting.loadSetting()
    }
}

