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
import org.kde.qtextracomponents 0.1
import org.kde.plasma.graphicswidgets 0.1 as PlasmaWidgets
import org.kde.plasma.components 0.1 as PlasmaComponents
import org.kde.active.settings 0.1 as ActiveSettings

Item {
    id: detailsWidget;

    property variant detailKeys;

    DetailsModel {
        id: availableDetailsModel;
    }

    ListModel {
        id: selectedDetailsModel;
    }

    PlasmaComponents.Label {
        id: availableDetailsLabel;

        anchors {
            left: availableDetails.left;
            top: parent.top;
            leftMargin: 5;
        }
        text: i18n("Available details");
        font.weight: Font.Bold;
        opacity: .8;
    }

    PlasmaComponents.Label {
        id: selectedDetailsLabel;

        anchors {
            left: selectedDetails.left;
            top: parent.top;
            leftMargin: 5;
        }
        text: i18n("Selected details");
        font.weight: Font.Bold;
        opacity: .8;
    }

    DetailsView {
        id: availableDetails;

        anchors {
            left: parent.left;
            top: availableDetailsLabel.bottom;
            bottom: parent.bottom;
        }
        width: parent.width/2 - 50;
        model: availableDetailsModel;

        onItemClicked: {
            downArrow.enabled = false;
            upArrow.enabled = false;
            rightArrow.enabled = true;
            leftArrow.enabled = false;
            selectedDetails.index = -1;
        }
    }

    DetailsView {
        id: selectedDetails;

        anchors {
            right: parent.right;
            top: selectedDetailsLabel.bottom;
            bottom: parent.bottom
        }
        width: parent.width/2 - 50;
        model: selectedDetailsModel;

        onItemClicked: {
            if (selectedDetailsModel.count >= 2 && selectedDetails.index != selectedDetailsModel.count - 1) {
                downArrow.enabled = true;
            } else {
                downArrow.enabled = false;
            }
            if (selectedDetailsModel.count >= 2 && selectedDetails.index != 0) {
                upArrow.enabled = true;
            } else {
                upArrow.enabled = false;
            }
            leftArrow.enabled = true;
            rightArrow.enabled = false;
            availableDetails.index = -1;
        }
    }

    Row {
        id: leftRightArrows;

        anchors {
            horizontalCenter: parent.horizontalCenter;
            verticalCenter: parent.verticalCenter;
        }
        spacing: 5;

        PlasmaComponents.ToolButton {
            id: leftArrow;

            width: 48; height: 48;
            flat: false;
            iconSource: "go-previous";
            enabled: false;

            onClicked: {
                if (selectedDetails.index != -1 && selectedDetailsModel.count != 0) {
                    availableDetailsModel.append(selectedDetailsModel.get(selectedDetails.index));
                    selectedDetailsModel.remove(selectedDetails.index);
                }

                if (selectedDetailsModel.count == 0) {
                    enabled = false;
                }

                if (selectedDetails.index == 0 ){
                    upArrow.enabled = false;
                }

                if (selectedDetails.index == selectedDetailsModel.count - 1) {
                    downArrow.enabled = false;
                }
            }
        }

        PlasmaComponents.ToolButton {
            id: rightArrow;

            width: 48;
            height: 48;
            flat: false;
            iconSource: "go-next";
            enabled: false;

            onClicked: {
                if (availableDetails.index != -1 && availableDetailsModel.count != 0) {
                    selectedDetailsModel.append(availableDetailsModel.get(availableDetails.index));
                    availableDetailsModel.remove(availableDetails.index);
                }

                if (availableDetailsModel.count == 0) {
                    enabled = false;
                }
            }
        }
    }

    PlasmaComponents.ToolButton {
        id: upArrow;

        anchors {
            horizontalCenter: parent.horizontalCenter;
            bottom: leftRightArrows.top;
        }
        width: 48; height: 48;
        flat: false;
        iconSource: "go-up";
        enabled: false;

        onClicked: {
            if (selectedDetailsModel.count >= 2) {
                selectedDetailsModel.move(selectedDetails.index, selectedDetails.index - 1, 1);
            }

            if (selectedDetails.index == 0) {
                enabled = false;
            }

            downArrow.enabled = true;
        }
    }

    PlasmaComponents.ToolButton {
        id: downArrow;

        anchors {
            horizontalCenter: parent.horizontalCenter;
            top: leftRightArrows.bottom;
        }
        width: 48;
        height: 48;
        flat: false;
        iconSource: "go-down";
        enabled: false;

        onClicked: {
            if (selectedDetailsModel.count >= 2) {
                selectedDetailsModel.move(selectedDetails.index, selectedDetails.index + 1, 1);
            }

            if (selectedDetails.index == selectedDetailsModel.count - 1) {
                enabled = false;
            }

            upArrow.enabled = true;
        }
    }

    ActiveSettings.ConfigGroup {
        id: detailsConfig;
        file: "networkactivesettingrc";
        group: "General";
    }

    Component.onCompleted: {
        availableDetailsModel.init();
        var tmp = detailsConfig.readEntry("DetailKeys");
        var keys = tmp.split(',');
        if (keys.length == 0 || keys == "") {
            keys = ["interface:status","interface:bitrate","interface:hardwareaddress","ipv4:address","ipv6:address","wireless:ssid","wireless:signal","wireless:security","mobile:operator","mobile:quality","mobile:technology","vpn:plugin","vpn:banner"];
        }
        for (var i in keys) {
            for (var j = 0; j < availableDetailsModel.count; j++) {
                if (availableDetailsModel.get(j).key == keys[i]) {
                    selectedDetailsModel.insert(selectedDetailsModel.count, availableDetailsModel.get(j));
                    availableDetailsModel.remove(j);
                }
            }
        }
    }

    function save() {
        var keysToSave = [];
        for (var i = 0; i < selectedDetailsModel.count; i++) {
            keysToSave.push(selectedDetailsModel.get(i).key);
        }
        detailsConfig.writeEntry("DetailKeys", keysToSave);
        detailKeys = keysToSave;
    }
}
