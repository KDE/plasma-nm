import QtQuick 2.2
import QtQuick.Controls 1.4 as Controls
import QtQuick.Layouts 1.2
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.extras 2.0 as PlasmaExtras

Item{
    property var details
    property var str: 0

    Column{
        anchors.fill: parent

        PlasmaComponents.Label{
            anchors.left: parent.left
            text: "SSID"
        }

        PlasmaComponents.TextField{
            width:100 // units.GridUnit * 100
        }

        PlasmaComponents.Label{
            anchors.left: parent.left
            text: "Security"
        }
        Controls.ComboBox{
            model: ["None","WEP","WPA","802.1x EAP"]
        }
        RowLayout{
            PlasmaComponents.Label{
                anchors.left: parent.left
                text: "Advanced options"
            }
            PlasmaComponents.Switch{

            }
        }
    }

}
