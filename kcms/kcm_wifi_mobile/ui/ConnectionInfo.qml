// SPDX-FileCopyrightText: 2024 Sebastian Kügler <sebas@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.coreaddons as KCoreAddons
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.plasma.networkmanagement as PlasmaNM
import org.kde.prison as Prison


FormCard.FormCardPage {
    id: connectionInfo
    title: i18nc("kcm page title", "Connection Info for \"%1\"", connectionName)

    property string connectionName: ""
    property var details: []
    property QtObject delegate: null // for reaching rx/txSpeed

    property string connectionPath: ""
    property string qrCodeString: ""

    function generateQrCode() {
        let wirelessSettings = kcm.getConnectionSettings(connectionPath, "802-11-wireless");
        let securitySettings = kcm.getConnectionSettings(connectionPath, "802-11-wireless-security");
        let secrets = kcm.getConnectionSettings(connectionPath, "secrets");

        let ssid = wirelessSettings["ssid"] || connectionName;
        let type = "nopass";
        let password = "";

        // Checks if security settings exist (prevents crashes on open networks)
        if (securitySettings && securitySettings["key-mgmt"]) {
            let keyMgmt = securitySettings["key-mgmt"];
            if (keyMgmt === "wpa-psk" || keyMgmt === "sae") {
                type = "WPA";
                password = secrets["psk"] || "";
            } else if (keyMgmt === "wep" || keyMgmt === "ieee8021x") {
                type = "WEP";
                password = secrets["wep-key0"] || "";
            }
        }

        // If no password, omit the P block entirely.
        if (type === "nopass" || password === "") {
            qrCodeString = "WIFI:T:nopass;S:" + ssid + ";;";
        } else {
            qrCodeString = "WIFI:T:" + type + ";S:" + ssid + ";P:" + password + ";;";
        }
    }

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
            /* details is the ConnectionDetails property of the
            * connection model item, a flat stringlist with
            * title / value pairs.
            */
            model: details.length / 2

            FormCard.FormTextDelegate {
                text: details[index * 2]
                description: details[(index * 2) + 1]
                enabled: true
            }
        }
    }

     // Share section
    FormCard.FormHeader {
        title: i18nc("@title:group", "Share")
    }

    FormCard.FormCard {
        FormCard.FormButtonDelegate {
            id: shareButton
            icon.name: "view-barcode-qr" 
            text: i18n("Share Wi-Fi via QR Code")
            onClicked: qrDialog.open()
        }
    }
  
    Kirigami.Dialog {
        id: qrDialog
        title: i18n("Wi-Fi QR Code")
        standardButtons: Kirigami.Dialog.Close

        onOpened: generateQrCode()
        onClosed: qrCodeString = ""

        ColumnLayout {
            spacing: Kirigami.Units.largeSpacing

            Prison.Barcode {
                Layout.alignment: Qt.AlignHCenter
                Layout.preferredWidth: Kirigami.Units.gridUnit * 12
                Layout.preferredHeight: Kirigami.Units.gridUnit * 12
                content: qrCodeString
                barcodeType: Prison.Barcode.QRCode
            }
        }
    }
}

