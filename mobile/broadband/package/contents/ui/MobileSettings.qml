
/*
 *   Copyright 2018 Martin Kacej <m.kacej@atlas.sk>
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

import QtQuick 2.6
import QtQuick.Controls 2.2 as Controls
import QtQuick.Layouts 1.3
import org.kde.plasma.networkmanagement 0.2 as PlasmaNM
import org.kde.kirigami 2.2 as Kirigami

Kirigami.Page {
    ColumnLayout {
        width: parent.width
        spacing: Kirigami.Units.gridUnit * 1.5
        RowLayout {
            width: parent.width
            Controls.Label {
                text: i18n("Enable mobile data network")
                font.weight: Font.Bold
                Layout.fillWidth: true
            }
            Controls.CheckBox {
                id: mobileDataCheckbox
                enabled: enabledConnections.wwanHwEnabled && availableDevices.modemDeviceAvailable
                anchors.rightMargin: Kirigami.Units.gridUnit
            }
        }
        RowLayout {
            width: parent.width
            enabled: mobileDataCheckbox.checked
            Controls.Label {
                text: i18n("Enable data roaming")
                font.weight: Font.Bold
                color: parent.enabled ? Kirigami.Theme.textColor : Kirigami.Theme.disabledTextColor
                Layout.fillWidth: true
            }
            Controls.CheckBox {
                enabled: parent.enabled
                checked: false
                anchors.rightMargin: Kirigami.Units.gridUnit
                onEnabledChanged: {
                    if (!enabled)
                        checked = false
                }
            }
        }
        Kirigami.Separator {}
        Column {
            Controls.Label {
                text: i18n("Access point name")
                font.weight: Font.Bold
            }
            Row {
                Controls.TextField {
                    text: kcm.getAPN()
                }
                Controls.ToolButton {
                    text: i18n("Edit APN")
                }
            }
        }
        Controls.Button {
            text: i18n("Data usage")
        }
    }
}
