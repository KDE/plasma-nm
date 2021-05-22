/*
    SPDX-FileCopyrightText: 2020 Devin Lin <espidev@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
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
