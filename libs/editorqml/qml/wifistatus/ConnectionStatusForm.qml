/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>
    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/
import QtQuick
import QtQuick.Controls as QQC2
import org.kde.kirigami as Kirigami
import org.kde.plasma.networkmanagement.editorqml

Kirigami.FormLayout {
    id: root
    required property ConnectionStatus connectionStatus
    QQC2.Label {
        visible: !root.connectionStatus.hasDetails
        text: i18n("Disconnected")
        horizontalAlignment: Text.AlignHCenter
    }
    Repeater {
        model: root.connectionStatus.detailsModel
        delegate: Item {
            required property bool isSection
            required property string sectionTitle
            required property string detailLabel
            required property string detailValue
            Kirigami.FormData.isSection: isSection
            Kirigami.FormData.label: isSection ? "" : detailLabel + ":"
            implicitHeight: isSection ? sectionLabel.implicitHeight : valueLabel.implicitHeight
            Kirigami.Heading {
                id: sectionLabel
                visible: isSection
                anchors.horizontalCenter: parent.horizontalCenter

                text: sectionTitle
                level: 2
            }
            QQC2.Label {
                id: valueLabel
                visible: !isSection
                text: detailValue
            }
        }
    }
}
