/*
      SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>
      SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.plasma.networkmanagement.editorqml

Kirigami.Dialog {
    id: dialog

    title: i18n("Alternative subject matches")
    standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel

    preferredWidth: Kirigami.Units.gridUnit * 28
    preferredHeight: Kirigami.Units.gridUnit * 20

    property alias addresses: listModel.stringList

    StringListModel {
        id: listModel
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Kirigami.Units.largeSpacing
        spacing: Kirigami.Units.largeSpacing

        RowLayout {
            Layout.fillWidth: true

            QQC2.TextField {
                id: input
                Layout.fillWidth: true

                validator: RegularExpressionValidator {
                    // Check for valid DNS address
                    regularExpression: /^(DNS:([A-Za-z0-9-]+\.)+[A-Za-z]{2,})$/
                }

                onAccepted: if (addButton.enabled)
                    addButton.clicked()
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: Kirigami.Units.largeSpacing

            QQC2.ScrollView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Kirigami.StyleHints.showFramedBackground: true
                Layout.minimumWidth: Kirigami.Units.gridUnit * 20

                ListView {
                    id: listView
                    width: parent.width
                    height: parent.height
                    clip: true
                    model: listModel
                    currentIndex: -1

                    delegate: QQC2.ItemDelegate {
                        width: ListView.view.width
                        text: model.display
                        highlighted: ListView.isCurrentItem
                        onClicked: listView.currentIndex = index
                    }
                }
            }

            ColumnLayout {
                Layout.alignment: Qt.AlignTop
                spacing: Kirigami.Units.smallSpacing

                QQC2.Button {
                    id: addButton
                    Layout.fillWidth: true
                    text: i18n("Add")
                    icon.name: "list-add"

                    enabled: input.acceptableInput && !listModel.contains(input.text)

                    onClicked: {
                        listModel.append(input.text);
                        input.clear();
                        input.forceActiveFocus();
                    }
                }

                QQC2.Button {
                    Layout.fillWidth: true
                    text: i18n("Remove")
                    icon.name: "list-remove"
                    enabled: listView.currentIndex >= 0
                    onClicked: listModel.removeAt(listView.currentIndex)
                }

                QQC2.Button {
                    Layout.fillWidth: true
                    text: i18n("Move Up")
                    icon.name: "arrow-up"
                    enabled: listView.currentIndex > 0

                    onClicked: {
                        const i = listView.currentIndex;
                        listModel.moveUp(i);
                        listView.currentIndex = i - 1;
                    }
                }

                QQC2.Button {
                    Layout.fillWidth: true
                    text: i18n("Move Down")
                    icon.name: "arrow-down"
                    enabled: listView.currentIndex >= 0 && listView.currentIndex < listView.count - 1

                    onClicked: {
                        const i = listView.currentIndex;
                        listModel.moveDown(i);
                        listView.currentIndex = i + 1;
                    }
                }
            }
        }
    }
}
