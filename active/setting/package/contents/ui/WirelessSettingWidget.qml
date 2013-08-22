/*
    Copyright 2013 Jan Grulich <jgrulich@redhat.com>

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

import QtQuick 1.1
import org.kde.plasma.components 0.1 as PlasmaComponents
import org.kde.plasma.extras 0.1 as PlasmaExtras

Item {
    id: wirelessSettingWidget;

    property variant modes: {
        INFRA: 0,
        ADHOC: 1,
        AP: 2
    }

    property int wirelessMode: modes.INFRA;

    height: childrenRect.height;

    PlasmaExtras.Heading {
        id: wirelessSettingLabel;

        anchors {
            right: parent.horizontalCenter;
            rightMargin: 12;
        }
        text: i18n("Wireless Configuration");
        level: 3;
    }

    Item {
        id: wirelessSsid;

        height: childrenRect.height;
        anchors {
            left: parent.left;
            right: parent.right;
            top: wirelessSettingLabel.bottom;
            topMargin: 12;
        }

        PlasmaComponents.Label {
            id: wirelessSssidLabel;

            anchors {
                right: parent.horizontalCenter;
                rightMargin: 12;
            }
            text: i18n("SSID:");
        }

        PlasmaComponents.TextField {
            id: wirelessSsidInput;

            width: 200;
            anchors {
                left: parent.horizontalCenter;
            }
        }
    }

    Item {
        id: wirelessModeSelection;

        height: childrenRect.height;
        anchors {
            left: parent.left;
            right: parent.right;
            top: wirelessSsid.bottom;
            topMargin: 12;
        }

        PlasmaComponents.Label {
            id: wirelessModeSelectionLabel;

            anchors {
                right: parent.horizontalCenter;
                rightMargin: 12;
            }
            text: i18n("Mode:");
        }

        PlasmaComponents.Button {
            id: wirelessModeSelectionCombo;

            width: 200;
            anchors {
                left: parent.horizontalCenter;
            }
            text: i18n("Infrastructure");


            MouseArea {
                anchors.fill: parent;

                onClicked: {
                    wirelessModeSelectionDialog.open();
                }
            }
        }
    }

    PlasmaComponents.Dialog {
        id: wirelessModeSelectionDialog;

        visualParent: wirelessModeSelectionCombo;
        content: Item {
            height: 96;
            width: 200;
            ListModel {
                id: wirelessModeModel;

                Component.onCompleted: {
                    append({"name": i18n("Infrastructure"), "mode": modes.INFRA});
                    append({"name": i18n("Ad-hoc"), "mode": modes.ADHOC});
                    append({"name": i18n("AP"), "mode": modes.AP});
                }
            }

            ListView {
                id: wirelessModeSelectionView;

                anchors.fill: parent;
                model: wirelessModeModel;
                delegate: PlasmaComponents.ListItem {
                    enabled: true
                    height: 32;

                    PlasmaComponents.Label {
                        anchors {
                            horizontalCenter: parent.horizontalCenter;
                            verticalCenter: parent.verticalCenter;
                        }
                        width: parent.width;
                        text: name;
                        horizontalAlignment: Text.AlignHCenter;
                    }

                    onClicked: {
                        wirelessModeSelectionDialog.close();
                        wirelessModeSelectionCombo.text = name;
                        wirelessSettingWidget.wirelessMode = mode;
                    }
                }
            }
        }
    }

    function loadSetting(settingMap) {
        wirelessSsidInput.text = settingMap["ssid"];
        if (settingMap["mode"] == "infra") {
            wirelessMode = modes.INFRA;
            wirelessModeSelectionCombo.text = i18n("Infrastructure");
        } else if (settingMap["mode"] == "adhoc") {
            wirelessMode = modes.ADHOC;
            wirelessModeSelectionCombo.text = i18n("Ad-hoc");
        } else if (settingMap["mode"] == "ap") {
            wirelessMode = modes.AP;
            wirelessModeSelectionCombo.text = i18n("AP");
        }
    }

    function getSetting() {
        var settingMap = [];

        settingMap.ssid = wirelessSsidInput.text;
        if (wirelessMode == modes.INFRA) {
            settingMap.mode = "infra";
        } else if (wirelessMode == modes.ADHOC) {
            settingMap.mode = "adhoc";
        } else {
            settingMap.mode = "ap";
        }

        return settingMap;
    }
}
