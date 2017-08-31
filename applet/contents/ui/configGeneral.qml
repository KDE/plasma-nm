/*
    Copyright 2017 Jan Grulich <jgrulich@redhat.com>

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

import QtQuick 2.0
import QtQuick.Controls 1.1 as Controls
import QtQuick.Layouts 1.1 as Layouts

Item {
    id: generalPage
    width: childrenRect.width
    height: childrenRect.height

    signal configurationChanged

    property alias cfg_unlockModemOnDetection: unlockModem.checked
    property alias cfg_manageVirtualConnections: manageVirtualConnections.checked

    Layouts.ColumnLayout {
        id: mainColumn

        Controls.CheckBox {
            id: unlockModem
            Layouts.Layout.fillWidth: true
            text: i18n("Ask for PIN on modem detection")
            onClicked: {
                generalPage.configurationChanged()
            }
        }

        Controls.CheckBox {
            id: manageVirtualConnections
            Layouts.Layout.fillWidth: true
            text: i18n("Show and configure virtual connections")
            onClicked: {
                generalPage.configurationChanged()
            }
        }
    }
}
