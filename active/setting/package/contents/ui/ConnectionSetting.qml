/*
    Copyright 2013 Jan Grulich <jgrulich@redhat.com>

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

import QtQuick 1.1
import org.kde.plasma.components 0.1 as PlasmaComponents
import org.kde.plasma.extras 0.1 as PlasmaExtras
import org.kde.plasmanm 0.1 as PlasmaNm

Item {
    property variant selectedItemModel;

    anchors.fill: parent;

    Column {
        id: settingsDescription;

        anchors {top: parent.top; left: parent.left; right: parent.right }

        PlasmaExtras.Heading {
            id: nameLabel;

            anchors { left: parent.left; right: parent.right }
            text: selectedItemModel ? i18n("<b>%1</b> settings").arg(selectedItemModel.itemName) : "";
            elide: Text.ElideRight;
            horizontalAlignment: Text.AlignHCenter;
            level: 2;
        }

        PlasmaComponents.Label {
            id: statusLabel;

            anchors { left: parent.left; right: parent.right }
            text: {
                if (selectedItemModel) {
                    if (selectedItemModel.itemConnected) {
                        i18n("Connected");
                    } else if (selectedItemModel.itemConnecting) {
                        i18n("Connecting");
                    } else {
                        i18n("Not connected");
                    }
                } else {
                    "";
                }
            }
            horizontalAlignment: Text.AlignHCenter;
            opacity: .3;
        }
    }

    PlasmaComponents.Button {
        id: addButton;

        anchors { right: parent.right; bottom: parent.bottom; rightMargin: 5 }
        text: {
            if (selectedItemModel) {
                if (selectedItemModel.itemConnected || selectedItemModel.itemConnecting) {
                    i18n("Disconnect");
                } else {
                    i18n("Connect");
                }
            } else {
                "";
            }
        }

        onClicked: {
            if (selectedItemModel) {
                if (!selectedItemModel.itemConnected && !selectedItemModel.itemConnecting) {
                    if (selectedItemModel.itemUuid) {
                        handler.activateConnection(selectedItemModel.itemConnectionPath, selectedItemModel.itemDevicePath, selectedItemModel.itemSpecificPath);
                    } else {
                        handler.addAndActivateConnection(selectedItemModel.itemDevicePath, selectedItemModel.itemSpecificPath);
                    }
                } else {
                    handler.deactivateConnection(selectedItemModel.itemConnectionPath);
                }
            }
        }
    }
}
