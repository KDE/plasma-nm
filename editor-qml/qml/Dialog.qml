/*
    Copyright 2014 Jan Grulich <jgrulich@redhat.com>

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

import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
// import QtQuick.Dialogs 1.0
// import org.kde.plasma.plasmoid 2.0
// import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.core 2.0 as PlasmaCore
// import org.kde.plasma.extras 2.0 as PlasmaExtras
import org.kde.plasma.networkmanagement 0.1 as PlasmaNM
import ConnectionEditor 0.1

Window {
    id: addConnectionDialog;

    height: 250;
    width: 500;
    color: myPalette.window;
    modality: Qt.ApplicationModal;
    flags: Qt.Dialog;
    title: i18n("Add a new connection");

    ConnectionEditor {
        id: connectionEditor;
    }

    Text {
        id: dialogHeadline;

        anchors {
            left: parent.left;
            leftMargin: units.iconSizes.medium;
            right: parent.right;
            rightMargin: units.iconSizes.medium;
            top: parent.top;
            topMargin: units.iconSizes.medium;
        }

        font.pointSize: theme.defaultFont.pointSize * 1.5;
        font.weight: Font.DemiBold;
        text: i18n("Select a connection type");
    }

    Text {
        id: dialogText;

        anchors {
            left: parent.left;
            leftMargin: units.iconSizes.medium;
            right: parent.right;
            rightMargin: units.iconSizes.medium;
            top: dialogHeadline.bottom;
            topMargin: 8;
        }

        text: i18n("Select a connection type you would like to create.\n\nIf you are trying to create" +
                   " a new VPN connection which is not listed here, it's possible you don't have installed correct VPN plugin.");
        wrapMode: Text.WordWrap;
    }

    ListModel {
        id: connectionTypesModel;

        Component.onCompleted: {
            // Hardware
            append( { "name": i18n("DSL"), "type": PlasmaNM.Enums.Pppoe } );
            append( { "name": i18n("InfiniBand"), "type": PlasmaNM.Enums.Infiniband } );
            append( { "name": i18n("Mobile Broadband"), "type": PlasmaNM.Enums.Gsm } );
            append( { "name": i18n("Wired"), "type": PlasmaNM.Enums.Wired } );
            append( { "name": i18n("Wired (shared)"), "type": PlasmaNM.Enums.Wired, "shared": true } );
            append( { "name": i18n("Wireless"), "type": PlasmaNM.Enums.Wireless } );
            append( { "name": i18n("Wireless (shared)"), "type": PlasmaNM.Enums.Wireless, "shared": true } );
            append( { "name": i18n("WiMAX"), "type": PlasmaNM.Enums.Wimax } );

            // Virtual
            append( { "name": i18n("Bond"), "type": PlasmaNM.Enums.Bond } );
            append( { "name": i18n("Bridge"), "type": PlasmaNM.Enums.Bridge } );
            append( { "name": i18n("VLAN"), "type": PlasmaNM.Enums.Vlan } );
//             append( { "name": i18n("Team"), "type": PlasmaNM.Enums.Team } );

            // VPN
            var index;
            for (index = 0; index < connectionEditor.availableVpnPlugins.length; ++index) {
                append( {"name": i18n(connectionEditor.availableVpnPlugins[index]), "type": PlasmaNM.Enums.Vpn, "plugin": connectionEditor.availableVpnPlugins[index] });
            }
        }
    }

    ComboBox {
        id: connectionTypesCombobox;

        anchors {
            left: parent.left;
            leftMargin: units.iconSizes.medium;
            right: parent.right;
            rightMargin: units.iconSizes.medium;
            top: dialogText.bottom;
            topMargin: 8;
        }

        model: connectionTypesModel;
        textRole: "name";
    }

    Button {
        id: okButton;

        anchors {
            bottom: parent.bottom;
            bottomMargin: 8;
            leftMargin: 8;
            right: cancelButton.left;
        }

        text: i18n("Create...");

        onClicked: {
            close();
            if (connectionTypesModel.get(connectionTypesCombobox.currentIndex).type != PlasmaNM.Enums.Vpn) {
                connectionEditor.addConnection(connectionTypesModel.get(connectionTypesCombobox.currentIndex).type,
                                               connectionTypesModel.get(connectionTypesCombobox.currentIndex).shared)
            } else {
                connectionEditor.addVpnConnection(connectionTypesModel.get(connectionTypesCombobox.currentIndex).plugin);
            }
        }
    }

    Button {
        id: cancelButton;

        anchors {
            bottom: parent.bottom;
            bottomMargin: 8;
            right: parent.right;
            rightMargin: 8;
        }

        text: i18n("Cancel");

        onClicked: {
            close();
        }
    }
}
