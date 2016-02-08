/*
    Copyright 2016 Jan Grulich <jgrulich@redhat.com>

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

import QtQuick 2.2
import org.kde.kcoreaddons 1.0 as KCoreAddons
import org.kde.kquickcontrolsaddons 2.0 as KQuickControlsAddons
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.extras 2.0 as PlasmaExtras
import org.kde.plasma.networkmanagement 0.2 as PlasmaNM

PlasmaComponents.ListItem {
    id: deviceItem

    checked: deviceItem.containsMouse || ListView.view.currentIndex == index
    enabled: true

    PlasmaCore.IconItem {
        id: deviceSvgIcon

        anchors {
            left: parent.left
            leftMargin: Math.round(units.gridUnit / 3)
            verticalCenter: parent.verticalCenter
        }
        source: DeviceIcon
        height: units.iconSizes.small; width: height
    }

    PlasmaComponents.Label {
        id: deviceNameLabel

        anchors {
            bottom: deviceSvgIcon.verticalCenter
            left: deviceSvgIcon.right
            leftMargin: Math.round(units.gridUnit / 2)
            right: parent.right
        }
        height: paintedHeight
        elide: Text.ElideRight
        text: DeviceLabel
        textFormat: Text.PlainText
    }

    PlasmaComponents.Label {
        id: deviceSubLabel

        anchors {
            left: deviceSvgIcon.right
            leftMargin: Math.round(units.gridUnit / 2)
            right: parent.right
            top: deviceNameLabel.bottom
        }
        height: paintedHeight
        elide: Text.ElideRight
        font.pointSize: theme.smallestFont.pointSize
        opacity: 0.6
        text: DeviceSubLabel
    }

    function selectDevice() {
        ListView.view.currentIndex = index

        kcmProxyModel.filteredDeviceName = DeviceName
        kcmProxyModel.filteredDeviceType = DeviceType

        devicePage.deviceLabelText = DeviceLabel
        devicePage.deviceSubLabelText = DeviceSubLabel
        devicePage.deviceIconSource = DeviceIcon
        devicePage.deviceDetailsModel = DeviceDetails
    }

    onClicked: {
        selectDevice()
    }

    Component.onCompleted: {
        // We are first device, let's make it a default selected device
        if (index == 0) {
            selectDevice()
        }
    }
}