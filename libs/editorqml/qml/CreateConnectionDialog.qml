/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami

import org.kde.plasma.networkmanagement as PlasmaNM
import org.kde.plasma.networkmanagement.editorqml as PlasmaNMQ

Kirigami.Dialog {
    id: dialog

    property bool saved: false

    title: i18nc("@title:window", "New Connection")

    parent: QQC2.Overlay.overlay

    preferredWidth: Kirigami.Units.gridUnit * 34

    standardButtons: Kirigami.Dialog.Save | Kirigami.Dialog.Cancel

    function openForNewConnection(): void {
        dialog.saved = false;
        dialog.open();
    }

    Component.onCompleted: {
        const saveButton = dialog.standardButton(Kirigami.Dialog.Save);
        if (saveButton) {
            saveButton.enabled = Qt.binding(() => kcm.connectionValid);
        }
    }

    onAccepted: {
        dialog.saved = true;
        kcm.save();
    }

    onClosed: {
        if (!dialog.saved) {
            kcm.cancelNewConnection();
        }
    }

    ColumnLayout {
        spacing: Kirigami.Units.largeSpacing

        Component {
            id: wirelessPage

            PlasmaNMQ.WirelessPage {}
        }

        Component {
            id: wiredPage

            PlasmaNMQ.WiredPage {}
        }

        Component {
            id: vpnPage

            PlasmaNMQ.VpnPage {}
        }

        Component {
            id: wireguardPage

            PlasmaNMQ.WireGuardPage {}
        }

        RowLayout {
            Layout.fillWidth: true

            spacing: Kirigami.Units.smallSpacing

            QQC2.Label {
                text: i18nc("@label", "Connection name:")
            }

            QQC2.TextField {
                Layout.fillWidth: true

                text: kcm.connectionName
                onTextEdited: kcm.connectionName = text
            }
        }

        Loader {
            Layout.fillWidth: true

            Layout.preferredHeight: Kirigami.Units.gridUnit * 24

            sourceComponent: {
                switch (kcm.connectionType) {
                case PlasmaNM.Enums.Wireless:
                    return wirelessPage;
                case PlasmaNM.Enums.Wired:
                    return wiredPage;
                case PlasmaNM.Enums.Vpn:
                    return vpnPage;
                case PlasmaNM.Enums.WireGuard:
                    return wireguardPage;
                default:
                    return null;
                }
            }
        }
    }
}
