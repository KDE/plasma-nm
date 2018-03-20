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

import QtQuick 2.6
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.4 as QQC
import QtQuick.Controls 2.5 as QQC2

import org.kde.kirigami 2.9 as Kirigami

import org.kde.plasma.networkmanagement 0.2 as PlasmaNM

Kirigami.BasicListItem {
    id: connectionItem

    Accessible.role: Accessible.ListItem
    Accessible.name: model.Name

    checked: ConnectionPath === root.currentConnectionPath
    highlighted: focus

    signal aboutToChangeConnection(bool exportable, string name, string path)
    signal aboutToExportConnection(string path)
    signal aboutToRemoveConnection(string name, string path)

    icon: model.KcmConnectionIcon
    text: model.Name
    subtitle: {
        if (ConnectionState == PlasmaNM.Enums.Activated) {
            return i18n("Connected")
        } else if (ConnectionState == PlasmaNM.Enums.Activating) {
            return i18n("Connecting")
        } else {
            return LastUsed
        }
    }

    onClicked: {
        aboutToChangeConnection(KcmVpnConnectionExportable, Name, ConnectionPath)
    }
}
