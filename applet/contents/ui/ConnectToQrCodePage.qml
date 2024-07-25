/*
    SPDX-FileCopyrightText: 2024 Kai Uwe Broulik <kde@broulik.de>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

import QtQuick
import QtQuick.Controls as QQC
import QtQuick.Layouts

import org.kde.plasma.components as PlasmaComponents
import org.kde.kirigami as Kirigami

import org.kde.plasma.networkmanagement as PlasmaNM

Item {
    id: page

    property alias meCard: meCardHandler.meCard

    PlasmaNM.MeCardHandler {
        id: meCardHandler
    }

    // Inspired by Kirigami.PlaceholderMessage.
    ColumnLayout {
        anchors.verticalCenter: parent.verticalCenter
        width: parent.width

        spacing: Kirigami.Units.gridUnit

        Kirigami.Icon {
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: Math.round(Kirigami.Units.iconSizes.huge * 1.5)
            Layout.preferredHeight: Math.round(Kirigami.Units.iconSizes.huge * 1.5)

            // Search the model for the connection and use its icon.
            source: {
                const model = full.connectionModel;
                for (let i = 0; i < model.rowCount(); ++i) {
                    const idx = model.index(i, 0);
                    const ssid = idx.data(PlasmaNM.NetworkModel.SsidRole);
                    if (ssid === meCardHandler.ssid) {
                        return idx.data(PlasmaNM.NetworkModel.ConnectionIconRole);
                    }
                }
                return "network-wireless-available";
            }
        }

        Kirigami.Heading {
            Layout.fillWidth: true
            text: meCardHandler.ssid
            type: Kirigami.Heading.Primary
            horizontalAlignment: Qt.AlignHCenter
            wrapMode: Text.WordWrap
            textFormat: Text.PlainText
        }

        PlasmaComponents.Button {
            Layout.alignment: Qt.AlignHCenter
            Layout.topMargin: Kirigami.Units.gridUnit
            text: i18nc("@action:button", "Connect")
            onClicked: {
                meCardHandler.activateConnection();
                page.QQC.StackView.view.pop(null); // go back to start page.
            }
        }
    }
}
