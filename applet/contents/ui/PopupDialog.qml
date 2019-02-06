/*
    Copyright 2013-2017 Jan Grulich <jgrulich@redhat.com>

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
import QtQuick.Layouts 1.2
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.extras 2.0 as PlasmaExtras
import org.kde.plasma.networkmanagement 0.2 as PlasmaNM

FocusScope {
    property var notificationInhibitorLock: undefined

    PlasmaNM.AvailableDevices {
        id: availableDevices
    }

    PlasmaNM.NetworkModel {
        id: connectionModel
    }

    PlasmaNM.AppletProxyModel {
        id: appletProxyModel

        sourceModel: connectionModel
    }

    ColumnLayout {
        anchors.fill: parent

        Toolbar {
            id: toolbar
            Layout.fillWidth: true
        }

        PlasmaExtras.ScrollArea {
            id: scrollView
            Layout.fillWidth: true
            Layout.fillHeight: true

            ListView {
                id: connectionView

                property bool availableConnectionsVisible: false
                property int currentVisibleButtonIndex: -1

                anchors.fill: parent
                clip: true
                model: appletProxyModel
                currentIndex: -1
                boundsBehavior: Flickable.StopAtBounds
                section.property: showSections ? "Section" : ""
                section.delegate: Header { text: section }
                delegate: ConnectionItem { }
            }
        }
    }

    Connections {
        target: plasmoid
        onExpandedChanged: {
            connectionView.currentVisibleButtonIndex = -1;
            if (expanded) {
                var service = notificationsEngine.serviceForSource("notifications");
                var operation = service.operationDescription("inhibit");
                operation.hint = "x-kde-appname";
                operation.value = "networkmanagement";
                var job = service.startOperationCall(operation);
                job.finished.connect(function(job) {
                    if (expanded) {
                        notificationInhibitorLock =  job.result;
                    }
                });
            } else {
                notificationInhibitorLock = undefined;
                toolbar.closeSearch()
            }
        }
    }

    PlasmaCore.DataSource {
        id: notificationsEngine
        engine: "notifications"
    }
}
