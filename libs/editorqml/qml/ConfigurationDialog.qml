/*
      SPDX-FileCopyrightText: 2019 Jan Grulich <jgrulich@redhat.com>
      SPDX-FileCopyrightText: 2023 ivan tkachenko <me@ratijas.tk>
      SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

      SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kcmutils as KCMUtils
import org.kde.kirigami as Kirigami
import org.kde.plasma.networkmanagement as PlasmaNM

QQC2.ApplicationWindow {
    id: root

    title: i18nc("@title:window", "Configuration")

    minimumHeight: Kirigami.Units.gridUnit * 6
    maximumHeight: page.implicitHeight + dialogButtonBox.implicitHeight
    height: maximumHeight
    minimumWidth: Kirigami.Units.gridUnit * 15
    width: Kirigami.Units.gridUnit * 25

    KCMUtils.SimpleKCM {
        id: page

        anchors.fill: parent

        Kirigami.FormLayout {
            Kirigami.Separator {
                Kirigami.FormData.label: i18n("General")
                Kirigami.FormData.isSection: true
            }

            QQC2.CheckBox {
                id: unlockModem
                Layout.fillWidth: true
                KeyNavigation.down: manageVirtualConnections
                text: i18n("Ask for PIN on modem detection")
            }

            QQC2.CheckBox {
                id: manageVirtualConnections
                Layout.fillWidth: true
                KeyNavigation.down: systemConnectionsByDefault
                text: i18n("Show virtual connections")
            }

            QQC2.CheckBox {
                id: systemConnectionsByDefault
                Layout.fillWidth: true
                text: i18n("Create new connections as system connections")
            }
        }
    }

    footer: QQC2.DialogButtonBox {
        id: dialogButtonBox

        standardButtons: QQC2.DialogButtonBox.Ok | QQC2.DialogButtonBox.Cancel | QQC2.DialogButtonBox.Reset

        onAccepted: root.accept()
        onRejected: root.reject()
        onReset: root.loadConfiguration()

        KeyNavigation.up: systemConnectionsByDefault

        Component.onCompleted: {
            const okButton = standardButton(QQC2.DialogButtonBox.Ok);
            if (okButton) {
                systemConnectionsByDefault.KeyNavigation.down = okButton;
            }
        }
    }

    Shortcut {
        sequences: ["Return", "Enter"]
        onActivated: root.accept()
    }

    Shortcut {
        sequences: [StandardKey.Back, StandardKey.Cancel, StandardKey.Close]
        onActivated: root.reject()
    }

    // Unfortunately, unlike Dialog, DialogButtonBox lacks accept and reject methods.
    function accept() {
        saveConfiguration();
        close();
    }

    function reject() {
        close();
    }

    function loadConfiguration() {
        unlockModem.checked = PlasmaNM.Configuration.unlockModemOnDetection;
        manageVirtualConnections.checked = PlasmaNM.Configuration.manageVirtualConnections;
        systemConnectionsByDefault.checked = PlasmaNM.Configuration.systemConnectionsByDefault;
    }

    function saveConfiguration() {
        PlasmaNM.Configuration.unlockModemOnDetection = unlockModem.checked;
        PlasmaNM.Configuration.manageVirtualConnections = manageVirtualConnections.checked;
        PlasmaNM.Configuration.systemConnectionsByDefault = systemConnectionsByDefault.checked;
    }

    onVisibleChanged: {
        if (visible) {
            loadConfiguration();
            unlockModem.forceActiveFocus(Qt.ActiveWindowFocusReason);
        }
    }
}
