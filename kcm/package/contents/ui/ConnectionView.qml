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
import QtQuick.Controls 2.2 as QtControls

import org.kde.kcm 1.0
import org.kde.kirigami 2.3  as Kirigami

Kirigami.ScrollablePage {
    id: scrollablePage

    property bool currentConnectionExportable: false
    property string currentConnectionName
    property string currentConnectionPath

    Kirigami.Theme.colorSet: Kirigami.Theme.View

    background: Rectangle {
        color: Kirigami.Theme.backgroundColor
    }

    ListView {
        anchors.fill: parent
        clip: true
        model: editorProxyModel
        currentIndex: -1
        boundsBehavior: Flickable.StopAtBounds

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
                // Shouldn't be problem to set this in advance
                scrollablePage.currentConnectionExportable = exportable
                if (kcm.needsSave) {
                    confirmSaveDialog.connectionName = name
                    confirmSaveDialog.connectionPath = path
                    confirmSaveDialog.open()
                } else {
                    scrollablePage.currentConnectionName = name
                    scrollablePage.currentConnectionPath = path
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
    }

    onCurrentConnectionPathChanged: {
        kcm.selectConnection(scrollablePage.currentConnectionPath)
    }
}
