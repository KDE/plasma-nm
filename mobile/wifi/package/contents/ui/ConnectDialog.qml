/*
 *   Copyright 2020 Devin Lin <espidev@gmail.com>
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
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.2 as Controls
import org.kde.kirigami 2.12 as Kirigami

Kirigami.OverlaySheet {
    id: sheetRoot
    property int securityType
    property string headingText
    property string devicePath
    property string specificPath
    
    signal donePressed(string password)
    
    function openAndClear() {
        warning.visible = false;
        this.open();
        passwordField.text = "";
        passwordField.focus = true;
    }
    
    header: Kirigami.Heading {
        level: 2
        text: headingText
        wrapMode: Text.WordWrap
    }
    footer: RowLayout {
        Item {
            Layout.fillWidth: true
        }
        Controls.Button {
            text: i18nc("@action:button", "Cancel")
            icon.name: "dialog-close"
            onClicked: sheetRoot.close();
        }
        Controls.Button {
            text: i18nc("@action:button", "Done")
            icon.name: "dialog-ok"
            onClicked: {
                if (passwordField.acceptableInput) {
                    sheetRoot.close();
                    handler.addAndActivateConnection(devicePath, specificPath, passwordField.text);
                } else {
                    warning.visible = true;
                }
            }
        }
    }
    
    ColumnLayout {
        Layout.fillWidth: true
        spacing: Kirigami.Units.largeSpacing
        
        PasswordField {
            id: passwordField
            Layout.fillWidth: true
            securityType: sheetRoot.securityType
        }
        
        Controls.Label {
            id: warning
            text: i18n("Invalid input.")
            visible: false
        }
    }
    
}
