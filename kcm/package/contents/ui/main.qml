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
import QtQuick.Controls 2.2 as QtControls

import org.kde.kcm 1.0
import org.kde.kirigami 2.3  as Kirigami // for Kirigami.Units
import org.kde.plasma.networkmanagement 0.2 as PlasmaNM

Kirigami.ApplicationItem {
    id: root

    property QtObject connectionSettingsObject: kcm.connectionSettings

    LayoutMirroring.enabled: Qt.application.layoutDirection === Qt.RightToLeft
    LayoutMirroring.childrenInherit: true

    wideScreen: width >= Kirigami.Units.gridUnit * 50

    pageStack.defaultColumnWidth: Kirigami.Units.gridUnit * 25
    pageStack.initialPage: connectionView

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

    ConnectionView {
        id: connectionView
    }

    ConnectionEditor {
        id: connectionEditor
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
