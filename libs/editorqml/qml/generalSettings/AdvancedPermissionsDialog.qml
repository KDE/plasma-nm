/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.plasma.networkmanagement.editorqml

QQC2.Dialog {
    id: dialog

    modal: true

    title: i18n("Allowed Users")

    standardButtons: QQC2.Dialog.Ok | QQC2.Dialog.Cancel

    implicitWidth: Kirigami.Units.gridUnit * 20
    implicitHeight: Kirigami.Units.gridUnit * 15

    property alias permissions: permissionsModel.permissions

    AdvancedPermissionsModel {
        id: permissionsModel
    }

    contentItem: ColumnLayout {
        spacing: Kirigami.Units.smallSpacing

        QQC2.Frame {
            Layout.fillWidth: true
            Layout.fillHeight: true
            padding: 0

            ListView {
                anchors.fill: parent
                clip: true

                model: permissionsModel.users

                delegate: QQC2.ItemDelegate {
                    width: ListView.view.width

                    contentItem: RowLayout {
                        spacing: Kirigami.Units.smallSpacing

                        QQC2.CheckBox {
                            enabled: true
                            checked: modelData.allowed

                            onToggled: {
                                if (checked) {
                                    permissionsModel.allow(modelData.loginName);
                                } else {
                                    permissionsModel.disallow(modelData.loginName);
                                }
                            }
                        }

                        QQC2.Label {
                            text: modelData.fullName
                            font.bold: modelData.isCurrentUser
                            Layout.preferredWidth: 160
                            elide: Text.ElideRight
                        }

                        QQC2.Label {
                            text: modelData.loginName
                            opacity: 0.7
                            Layout.fillWidth: true
                            elide: Text.ElideRight
                        }
                    }
                }
            }
        }
    }
}
