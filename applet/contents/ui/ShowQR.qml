/*
    SPDX-FileCopyrightText: 2019 Aleix Pol Gonzalez <aleixpol@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

import org.kde.plasma.core 2.1 as PlasmaCore
import org.kde.plasma.components 3.0 as PlasmaComponents3
import org.kde.prison 1.0 as Prison

Window {
    id: window
    color: Qt.rgba(0, 0, 0, 0.3)
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint

    property alias content: dataInput.content

    function destroyWindow() {
        window.close();
        window.destroy();
    }

    Shortcut {
        sequences: [StandardKey.Back, StandardKey.Cancel, StandardKey.Close, StandardKey.Delete, StandardKey.Quit]
        onActivated: window.destroyWindow()
    }

    MouseArea {
        anchors.fill: parent

        onClicked: window.destroyWindow()

        Prison.Barcode {
            id: dataInput

            anchors {
                fill: parent
                margins: PlasmaCore.Units.smallSpacing * 8
            }

            barcodeType: "QRCode"
        }

        // Because some people like to have something visible to click on,
        // and to them, it might not be obvious that you close the overlay
        // by clicking anywhere on it.
        PlasmaComponents3.Button {
            anchors {
                right: parent.right
                top: parent.top
            }

            icon.name: "window-close"
            onClicked: window.destroyWindow()
        }
    }
}
