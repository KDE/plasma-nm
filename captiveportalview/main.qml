/*
    SPDX-FileCopyrightText: 2023 David Redondo <kde@david-redondo.de>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtWebEngine

import org.kde.kirigami as Kirigami
import org.kde.plasma.networkmanagement

Kirigami.ApplicationWindow {
    property bool loggedIn: !connectionIconProvider.needsPortal

    title: i18nc("@title:window", "Log into Network")

    header: ToolBar {
        visible: !loggedIn
        RowLayout {
            anchors.fill: parent
            TextField {
                id:text
                Layout.fillWidth: true
                color: Kirigami.Theme.disabledTextColor
                horizontalAlignment: TextInput.AlignLeft
                readOnly: true
                text: webView.url
                // keep text left aligned when url is longer than  what fits
                onTextChanged: cursorPosition = 0
            }
            ToolButton {
                Layout.alignment: Layout.AlignRight
                text: i18nc("@action:button", "Open in Browser")
                icon.name: "document-open"
                onClicked: Qt.openUrlExternally(webView.url)
            }
        }
    }

    WebEngineView {
        id: webView
        anchors.fill: parent
        url: "https://networkcheck.kde.org"
        visible: !loggedIn
    }
    Kirigami.PlaceholderMessage {
        anchors.centerIn: parent
        visible: loggedIn
        text: i18n("Logged in successfully")
        icon.name: "online"
    }

    ConnectionIcon {
        id: connectionIconProvider
    }
}
