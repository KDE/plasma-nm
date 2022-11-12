/*
    SPDX-FileCopyrightText: 2013-2017 Jan Grulich <jgrulich@redhat.com>
    SPDX-FileCopyrightText: 2022 Kai Uwe Broulik <kde@broulik.de>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

import QtQuick 2.15
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.15 as QQC2
import org.kde.plasma.components 3.0 as PlasmaComponents3
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.extras 2.0 as PlasmaExtras
import org.kde.plasma.networkmanagement 0.2 as PlasmaNM

PlasmaExtras.Representation {
    id: full
    collapseMarginsHint: true
    property alias toolbarValues: toolbar

    Component {
        id: networkModelComponent
        PlasmaNM.NetworkModel {}
    }

    property PlasmaNM.NetworkModel connectionModel: null

    PlasmaNM.AppletProxyModel {
        id: appletProxyModel

        sourceModel: full.connectionModel
    }

    header: stack.currentItem.header || connectionListHeader

    Keys.forwardTo: [stack.currentItem]
    Keys.onPressed: {
        if (event.modifiers & Qt.ControlModifier && event.key == Qt.Key_F) {
            toolbar.searchTextField.forceActiveFocus();
            toolbar.searchTextField.selectAll();
            event.accepted = true;
        } else if (event.key === Qt.Key_Back || (event.modifiers & Qt.AltModifier && event.key == Qt.Key_Left)) {
            if (stack.depth > 1) {
                stack.pop();
                event.accepted = true;
            }
        }

        event.accepted = false;
    }

    PlasmaExtras.PlasmoidHeading {
        id: connectionListHeader
        focus: true
        leftPadding: PlasmaCore.Units.smallSpacing
        contentItem: Toolbar {
            id: toolbar
            width: parent.width
            hasConnections: connectionListPage.view.count > 0
            qrCodeScanSupported: {
                // Checks whether Prison scanner and QtMultimedia imports are available
                // and that there is a camera.
                try {
                    const testItem = Qt.createQmlObject(`
                        import QtQml 2.15
                        import QtMultimedia 5.15
                        import org.kde.prison.scanner 1.0 as Prison

                        QtObject {
                            readonly property bool hasCamera: QtMultimedia.defaultCamera !== null
                        }
                    `, this, "qrCodeScanTest");

                    const supported = testItem && testItem.hasCamera;

                    testItem.destroy();

                    return supported;
                } catch (e) {
                    console.log("QR code scanning is not supported", e);
                    return false;
                }
            }

            onScanQrCodeRequested: stack.push("QrCodePage.qml")
        }
    }

    QQC2.StackView {
        id: stack
        anchors.fill: parent
        initialItem: ConnectionListPage {
            id: connectionListPage
        }

        Connections {
            target: stack.currentItem
            ignoreUnknownSignals: true
            function onBackRequested() {
                stack.pop();
            }
        }
    }

    Connections {
        target: plasmoid
        function onExpandedChanged(expanded) {
            if (expanded) {
                handler.requestScan();
                full.connectionModel = networkModelComponent.createObject(full)
            } else {
                full.connectionModel.destroy()
                stack.pop(null); // go back to start page
            }
        }
    }
}
