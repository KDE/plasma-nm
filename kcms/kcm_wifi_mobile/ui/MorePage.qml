// SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls

import org.kde.plasma.networkmanagement as PlasmaNM
import org.kde.kirigami as Kirigami
import org.kde.kcmutils
import org.kde.kirigamiaddons.formcard as FormCard

Kirigami.ScrollablePage {
    title: i18n("Wi-Fi Settings")

    topPadding: Kirigami.Units.gridUnit
    bottomPadding: Kirigami.Units.gridUnit
    leftPadding: 0
    rightPadding: 0

    ColumnLayout {

        FormCard.FormCard {
            FormCard.FormSwitchDelegate {
                id: connectionNotificationsSwitch
                text: i18n("Notifications for connection changes")
                checked: kcm.connectionNotificationsEnabled
                onCheckedChanged: {
                    kcm.connectionNotificationsEnabled = checked;
                    checked = Qt.binding(() => kcm.connectionNotificationsEnabled);
                }
            }
        }
    }
}
