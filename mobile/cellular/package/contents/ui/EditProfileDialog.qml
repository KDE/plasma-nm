/*
 *   Copyright 2020-2021 Devin Lin <espidev@gmail.com>
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

PopupDialog {
    id: dialog
    title: i18n("Edit APN")
    
    property Modem modem
    property ProfileSettings profile
    
    property int pageWidth
    
    standardButtons: Controls.Dialog.Ok | Controls.Dialog.Cancel
    
    onAccepted: {
        if (profile == null) { // create new profile
            modem.addProfile(profileName.text, profileApn.text, profileUsername.text, profilePassword.text, profileNetworkType.value);
        } else { // edit existing profile
            modem.updateProfile(profile.connectionUni, profileName.text, profileApn.text, profileUsername.text, profilePassword.text, profileNetworkType.value);
        }
    }
    width: pageWidth - Kirigami.Units.gridUnit * 4
    padding: Kirigami.Units.gridUnit
    
    ColumnLayout {
        Kirigami.FormLayout {
            Layout.fillWidth: true
            wideMode: false
            
            Controls.TextField {
                id: profileName
                Kirigami.FormData.label: i18n("Name")
                text: profile != null ? profile.name : ""
            }
            Controls.TextField {
                id: profileApn
                Kirigami.FormData.label: i18n("APN")
                text: profile != null ? profile.apn : ""
            }
            Controls.TextField {
                id: profileUsername
                Kirigami.FormData.label: i18n("Username")
                text: profile != null ? profile.user : ""
            }
            Controls.TextField {
                id: profilePassword
                Kirigami.FormData.label: i18n("Password")
                text: profile != null ? profile.password : ""
            }
            Controls.ComboBox {
                id: profileNetworkType
                Kirigami.FormData.label: i18n("Network type")
                model: ["4G/3G/2G", "3G/2G", "2G", "Only 4G", "Only 3G", "Only 2G", "Any"]
                Component.onCompleted: {
                    if (profile != null) {
                        currentIndex = indexOfValue(profile.networkType)
                    }
                }
            }
            Controls.Button {
                icon.name: "list-add"
                text: i18n("Autodetect Settings")
                onClicked: {
                    dialog.close();
                    modem.addDetectedProfileSettings();
                }
            }
        }
    }
}
