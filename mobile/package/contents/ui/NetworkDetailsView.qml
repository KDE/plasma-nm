import QtQuick 2.2
import QtQuick.Controls 1.4 as Controls
import QtQuick.Layouts 1.2
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.extras 2.0 as PlasmaExtras

import QtQuick 2.0

Item {
    property var details
    property var str: 0

    Column{
        PlasmaComponents.Label{
            anchors.left: parent.left
            text: "Strength: "
        }

        PlasmaComponents.Label{
            anchors.right: parent.right
            text: str
        }
    }

    function fillDetails(){
        for(var i=0;i<(details.length/2);i++){
            console.info(details[(i*2)+1])
        }
        str = details[3]
    }
}
