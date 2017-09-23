import QtQuick 2.2
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.extras 2.0 as PlasmaExtras

Item{
    anchors.fill:parent
    Rectangle{
        //anchors.horizontalCenter: parent
        width: units.gridUnit * 20
        height: units.gridUnit * 5
        color: "red"
        Text {
            text: qsTr("TEST")
        }
    }

}
