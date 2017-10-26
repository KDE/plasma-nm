import QtQuick 2.0
import QtQuick.Controls 1.4 as Controls
import QtQuick.Layouts 1.2
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.networkmanagement 0.2 as PlasmaNM

PlasmaComponents.ListItem {
    width: parent.width
    property var foo : []
    RowLayout {
        width: parent.width
        
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    foo = ConnectionDetails
                    clicked(foo)
                    connectionEditorDialog.open()
                }
            }
            
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

        PlasmaCore.SvgItem {
            id: connectionSvgIcon

            anchors {
                right: parent.right
                verticalCenter: parent.verticalCenter
            }
            elementId: ConnectionIcon
            height: units.iconSizes.medium; width: height
            svg: PlasmaCore.Svg {
                multipleImages: true
                imagePath: "icons/network"
                colorGroup: PlasmaCore.ColorScope.colorGroup
            }
        }
    }

PlasmaComponents.CommonDialog {
        id: connectionEditorDialog
        titleText: i18n("Connection Editor")
        buttonTexts: [i18n("Close")]
        onButtonClicked: close()
        content: Loader {
            id: connectionEditorDialogLoader
            width: units.gridUnit * 22
            height: units.gridUnit * 25
            }

        onStatusChanged: {
            if (status == PlasmaComponents.DialogStatus.Open) {
                connectionEditorDialogLoader.source = "ConnectionEditorDialog.qml"
                connectionEditorDialogLoader.item.focusTextInput()
            }
        }
    }
}