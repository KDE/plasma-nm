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

    property bool passwordIsStatic: (model.SecurityType === PlasmaNM.Enums.StaticWep || model.SecurityType == PlasmaNM.Enums.WpaPsk ||
                                     model.SecurityType === PlasmaNM.Enums.Wpa2Psk || model.SecurityType == PlasmaNM.Enums.SAE)
    property bool predictableWirelessPassword: !model.Uuid && model.Type === PlasmaNM.Enums.Wireless && passwordIsStatic
    property bool isUnknownNewConnection: model.ItemType !== PlasmaNM.NetworkModelItem.UnavailableConnection && delegate.isConnectionDeactivated() && !model.ConnectionPath

    // QQC2.ItemDelegate exposes the label text property, but not textFormat, so it's possible for model.Name to contain unescaped HTML.
    // Therefore we need to escape it manually. Going via i18n is a way to use something pre-exposed to QML.
    text: xi18nc("@label/rich", "%1", model.Name)

    property string subtitle: itemText()
    Accessible.description: subtitle

    checked: model.ConnectionPath && model.ConnectionPath === currentConnectionPath
    highlighted: checked || pressed

    signal aboutToChangeConnection(bool exportable, string name, string path)
    signal aboutToRemoveConnection(string name, string path)

    onClicked: {
        if (isUnknownNewConnection) {
            changeState()
        } else {
            aboutToChangeConnection(model.KcmVpnConnectionExportable, model.Name, model.ConnectionPath)
        }
    }
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
            enabled: model.ItemType === PlasmaNM.NetworkModelItem.AvailableConnection || model.ItemType === PlasmaNM.NetworkModelItem.AvailableAccessPoint
            onTriggered: changeState()
            QQC2.ToolTip.visible: !enabled && hovered
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
            QQC2.ToolTip.text: i18nc("@info:tooltip", "Cannot connect to this network because it was not detected.")
        }

        QQC2.MenuItem {
            icon.name: "list-remove-symbolic"
            text: i18n("Delete");
            visible: !delegate.isUnknownNewConnection

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
        } else if (model.ItemType === PlasmaNM.NetworkModelItem.AvailableAccessPoint) {
            return i18nc("@title:column Available networks", "Available")
        } else {
            return model.LastUsed
        }
    }

    function changeState() {
        if (!predictableWirelessPassword && (model.ConnectionState == PlasmaNM.Enums.Deactivated || model.ConnectionState === PlasmaNM.Enums.Deactivating)) {
            if (!predictableWirelessPassword && !model.Uuid) {
                handler.addAndActivateConnection(model.DevicePath, model.SpecificPath)
            } else {
                handler.activateConnection(model.ConnectionPath, model.DevicePath, model.SpecificPath)
            }
        } else if (!predictableWirelessPassword) {
            handler.deactivateConnection(model.ConnectionPath, model.DevicePath)
        } else if (predictableWirelessPassword) {
            root.activateConnectionWithDialog(model.ItemUniqueName, model.DevicePath, model.SpecificPath, model.SecurityType)
        }
    }
}
