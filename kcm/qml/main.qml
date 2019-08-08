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

import QtQuick 2.1
import QtQuick.Dialogs 1.1
import QtQuick.Controls 1.2 as QtControls
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.extras 2.0 as PlasmaExtras
import org.kde.plasma.networkmanagement 0.2 as PlasmaNM

Item {
    id: root

    focus: true

    signal selectedConnectionChanged(string connection)
    signal requestCreateConnection(int type, string vpnType, string specificType, bool shared)
    signal requestExportConnection(string connection)
    signal requestToChangeConnection(string name, string path)

    Rectangle {
        id: background
        anchors.fill: parent
        focus: true
        color: baseColor
    }

    PlasmaNM.Handler {
        id: handler
    }

    PlasmaNM.KcmIdentityModel {
        id: connectionModel
    }

    PlasmaNM.EditorProxyModel {
        id: editorProxyModel

        sourceModel: connectionModel
    }

    QtControls.TextField {
        id: searchField

        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }

        placeholderText: i18n("Search...")

        onTextChanged: {
            editorProxyModel.setFilterRegExp(text)
        }
    }

    QtControls.ScrollView {
        id: scrollView

        anchors {
            bottom: buttonRow.top
            bottomMargin: Math.round(units.gridUnit / 3)
            left: parent.left
            right: parent.right
            top: searchField.bottom
        }

        ListView {
            id: connectionView

            property bool currentConnectionExportable: false
            property string currentConnectionName
            property string currentConnectionPath

            anchors.fill: parent
            clip: true
            model: editorProxyModel
            currentIndex: -1
            boundsBehavior: Flickable.StopAtBounds
            section.property: "KcmConnectionType"
            section.delegate: Header { text: section }
            delegate: ConnectionItem {
                onAboutToChangeConnection: {
                    // Shouldn't be problem to set this in advance
                    connectionView.currentConnectionExportable = exportable
                    if (connectionModified) {
                        requestToChangeConnection(name, path)
                    } else {
                        connectionView.currentConnectionName = name
                        connectionView.currentConnectionPath = path
                    }
                }

                onAboutToExportConnection: {
                    requestExportConnection(path)
                }

                onAboutToRemoveConnection: {
                    deleteConfirmationDialog.connectionName = name
                    deleteConfirmationDialog.connectionPath = path
                    deleteConfirmationDialog.open()
                }
            }

            onCurrentConnectionPathChanged: {
                root.selectedConnectionChanged(currentConnectionPath)
            }
        }
    }

    Row {
        id: buttonRow

        anchors {
            bottom: parent.bottom
            right: parent.right
            margins: Math.round(units.gridUnit / 3)
        }
        spacing: Math.round(units.gridUnit / 2)

        QtControls.ToolButton {
            id: addConnectionButton

            iconName: "list-add"
            tooltip: i18n("Add new connection")

            onClicked: {
                addNewConnectionDialog.open()
            }
        }

        QtControls.ToolButton {
            id: removeConnectionButton

            enabled: connectionView.currentConnectionPath && connectionView.currentConnectionPath.length
            iconName: "list-remove"
            tooltip: i18n("Remove selected connection")

            onClicked: {
                deleteConfirmationDialog.connectionName = connectionView.currentConnectionName
                deleteConfirmationDialog.connectionPath = connectionView.currentConnectionPath
                deleteConfirmationDialog.open()
            }
        }

        QtControls.ToolButton {
            id: exportConnectionButton

            enabled: connectionView.currentConnectionExportable
            iconName: "document-export"
            tooltip: i18n("Export selected connection")

            onClicked: {
                root.requestExportConnection(connectionView.currentConnectionPath)
            }
        }
    }

    MessageDialog {
        id: deleteConfirmationDialog

        property string connectionName
        property string connectionPath

        icon: StandardIcon.Question
        standardButtons: StandardButton.Ok | StandardButton.Cancel
        title: i18nc("@title:window", "Remove Connection")
        text: i18n("Do you want to remove the connection '%1'?", connectionName)

        onAccepted: {
            if (connectionPath == connectionView.currentConnectionPath) {
                // Deselect now non-existing connection
                deselectConnections()
            }
            handler.removeConnection(connectionPath)
        }
    }

    Dialog {
        id: addNewConnectionDialog

        onRequestCreateConnection: {
            root.requestCreateConnection(type, vpnType, specificType, shared)
        }
    }

    function deselectConnections() {
        connectionView.currentConnectionPath = ""
    }

    function selectConnection(connectionName, connectionPath) {
        connectionView.currentConnectionName = connectionName
        connectionView.currentConnectionPath = connectionPath
    }
}
