/*
    SPDX-FileCopyrightText: 2013-2017 Jan Grulich <jgrulich@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

import QtQuick 2.15
import QtQuick.Layouts 1.2
import org.kde.plasma.components 3.0 as PlasmaComponents3
import org.kde.kirigami 2.20 as Kirigami
import org.kde.plasma.extras as PlasmaExtras
import org.kde.plasma.networkmanagement as PlasmaNM
import org.kde.networkmanager as NMQt

ColumnLayout {
    id: connectionListPage

    required property PlasmaNM.NetworkStatus nmStatus
    property alias model: connectionView.model
    property alias count: connectionView.count

    spacing: 0

    Keys.forwardTo: [connectionView]

    Kirigami.InlineMessage {
        id: connectivityMessage
        Layout.fillWidth: true
        Layout.preferredHeight: contentItem.implicitHeight + topPadding + bottomPadding
        type: Kirigami.MessageType.Information
        position: Kirigami.InlineMessage.Position.Header
        icon.name: "dialog-password"
        text: i18n("You need to log in to this network")
        visible: connectionListPage.nmStatus.connectivity === NMQt.NetworkManager.Portal

        actions: Kirigami.Action {
            text: i18nc("@action:button", "Log in")
            onTriggered: {
                Qt.openUrlExternally(connectionListPage.nmStatus.networkCheckUrl);
            }
        }
    }

    PlasmaComponents3.ScrollView {
        id: scrollView

        Layout.fillWidth: true
        Layout.fillHeight: true
        contentWidth: availableWidth - contentItem.leftMargin - contentItem.rightMargin
        PlasmaComponents3.ScrollBar.horizontal.policy: PlasmaComponents3.ScrollBar.AlwaysOff

        contentItem: ListView {
            id: connectionView

            property int currentVisibleButtonIndex: -1

            Keys.onDownPressed: event => {
                connectionView.incrementCurrentIndex();
                connectionView.currentItem.forceActiveFocus();
            }
            Keys.onUpPressed: event => {
                if (connectionView.currentIndex === 0) {
                    connectionView.currentIndex = -1;
                    toolbar.searchTextField.forceActiveFocus();
                    toolbar.searchTextField.selectAll();
                } else {
                    event.accepted = false;
                }
            }

            // No topMargin because ListSectionHeader and InlineMessage both bring
            // their own.
            bottomMargin: Kirigami.Units.smallSpacing * 2
            leftMargin: Kirigami.Units.smallSpacing * 2
            rightMargin: Kirigami.Units.smallSpacing * 2
            spacing: Kirigami.Units.smallSpacing
            model: appletProxyModel
            currentIndex: -1
            boundsBehavior: Flickable.StopAtBounds
            section.property: "Section"
            section.delegate: PlasmaExtras.ListSectionHeader {
                width: connectionView.width - connectionView.leftMargin - connectionView.rightMargin
                text: section
            }
            highlight: PlasmaExtras.Highlight { }
            highlightMoveDuration: Kirigami.Units.shortDuration
            highlightResizeDuration: Kirigami.Units.shortDuration
            delegate: ConnectionItem {
                listView: connectionView
                width: connectionView.width - Kirigami.Units.smallSpacing * 4
            }

            // Placeholder message
            Loader {
                anchors.centerIn: parent
                width: parent.width - (Kirigami.Units.largeSpacing * 4)
                active: connectionView.count === 0
                asynchronous: true
                visible: status === Loader.Ready
                sourceComponent: PlasmaExtras.PlaceholderMessage {
                    iconName: {
                        if (toolbar.displayplaneModeMessage) {
                            return "network-flightmode-on"
                        }
                        if (toolbar.displayWifiOffMessage) {
                            return "network-wireless-off"
                        }
                        if (toolbar.displayWifiConnectingMessage) {
                            return "view-refresh-symbolic"
                        }
                        if (toolbar.displayWwanMessage) {
                            return "network-mobile-off"
                        }
                        return "edit-none"
                    }
                    text: {
                        if (toolbar.displayplaneModeMessage) {
                            return i18n("Airplane mode is enabled")
                        }
                        if (toolbar.displayWifiOffMessage) {
                            if (toolbar.displayWwanMessage) {
                                return i18n("Wi-Fi and mobile data are deactivated")
                            }
                            return i18n("Wi-Fi is deactivated")
                        }
                        if (toolbar.displayWifiConnectingMessage) {
                            return i18n("Looking for Wi-Fi networks")
                        }
                        if (toolbar.displayWwanMessage) {
                            return i18n("Mobile data is deactivated")
                        }
                        if (toolbar.searchTextField.text.length > 0) {
                            return i18n("No matches")
                        }
                        if (connectionListPage.nmStatus.connectivity === NMQt.NetworkManager.Full) {
                            return i18n("No available connections")
                        }
                        return nmStatus.checkUnknownReason()
                    }
                    explanation: {
                        if (toolbar.searchTextField.text.length === 0) {
                            return ""
                        }
                        if (toolbar.displayplaneModeMessage) {
                            return i18nc("@info:placeholder", "Turn off airplane mode to search for networks")
                        }
                        if (toolbar.displayWifiOffMessage) {
                            if (toolbar.displayWwanMessage) {
                                return i18nc("@info:placeholder", "Turn on Wi-Fi or mobile data to search for networks")
                            }
                            return i18nc("@info:placeholder", "Turn on Wi-Fi to search for networks")
                        }
                        if (toolbar.displayWwanMessage) {
                            return i18nc("@info:placeholder", "Turn on mobile data to search for networks")
                        }
                    }
                }
            }

            function newIndexSelected(index) {
                for (let i = 0; i < connectionView.count; i++) {
                    if (i === index)
                        continue

                    let item = connectionView.itemAtIndex(i)
                    if (item && item.expanded && item.customExpandedViewContent === item.passwordDialogComponent) {
                        item.collapse()
                    }
                }
            }
        }
    }
}
