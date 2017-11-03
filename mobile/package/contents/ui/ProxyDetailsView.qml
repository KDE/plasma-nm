import QtQuick 2.2
import QtQuick.Controls 1.4 as Controls
import QtQuick.Layouts 1.2
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.extras 2.0 as PlasmaExtras
ColumnLayout{
        spacing: units.gridUnit

    PlasmaComponents.Label{
        text:i18n("Proxy settings")
    }
/*
    Controls.ComboBox{
        id: proxyComboBox
        model: [i18n("None"),i18n("Manual")]
    }
*/
    Controls.CheckBox{
        id: manualProxyCheck
        checked: false
        onCheckedChanged: {
            manuaProxylSettings.visible = checked
        }
    }

    Item{
        id: manuaProxylSettings
        anchors.top: manualProxyCheck.bottom
        visible: false
        Controls.TextField{
            placeholderText: i18n("None")
        }
    }
}
