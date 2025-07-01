/*
    SPDX-FileCopyrightText: 2016 Jan Grulich <jgrulich@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import org.kde.kirigami as Kirigami
import org.kde.plasma.components as PlasmaComponents3
import org.kde.plasma.networkmanagement as PlasmaNM

QQC2.ItemDelegate {
    id: delegate

    required property var model
    required property string currentConnectionPath

    icon.name: model.KcmConnectionIcon

    text: model.Name
    property string subtitle: itemText()
    Accessible.description: subtitle

    checked: model.ConnectionPath === currentConnectionPath
    highlighted: checked || pressed

    signal aboutToChangeConnection(bool exportable, string name, string path)
    signal aboutToRemoveConnection(string name, string path)

    onClicked: aboutToChangeConnection(model.KcmVpnConnectionExportable, model.Name, model.ConnectionPath)
    Keys.onSpacePressed: aboutToChangeConnection(model.KcmVpnConnectionExportable, model.Name, model.ConnectionPath)

    TapHandler {
        id: mouseArea
        acceptedButtons: Qt.RightButton
        onTapped: connectionItemMenu.popup()
    }

    contentItem: RowLayout {
        spacing: Kirigami.Units.smallSpacing
        width: delegate.availableWidth
        implicitHeight: Math.max(iconTitleSubtitle.implicitHeight, Kirigami.Units.iconSizes.medium)

        Kirigami.IconTitleSubtitle {
            id: iconTitleSubtitle
            Layout.fillWidth: true
            icon: icon.fromControlsIcon(delegate.icon)
            title: delegate.text
            selected: delegate.highlighted
            subtitle: delegate.subtitle
            font.weight: delegate.model.ConnectionState === PlasmaNM.Enums.Activated ? Font.DemiBold : Font.Normal
            font.italic: delegate.model.ConnectionState === PlasmaNM.Enums.Activating ? true : false
        }

        PlasmaComponents3.BusyIndicator {
            id: connectingIndicator
            height: Kirigami.Units.iconSizes.medium
            width: Kirigami.Units.iconSizes.medium
            running: model.ConnectionState === PlasmaNM.Enums.Activating
            visible: running
        }
    }

    QQC2.Menu {
        id: connectionItemMenu

        QQC2.MenuItem {
            icon.name: delegate.isConnectionDeactivated() ? "network-connect-symbolic" : "network-disconnect-symbolic"
            text: delegate.isConnectionDeactivated() ? i18n("Connect") : i18n("Disconnect")
            enabled: model.ItemType === 1 /* available */
            onTriggered: {
                if (delegate.isConnectionDeactivated()) {
                    handler.activateConnection(model.ConnectionPath, model.DevicePath, model.SpecificPath);
                } else {
                    handler.deactivateConnection(model.ConnectionPath, model.DevicePath);
                }
            }
            QQC2.ToolTip.visible: !enabled && hovered
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
            QQC2.ToolTip.text: i18nc("@info:tooltip", "Cannot connect to this network because it was not detected.")
        }

        QQC2.MenuItem {
            icon.name: "list-remove-symbolic"
            text: i18n("Delete");

            onTriggered: {
                delegate.aboutToRemoveConnection(delegate.model.Name, delegate.model.ConnectionPath)
            }
        }

        QQC2.MenuItem {
            icon.name: "document-export-symbolic"
            visible: model.KcmVpnConnectionExportable
            text: i18n("Export");

            onTriggered: kcm.onRequestExportConnection(model.ConnectionPath)
        }
    }

    function isConnectionDeactivated(): bool {
        return model.ConnectionState === PlasmaNM.Enums.Deactivated;
    }

    /* This generates the status description under each connection
       in the list at the left side of the applet. */
    function itemText() {
        if (model.ConnectionState === PlasmaNM.Enums.Activated) {
            return i18n("Connected")
        } else if (model.ConnectionState === PlasmaNM.Enums.Activating) {
            return i18n("Connecting")
        } else {
            return model.LastUsed
        }
    }
}
