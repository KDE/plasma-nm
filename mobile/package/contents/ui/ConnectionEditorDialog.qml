import QtQuick 2.2
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.extras 2.0 as PlasmaExtras

Item{
    property var details: []
    anchors.fill:parent
    height: 100
    
    function clicked(f){
            details = f
       }
       
    Column {
        anchors.fill: parent
        Text{
            text:details[0]
            anchors.fill:parent
        }
        Repeater {
            id: repeater

            property int contentHeight: 0
            property int longestString: 0

            model: details.length / 2

            Item {
                anchors {
                    left: parent.left
                    right: parent.right
                    topMargin: Math.round(units.gridUnit / 3)
                }
                height: Math.max(detailNameLabel.height, detailValueLabel.height)

                PlasmaComponents.Label {
                    id: detailNameLabel

                    anchors {
                        left: parent.left
                       // leftMargin: repeater.longestString - paintedWidth + Math.round(units.gridUnit / 2)
                        verticalCenter: parent.verticalCenter
                    }
                    height: paintedHeight
                    font.pointSize: theme.smallestFont.pointSize
                    horizontalAlignment: Text.AlignRight
                    opacity: 0.6
                    text: "<b>" + details[index*2] + "</b>: &nbsp"

                    Component.onCompleted: {
                        if (paintedWidth > repeater.longestString) {
                            repeater.longestString = paintedWidth
                        }
                    }
                }

                PlasmaComponents.Label {
                    id: detailValueLabel

                    anchors {
                        left: detailNameLabel.right
                        right: parent.right
                        verticalCenter: parent.verticalCenter
                    }
                    height: paintedHeight
                    elide: Text.ElideRight
                    font.pointSize: theme.smallestFont.pointSize
                    opacity: 0.6
                    text: details[(index*2)+1]
                    textFormat: Text.PlainText

                }
            }

            // Count total height from added items, somehow contentRect.height doesn't work
            onItemAdded: {
                contentHeight = contentHeight + item.height
            }
        }
    
    }

}
