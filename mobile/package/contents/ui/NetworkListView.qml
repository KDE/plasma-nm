import QtQuick 2.6
import QtQuick.Controls 2.2 as Controls
import QtQuick.Layouts 1.2
import org.kde.plasma.networkmanagement 0.2 as PlasmaNM
import org.kde.kirigami 2.2 as Kirigami

Kirigami.ScrollablePage  {
    anchors.leftMargin: Kirigami.Units.largeSpacing * 2

    header: RowLayout {
        id: layoutrow
        width: parent.width

        Controls.Label {
            anchors.left: parent.left
            text: i18n("Wifi")
            Layout.fillWidth: true
        }

        Controls.Switch {
            id: wifiSwitchButton
            checked: enabled && enabledConnections.wirelessEnabled
            enabled: enabledConnections.wirelessHwEnabled
            onClicked: {
                handler.enableWireless(checked);
            }
        }

        Kirigami.Separator {
            id: separator
            anchors.top: layoutrow.bottom
            width: parent.width
        }
    }

    ListView {
        id: view

        anchors.fill: parent
        clip: true
        width: parent.width
        currentIndex: -1
        boundsBehavior: Flickable.StopAtBounds
        header: Controls.Label {
            text: "Available networks"
        }
        model: mobileProxyModel
        delegate: RowItemDelegate {}
    }

    actions.contextualActions: [

        Kirigami.Action {
            iconName: "edit"
            text:"Add custom connection"
            onTriggered: {
                applicationWindow().pageStack.push(connectionEditorDialogComponent)
            }
        },

        Kirigami.Action {
            iconName: "edit"
            text: "Create Hotspot"
            onTriggered: {
                showPassiveNotification("Open tethering")
            }
        },
        Kirigami.Action {
            iconName: "edit"
            text: "Saved Connections"
            checkable: true
            checked: false
            onTriggered: {
                mobileProxyModel.showSavedMode = checked
            }
        }
    ]
/*
    footer: Controls.Button {
        width: parent.width
        text: "ContextualActions"
        iconName: "edit"
        onClicked: bottomDrawer.open()
    }

    Kirigami.OverlayDrawer {
            id: bottomDrawer
            edge: Qt.BottomEdge
            contentItem: Item {
                implicitHeight: childrenRect.height + Kirigami.Units.gridUnit
                ColumnLayout{
                    anchors.centerIn: parent
                    Controls.Button {
                        text: "Add custom connection"
                        onClicked: applicationWindow().pageStack.push(connectionEditorDialogComponent)
                    }
                    Controls.Button {
                        text: "Create Hotspot"
                        onClicked: showPassiveNotification("Open tethering")
                    }
                    Item {
                        Layout.minimumHeight: Units.gridUnit * 4
                    }
                }
            }
        }
*/
}
