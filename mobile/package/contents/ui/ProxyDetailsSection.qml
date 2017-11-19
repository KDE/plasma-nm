import QtQuick 2.2
import QtQuick.Controls 1.4 as Controls
import QtQuick.Layouts 1.2
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.extras 2.0 as PlasmaExtras
ColumnLayout{
        spacing: units.gridUnit

    PlasmaComponents.Label{
        text: i18n("Proxy settings")
    }

    Controls.CheckBox{
        id: manualProxyCheck
        checked: false
        onCheckedChanged: {
            manuaProxySettings.visible = checked
        }
    }

    ColumnLayout{
        id: manuaProxySettings
        anchors.top: manualProxyCheck.bottom
        visible: false

        PlasmaComponents.Label{
            text: i18n("Proxy hostname")
        }

        Controls.TextField{
            placeholderText: i18n("my.proxy.com")
        }

        PlasmaComponents.Label{
            text: i18n("Proxy port")
        }

        Controls.TextField{
            placeholderText: i18n("8080")
        }

        PlasmaComponents.Label{
            text: i18n("Bypass proxy for")
        }

        Controls.TextField{
            placeholderText: i18n("example.com,mytest.com")
        }
    }
}
