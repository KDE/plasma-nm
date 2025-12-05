/*
    SPDX-FileCopyrightText: 2013-2017 Jan Grulich <jgrulich@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

pragma ComponentBehavior: Bound

import QtQuick 2.2
import QtQuick.Layouts 1.15

import org.kde.kquickcontrolsaddons 2.0 as KQuickControlsAddons
import org.kde.kirigami 2.20 as Kirigami
import org.kde.plasma.extras 2.0 as PlasmaExtras
import org.kde.plasma.components 3.0 as PlasmaComponents3

MouseArea {
    id: root

    height: detailsGrid.implicitHeight

    property var details: []

    acceptedButtons: Qt.RightButton

    onPressed: mouse => {
        const item = detailsGrid.childAt(mouse.x, mouse.y);
        if (!item || !item.isContent) {
            return;
        }
        contextMenu.show(this, item.text, mouse.x, mouse.y);
    }

    KQuickControlsAddons.Clipboard {
        id: clipboard
    }

    PlasmaExtras.Menu {
        id: contextMenu
        property string text

        function show(item, text, x, y) {
            contextMenu.text = text
            visualParent = item
            open(x, y)
        }

        PlasmaExtras.MenuItem {
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
        rowSpacing: Kirigami.Units.smallSpacing / 4

        Repeater {
            id: repeater

            model: root.details

            delegate: Item {
                id: delegateItem
                required property var modelData

                readonly property string label: modelData.label || ""
                readonly property string value: modelData.value || ""
                readonly property bool isSection: modelData.label === "__section__"
                // isEmpty handles defensive case where modelData.label is null/undefined
                readonly property bool isEmpty: !(modelData.label) || modelData.label.length === 0

                Layout.fillWidth: true
                Layout.columnSpan: 2
                Layout.preferredHeight: (delegateItem.isEmpty || delegateItem.isSection) ? 0 : labelItem.implicitHeight

                PlasmaComponents3.Label {
                    id: labelItem
                    anchors.left: parent.left
                    anchors.right: parent.horizontalCenter
                    anchors.rightMargin: detailsGrid.columnSpacing / 2

                    visible: !delegateItem.isEmpty && !delegateItem.isSection
                    elide: Text.ElideNone
                    font: Kirigami.Theme.smallFont
                    horizontalAlignment: Text.AlignRight
                    text: `${delegateItem.label}:`
                    textFormat: Text.PlainText
                    opacity: 0.6
                }

                PlasmaComponents3.Label {
                    anchors.left: parent.horizontalCenter
                    anchors.leftMargin: detailsGrid.columnSpacing / 2
                    anchors.right: parent.right

                    visible: !delegateItem.isEmpty && !delegateItem.isSection
                    elide: Text.ElideRight
                    font: Kirigami.Theme.smallFont
                    horizontalAlignment: Text.AlignLeft
                    text: delegateItem.value
                    textFormat: Text.PlainText
                    opacity: 1
                    readonly property bool isContent: true
                }
            }
        }
    }
}
