/*
    SPDX-FileCopyrightText: 2023 Kai Uwe Broulik <kde@broulik.de>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

import QtQuick
import QtQuick.Layouts

import org.kde.plasma.components as PlasmaComponents3
import org.kde.kirigami as Kirigami

import org.kde.prison as Prison

ColumnLayout {
    id: page

    property alias content: barcode.content
    property string ssid
    property string password

    readonly property real verticalPadding: Kirigami.Units.largeSpacing
    spacing: Kirigami.Units.smallSpacing

    PlasmaComponents3.Label {
        Layout.topMargin: page.verticalPadding
        Layout.alignment: Qt.AlignHCenter
        Layout.minimumWidth: barcode.height
        Layout.maximumWidth: barcode.width - page.spacing * 2
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignHCenter
        textFormat: Text.PlainText
        text: i18n("Scan this QR code with another device to connect to the \"%1\" network.", page.ssid)
    }

    Prison.Barcode {
        id: barcode

        readonly property bool valid: implicitWidth > 0 && implicitHeight > 0

        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.margins: Kirigami.Units.gridUnit

        barcodeType: Prison.Barcode.QRCode

        Drag.dragType: Drag.Automatic
        Drag.supportedActions: Qt.CopyAction

        HoverHandler {
            enabled: barcode.valid
            cursorShape: Qt.OpenHandCursor
        }

        DragHandler {
            id: dragHandler
            enabled: barcode.valid
            cursorShape: Qt.ClosedHandCursor
            target: null

            onActiveChanged: {
                if (active) {
                    barcode.grabToImage((result) => {
                        barcode.Drag.mimeData = {
                            "image/png": result.image,
                        };
                        barcode.Drag.active = dragHandler.active;
                    });
                } else {
                    barcode.Drag.active = false;
                }
            }
        }

        Rectangle {
            anchors.centerIn: barcode
            width: Math.floor(Math.min(barcode.width, barcode.height)) + 2 * radius
            height: width
            color: barcode.backgroundColor
            radius: Kirigami.Units.cornerRadius
            z: -1
        }
    }

    // Try to keep them centered, but keep the label visible and wrap the password if needed
    GridLayout {
        id: passwordLayout

        readonly property real implicitContentWidth: passwordLabel.implicitWidth + passwordSelectable.implicitWidth + columnSpacing
        state: implicitContentWidth <= width ? "inline" : "wrap"

        visible: page.password !== ""
        columns: passwordLayout.state === "inline" ? 2 : 1
        columnSpacing: Kirigami.Units.largeSpacing
        rowSpacing: 0
        Layout.bottomMargin: page.verticalPadding
        Layout.alignment: Qt.AlignHCenter
        Layout.maximumWidth: barcode.width - page.spacing * 2

        PlasmaComponents3.Label {
            id: passwordLabel
            Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
            Layout.fillWidth: passwordLayout.state === "wrap"
            Layout.preferredWidth: implicitWidth + (passwordLayout.width - passwordLayout.implicitContentWidth) / 2
            horizontalAlignment: passwordLayout.state === "inline" ? Text.AlignRight : Text.AlignLeft
            wrapMode: Text.NoWrap
            textFormat: Text.PlainText
            text: i18n("Wi-Fi password:")
        }

        Kirigami.SelectableLabel {
            id: passwordSelectable
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignLeft
            textFormat: Text.PlainText
            wrapMode: TextEdit.Wrap
            font.family: "monospace"
            text: page.password
        }
    }
}
