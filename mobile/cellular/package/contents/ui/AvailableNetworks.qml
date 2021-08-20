/*
 *   Copyright 2021 Devin Lin <espidev@gmail.com>
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

import QtQuick 2.12
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.12 as Controls
import org.kde.kirigami 2.12 as Kirigami
import cellularnetworkkcm 1.0

Kirigami.ScrollablePage {
    id: root
    title: i18n("Available Networks")
    
    property Modem modem
    
    ColumnLayout {
        Controls.Button {
            icon.name: "view-refresh"
            text: "Scan"
            enabled: !modem.details.isScanningNetworks
            onClicked: modem.details.scanNetworks()
        }

        ListView {
            Layout.fillWidth: true
            model: modem.details.networks
            
            Controls.BusyIndicator {
                anchors.centerIn: parent
                visible: modem.details.isScanningNetworks
                Layout.minimumWidth: Kirigami.Units.iconSizes.medium
                Layout.minimumHeight: width
            }
            
            delegate: Kirigami.SwipeListItem {
                // TODO onClicked:
                
                contentItem: RowLayout {
                    Layout.fillWidth: true
                    
                    ColumnLayout {
                        spacing: Kirigami.Units.smallSpacing
                        Kirigami.Heading {
                            level: 3
                            text: modelData.operatorLong + " | " + modelData.operatorShort + "(" + modelData.operatorCode + ")"
                        }
                        Controls.Label {
                            text: modelData.accessTechnology
                        }
                    }
                    Controls.RadioButton {
                        Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                        checked: modelData.isCurrentlyUsed
                        // TODO onClicked: 
                    }
                }
            }
        }
    }
}


