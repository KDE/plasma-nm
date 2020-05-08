/*
    Copyright 2016-2018 Jan Grulich <jgrulich@redhat.com>

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

import QtQuick 2.3
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.2 as QtControls

import org.kde.kirigami 2.12 as Kirigami

import org.kde.plasma.networkmanagement 0.2 as PlasmaNM

Kirigami.OverlaySheet {
    id: addConnectionDialog

    objectName: "sheety"

    signal requestCreateConnection(int type, string vpnType, string specificType, bool shared)

    header: Kirigami.Heading {
        text: i18nc("@title:window", "Choose a Connection Type")
    }

    ListView {
        id: view

        property int currentlySelectedIndex: -1
        property bool connectionShared
        property string connectionSpecificType
        property int connectionType
        property string connectionVpnType

        Layout.preferredWidth: Kirigami.Units.gridUnit*10

        clip: true
        model: connectionModel
        currentIndex: -1
        boundsBehavior: Flickable.StopAtBounds

        section {
            property: "ConnectionTypeSection"
            delegate: Kirigami.ListSectionHeader {
                text: section
            }
        }

        delegate: Kirigami.BasicListItem {
            checked: view.currentlySelectedIndex == index
            highlighted: hovered

            icon: model.ConnectionIcon
            label: model.ConnectionTypeName
            subtitle: model.ConnectionDescription || ""

            onClicked: {
                view.currentlySelectedIndex = index
                view.connectionSpecificType = model.ConnectionSpecificType
                view.connectionShared = model.ConnectionShared
                view.connectionType = model.ConnectionType
                view.connectionVpnType = model.ConnectionVpnType
                addConnectionDialog.close()
                addConnectionDialog.requestCreateConnection(view.connectionType, view.connectionVpnType, view.connectionSpecificType, view.connectionShared)
            }
        }
    }
}
