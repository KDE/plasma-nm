// SPDX-FileCopyrightText: 2024 Sebastian Kügler <sebas@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.coreaddons as KCoreAddons
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.plasma.networkmanagement as PlasmaNM


FormCard.FormCardPage {
    id: connectionInfo
    title: i18nc("kcm page title", "Connection Info for \"%1\"", connectionName)

    property string connectionName: ""
    property PlasmaNM.ConnectionDetailsModel detailsModel: null
    property QtObject delegate: null // for reaching rx/txSpeed

    FormCard.FormHeader {
        title: i18nc("@title:group", "Transfer Rates")
    }

    FormCard.FormCard {
        padding: Math.round(Kirigami.Units.gridUnit / 2)

        PlasmaNM.TrafficMonitor {
            id: trafficMonitorGraph
            width: parent.width
            downloadSpeed: delegate.rxSpeed
            uploadSpeed: delegate.txSpeed
        }
        Controls.Label {
            font: Kirigami.Theme.smallFont
            horizontalAlignment: Text.AlignRight
            Layout.fillWidth: true
            text: i18n("Connected, ↓ %1/s, ↑ %2/s",
                KCoreAddons.Format.formatByteSize(delegate.rxSpeed),
                KCoreAddons.Format.formatByteSize(delegate.txSpeed))
            textFormat: Text.PlainText
        }
    }

    FormCard.FormHeader {
        title: i18nc("@title:group", "Connection Details")
    }

    FormCard.FormCard {
        Repeater {
            model: connectionInfo.detailsModel

            FormCard.FormTextDelegate {
                required property bool isSection
                required property string sectionTitle
                required property string detailLabel
                required property string detailValue

                text: isSection ? sectionTitle : detailLabel
                description: isSection ? "" : detailValue
                textItem.font.bold: isSection
                enabled: true
            }
        }
    }
}
