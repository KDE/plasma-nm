/*
    SPDX-FileCopyrightText: 2016 Jan Grulich <jgrulich@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

import QtQuick
import QtQuick.Dialogs
import QtQuick.Controls as QQC2
import org.kde.plasma.networkmanagement as PlasmaNM
import org.kde.kirigami 2.15 as Kirigami

QQC2.Page {
    id: root

    focus: true

    Kirigami.Theme.colorSet: Kirigami.Theme.Window

    Connections {
        target: PlasmaNM.Configuration
        function onManageVirtualConnectionsChanged() {
            editorProxyModel.invalidate()
        }
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

    Kirigami.SearchField {
        id: searchField

        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }
        Keys.onDownPressed: event => {
            connectionView.currentIndex = 0
            event.accepted = false // pass to KeyNavigation
        }
        KeyNavigation.down: scrollView

        onTextChanged: {
            editorProxyModel.setFilterFixedString(text)
        }
    }

    QQC2.ScrollView {
        id: scrollView

        Component.onCompleted: {
            if (scrollView.background) {
                scrollView.background.visible = true;
            }
        }

        anchors {
            bottom: rightButtonRow.top
            bottomMargin: Kirigami.Units.smallSpacing
            left: parent.left
            right: parent.right
            top: searchField.bottom
        }

        ListView {
            id: connectionView

            property bool currentConnectionExportable: false
            property string currentConnectionName
            property string currentConnectionPath

            clip: true
            focus: true
            onActiveFocusChanged: {
                if (currentIndex === -1) {
                    currentIndex = 0
                }
            }
            activeFocusOnTab: true
            KeyNavigation.up: searchField
            KeyNavigation.down: configureButton
            Accessible.role: Accessible.List
            model: editorProxyModel
            currentIndex: -1
            boundsBehavior: Flickable.StopAtBounds
            section.property: "KcmConnectionType"
            section.delegate: Kirigami.ListSectionHeader {
                text: section
                width: ListView.view.width
            }
            delegate: ConnectionItem {
                currentConnectionPath: connectionView.currentConnectionPath
                focus: ListView.isCurrentItem
                width: connectionView.width
                onAboutToChangeConnection: (exportable, name, path) => {
                    // Shouldn't be problem to set this in advance
                    connectionView.currentConnectionExportable = exportable
                    if (connectionModified) {
                        kcm.onRequestToChangeConnection(name, path)
                    } else {
                        connectionView.currentConnectionName = name
                        connectionView.currentConnectionPath = path
                    }
                }

                onAboutToRemoveConnection: (name, path) => {
                    deleteConfirmationDialog.connectionName = name
                    deleteConfirmationDialog.connectionPath = path
                    deleteConfirmationDialog.open()
                }
            }

            onCurrentConnectionPathChanged: {
                kcm.onSelectedConnectionChanged(currentConnectionPath)
            }
        }
    }

    Row {
        id: rightButtonRow

        anchors {
            bottom: parent.bottom
            right: parent.right
            margins: Kirigami.Units.smallSpacing
        }
        spacing: Kirigami.Units.smallSpacing

        QQC2.ToolButton {
            id: addConnectionButton

            text: QQC2.ToolTip.text
            display: QQC2.AbstractButton.IconOnly
            icon.name: "list-add"

            KeyNavigation.right: removeConnectionButton

            QQC2.ToolTip.text: i18n("Add new connection")
            QQC2.ToolTip.visible: hovered

            onClicked: {
                addNewConnectionDialog.show()
            }
        }

        QQC2.ToolButton {
            id: removeConnectionButton

            text: QQC2.ToolTip.text
            display: QQC2.AbstractButton.IconOnly
            enabled: connectionView.currentConnectionPath && connectionView.currentConnectionPath.length
            icon.name: "list-remove"

            KeyNavigation.right: exportConnectionButton

            QQC2.ToolTip.text: i18n("Remove selected connection")
            QQC2.ToolTip.visible: hovered

            onClicked: {
                deleteConfirmationDialog.connectionName = connectionView.currentConnectionName
                deleteConfirmationDialog.connectionPath = connectionView.currentConnectionPath
                deleteConfirmationDialog.open()
            }
        }

        QQC2.ToolButton {
            id: exportConnectionButton

            text: QQC2.ToolTip.text
            display: QQC2.AbstractButton.IconOnly
            enabled: connectionView.currentConnectionExportable
            icon.name: "document-export"

            QQC2.ToolTip.text: i18n("Export selected connection")
            QQC2.ToolTip.visible: hovered

            onClicked: {
                kcm.onRequestExportConnection(connectionView.currentConnectionPath)
            }
        }
    }

    Row {
        anchors {
            bottom: parent.bottom
            left: parent.left
            margins: Kirigami.Units.smallSpacing
        }
        spacing: Kirigami.Units.smallSpacing

        QQC2.ToolButton {
            id: configureButton
            text: QQC2.ToolTip.text
            display: QQC2.AbstractButton.IconOnly
            icon.name: "configure"

            KeyNavigation.right: addConnectionButton

            QQC2.ToolTip.text: i18n("Configuration")
            QQC2.ToolTip.visible: hovered

            onClicked: {
                root.showConfigurationDialog();
            }
        }
    }

    Kirigami.PromptDialog {
        id: deleteConfirmationDialog

        property string connectionName
        property string connectionPath

        /* Like QString::toHtmlEscaped */
        function toHtmlEscaped(s) {
            return s.replace(/[&<>]/g, function (tag) {
                return {
                    '&': '&amp;',
                    '<': '&lt;',
                    '>': '&gt;'
                }[tag] || tag
            });
        }

        standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel
        title: i18nc("@title:window", "Remove Connection")
        subtitle: i18n("Do you want to remove the connection '%1'?", toHtmlEscaped(connectionName))

        // QTBUG-122770 accepted signal isn't emitted for Ok button.
        onAccepted: {
            if (connectionPath === connectionView.currentConnectionPath) {
                // Deselect now non-existing connection
                root.deselectConnections()
            }
            handler.removeConnection(connectionPath)
        }
    }
    AddConnectionDialog {
        id: addNewConnectionDialog

        onConfigurationDialogRequested: {
            root.showConfigurationDialog();
        }
    }

    ConfigurationDialog {
        id: configurationDialog

        handler: handler
    }

    function showConfigurationDialog() {
        configurationDialog.show();
        configurationDialog.requestActivate();
    }

    function deselectConnections() {
        connectionView.currentConnectionName = ""
        connectionView.currentConnectionPath = ""
    }

    function selectConnection(connectionName, connectionPath) {
        connectionView.currentConnectionName = connectionName
        connectionView.currentConnectionPath = connectionPath
    }
}
