/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>
    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami

Item {
    id: root

    property string iconSource: "dialog-password"

    property string headline: ""
    property string text: ""

    property string password: ""
    property int maxLength: 64

    enum PasswordOption {
        StoreForUser = 0,
        StoreForAllUsers = 1,
        AlwaysAsk = 2,
        NotRequired = 3
    }

    property bool showPasswordOptions: false
    property int passwordOption: PasswordField.PasswordOption.StoreForUser

    property bool showNotRequired: false

    implicitWidth: mainLayout.implicitWidth
    implicitHeight: mainLayout.implicitHeight

    ColumnLayout {
        id: mainLayout

        anchors.fill: parent
        spacing: Kirigami.Units.smallSpacing

        RowLayout {
            Layout.fillWidth: true
            spacing: Kirigami.Units.largeSpacing
            visible: root.headline !== "" || root.text !== ""

            Kirigami.Icon {
                source: root.iconSource

                Layout.preferredWidth: Kirigami.Units.iconSizes.large
                Layout.preferredHeight: Kirigami.Units.iconSizes.large
                Layout.margins: Kirigami.Units.smallSpacing
            }

            ColumnLayout {
                Layout.fillWidth: true
                spacing: Kirigami.Units.smallSpacing

                QQC2.Label {
                    Layout.fillWidth: true
                    visible: root.headline !== ""

                    text: root.headline
                    font.bold: true
                    textFormat: Text.PlainText
                }

                QQC2.Label {
                    Layout.fillWidth: true
                    visible: root.text !== ""

                    text: root.text
                    wrapMode: Text.WordWrap
                    textFormat: Text.PlainText
                }
            }
        }
        Kirigami.FormLayout {
            Layout.fillWidth: true

            Kirigami.PasswordField {
                id: passwordField

                Kirigami.FormData.label: i18n("Password:")
                Layout.fillWidth: true

                maximumLength: root.maxLength

                enabled: root.passwordOption !== PasswordField.PasswordOption.AlwaysAsk && root.passwordOption !== PasswordField.PasswordOption.NotRequired

                placeholderText: {
                    switch (root.passwordOption) {
                    case PasswordField.PasswordOption.AlwaysAsk:
                        return i18n("Always asked");
                    case PasswordField.PasswordOption.NotRequired:
                        return i18n("Not required");
                    default:
                        return "";
                    }
                }
                text: root.password

                onTextEdited: root.password = text
            }

            QQC2.ComboBox {
                Layout.fillWidth: true
                visible: root.showPasswordOptions

                model: {
                    let items = [i18n("Store password for this user only (encrypted)"), i18n("Store password for all users (not encrypted)"), i18n("Ask for this password every time")];

                    if (root.showNotRequired) {
                        items.push(i18n("This password is not required"));
                    }

                    return items;
                }

                currentIndex: root.passwordOption

                onActivated: {
                    root.passwordOption = currentIndex;

                    if (currentIndex === PasswordField.PasswordOption.AlwaysAsk || currentIndex === PasswordField.PasswordOption.NotRequired) {
                        passwordField.clear();
                        root.password = "";
                    }
                }
            }
        }
    }
}
