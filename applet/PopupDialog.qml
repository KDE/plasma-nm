/*
    SPDX-FileCopyrightText: 2013-2017 Jan Grulich <jgrulich@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.plasma.components as PlasmaComponents3
import org.kde.plasma.extras as PlasmaExtras
import org.kde.plasma.networkmanagement as PlasmaNM
import org.kde.plasma.plasmoid

PlasmaExtras.Representation {
    id: full

    required property PlasmaNM.Handler nmHandler
    required property PlasmaNM.NetworkStatus nmStatus

    collapseMarginsHint: true

    Component {
        id: networkModelComponent
        PlasmaNM.NetworkModel {}
    }

    property PlasmaNM.NetworkModel connectionModel: null

    PlasmaNM.AppletProxyModel {
        id: appletProxyModel

        sourceModel: full.connectionModel
    }

    header: PlasmaExtras.PlasmoidHeading {
        focus: true
        contentItem: RowLayout {
            Layout.fillWidth: true

            Toolbar {
                id: toolbar
                Layout.fillWidth: true
                hasConnections: connectionListPage.count > 0
                visible: stack.depth === 1
            }

            PlasmaComponents3.Button {
                Layout.fillWidth: true
                icon.name: mirrored ? "go-next" : "go-previous"
                text: i18nc("@action:button", "Return to Network Connections")
                visible: stack.depth > 1
                onClicked: {
                    stack.pop()
                }
            }

            Loader {
                sourceComponent: stack.currentItem?.headerItems
                visible: !!item
            }
        }
    }

    Connections {
        target: full.nmHandler
        function onWifiCodeReceived(data, ssid, password) {
            if (data.length === 0) {
                console.error("Cannot create QR code component: Unsupported connection");
                return;
            }

            const showQRComponent = Qt.createComponent("ShareNetworkQrCodePage.qml");
            if (showQRComponent.status === Component.Error) {
                console.warn("Cannot create QR code component:", showQRComponent.errorString());
                return;
            }

            mainWindow.expanded = true; // just in case.
            stack.push(showQRComponent, {
                content: data,
                ssid,
                password,
            });
        }
    }

    Keys.forwardTo: [stack.currentItem]
    Keys.onPressed: event => {
        if (event.modifiers & Qt.ControlModifier && event.key == Qt.Key_F) {
            toolbar.searchTextField.forceActiveFocus();
            toolbar.searchTextField.selectAll();
            event.accepted = true;
        } else if (event.key === Qt.Key_Back || (event.modifiers & Qt.AltModifier && event.key == Qt.Key_Left)) {
            if (stack.depth > 1) {
                stack.pop();
                event.accepted = true;
            }
        } else {
            event.accepted = false;
        }
    }

    QQC2.StackView {
        id: stack
        anchors.fill: parent
        initialItem: ConnectionListPage {
            id: connectionListPage
            model: appletProxyModel
            nmStatus: full.nmStatus
        }
    }

    Connections {
        target: mainWindow
        function onExpandedChanged(expanded) {
            if (expanded) {
                handler.requestScan();
                if (!full.connectionModel) {
                    full.connectionModel = networkModelComponent.createObject(full);
                }
            } else {
                if (full.connectionModel) {
                    full.connectionModel.destroy();
                    full.connectionModel = null;
                }
            }
        }
    }
}
