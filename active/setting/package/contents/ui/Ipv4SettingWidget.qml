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
    id: ipv4SettingWidget;

    property variant methods: {
        AUTO: 0,
        MANUAL: 1,
        SHARED: 2
    }

    property int ipv4Method: methods.AUTO;

    PlasmaExtras.Heading {
        id: ipv4SettingLabel;

        anchors {
            right: parent.horizontalCenter;
            rightMargin: 12;
        }
        text: i18n("IPv4 configuration");
        level: 3;
    }

    Item {
        id: ipv4MethodSelection;

        anchors {
            left: parent.left;
            right: parent.right;
            top: ipv4SettingLabel.bottom;
            topMargin: 24;
        }

        PlasmaComponents.Label {
            id: ipv4MethodSelectionLabel;

            anchors {
                right: parent.horizontalCenter;
                verticalCenter: parent.verticalCenter;
                rightMargin: 12;
            }
            text: i18n("Method:");
        }

        PlasmaComponents.Button {
            id: ipv4MethodSelectionCombo;

            width: 200;
            anchors {
                left: parent.horizontalCenter;
                verticalCenter: parent.verticalCenter;
            }
            text: i18n("Automatic");


            MouseArea {
                anchors.fill: parent;

                onClicked: {
                    ipv4MethodSelectionDialog.open();
                }
            }
        }
    }

    Loader {
        id: ipv4ManualConfigurationLoader;

        anchors {
            left: parent.left;
            right: parent.right;
            top: ipv4MethodSelection.bottom;
        }
    }

    Component {
        id: ipv4ManualConfiguration;

        Item {
            property alias address: ipv4AddressInput.text;
            property alias gateway: ipv4GatewayInput.text;
            property alias netmask: ipv4NetmaskInput.text;
            property alias dns1: ipv4Dns1Input.text;
            property alias dns2: ipv4Dns2Input.text;

            anchors {
                fill: parent;
            }

            Item {
                id: ipv4AddressConfiguration;

                anchors {
                    top: parent.top;
                    left: parent.left;
                    right: parent.right;
                    topMargin: 36;
                }

                PlasmaComponents.Label {
                    id: ipv4AddressLabel;

                    anchors {
                        right: parent.horizontalCenter;
                        verticalCenter: parent.verticalCenter;
                        rightMargin: 12;
                    }
                    text: i18n("IP Address:");
                }

                PlasmaComponents.TextField {
                    id: ipv4AddressInput;

                    width: 200;
                    anchors {
                        left: parent.horizontalCenter;
                        verticalCenter: parent.verticalCenter;
                    }
//                     validator: RegExpValidator { regExp: /^(([01]?[0-9]?[0-9]|2([0-4][0-9]|5[0-5]))\.){3}([01]?[0-9]?[0-9]|2([0-4][0-9]|5[0-5]))$/ }
                    inputMask: "000.000.000.000;_";
                }

                Item {
                    id: ipv4GatewayConfiguration;

                    anchors {
                        top: ipv4AddressConfiguration.bottom;
                        left: parent.left;
                        right: parent.right;
                        topMargin: 36;
                    }

                    PlasmaComponents.Label {
                        id: ipv4GatewayLabel;

                        anchors {
                            right: parent.horizontalCenter;
                            verticalCenter: parent.verticalCenter;
                            rightMargin: 12;
                        }
                        text: i18n("Gateway:");
                        horizontalAlignment: Text.AlignRight;
                    }

                    PlasmaComponents.TextField {
                        id: ipv4GatewayInput;

                        width: 200;
                        anchors {
                            left: parent.horizontalCenter;
                            verticalCenter: parent.verticalCenter;
                        }
//                         validator: RegExpValidator { regExp: /^(([01]?[0-9]?[0-9]|2([0-4][0-9]|5[0-5]))\.){3}([01]?[0-9]?[0-9]|2([0-4][0-9]|5[0-5]))$/ }
                        inputMask: "000.000.000.000;_";
                    }
                }

                Item {
                    id: ipv4NetmaskConfiguration;

                    anchors {
                        top: ipv4GatewayConfiguration.bottom;
                        left: parent.left;
                        right: parent.right;
                        topMargin: 36;
                    }

                    PlasmaComponents.Label {
                        id: ipv4NetmaskLabel;

                        anchors {
                            right: parent.horizontalCenter;
                            verticalCenter: parent.verticalCenter;
                            rightMargin: 12;
                        }
                        text: i18n("Netmask:");
                        horizontalAlignment: Text.AlignRight;
                    }

                    PlasmaComponents.TextField {
                        id: ipv4NetmaskInput;

                        width: 200;
                        anchors {
                            left: parent.horizontalCenter;
                            verticalCenter: parent.verticalCenter;
                        }
//                         validator: RegExpValidator { regExp: /^(([01]?[0-9]?[0-9]|2([0-4][0-9]|5[0-5]))\.){3}([01]?[0-9]?[0-9]|2([0-4][0-9]|5[0-5]))$/ }
                        inputMask: "000.000.000.000;_";
                    }
                }

                Item {
                    id: ipv4Dns1Configuration;

                    anchors {
                        top: ipv4NetmaskConfiguration.bottom;
                        left: parent.left;
                        right: parent.right;
                        topMargin: 36;
                    }

                    PlasmaComponents.Label {
                        id: ipv4Dns1Label;

                        anchors {
                            right: parent.horizontalCenter;
                            verticalCenter: parent.verticalCenter;
                            rightMargin: 12;
                        }
                        text: i18n("DNS Server 1:");
                        horizontalAlignment: Text.AlignRight;
                    }

                    PlasmaComponents.TextField {
                        id: ipv4Dns1Input;

                        width: 200;
                        anchors {
                            left: parent.horizontalCenter;
                            verticalCenter: parent.verticalCenter;
                        }
//                         validator: RegExpValidator { regExp: /^(([01]?[0-9]?[0-9]|2([0-4][0-9]|5[0-5]))\.){3}([01]?[0-9]?[0-9]|2([0-4][0-9]|5[0-5]))$/ }
                        inputMask: "000.000.000.000;_";
                    }
                }

                Item {
                    id: ipv4Dns2Configuration;

                    anchors {
                        top: ipv4Dns1Configuration.bottom;
                        left: parent.left;
                        right: parent.right;
                        topMargin: 36;
                    }

                    PlasmaComponents.Label {
                        id: ipv4Dns2Label;

                        anchors {
                            right: parent.horizontalCenter;
                            verticalCenter: parent.verticalCenter;
                            rightMargin: 12;
                        }
                        text: i18n("DNS Server 2:");
                        horizontalAlignment: Text.AlignRight;
                    }

                    PlasmaComponents.TextField {
                        id: ipv4Dns2Input;

                        width: 200;
                        anchors {
                            left: parent.horizontalCenter;
                            verticalCenter: parent.verticalCenter;
                        }
//                         validator: RegExpValidator { regExp: /^(([01]?[0-9]?[0-9]|2([0-4][0-9]|5[0-5]))\.){3}([01]?[0-9]?[0-9]|2([0-4][0-9]|5[0-5]))$/ }
                        inputMask: "000.000.000.000;_";
                    }
                }
            }
        }
    }


    PlasmaComponents.Dialog {
        id: ipv4MethodSelectionDialog;

        visualParent: ipv4MethodSelectionCombo;
        content: Item {
            height: 100;
            width: 200;
            ListModel {
                id: ipv4MethodsModel;

                Component.onCompleted: {
                    append({"name": i18n("Automatic"), "method": methods.AUTO});
                    append({"name": i18n("Manual"), "method": methods.MANUAL});
                    append({"name": i18n("Shared"), "method": methods.SHARED});
                }
            }

            ListView {
                id: ipv4MethodSelectionView;

                anchors.fill: parent;
                model: ipv4MethodsModel;
                delegate: PlasmaComponents.ListItem {
                    enabled: true
                    height: 32;

                    PlasmaComponents.Label {
                        anchors { horizontalCenter: parent.horizontalCenter; verticalCenter: parent.verticalCenter }
                        width: parent.width;
                        text: name;
                        horizontalAlignment: Text.AlignHCenter;
                    }

                    onClicked: {
                        ipv4MethodSelectionDialog.close();
                        ipv4MethodSelectionCombo.text = name;
                        ipv4SettingWidget.ipv4Method = method;
                    }
                }
            }
        }
    }

    states: [
        State {
            id: autoShared;
            when: ipv4Method == methods.AUTO || ipv4Method == methods.SHARED;
            PropertyChanges { target: ipv4ManualConfigurationLoader; sourceComponent: undefined }
        },

        State {
            id: manual;
            when: ipv4Method == methods.MANUAL;
            PropertyChanges { target: ipv4ManualConfigurationLoader; sourceComponent: ipv4ManualConfiguration }
        }
    ]

    function loadSetting(settingMap) {
        if (settingMap["method"] == "auto") {
            ipv4Method = methods.AUTO;
            ipv4MethodSelectionCombo.text = i18n("Automatic");
        } else if (settingMap["method"] == "shared") {
            ipv4Method = methods.SHARED;
            ipv4MethodSelectionCombo.text = i18n("Shared");
        } else if (settingMap["method"] == "manual") {
            ipv4Method = methods.MANUAL;
            ipv4MethodSelectionCombo.text = i18n("Manual");
            ipv4AddressInput.text = settingMap["address"];
            ipv4GatewayInput.text = settingMap["gateway"];
            ipv4NetmaskInput.text = settingMap["netmask"];
            ipv4Dns1Input.text = settingMap["dns1"];
            ipv4Dns2Input.text = settingMap["dns2"];
        } else {
            // Default
            ipv4MethodSelectionCombo.text = i18n("Automatic");
            ipv4Method = methods.AUTO;
        }
    }

    function getSetting() {
        var settingMap = [];

        if (ipv4Method == methods.AUTO) {
            settingMap.method = "auto";
        } else if (ipv4Method == methods.SHARED) {
            settingMap.method = "shared";
        } else {
            settingMap.method = "manual";
            settingMap.address = ipv4ManualConfigurationLoader.item.address;
            settingMap.netmask = ipv4ManualConfigurationLoader.item.netmask;
            settingMap.gateway = ipv4ManualConfigurationLoader.item.gateway;
            settingMap.dns1 = ipv4ManualConfigurationLoader.item.dns1;
            settingMap.dns2 = ipv4ManualConfigurationLoader.item.dns2;
        }

        return settingMap;
    }
}
