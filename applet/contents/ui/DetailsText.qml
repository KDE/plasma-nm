/*
    SPDX-FileCopyrightText: 2013-2017 Jan Grulich <jgrulich@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

import QtQuick 2.2
import QtQuick.Layouts 1.15

import org.kde.kquickcontrolsaddons 2.0 as KQuickControlsAddons
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents // for ContextMenu+MenuItem
import org.kde.plasma.components 3.0 as PlasmaComponents3

MouseArea {
    height: detailsGrid.implicitHeight

    property var details: []

    acceptedButtons: Qt.RightButton

    onPressed: {
        const item = detailsGrid.childAt(mouse.x, mouse.y);
        if (!item || !item.isContent) {
            return;
        }
        contextMenu.show(this, item.text, mouse.x, mouse.y);
    }

    KQuickControlsAddons.Clipboard {
        id: clipboard
    }

    PlasmaComponents.ContextMenu {
        id: contextMenu
        property string text

        function show(item, text, x, y) {
            contextMenu.text = text
            visualParent = item
            open(x, y)
        }

        PlasmaComponents.MenuItem {
            text: i18n("Copy")
            icon: "edit-copy"
            enabled: contextMenu.text !== ""
            onClicked: clipboard.content = contextMenu.text
        }
    }

    GridLayout {
        id: detailsGrid
        width: parent.width
        columns: 2
        rowSpacing: PlasmaCore.Units.smallSpacing / 4

        Repeater {
            id: repeater

            model: details.length

            PlasmaComponents3.Label {
                Layout.fillWidth: true

                readonly property bool isContent: index % 2

                elide: isContent ? Text.ElideRight : Text.ElideNone
                font: PlasmaCore.Theme.smallestFont
                horizontalAlignment: isContent ? Text.AlignLeft : Text.AlignRight
                text: isContent ? details[index] : `${details[index]}:`
                textFormat: Text.PlainText
                opacity: isContent ? 1 : 0.6
            }
        }
    }
}
