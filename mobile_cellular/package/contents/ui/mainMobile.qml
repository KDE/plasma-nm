import QtQuick 2.6
import QtQuick.Controls 2.2 as Controls
import org.kde.plasma.networkmanagement 0.2 as PlasmaNM
import org.kde.kirigami 2.2 as Kirigami

Kirigami.ApplicationItem {
    id: main
    objectName: "mobileDataMain"

    pageStack.defaultColumnWidth: Kirigami.Units.gridUnit * 25
    //pageStack.initialPage:
    Kirigami.Theme.colorSet: Kirigami.Theme.Window

    anchors.fill: parent

    Controls.Label {
        text: i18n("Mobile data settings")
        font.weight: Font.Bold
    }
}
