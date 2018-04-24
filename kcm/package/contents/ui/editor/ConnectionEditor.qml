/*
    Copyright 2018 Jan Grulich <jgrulich@redhat.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

import QtQuick 2.6
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2 as QtControls

import org.kde.kirigami 2.0 as Kirigami
import org.kde.plasma.networkmanagement 0.2 as PlasmaNM

Kirigami.ScrollablePage {
    id: connectionEditorPage

    title: connectionNameTextField.text

    PlasmaNM.Utils {
        id: nmUtils
    }

    header: MouseArea {
        width: connectionEditorPage.width
        height: Math.round(Kirigami.Units.gridUnit * 2.5)
        enabled: !applicationWindow().wideScreen

        Accessible.role: Accessible.Button
        Accessible.name: i18n("Back")

        onClicked: {
            root.pageStack.currentIndex = 0
        }

        Item {
            id: headerControls
            anchors.fill: parent

            QtControls.ToolButton {
                id: backButton
                anchors.fill: parent
                anchors.margins: Kirigami.Units.smallSpacing
                visible: !applicationWindow().wideScreen

                onClicked: {
                    root.pageStack.currentIndex = 0
                }

                RowLayout {
                    anchors.fill: parent

                    Kirigami.Icon {
                        id: toolButtonIcon

                        Layout.alignment: Qt.AlignVCenter
                        Layout.preferredHeight: Kirigami.Units.iconSizes.small
                        Layout.preferredWidth: Layout.preferredHeight

                        source: LayoutMirroring.enabled ? "go-next" : "go-previous"
                    }

                    QtControls.Label {
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        height: toolButtonIcon.height
                        text: connectionEditorPage.title
                        verticalAlignment: Text.AlignVCenter
                        elide: Text.ElideRight

                        //FIXME: QtControls bug, why?
                        Component.onCompleted: {
                            font.bold = true
                        }
                    }
                }
            }

            QtControls.Label {
                anchors.verticalCenter: parent.verticalCenter
                x: y

                text: connectionEditorPage.title
                elide: Text.ElideRight
                visible: !backButton.visible
                opacity: 0.3
                //FIXME: QtControls bug, why?
                Component.onCompleted: {
                    font.bold = true
                }
            }
        }

        Kirigami.Separator {
            anchors {
                left: parent.left
                right: parent.right
                top: parent.bottom
            }
            visible: !connectionEditorPage.atYBeginning
        }
    }

    ColumnLayout {
        width: connectionEditorPage.width

        QtControls.TextField  {
            id: connectionNameTextField

            Layout.fillWidth: true

            hoverEnabled: true
        }

        QtControls.TabBar {
            id: tabBar

            Layout.fillWidth: true

            QtControls.TabButton {
                text: connectionSetting.settingName
            }

            // FIXME just placeholders for now
            QtControls.TabButton {
                text: connectionSpecificSetting.item.settingName
            }

            QtControls.TabButton {
                text: i18n("IP")
            }
        }

        StackLayout {
            Layout.fillWidth: true

            currentIndex: tabBar.currentIndex

            ConnectionSetting {
                id: connectionSetting
            }

            Loader {
                id: connectionSpecificSetting
            }
        }
    }

    function loadConnectionSettings() {
        connectionNameTextField.text = connectionSettingsObject.id
        // Load general connection setting
        connectionSetting.loadSettings()

        // Load connection specific setting
        if (connectionSettingsObject.connectionType == PlasmaNM.Enums.Wired) {
            connectionSpecificSetting.source = "WiredSetting.qml"
        } else if (connectionSettingsObject.connectionType == PlasmaNM.Enums.Wireless) {
            connectionSpecificSetting.source = "WirelessSetting.qml"
        }
        connectionSpecificSetting.item.loadSettings()
    }
}

