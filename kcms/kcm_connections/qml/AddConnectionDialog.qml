/*
    SPDX-FileCopyrightText: 2016 Jan Grulich <jgrulich@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

import QtQuick
import QtQuick.Window
import QtQuick.Controls as QQC2
import org.kde.kirigami 2 as Kirigami
import org.kde.plasma.networkmanagement as PlasmaNM

Window {
    id: dialog

    signal configurationDialogRequested()

    title: i18nc("@title:window", "Choose a Connection Type")

    height: 600
    minimumHeight: 400
    width: 500
    minimumWidth: 500

    PlasmaNM.CreatableConnectionsModel {
        id: connectionModel
    }

    Rectangle {
        id: background
        anchors.fill: parent
        focus: true
        color: Kirigami.Theme.backgroundColor
    }

    QQC2.ScrollView {
        id: scrollView
        anchors.fill: parent

        QQC2.ScrollBar.horizontal.policy: QQC2.ScrollBar.AlwaysOff

        ListView {
            id: view

            property bool connectionShared
            property string connectionSpecificType
            property int connectionType
            property string connectionVpnType

            clip: true
            model: connectionModel
            currentIndex: -1
            boundsBehavior: Flickable.StopAtBounds
            section.property: "ConnectionTypeSection"
            section.delegate: Kirigami.ListSectionHeader {
                text: section
                width: ListView.view.width
            }
            Rectangle {
                id: background1
                z: -1
                anchors.fill: parent
                focus: true
                Kirigami.Theme.colorSet: Kirigami.Theme.View
                color: Kirigami.Theme.backgroundColor
            }
            delegate: QQC2.ItemDelegate {
                id: delegate
                required property int index
                required property var model

                highlighted: delegate.pressed || delegate.down || delegate.index === view.currentIndex

                text: model.ConnectionTypeName
                property string subtitle: model.ConnectionType === PlasmaNM.Enums.Vpn ? delegate.model.ConnectionDescription : null

                width: view.width
                icon.name: model.ConnectionIcon

                onClicked: {
                    dialog.close()
                    kcm.onRequestCreateConnection(model.ConnectionType, model.ConnectionVpnType, model.ConnectionSpecificType, model.ConnectionShared)
                }

                contentItem: Kirigami.IconTitleSubtitle {
                    icon: icon.fromControlsIcon(delegate.icon)
                    title: delegate.text
                    subtitle: delegate.subtitle
                    selected: delegate.highlighted
                }
            }
        }
    }
}
