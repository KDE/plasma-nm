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
import org.kde.plasmanm 0.1 as PlasmaNm

Item {
    id: ipv4SettingWidget;

    property int ipv4Method: PlasmaNm.Enums.Automatic;

    height: childrenRect.height;

    PlasmaExtras.Heading {
        id: ipv4SettingLabel;

        anchors {
            right: parent.horizontalCenter;
            rightMargin: 12;
        }
        text: i18n("IPv4 Settings");
        level: 3;
    }

    Item {
        id: ipv4MethodSelection;

        height: childrenRect.height;
        anchors {
            left: parent.left;
            right: parent.right;
            top: ipv4SettingLabel.bottom;
            topMargin: 12;
        }

        PlasmaComponents.Label {
            id: ipv4MethodSelectionLabel;

            anchors {
                right: parent.horizontalCenter;
                rightMargin: 12;
            }
            text: i18n("Method:");
        }

        PlasmaComponents.Button {
            id: ipv4MethodSelectionCombo;

            width: 200;
            anchors {
                left: parent.horizontalCenter;
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

            height: childrenRect.height;

            Item {
                id: ipv4AddressConfiguration;

                height: childrenRect.height;
                anchors {
                    top: parent.top;
                    left: parent.left;
                    right: parent.right;
                    topMargin: 12;
                }

                PlasmaComponents.Label {
                    id: ipv4AddressLabel;

                    anchors {
                        right: parent.horizontalCenter;
                        rightMargin: 12;
                    }
                    text: i18n("IP Address:");
                }

                PlasmaComponents.TextField {
                    id: ipv4AddressInput;

                    width: 200;
                    anchors {
                        left: parent.horizontalCenter;
                    }
                    validator: RegExpValidator { regExp: /^(([01]?[0-9]?[0-9]|2([0-4][0-9]|5[0-5]))\.){3}([01]?[0-9]?[0-9]|2([0-4][0-9]|5[0-5]))$/ }
                }
            }

            Item {
                id: ipv4GatewayConfiguration;

                height: childrenRect.height;
                anchors {
                    top: ipv4AddressConfiguration.bottom;
                    left: parent.left;
                    right: parent.right;
                    topMargin: 12;
                }

                PlasmaComponents.Label {
                    id: ipv4GatewayLabel;

                    anchors {
                        right: parent.horizontalCenter;
                        rightMargin: 12;
                    }
                    text: i18n("Gateway:");
                }

                PlasmaComponents.TextField {
                    id: ipv4GatewayInput;

                    width: 200;
                    anchors {
                        left: parent.horizontalCenter;
                    }
                    validator: RegExpValidator { regExp: /^(([01]?[0-9]?[0-9]|2([0-4][0-9]|5[0-5]))\.){3}([01]?[0-9]?[0-9]|2([0-4][0-9]|5[0-5]))$/ }
                }
            }

            Item {
                id: ipv4NetmaskConfiguration;

                height: childrenRect.height;
                anchors {
                    top: ipv4GatewayConfiguration.bottom;
                    left: parent.left;
                    right: parent.right;
                    topMargin: 12;
                }

                PlasmaComponents.Label {
                    id: ipv4NetmaskLabel;

                    anchors {
                        right: parent.horizontalCenter;
                        rightMargin: 12;
                    }
                    text: i18n("Netmask:");
                }

                PlasmaComponents.TextField {
                    id: ipv4NetmaskInput;

                    width: 200;
                    anchors {
                        left: parent.horizontalCenter;
                    }
                    validator: RegExpValidator { regExp: /^(([01]?[0-9]?[0-9]|2([0-4][0-9]|5[0-5]))\.){3}([01]?[0-9]?[0-9]|2([0-4][0-9]|5[0-5]))$/ }
                }
            }

            Item {
                id: ipv4Dns1Configuration;

                height: childrenRect.height;
                anchors {
                    top: ipv4NetmaskConfiguration.bottom;
                    left: parent.left;
                    right: parent.right;
                    topMargin: 12;
                }

                PlasmaComponents.Label {
                    id: ipv4Dns1Label;

                    anchors {
                        right: parent.horizontalCenter;
                        rightMargin: 12;
                    }
                    text: i18n("DNS Server 1:");
                }

                PlasmaComponents.TextField {
                    id: ipv4Dns1Input;

                    width: 200;
                    anchors {
                        left: parent.horizontalCenter;
                    }
                    validator: RegExpValidator { regExp: /^(([01]?[0-9]?[0-9]|2([0-4][0-9]|5[0-5]))\.){3}([01]?[0-9]?[0-9]|2([0-4][0-9]|5[0-5]))$/ }
                }
            }

            Item {
                id: ipv4Dns2Configuration;

                height: childrenRect.height;
                anchors {
                    top: ipv4Dns1Configuration.bottom;
                    left: parent.left;
                    right: parent.right;
                    topMargin: 12;
                }

                PlasmaComponents.Label {
                    id: ipv4Dns2Label;

                    anchors {
                        right: parent.horizontalCenter;
                        rightMargin: 12;
                    }
                    text: i18n("DNS Server 2:");
                }

                PlasmaComponents.TextField {
                    id: ipv4Dns2Input;

                    width: 200;
                    anchors {
                        left: parent.horizontalCenter;
                    }
                    validator: RegExpValidator { regExp: /^(([01]?[0-9]?[0-9]|2([0-4][0-9]|5[0-5]))\.){3}([01]?[0-9]?[0-9]|2([0-4][0-9]|5[0-5]))$/ }
                }
            }
        }
    }


    PlasmaComponents.Dialog {
        id: ipv4MethodSelectionDialog;

        visualParent: ipv4MethodSelectionCombo;
        content: Item {
            height: 96;
            width: 200;
            ListModel {
                id: ipv4MethodsModel;

                Component.onCompleted: {
                    append({"name": i18n("Automatic"), "method": PlasmaNm.Enums.Automatic});
                    append({"name": i18n("Manual"), "method": PlasmaNm.Enums.Manual});
                    append({"name": i18n("Shared"), "method": PlasmaNm.Enums.Shared});
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
                        anchors {
                            horizontalCenter: parent.horizontalCenter;
                            verticalCenter: parent.verticalCenter;
                        }
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
            when: ipv4Method == PlasmaNm.Enums.Automatic || ipv4Method == PlasmaNm.Enums.Shared;
            PropertyChanges { target: ipv4ManualConfigurationLoader; sourceComponent: undefined }
        },

        State {
            id: manual;
            when: ipv4Method == PlasmaNm.Enums.Manual;
            PropertyChanges { target: ipv4ManualConfigurationLoader; sourceComponent: ipv4ManualConfiguration }
        }
    ]

    function resetSetting() {
        ipv4Method = PlasmaNm.Enums.Automatic;
        ipv4MethodSelectionCombo.text = i18n("Automatic");
    }

    function loadSetting(settingMap) {
        resetSetting();

        if (settingMap["method"] == "auto") {
            ipv4Method =PlasmaNm.Enums.Automatic;
            ipv4MethodSelectionCombo.text = i18n("Automatic");
        } else if (settingMap["method"] == "shared") {
            ipv4Method = PlasmaNm.Enums.Shared;
            ipv4MethodSelectionCombo.text = i18n("Shared");
        } else if (settingMap["method"] == "manual") {
            ipv4Method = PlasmaNm.Enums.Manual;
            ipv4MethodSelectionCombo.text = i18n("Manual");
            // These properties are transferred and customized from the original NMVariantMap from NM
            if (settingMap["address"])
                ipv4ManualConfigurationLoader.item.address = settingMap["address"];
            if (settingMap["gateway"])
                ipv4ManualConfigurationLoader.item.netmask = settingMap["gateway"];
            if (settingMap["netmask"])
                ipv4ManualConfigurationLoader.item.gateway = settingMap["netmask"];
            if (settingMap["dns1"])
                ipv4ManualConfigurationLoader.item.dns1 = settingMap["dns1"];
            if (settingMap["dns2"])
                ipv4ManualConfigurationLoader.item.dns2 = settingMap["dns2"];
        }
    }

    function getSetting() {
        var settingMap = {};

        if (ipv4Method == PlasmaNm.Enums.Automatic) {
            settingMap["method"] = "auto";
        } else if (ipv4Method == PlasmaNm.Enums.Shared) {
            settingMap["method"] = "shared";
        } else {
            settingMap["method"] = "manual";
            // These properties have to be fixed in the NMVariantMap passed to NM
            settingMap["address"] = ipv4ManualConfigurationLoader.item.address;
            settingMap["netmask"] = ipv4ManualConfigurationLoader.item.netmask;
            settingMap["gateway"] = ipv4ManualConfigurationLoader.item.gateway;
            settingMap["dns1"] = ipv4ManualConfigurationLoader.item.dns1;
            settingMap["dns2"] = ipv4ManualConfigurationLoader.item.dns2;
        }

        return settingMap;
    }
}
