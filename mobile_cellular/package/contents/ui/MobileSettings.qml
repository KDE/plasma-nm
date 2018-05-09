import QtQuick 2.6
import QtQuick.Controls 2.2 as Controls
import QtQuick.Layouts 1.3
import org.kde.plasma.networkmanagement 0.2 as PlasmaNM
import org.kde.kirigami 2.2 as Kirigami

Kirigami.Page {
    ColumnLayout {
        width: parent.width
        spacing: Kirigami.Units.gridUnit * 1.5
        RowLayout {
            width: parent.width
            Controls.Label {
                text: i18n("Enable mobile data network")
                font.weight: Font.Bold
                Layout.fillWidth: true
            }
            Controls.CheckBox {
                id: mobileDataCheckbox
               // enabled: enabledConnections.wwanHwEnabled && availableDevices.modemDeviceAvailable
                anchors.rightMargin: Kirigami.Units.gridUnit
            }
        }
        RowLayout {
            width: parent.width
            enabled: mobileDataCheckbox.checked
            Controls.Label {
                text: i18n("Enable data roaming")
                font.weight: Font.Bold
                color: parent.enabled ? Kirigami.Theme.textColor : Kirigami.Theme.disabledTextColor
                Layout.fillWidth: true
            }
            Controls.CheckBox {
                enabled: parent.enabled
                checked: false
                anchors.rightMargin: Kirigami.Units.gridUnit
                onEnabledChanged: {
                    if (!enabled)
                        checked = false
                }
            }
        }
        Kirigami.Separator {}
        Column {
            Controls.Label {
                text: i18n("Access point name")
                font.weight: Font.Bold
            }
            Row {
                Controls.TextField {
                    text: kcm.getAPN()
                }
                Controls.ToolButton {
                    text: i18n("Edit APN")
                }
            }
        }
        Controls.Button {
            text: i18n("Data usage")
        }
    }
}
