/*
 * SPDX-FileCopyrightText: 2020-2021 Devin Lin <espidev@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
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
                model: [i18n("4G/3G/2G"), i18n("3G/2G"), i18n("2G"), i18n("Only 4G"), i18n("Only 3G"), i18n("Only 2G"), i18n("Any")]
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
