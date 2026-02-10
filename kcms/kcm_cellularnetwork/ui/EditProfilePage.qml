// SPDX-FileCopyrightText: 2020-2022 Devin Lin <espidev@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick 2.12
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.12 as Controls

import org.kde.kirigami 2.19 as Kirigami
import org.kde.kirigamiaddons.formcard 1 as FormCard

import org.kde.plasma.networkmanagement.cellular as Cellular

FormCard.FormCardPage {
    id: editProfile
    title: isNewProfile ? i18n("New APN") : i18n("Edit APN")

    topPadding: Kirigami.Units.gridUnit
    bottomPadding: Kirigami.Units.gridUnit
    leftPadding: 0
    rightPadding: 0

    property Cellular.CellularModem modem
    property bool isNewProfile: true
    property string profileConnectionUni: ""
    property string profileName: ""
    property string profileApn: ""
    property string profileUser: ""
    property string profilePassword: ""
    property string profileNetworkType: ""
    property bool profileRoamingAllowed: false

    FormCard.FormCard {
        Layout.topMargin: Kirigami.Units.gridUnit

        FormCard.FormTextFieldDelegate {
            id: profileNameField
            label: i18n("Name")
            text: profileName
        }

        FormCard.FormDelegateSeparator { above: profileNameField; below: profileApnField }

        FormCard.FormTextFieldDelegate {
            id: profileApnField
            label: i18n("APN")
            text: profileApn
        }

        FormCard.FormDelegateSeparator { above: profileApnField; below: profileUsernameField }

        FormCard.FormTextFieldDelegate {
            id: profileUsernameField
            label: i18n("Username")
            text: profileUser
        }

        FormCard.FormDelegateSeparator { above: profileUsernameField; below: profilePasswordField }

        FormCard.FormTextFieldDelegate {
            id: profilePasswordField
            label: i18n("Password")
            text: profilePassword
        }

        FormCard.FormDelegateSeparator { above: profilePasswordField; below: profileNetworkTypeField }

        FormCard.FormComboBoxDelegate {
            id: profileNetworkTypeField
            text: i18n("Network type")
            model: [i18n("4G/3G/2G"), i18n("3G/2G"), i18n("2G"), i18n("Only 4G"), i18n("Only 3G"), i18n("Only 2G"), i18n("Any")]
            Component.onCompleted: {
                if (!isNewProfile) {
                    currentIndex = indexOfValue(profileNetworkType)
                }
            }
        }

        FormCard.FormDelegateSeparator { above: profileNetworkTypeField; below: profileSave }

        FormCard.FormButtonDelegate {
            id: profileSave
            text: i18n("Save profile")
            icon.name: "document-save"
            onClicked: {
                if (isNewProfile) {
                    modem.addProfile(profileNameField.text, profileApnField.text, profileUsernameField.text, profilePasswordField.text, profileNetworkTypeField.currentText);
                } else {
                    modem.updateProfile(profileConnectionUni, profileNameField.text, profileApnField.text, profileUsernameField.text, profilePasswordField.text, profileNetworkTypeField.currentText);
                }
                kcm.pop()
            }
        }
    }
}
