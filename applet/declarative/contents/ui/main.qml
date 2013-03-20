/*
    Copyright 2012-2013  Jan Grulich <jgrulich@redhat.com>

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

import QtQuick 1.1
import org.kde.qtextracomponents 0.1
import org.kde.plasma.graphicswidgets 0.1 as PlasmaWidgets
import org.kde.plasma.components 0.1 as PlasmaComponents
import org.kde.plasma.extras 0.1 as PlasmaExtras
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.plasmanm 0.1 as PlasmaNm

Item {
    id: mainWindow;

    property int minimumWidth: 300;
    property int minimumHeight: 300;
    property Component compactRepresentation: CompactRepresantation{}

    signal activateConnection(string connection, string device, string specificObject);
    signal addAndActivateConnection(string connection, string device, string specificObject);
    signal deactivateConnection(string connection);

    width: 300;
    height: 400;

//     signal enableNetworking(bool enable);
//     signal enableWireless(bool enable);
//     signal enableWwan(bool enable);

    PlasmaNm.Handler {
            id: handler;

            Component.onCompleted: {
                mainWindow.activateConnection.connect(activateConnection);
                mainWindow.addAndActivateConnection.connect(addAndActivateConnection);
                mainWindow.deactivateConnection.connect(deactivateConnection);
//                 mainWindow.enableNetworking.connect(enableNetworking);
//                 mainWindow.enableWireless.connect(enableWireless);
//                 mainWindow.enableWwan.connect(enableWwan);
            }
    }

    PlasmaNm.Model {
        id: connectionModel;
    }

    PlasmaNm.SortModel {
        id: sortedConnectionModel;

        sourceModel: connectionModel;
    }

    PlasmaNm.SecretAgent {
        id: secretAgent;
    }

    ListView {
        id: connectionView;

        anchors { fill: parent; topMargin: 5 }
        spacing: 8;
        model: sortedConnectionModel;
        delegate: ConnectionItem {
            onActivateConnectionItem: activateConnection(connection, device, specificObject);
            onAddAndActivateConnectionItem: addAndActivateConnection(connection, device, specificObject);
            onDeactivateConnectionItem: deactivateConnection(connection);
        }
    }
}
