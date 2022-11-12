/*
    SPDX-FileCopyrightText: 2022 Kai Uwe Broulik <kde@broulik.de>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

import QtQuick 2.15
import QtQuick.Layouts 1.12
import QtMultimedia 5.15 as QtMultimedia
import org.kde.plasma.components 3.0 as PlasmaComponents3
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.extras 2.0 as PlasmaExtras
import org.kde.plasma.networkmanagement 0.2 as PlasmaNM
import org.kde.prison.scanner 1.0 as Prison

ColumnLayout {
    id: qrCodePage

    signal backRequested

    property Item header: PlasmaExtras.PlasmoidHeading {
        RowLayout {
            anchors.fill: parent
            PlasmaComponents3.Button {
                Layout.fillWidth: true
                icon.name: "go-previous-view"
                text: i18n("Return to Network Connections")
                onClicked: qrCodePage.backRequested()
            }

            // TODO add camera selector?
        }
    }

    QtMultimedia.Camera {
        id: camera
        focus {
            focusMode: QtMultimedia.Camera.FocusContinuous
            focusPointMode: QtMultimedia.Camera.FocusPointCenter
        }
    }

    Prison.VideoScanner {
        id: scanner
        formats: Prison.Format.QRCode | Prison.Format.Aztec | Prison.Format.DataMatrix | Prison.Format.PDF417
        onResultContentChanged: {
            if (result.hasText) {
                // TODO parse result.text
                console.log("Found QR code:", result.text);
            }
        }
    }

    PlasmaComponents3.Label {
        Layout.fillWidth: true
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.WordWrap
        text: i18n("Join WiFi network by scanning a QR code.")
    }

    QtMultimedia.VideoOutput {
        Layout.fillWidth: true
        Layout.fillHeight: true

        source: camera
        filters: [scanner]
        autoOrientation: true
        fillMode: QtMultimedia.VideoOutput.PreserveAspectCrop

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.BackButton
            onPressed: {
                qrCodePage.backRequested();
            }
        }

        Item {
            anchors.fill: parent
            // FIXME how is this supposed to work?! All I want is dark background with light text and light icons :(((
            PlasmaCore.ColorScope.colorGroup: PlasmaCore.Theme.ComplementaryColorGroup
            PlasmaCore.ColorScope.inherit: true

            Rectangle {
                anchors.fill: parent
                color: PlasmaCore.ColorScope.backgroundColor
                opacity: camera.cameraStatus === QtMultimedia.Camera.ActiveStatus ? 0 : 1

                Behavior on opacity {
                    OpacityAnimator {
                        duration: PlasmaCore.Units.longDuration
                        easing.type: Easing.InOutQuad
                    }
                }
                visible: opacity > 0
            }

            // FIXME why does this not get proper inverted color from ComplementaryColorGroup above?
            PlasmaExtras.PlaceholderMessage {
                anchors.fill: parent

                iconName: "edit-none"
                text: camera.errorString
                visible: camera.errorCode !== QtMultimedia.Camera.NoError
            }
        }
    }
}
