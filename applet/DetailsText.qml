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
import org.kde.plasma.networkmanagement as PlasmaNM

MouseArea {
    id: root

    height: detailsGrid.implicitHeight

    property PlasmaNM.ConnectionDetailsModel detailsModel: null

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

            model: root.detailsModel

            delegate: Item {
                id: delegateItem

                // Required properties from ConnectionDetailsModel
                required property bool isSection
                required property string sectionTitle
                required property string detailLabel
                required property string detailValue

                Layout.fillWidth: true
                Layout.columnSpan: 2
                Layout.preferredHeight: {
                    if (delegateItem.isSection) {
                        return Math.round(sectionTitleLabel.implicitHeight * 1.2); // Add some padding for sections
                    } else if (delegateItem.detailLabel || delegateItem.detailValue) {
                        return detailLabelItem.implicitHeight;
                    }
                    return 0; // Hide empty items
                }

                // Basically ListSectionHeader but centered and without the line.
                Kirigami.Heading {
                    id: sectionTitleLabel
                    anchors.fill: parent
                    visible: delegateItem.isSection
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignBottom
                    elide: Text.ElideRight
                    opacity: 0.75
                    level: 5
                    type: Kirigami.Heading.Primary
                    text: delegateItem.sectionTitle
                    font.weight: Font.Bold
                    textFormat: Text.PlainText
                }

                // Detail Label
                PlasmaComponents3.Label {
                    id: detailLabelItem // Renamed from labelItem
                    anchors.left: parent.left
                    anchors.right: parent.horizontalCenter
                    anchors.rightMargin: detailsGrid.columnSpacing / 2

                    visible: !delegateItem.isSection && (delegateItem.detailLabel || delegateItem.detailValue)
                    elide: Text.ElideNone
                    font: Kirigami.Theme.smallFont
                    horizontalAlignment: Text.AlignRight
                    text: `${delegateItem.detailLabel}:`
                    textFormat: Text.PlainText
                    opacity: 0.6
                }

                // Detail Value
                PlasmaComponents3.Label {
                    id: detailValueItem
                    anchors.left: parent.horizontalCenter
                    anchors.leftMargin: detailsGrid.columnSpacing / 2
                    anchors.right: parent.right

                    visible: !delegateItem.isSection && (delegateItem.detailLabel || delegateItem.detailValue)
                    elide: Text.ElideRight
                    font: Kirigami.Theme.smallFont
                    horizontalAlignment: Text.AlignLeft
                    text: delegateItem.detailValue
                    textFormat: Text.PlainText
                    opacity: 1

                    MouseArea {
                        anchors.fill: parent
                        acceptedButtons: Qt.RightButton
                        onPressed: mouse => {
                            contextMenu.show(this, detailValueItem.text, mouse.x, mouse.y);
                        }
                    }
                }
            }
        }
    }
}
