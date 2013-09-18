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
import org.kde.plasmanm 0.1 as PlasmaNm
import org.kde.active.settings 0.1

Item {
    property variant selectedItemModel;

    signal connectionAdded();

    ConnectionSettingsHandler {
        id: connectionSettingsHandler;

        onLoadSecrets: {
            connectionSettingsLoader.item.loadSecrets(secrets);
        }
    }

    Loader {
        id: connectionSettingsLoader;

        anchors {
            left: parent.left;
            right: parent.right;
            top: parent.top;
            bottom: connectDisconnectButton.top;
            bottomMargin: 10;
        }

        onLoaded: {
            loadSettings();
        }
    }

    PlasmaComponents.Button {
        anchors {
            right: connectDisconnectButton.left;
            bottom: parent.bottom;
            rightMargin: 10;
        }
        text: if (selectedItemModel) {
                  if (selectedItemModel.itemUuid)
                      i18n("Save");
                  else
                      i18n("Add");
              } else {
                  i18n("Add");
              }

        onClicked: {
            if (connectionSettingsLoader.status == Loader.Ready) {
                var map = {};
                map = connectionSettingsLoader.item.getSettings();
                if (selectedItemModel && selectedItemModel.itemUuid) {
                    map["connection"]["uuid"] = selectedItemModel.itemUuid;
                    connectionSettingsHandler.saveSettings(map, selectedItemModel.itemConnectionPath);
                } else {
                    connectionSettingsHandler.addConnection(map);
                    connectionAdded();
                }
            }
        }
    }

    PlasmaComponents.Button {
        id: connectDisconnectButton;

        anchors {
            right: parent.right;
            bottom: parent.bottom;
            rightMargin: 5;
        }
        text: if (selectedItemModel) {
                  if (selectedItemModel.itemConnected || selectedItemModel.itemConnecting) {
                      i18n("Disconnect");
                  } else {
                      i18n("Connect");
                  }
              } else {
                  i18n("Connect");
              }


        enabled: if (selectedItemModel) {
                    (selectedItemModel.itemType == PlasmaNm.Enums.Wireless ||
                     selectedItemModel.itemType == PlasmaNm.Enums.Wired ||
                     selectedItemModel.itemType == PlasmaNm.Enums.Gsm)
                 } else {
                    false;
                 }

        onClicked: {
            if (selectedItemModel) {
                if (!selectedItemModel.itemConnected && !selectedItemModel.itemConnecting) {
                    if (selectedItemModel.itemUuid) {
                        handler.activateConnection(selectedItemModel.itemConnectionPath, selectedItemModel.itemDevicePath, selectedItemModel.itemSpecificPath);
                    } else {
                        var map = {};
                        map = connectionSettingsLoader.item.getSettings();
                        connectionSettingsHandler.addAndActivateConnection(map, selectedItemModel.itemDevicePath, selectedItemModel.itemSpecificPath);
                    }
                } else {
                    handler.deactivateConnection(selectedItemModel.itemConnectionPath);
                }
            }
        }
    }

    states: [
        State {
            id: wirelessSetting;
            when: networkSettings.connectionType == PlasmaNm.Enums.Wireless;
            PropertyChanges { target: connectionSettingsLoader; source: "WirelessSettings.qml" }
        },
        State {
            id: wiredSetting;
            when: networkSettings.connectionType == PlasmaNm.Enums.Wired;
            PropertyChanges { target: connectionSettingsLoader; source: "WiredSettings.qml" }
        },
        State {
            id: gsmSetting;
            when: networkSettings.connectionType == PlasmaNm.Enums.Gsm;
            PropertyChanges { target: connectionSettingsLoader; source: "GsmSettings.qml" }
        }
    ]

    onSelectedItemModelChanged: {
        loadSettings();
    }

    function loadSettings() {
        if (selectedItemModel) {
            if (selectedItemModel.itemUuid && connectionSettingsLoader.status == Loader.Ready) {
                var map = {};
                map = connectionSettingsHandler.loadSettings(selectedItemModel.itemUuid);
                connectionSettingsLoader.item.loadSettings(map);
            } else if (connectionSettingsLoader.status == Loader.Ready) {
                if (selectedItemModel.itemType == PlasmaNm.Enums.Wireless) {
                    connectionSettingsLoader.item.resetSettings();
                    // For uknown wireless connections we can pre-fill some properties
                    var connectionMap = [];
                    connectionMap["id"] = selectedItemModel.itemSsid;
                    connectionMap["autoconnect"] = true;
                    var wirelessMap = [];
                    wirelessMap["ssid"] = selectedItemModel.itemSsid;
                    var wirelessSecurityMap = [];
                    if (selectedItemModel.itemSecure) {
                        // StaticWep
                        if (selectedItemModel.itemSecurityType == PlasmaNm.Enums.StaticWep) {
                            wirelessSecurityMap["key-mgmt"] = "none";
                        // DynamicWep
                        } else if (selectedItemModel.itemSecurityType == PlasmaNm.Enums.DynamicWep) {
                            wirelessSecurityMap["key-mgmt"] = "ieee8021x";
                        // LEAP
                        } else if (selectedItemModel.itemSecurityType == PlasmaNm.Enums.Leap) {
                            wirelessSecurityMap["key-mgmt"] = "ieee8021x";
                            wirelessSecurityMap["auth-alg"] = "leap";
                        // WPA/WPA2
                        } else if (selectedItemModel.itemSecurityType == PlasmaNm.Enums.WpaPsk || selectedItemModel.itemSecurityType == PlasmaNm.Enums.Wpa2Psk) {
                            wirelessSecurityMap["key-mgmt"] = "wpa-psk";
                        // WPA/WPA2 Enterprise
                        } else if (selectedItemModel.itemSecurityType == PlasmaNm.Enums.WpaEap || selectedItemModel.itemSecurityType == PlasmaNm.Enums.Wpa2Eap) {
                            wirelessSecurityMap["key-mgmt"] = "wpa-eap";
                        }
                    }
                    var settingsMap = [];
                    settingsMap["connection"] = connectionMap;
                    settingsMap["802-11-wireless"] = wirelessMap;
                    settingsMap["802-11-wireless-security"] = wirelessSecurityMap;
                    connectionSettingsLoader.item.loadSettings(settingsMap);
                } else {
                    connectionSettingsLoader.item.resetSettings();
                }
            }
        } else {
            if (connectionSettingsLoader.status == Loader.Ready) {
                connectionSettingsLoader.item.resetSettings();
            }
        }
    }
}
