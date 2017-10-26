/*
 *   
 *   Copyright 2017 Martin Kacej <>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

import QtQuick 2.0
import QtQuick.Controls 1.4 as Controls
import QtQuick.Layouts 1.2
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.extras 2.0 as PlasmaExtras
import org.kde.active.settings 2.0 as ActiveSettings
import org.kde.plasma.networkmanagement 0.2 as PlasmaNM
//import org.kde.kirigami 1.0 as Kirigami

Item {
    id: main
    objectName: "wifiMain"
    width: units.gridUnit * 30
    height: width * 1.5
    
    PlasmaNM.Handler{
        id: handler
    }
    
    PlasmaNM.EnabledConnections {
        id: enabledConnections
        
        onWirelessEnabledChanged: {
            wifiSwitchButton.checked = wifiSwitchButton.enabled && enabled
        }
    }
    
    PlasmaNM.NetworkModel {
        id: connectionModel
    }
    
   PlasmaNM.MobileAppletProxyModel{
        id: mobileappletProxyModel

        sourceModel: connectionModel
   }
    Column {
        id: formLayout
        spacing: units.gridUnit
        anchors {
            fill: parent
            margins: units.gridUnit
            leftMargin: units.gridUnit/2
        }

        RowLayout{
            id:layoutrow
            width: parent.width
            
            PlasmaComponents.Label {
                anchors.left: parent.left
                text: i18n("Wifi")
                Layout.fillWidth: true
            }
            
            PlasmaComponents.Switch {
                id: wifiSwitch
                checked: enabled && enabledConnections.wirelessEnabled
                enabled: enabledConnections.wirelessHwEnabled 
                            && availableDevices.wirelessDeviceAvailable 
                
                //icon: enabled ? "network-wireless-on" : "network-wireless-off"
                onClicked: {
                    handler.enableWireless(checked);
                }
            }
        }
       
         Rectangle{
            id: separator
            anchors.top: layoutrow.bottom
            width: parent.width
            height: units.gridUnit/8
            border.color: "grey"
        }
        
        PlasmaComponents.Label {
            id:label
            anchors{
                left: parent.left
                top:separator.bottom
            }
            text: i18n("<b>Available wifi networks</b>")
            Layout.fillWidth: true
        }

        PlasmaExtras.ScrollArea{
            id:wifiarea
            anchors {
                top:label.bottom
                bottomMargin:units.gridUnit*2
                bottom: parent.bottom
                left: parent.left
                right: parent.right
            }
            
            ListView {
                
                property bool availableConnectionsVisible: false
                property int currentVisibleButtonIndex: -1
                
                anchors.fill: parent
                anchors.margins: units.gridUnit
                clip: true
                width: parent.width
                currentIndex: -1
                boundsBehavior: Flickable.StopAtBounds
                model: mobileappletProxyModel
                delegate: RowItemDelegate{
                    
                }
            }
        }
        PlasmaComponents.Button{
            id:customConnectionButton
            anchors.top:wifiarea.bottom
            text:"Add custom connection"
            
        }
        
    }
}

