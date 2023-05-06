/*
    SPDX-FileCopyrightText: 2013-2017 Jan Grulich <jgrulich@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.2
import org.kde.plasma.components 3.0 as PlasmaComponents3
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.extras 2.0 as PlasmaExtras

PlasmaComponents3.ScrollView {
    id: scrollView

    property alias model: connectionView.model
    property alias count: connectionView.count

    contentWidth: availableWidth - contentItem.leftMargin - contentItem.rightMargin

    contentItem: ListView {
        id: connectionView

        property int currentVisibleButtonIndex: -1
        property bool showSeparator: false

        Keys.onDownPressed: {
            connectionView.incrementCurrentIndex();
            connectionView.currentItem.forceActiveFocus();
        }
        Keys.onUpPressed: {
            if (connectionView.currentIndex === 0) {
                connectionView.currentIndex = -1;
                toolbar.searchTextField.forceActiveFocus();
                toolbar.searchTextField.selectAll();
            } else {
                event.accepted = false;
            }
        }

        topMargin: PlasmaCore.Units.smallSpacing * 2
        bottomMargin: PlasmaCore.Units.smallSpacing * 2
        leftMargin: PlasmaCore.Units.smallSpacing * 2
        rightMargin: PlasmaCore.Units.smallSpacing * 2
        spacing: PlasmaCore.Units.smallSpacing
        model: appletProxyModel
        currentIndex: -1
        boundsBehavior: Flickable.StopAtBounds
        section.property: showSeparator ? "Section" : ""
        section.delegate: ListItem {
            separator: true
        }
        highlight: PlasmaExtras.Highlight { }
        highlightMoveDuration: 0
        highlightResizeDuration: 0
        delegate: ConnectionItem {
            width: connectionView.width - PlasmaCore.Units.smallSpacing * 4
        }
    }

    Loader {
        anchors.centerIn: parent
        width: parent.width - (PlasmaCore.Units.largeSpacing * 4)
        active: connectionView.count === 0
        asynchronous: true
        visible: status == Loader.Ready
        sourceComponent: PlasmaExtras.PlaceholderMessage {
            iconName: {
                if (toolbar.displayplaneModeMessage) {
                    return "network-flightmode-on"
                }
                if (toolbar.displayWifiMessage) {
                    return "network-wireless-off"
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
                if (toolbar.displayWifiMessage) {
                    if (toolbar.displayWwanMessage) {
                        return i18n("Wireless and mobile networks are deactivated")
                    }
                    return i18n("Wireless is deactivated")
                }
                if (toolbar.displayWwanMessage) {
                    return i18n("Mobile network is deactivated")
                }
                if (toolbar.searchTextField.text.length > 0) {
                    return i18n("No matches")
                }
                return i18n("No available connections")
            }
        }
    }
}
