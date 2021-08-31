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

ColumnLayout {
    id: root
    property var model
    property alias count: repeater.count
    spacing: 0
    
    Repeater {
        id: repeater
        model: root.model
        
        delegate: Kirigami.InlineMessage {
            Layout.bottomMargin: Kirigami.Units.largeSpacing
            Layout.fillWidth: true
            visible: true
            text: modelData.message
            type: {
                switch (modelData.type) {
                    case InlineMessage.Information: return Kirigami.MessageType.Information;
                    case InlineMessage.Positive: return Kirigami.MessageType.Positive;
                    case InlineMessage.Warning: return Kirigami.MessageType.Warning;
                    case InlineMessage.Error: return Kirigami.MessageType.Error;
                }
                return Kirigami.MessageType.Error; 
            }
            
            actions: [
                Kirigami.Action {
                    icon.name: "dialog-close"
                    onTriggered: kcm.removeMessage(model.index)
                }
            ]
        }
    }
}
