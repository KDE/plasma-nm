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
    id: wirelessSecuritySettingWidget;

    property int wirelessSecurity: securityTypes.NONE;

    property variant securityTypes: {
        NONE: 0,
        WEP: 1,
        LEAP: 2,
        DYNAMICWEP: 3,
        WPA: 4,
        WPAEAP: 5
    }

    height: childrenRect.height;

    Item {
        id: wirelessSecuritySelection;

        height: childrenRect.height;
        anchors {
            left: parent.left;
            right: parent.right;
            top: parent.top;
            topMargin: 12;
        }

        PlasmaComponents.Label {
            id: wirelessSecuritySelectionLabel;

            anchors {
                right: parent.horizontalCenter;
                rightMargin: 12;
            }
            text: i18n("Security:");
        }

        PlasmaComponents.Button {
            id: wirelessSecuritySelectionCombo;

            width: 200;
            anchors {
                left: parent.horizontalCenter;
            }
            text: i18n("None");


            MouseArea {
                anchors.fill: parent;

                onClicked: {
                    wirelessSecuritySelectionDialog.open();
                }
            }
        }
    }

    Loader {
        id: wirelessSecurityPasswordConfigurationLoader;

        height: childrenRect.height;
        anchors {
            left: parent.left;
            right: parent.right;
            top: wirelessSecuritySelection.bottom;
        }
    }

    Component {
        id: wirelessSecurityPasswordConfiguration;

        Item {
            property alias username: usernameInput.text;
            property alias password: passwordInput.text;

            height: childrenRect.height;

            Item {
                id: usernameConfiguration;

                height: wirelessSecurity == securityTypes.LEAP ? childrenRect.height : 0;
                anchors {
                    top: parent.top;
                    left: parent.left;
                    right: parent.right;
                    topMargin: wirelessSecurity == securityTypes.LEAP ? 12 : 0;
                }
                visible: wirelessSecurity == securityTypes.LEAP;

                PlasmaComponents.Label {
                    id: usernameLabel;

                    anchors {
                        right: parent.horizontalCenter;
                        rightMargin: 12;
                    }
                    text: i18n("Username:");
                }

                PlasmaComponents.TextField {
                    id: usernameInput;

                    width: 200;
                    anchors {
                        left: parent.horizontalCenter;
                    }
                }
            }

            Item {
                id: passwordConfiguration;

                height: childrenRect.height;
                anchors {
                    top: usernameConfiguration.bottom;
                    left: parent.left;
                    right: parent.right;
                    topMargin: 12;
                }

                PlasmaComponents.Label {
                    id: passwordLabel;

                    anchors {
                        right: parent.horizontalCenter;
                        rightMargin: 12;
                    }
                    text: if (wirelessSecurity == securityTypes.WEP) {
                              i18n("Key:");
                          } else if (wirelessSecurity == securityTypes.LEAP || wirelessSecurity == securityTypes.WPA) {
                              i18n("Password:");
                          } else {
                              i18n("Password:");
                          }
                }

                PlasmaComponents.TextField {
                    id: passwordInput;

                    width: 200;
                    anchors {
                        left: parent.horizontalCenter;
                    }
                    echoMode: TextInput.Password;
                }
            }

            Item {
                id: showPassword;

                height: childrenRect.height;

                anchors {
                    left: parent.left;
                    right: parent.right;
                    top: passwordConfiguration.bottom;
                    topMargin: 12;
                }

                PlasmaComponents.Label {
                    id: showPassowrdLabel;

                    anchors {
                        right: parent.horizontalCenter;
                        rightMargin: 12;
                    }
                    text: i18n("Show password:");
                }

                PlasmaComponents.Switch {
                    id: showPasswordSwitch;

                    anchors {
                        left: parent.horizontalCenter;
                    }
                    checked: false;

                    onCheckedChanged: {
                        if (checked) {
                            passwordInput.echoMode = TextInput.Normal;
                        } else {
                            passwordInput.echoMode = TextInput.Password;
                        }
                    }
                }
            }

        }
    }


    PlasmaComponents.Dialog {
        id: wirelessSecuritySelectionDialog;

        visualParent: wirelessSecuritySelectionCombo;
        content: Item {
            height: 128;
//             height: 192;
            width: 200;
            ListModel {
                id: wirelessSecurityModel;

                Component.onCompleted: {
                    // TODO: Disable Dynamic WEP and WPA/WPA2 Enterprise for now
                    append({"name": i18n("None"), "security": securityTypes.NONE});
                    append({"name": i18n("WEP"), "security": securityTypes.WEP});
                    append({"name": i18n("LEAP"), "security": securityTypes.LEAP});
//                     append({"name": i18n("Dynamic WEP (802.1x)"), "security": securityTypes.DYNAMICWEP});
                    append({"name": i18n("WPA/WPA2 Personal"), "security": securityTypes.WPA});
//                     append({"name": i18n("WPA/WPA2 Enterprise"), "security": securityTypes.WPAEAP});
                }
            }

            ListView {
                id: wirelessSecuritySelectionView;

                anchors.fill: parent;
                model: wirelessSecurityModel;
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
                        wirelessSecuritySelectionDialog.close();
                        wirelessSecuritySelectionCombo.text = name;
                        wirelessSecuritySettingWidget.wirelessSecurity = security;
                    }
                }
            }
        }
    }

    states: [
        State {
            id: none;
            when: wirelessSecurity == securityTypes.NONE;
            PropertyChanges { target: wirelessSecurityPasswordConfigurationLoader; sourceComponent: undefined }
        },

        State {
            id: secured;
            when: wirelessSecurity != securityTypes.NONE;
            PropertyChanges { target: wirelessSecurityPasswordConfigurationLoader; sourceComponent: wirelessSecurityPasswordConfiguration }
        }
    ]

    function loadSetting(settingMap) {
        if (settingMap["key-mgmt"] == "none") {
            wirelessSecurity = securityTypes.WEP;
            wirelessSecuritySelectionCombo.text = i18n("WEP");
            passwordInput.text = settingMap["wep-key0"];
        } else if (settingMap["key-mgmt"] == "ieee8021x") {
            wirelessSecurity = securityTypes.DYNAMICWEP;
            wirelessSecuritySelectionCombo.text = i18n("Dynamic WEP");
            if (settingMap["auth-alg"] == "leap") {
                usernameInput.text = settingMap["leap-username"];
                passwordInput.text = settingMap["leap-password"];
            }
            // TODO implement WPA enterprise later
        } else if (settingMap["key-mgmt"] == "wpa-none" || settingMap["key-mgmg"] == "wpa-psk") {
            wirelessSecurity = securityTypes.WPA;
            wirelessSecuritySelectionCombo.text = i18n("WPA/WPA2 Personal");
            passwordInput.text = settingMap["psk"];
        } else if (settingMap["key-mgmt"] == "wpa-eap") {
            wirelessSecurity = securityTypes.WPAEAP;
            wirelessSecuritySelectionCombo.text = i18n("WPA/WPA2 Enterprise");
            // TODO implement WPA enterprise later
        }
    }

    function getSetting() {
        var settingMap = [];

        if (wirelessSecurity == securityTypes.NONE) {
            // nothing
        } else if (wirelessSecurity == securityTypes.WEP) {
            settingMap["key-mgmt"] = "none";
            // TODO: probably check wep-key index
            settingMap["wep-key0"] = wirelessSecurityPasswordConfigurationLoader.item.password;
        } else if (wirelessSecurity == securityTypes.LEAP) {
            settingMap["key-mgmt"] = "ieee8021x";
            settingMap["auth-alg"] = "leap";
            settingMap["leap-username"] = wirelessSecurityPasswordConfigurationLoader.item.username;
            settingMap["leap-password"] = wirelessSecurityPasswordConfigurationLoader.item.password;
        } else if (wirelessSecurity == securityTypes.DYNAMICWEP) {
            settingMap["key-mgmt"] = "ieee8021x";
            // TODO implement WPA enterprise later
        } else if (wirelessSecurity == securityTypes.WPA) {
            // Have to check whether this connection is in ad-hoc mode
            settingMap["key-mgmt"] = "wpa-psk";
            settingMap["psk"] = wirelessSecurityPasswordConfigurationLoader.item.password;
        } else if (wirelessSecurity == securityTypes.WPAEAP) {
            settingMap["key-mgmt"] = "wpa-eap";
            // TODO implement WPA enterprise later
        }

        return settingMap;
    }
}
