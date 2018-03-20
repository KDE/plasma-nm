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

import "editor"

import QtQuick 2.6
import QtQuick.Dialogs 1.1
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.2 as QtControls

import org.kde.kcm 1.0
import org.kde.kirigami 2.3  as Kirigami // for Kirigami.Units
import org.kde.plasma.networkmanagement 0.2 as PlasmaNM

Kirigami.ApplicationItem {
    id: root

    property QtObject connectionSettingsObject: kcm.connectionSettings

    implicitWidth: Kirigami.Units.gridUnit * 20
    implicitHeight: Kirigami.Units.gridUnit * 20

    SystemPalette {
        id: palette
        colorGroup: SystemPalette.Active
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

    RowLayout {
        anchors.fill: parent

        spacing: Math.round(Kirigami.Units.gridUnit / 2)

        ColumnLayout {

            spacing: Math.round(Kirigami.Units.gridUnit / 2)

            ConnectionView {
                id: connectionView
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.minimumWidth: 300
                Layout.preferredWidth: 400
            }

            RowLayout {
                spacing: Math.round(Kirigami.Units.gridUnit / 2)

                QtControls.TextField {
                    id: searchField

                    Layout.fillWidth: true

                    placeholderText: i18n("Search...")

                    onTextChanged: {
                        editorProxyModel.setFilterRegExp(text)
                    }
                }

                QtControls.Button {
                    id: addConnectionButton

                    width: Kirigami.Units.iconSizes.medium * 3
                    height: Kirigami.Units.iconSizes.medium

                    icon.name: "list-add"

                    QtControls.ToolTip.text: i18n("Add new connection")
                    QtControls.ToolTip.visible: addConnectionButton.hovered

                    onClicked: {
                        addNewConnectionDialog.open()
                    }
                }

                QtControls.Button {
                    id: removeConnectionButton

                    height: Kirigami.Units.iconSizes.medium
                    width: Kirigami.Units.iconSizes.medium

                    enabled: connectionView.currentConnectionPath && connectionView.currentConnectionPath.length
                    icon.name: "list-remove"

                    QtControls.ToolTip.text: i18n("Remove selected connection")
                    QtControls.ToolTip.visible: removeConnectionButton.hovered

                    onClicked: {
                        deleteConfirmationDialog.connectionName = connectionView.currentConnectionName
                        deleteConfirmationDialog.connectionPath = connectionView.currentConnectionPath
                        deleteConfirmationDialog.open()
                    }
                }

                QtControls.Button {
                    id: exportConnectionButton

                    height: Kirigami.Units.iconSizes.medium
                    width: Kirigami.Units.iconSizes.medium

                    enabled: connectionView.currentConnectionExportable
                    icon.name: "document-export"

                    QtControls.ToolTip.text: i18n("Export selected connection")
                    QtControls.ToolTip.visible: exportConnectionButton.hovered

                    onClicked: {
                        kcm.requestExportConnection(connectionView.currentConnectionPath)
                    }
                }
            }
        }

        ConnectionEditor {
            id: connectionEditor

            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumWidth: childrenRect.width
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
                deselectConnectionsInView()
            }
            handler.removeConnection(connectionPath)
        }
    }

    MessageDialog {
        id: confirmSaveDialog

        property string connectionName
        property string connectionPath

        icon: StandardIcon.Question
        standardButtons: StandardButton.Ok | StandardButton.Cancel
        title: i18nc("@title:window", "Save Connection")
        text: i18n("Do you want to save changes made to the connection '%1'?", connectionView.currentConnectionName)

        onAccepted: {
            kcm.save()

            selectConnectionInView(connectionName, connectionPath)
        }
    }

    AddConnectionDialog {
        id: addNewConnectionDialog

        onRequestCreateConnection: {
            kcm.requestCreateConnection(type, vpnType, specificType, shared)
        }
    }

    function loadConnectionSetting() {
        connectionEditor.loadConnectionSettings()
    }

    function deselectConnectionsInView() {
        connectionView.currentConnectionPath = ""
    }

    function selectConnectionInView(connectionName, connectionPath) {
        connectionView.currentConnectionName = connectionName
        connectionView.currentConnectionPath = connectionPath
    }
}
