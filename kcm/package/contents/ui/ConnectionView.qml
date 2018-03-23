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
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2 as QtControls

import org.kde.kcm 1.0
import org.kde.kirigami 2.3  as Kirigami

Kirigami.ScrollablePage {
    id: connectionViewPage

    property bool currentConnectionExportable: false
    property string currentConnectionName
    property string currentConnectionPath

    title: "Connections"

    Kirigami.Theme.colorSet: Kirigami.Theme.View
    background: Rectangle {
        color: Kirigami.Theme.backgroundColor
    }

    header: Rectangle {
        color: Kirigami.Theme.backgroundColor

        width: connectionViewPage.width
        height: Math.round(Kirigami.Units.gridUnit * 2.5)

        RowLayout {
            id: searchLayout

            spacing: Kirigami.Units.smallSpacing
            anchors {
                fill: parent
                margins: Kirigami.Units.smallSpacing
            }

            QtControls.TextField {
                id: searchField

                Layout.minimumHeight: Layout.maximumHeight
                Layout.maximumHeight: Kirigami.Units.iconSizes.smallMedium + Kirigami.Units.smallSpacing * 2
                Layout.fillWidth: true

                focus: true
                placeholderText: i18n("Type here to search connection...")

                onTextChanged: {
                    editorProxyModel.setFilterRegExp(text)
                }

                MouseArea {
                    anchors {
                        right: parent.right
                        verticalCenter: parent.verticalCenter
                        rightMargin: y
                    }

                    opacity: searchField.text.length > 0 ? 1 : 0
                    width: Kirigami.Units.iconSizes.small
                    height: width

                    onClicked: {
                        searchField.text = ""
                    }

                    Kirigami.Icon {
                        anchors.fill: parent
                        source: LayoutMirroring.enabled ? "edit-clear-rtl" : "edit-clear"
                    }

                    Behavior on opacity {
                        OpacityAnimator {
                            duration: Kirigami.Units.longDuration
                            easing.type: Easing.InOutQuad
                        }
                    }
                }
            }
        }

        Kirigami.Separator {
            visible: !connectionView.atYBeginning
            anchors {
                left: parent.left
                right: parent.right
                top: parent.bottom
            }
        }
    }

    ListView {
        id: connectionView
        clip: true
        model: editorProxyModel
        currentIndex: -1
        boundsBehavior: Flickable.StopAtBounds
        activeFocusOnTab: true
        keyNavigationWraps: true
        Accessible.role: Accessible.List
        Keys.onTabPressed: {
            if (applicationWindow().wideScreen && root.pageStack.depth > 1) {
                connectionEditor.focus = true;
            }
        }

        section {
            property: "KcmConnectionType"
            delegate: Kirigami.AbstractListItem {
                supportsMouseEvents: false
                background: Rectangle {
                    color: palette.window
                }
                QtControls.Label {
                    id: headerLabel
                    anchors.centerIn: parent
                    font.weight: Font.DemiBold
                    text: section
                }
            }
        }

        delegate: ConnectionItemDelegate {
            onAboutToChangeConnection: {
//                         // Shouldn't be problem to set this in advance
//                         connectionViewPage.currentConnectionExportable = exportable
//                         if (kcm.needsSave) {
//                             confirmSaveDialog.connectionName = name
//                             confirmSaveDialog.connectionPath = path
//                             confirmSaveDialog.open()
//                         } else {
                    connectionViewPage.currentConnectionName = name
                    connectionViewPage.currentConnectionPath = path

//                         }
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
    }

    footer: Row {
        layoutDirection: Qt.RightToLeft
        spacing: Kirigami.Units.smallSpacing
        padding: Kirigami.Units.smallSpacing

        QtControls.Button {
            id: exportConnectionButton

            height: Kirigami.Units.iconSizes.medium
            width: Kirigami.Units.iconSizes.medium

            enabled: connectionViewPage.currentConnectionExportable
            icon.name: "document-export"

            QtControls.ToolTip.text: i18n("Export selected connection")
            QtControls.ToolTip.visible: exportConnectionButton.hovered

            onClicked: {
                kcm.requestExportConnection(connectionViewPage.currentConnectionPath)
            }
        }

        QtControls.Button {
            id: removeConnectionButton

            height: Kirigami.Units.iconSizes.medium
            width: Kirigami.Units.iconSizes.medium

            enabled: connectionViewPage.currentConnectionPath && connectionViewPage.currentConnectionPath.length
            icon.name: "list-remove"

            QtControls.ToolTip.text: i18n("Remove selected connection")
            QtControls.ToolTip.visible: removeConnectionButton.hovered

            onClicked: {
                deleteConfirmationDialog.connectionName = connectionViewPage.currentConnectionName
                deleteConfirmationDialog.connectionPath = connectionViewPage.currentConnectionPath
                deleteConfirmationDialog.open()
            }
        }

        QtControls.Button {
            id: addConnectionButton

            width: Kirigami.Units.iconSizes.medium
            height: Kirigami.Units.iconSizes.medium

            icon.name: "list-add"

            QtControls.ToolTip.text: i18n("Add new connection")
            QtControls.ToolTip.visible: addConnectionButton.hovered

            onClicked: {
                addNewConnectionDialog.open()
            }
        }
    }

    onCurrentConnectionPathChanged: {
        if (currentConnectionPath) {
            if (root.pageStack.depth < 2) {
                root.pageStack.push(connectionEditor)
            } else {
                root.pageStack.currentIndex = 1
            }
            kcm.selectConnection(connectionViewPage.currentConnectionPath)
        }
    }
}
