/*
    SPDX-FileCopyrightText: 2024 Kai Uwe Broulik <kde@broulik.de>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

import QtQuick
import QtQuick.Controls as QQC
import QtQuick.Layouts
import QtMultimedia as QtMultimedia

import org.kde.plasma.components as PlasmaComponents
import org.kde.kirigami as Kirigami

import org.kde.plasma.networkmanagement as PlasmaNM

import org.kde.prison.scanner as PrisonScanner

Rectangle {
    id: page

    Kirigami.Theme.inherit: false
    Kirigami.Theme.colorSet: Kirigami.Theme.Complementary
    color: Kirigami.Theme.backgroundColor

    property Component headerItems: mediaDevices.videoInputs.length > 1 ? cameraMenuComponent : null

    QtMultimedia.MediaDevices {
        id: mediaDevices
    }

    Component {
        id: cameraMenuComponent

        PlasmaComponents.ToolButton {
            display: PlasmaComponents.ToolButton.IconOnly
            text: i18nc("@action:button", "Select Camera")
            icon.name: "camera-web"
            checked: cameraMenu.opened
            onPressed: cameraMenu.opened ? cameraMenu.close() : cameraMenu.popup()

            PlasmaComponents.ToolTip {
                text: parent.text
            }

            PlasmaComponents.Menu {
                id: cameraMenu
            }

            Instantiator {
                id: cameraInstantiator
                delegate: PlasmaComponents.MenuItem {
                    text: modelData.description
                    checkable: true
                    autoExclusive: true
                    checked: camera.cameraDevice === modelData;

                    onClicked: {
                        camera.cameraDevice = modelData;
                    }
                }
                model: mediaDevices.videoInputs

                onObjectAdded: (index, object) => cameraMenu.insertItem(index, object)
                onObjectRemoved: (index, object) => cameraMenu.removeItem(object)
            }
        }
    }

    PrisonScanner.VideoScanner {
        id: scanner
        formats: PrisonScanner.Format.QRCode
        videoSink: viewFinder.videoSink
        onResultChanged: (result) => {
            if (result.text) {
                meCardHandler.text = result.text;
            }
        }
    }

    QtMultimedia.CaptureSession {
        camera: QtMultimedia.Camera {
            id: camera
            active: page.QQC.StackView.status === QQC.StackView.Active
        }
        videoOutput: viewFinder
    }

    PlasmaNM.MeCardHandler {
        id: meCardHandler
        onSsidChanged: {
            if (ssid) {
                page.QQC.StackView.view.push("ConnectToQrCodePage.qml", {
                    meCard: meCardHandler.meCard
                });

                reset();
            }
        }
    }

    QtMultimedia.VideoOutput {
        id: viewFinder
        anchors.fill: parent

        PlasmaComponents.Label {
            width: parent.width
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            textFormat: Text.PlainText
            text: i18n("Hold a Wi-Fi network's QR code up to your camera to connect to that network.")
            // Place label ontop of viewfinder content rect.
            // There is no state property in Qt 6 anymore so we cannot hide the label
            // until the camera is fully active and the content rect determined.
            // Try to hide this a bit through the animation :-)
            y: Math.round(Math.max(0, viewFinder.contentRect.y - height - Kirigami.Units.smallSpacing))
            opacity: camera.active && camera.error === QtMultimedia.Camera.NoError ? 1 : 0
            visible: camera.active
            Behavior on opacity {
                SequentialAnimation {
                    PauseAnimation { duration: 250 }
                    NumberAnimation { }
                }
            }
        }

        PlasmaComponents.Label {
            anchors.fill: parent
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            textFormat: Text.PlainText
            text: camera.errorString
            visible: camera.error !== QtMultimedia.Camera.NoError
        }
    }
}
