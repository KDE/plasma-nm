import QtQuick 2.2
import QtQuick.Controls 1.4 as Controls
import QtQuick.Layouts 1.2
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.extras 2.0 as PlasmaExtras

ColumnLayout{
        spacing: units.gridUnit

    PlasmaComponents.Label{
        text: i18n("IP Settings")
    }

    Controls.CheckBox{
        id: manualIPCheckbox
        checked: false
        onCheckedChanged: {
            manuaIPSettings.visible = checked
        }
    }

    ColumnLayout{
        id: manuaIPSettings
        anchors.top: manualIPCheckbox.bottom
        visible: false
        PlasmaComponents.Label{
            text: i18n("IP Address")
        }

        Controls.TextField{
            placeholderText: i18n("193.168.1.128")
        }

        PlasmaComponents.Label{
            text: i18n("Gateway")
        }

        Controls.TextField{
            placeholderText: i18n("192.168.1.1")
        }

        PlasmaComponents.Label{
            text: i18n("Network proxy length")
        }

        Controls.TextField{
            placeholderText: i18n("24")
        }

        PlasmaComponents.Label{
            text: i18n("DNS")
        }

        Controls.TextField{
            placeholderText: i18n("8.8.8.8")
        }

        PlasmaComponents.Label{
            text: i18n("Network proxy length")
        }

        Controls.TextField{
            placeholderText: i18n("24")
        }
    }
}
