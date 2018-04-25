import QtQuick 2.6
import QtQuick.Controls 2.2 as Controls
import QtQuick.Layouts 1.3
import org.kde.plasma.networkmanagement 0.2 as PlasmaNM
import org.kde.kirigami 2.2 as Kirigami

Kirigami.Page {

    header: RowLayout {
        width: parent.width
        Controls.Label {
            text: "Wi-fi hotspot"
        }
        Controls.Switch {
        }
    }

    Column {
        Controls.Label {
            text: i18n("SSID")
            font.weight: Font.Bold
        }

        Controls.TextField {
            id: hotSpotName
            text: i18n("My Hotspot")
        }


    }
}

