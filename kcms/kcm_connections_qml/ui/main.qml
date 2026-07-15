/*
    SPDX-FileCopyrightText: 2016 Jan Grulich <jgrulich@redhat.com>
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami

import org.kde.plasma.networkmanagement as PlasmaNM
import org.kde.plasma.networkmanagement.editorqml as PlasmaNMQ

Kirigami.Page {
    id: root

    focus: true

    Kirigami.Theme.colorSet: Kirigami.Theme.Window

    //Each network type Components
    Component {
        id: wireless
        PlasmaNMQ.Wireless {}
    }
    Component {
        id: disconnected

        Item {
            anchors.fill: parent

            QQC2.Label {
                anchors.centerIn: parent
                text: i18n("Disconnected")
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }

    Connections {
        target: PlasmaNM.Configuration
        function onManageVirtualConnectionsChanged() {
            editorProxyModel.invalidate();
        }
    }

    PlasmaNM.KcmIdentityModel {
        id: connectionModel
    }

    PlasmaNM.EditorProxyModel {
        id: editorProxyModel

        sourceModel: connectionModel
    }

    Connections {
        target: kcm
        function onConnectionLoaded(path) {
            if (!path) {
                root.deselectConnections();
            }
        }
    }

    RowLayout {
        anchors.fill: parent
        Item {
            id: leftPanel
            Layout.fillHeight: true
            Layout.fillWidth: !rightPanel.visible
            Layout.preferredWidth: Kirigami.Units.gridUnit * 15

            Kirigami.SearchField {
                id: searchField

                anchors {
                    left: parent.left
                    right: parent.right
                    top: parent.top
                }
                Keys.onDownPressed: event => {
                    connectionView.currentIndex = 0;
                    event.accepted = false; // pass to KeyNavigation
                }
                KeyNavigation.down: scrollView

                onTextChanged: {
                    editorProxyModel.setFilterFixedString(text);
                }
            }

            QQC2.ScrollView {
                id: scrollView

                Kirigami.StyleHints.showFramedBackground: true

                anchors {
                    bottom: rightButtonRow.top
                    bottomMargin: Kirigami.Units.smallSpacing
                    left: parent.left
                    right: parent.right
                    topMargin: Kirigami.Units.smallSpacing
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
                            currentIndex = 0;
                        }
                    }
                    activeFocusOnTab: true
                    KeyNavigation.up: searchField
                    KeyNavigation.down: configureButton
                    Accessible.role: Accessible.List
                    model: editorProxyModel
                    currentIndex: -1
                    boundsBehavior: Flickable.StopAtBounds
                    section.property: "Section"
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
                            connectionView.currentConnectionExportable = exportable;
                            if (kcm.needsSave) {
                                kcm.onRequestToChangeConnection(name, path);
                            } else {
                                connectionView.currentConnectionName = name;
                                connectionView.currentConnectionPath = path;
                            }
                        }

                        onAboutToRemoveConnection: (name, path) => {
                            deleteConfirmationDialog.connectionDisplayName = name;
                            deleteConfirmationDialog.connectionPath = path;
                            deleteConfirmationDialog.open();
                        }
                    }

                    onCurrentConnectionPathChanged: {
                        kcm.onSelectedConnectionChanged(currentConnectionPath);
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
                        addNewConnectionDialog.show();
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
                        deleteConfirmationDialog.connectionName = connectionView.currentConnectionName;
                        deleteConfirmationDialog.connectionPath = connectionView.currentConnectionPath;
                        deleteConfirmationDialog.open();
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
                        kcm.onRequestExportConnection(connectionView.currentConnectionPath);
                    }
                }
            }
        }

        ColumnLayout {
            id: rightPanel

            Layout.fillHeight: true
            Layout.fillWidth: true
            visible: connectionView.currentConnectionPath.length > 0

            spacing: Kirigami.Units.largeSpacing

            RowLayout {
                Layout.fillWidth: true
                spacing: Kirigami.Units.smallSpacing

                QQC2.Label {
                    text: i18n("Connection name:")
                }

                QQC2.TextField {
                    id: connectionName
                    Layout.fillWidth: true
                    text: connectionView.currentConnectionName
                }
            }

            Loader {
                Layout.fillWidth: true
                Layout.fillHeight: true

                active: connectionView.currentConnectionPath.length > 0
                sourceComponent: {
                    switch (kcm.connectionType) {
                    case PlasmaNM.Enums.Wireless:
                        return wireless;
                    default:
                        return disconnected;
                    }
                }
            }
        }
    }

    Kirigami.PromptDialog {
        id: deleteConfirmationDialog

        property string connectionDisplayName: ""
        property string connectionPath: ""

        standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel

        title: i18nc("@title:window", "Remove Connection")
        subtitle: i18n("Do you want to remove the connection '%1'?", connectionDisplayName)

        onConnectionDisplayNameChanged: {
            console.log("connectionDisplayName:", connectionDisplayName);
        }

        onAccepted: {
            if (connectionPath === connectionView.currentConnectionPath) {
                root.deselectConnections();
            }
            handler.removeConnection(connectionPath);
        }
    }

    PlasmaNMQ.AddConnectionDialog {
        id: addNewConnectionDialog

        onConfigurationDialogRequested: {
            root.showConfigurationDialog();
        }
    }

    PlasmaNMQ.PasswordPromptDialog {
        id: passwordPrompt
        handler: kcm.handler
    }

    PlasmaNMQ.ConfigurationDialog {
        id: configurationDialog
    }

    function showConfigurationDialog() {
        configurationDialog.show();
        configurationDialog.requestActivate();
    }
    function activateConnectionWithDialog(uniqueName, devicePath, specificPath, securityType) {
        passwordPrompt.uniqueName = uniqueName;
        passwordPrompt.devicePath = devicePath;
        passwordPrompt.specificPath = specificPath;
        passwordPrompt.securityType = securityType;
        passwordPrompt.open();
    }
}
