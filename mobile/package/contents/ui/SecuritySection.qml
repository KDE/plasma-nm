import QtQuick 2.0
import QtQuick.Controls 1.4 as Controls
import QtQuick.Layouts 1.2 as Layouts
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.networkmanagement 0.2 as PlasmaNM

Layouts.ColumnLayout{
    id:securitySectionView
    property var securityMap: {}
    property var enabledSave: false
    width: parent.width

    Column{
        id: securitySectionHeader

        width: parent.width
        anchors {
            top:parent.top
            topMargin: units.Gridunit
            bottomMargin: units.Gridunit
            left: parent.left
        }
        Controls.Label {
            text: i18n("Security")
            font.weight: Font.Bold
        }
        Controls.ComboBox {
            id: securityCombobox
            //anchors.bottomMargin: units.Gridunit
            anchors.bottomMargin: 50
            model: [i18n("None"), i18n("WEP Key"), i18n("Dynamic WEP"), i18n("WPA/WPA2 Personal"), i18n("WPA/WPA2 Enterprise")]
        }
        Controls.Label{
            anchors.bottomMargin: units.Gridunit
            visible: !(securityCombobox.currentText == "None")
            text: securityCombobox.currentText
        }
    }

    Layouts.ColumnLayout{
        id: wep
        anchors.top: securitySectionHeader.bottom
        width: parent.width
        Column{
            width: parent.width
            visible: (securityCombobox.currentText === "WEP Key")
            PasswordField{
                width: parent.width
                securityType: PlasmaNM.Enums.StaticWep
            }
        }
    }

    Layouts.ColumnLayout{
        id: wpaPSK
        width: parent.width
        anchors.top: securitySectionHeader.bottom
        Column{
            width: parent.width
            visible: (securityCombobox.currentText === "WPA/WPA2 Personal")
            PasswordField{
                width: parent.width
                securityType: PlasmaNM.Enums.Wpa2Psk
            }
        }
    }

    Layouts.ColumnLayout{
        id: eap
        anchors.top: securitySectionHeader.bottom
        Column{
            visible: (securityCombobox.currentText === "Dynamic WEP")
            Controls.Label{
                text:i18n("Authentication")
            }
            Controls.ComboBox {
                id: authComboBox
                model: [i18n("TLS"), i18n("LEAP"), i18n("FAST"), i18n("Tunneled TLS"), i18n("Protected EAP")] // more - SIM, AKA, PWD ?
            }
        }
    }
}
