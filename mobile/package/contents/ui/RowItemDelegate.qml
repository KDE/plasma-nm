import QtQuick 2.0
import QtQuick.Controls 1.4 as Controls
import QtQuick.Layouts 1.2
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.networkmanagement 0.2 as PlasmaNM

PlasmaComponents.ListItem {
    width: parent.width
    
    RowLayout {
        width: parent.width

        PlasmaComponents.Label {
            id: connectionNameLabel

            anchors {
                leftMargin: Math.round(units.gridUnit / 2)
            }
            
            height: paintedHeight
            elide: Text.ElideRight
            font.weight: ConnectionState == PlasmaNM.Enums.Activated ? Font.DemiBold : Font.Normal
            font.italic: ConnectionState == PlasmaNM.Enums.Activating ? true : false
            text: ItemUniqueName
            textFormat: Text.PlainText
        }

        Rectangle {
            anchors.horizontalCenter: parent.Center
            width: units.gridUnit
            height: units.gridUnit
            color: status
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    connectionEditorDialog.open()
                }
            }
        }
    }
}
