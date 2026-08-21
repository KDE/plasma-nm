/*
        SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

        SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.plasma.networkmanagement.editorqml

Kirigami.Dialog {
    id: dialog

    required property IPv6Setting setting

    title: i18n("Edit IPv6 Routes")
    standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel

    preferredWidth: Kirigami.Units.gridUnit * 46
    preferredHeight: Kirigami.Units.gridUnit * 24

    property var previousState: null

    onOpened: previousState = {
        routes: dialog.setting.routes,
        ignoreAutoRoutes: dialog.setting.ignoreAutoRoutes,
        neverDefault: dialog.setting.neverDefault
    }

    onRejected: {
        if (!previousState)
            return;
        dialog.setting.routes = previousState.routes;
        dialog.setting.ignoreAutoRoutes = previousState.ignoreAutoRoutes;
        dialog.setting.neverDefault = previousState.neverDefault;
    }

    ColumnLayout {
        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.margins: Kirigami.Units.largeSpacing

        spacing: Kirigami.Units.smallSpacing

        QQC2.Frame {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumHeight: Kirigami.Units.gridUnit * 12

            padding: 0

            GridLayout {
                anchors.fill: parent

                columns: 2
                rowSpacing: 0
                columnSpacing: 0

                Item {
                    Layout.preferredWidth: verticalHeader.width
                    Layout.preferredHeight: horizontalHeader.height
                }

                QQC2.HorizontalHeaderView {
                    id: horizontalHeader

                    Layout.fillWidth: true

                    syncView: routeTable
                    clip: true
                }

                QQC2.VerticalHeaderView {
                    id: verticalHeader

                    Layout.fillHeight: true

                    syncView: routeTable
                    clip: true
                }

                TableView {
                    id: routeTable

                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    clip: true
                    rowSpacing: 1
                    columnSpacing: 1

                    model: dialog.setting.routeModel

                    selectionModel: ItemSelectionModel {
                        id: routeSelection

                        model: routeTable.model
                    }

                    onWidthChanged: Qt.callLater(routeTable.forceLayout)

                    columnWidthProvider: function (column) {
                        const prefix = Kirigami.Units.gridUnit * 5;
                        const metric = Kirigami.Units.gridUnit * 6;
                        if (column === Ipv6RouteTableModel.PrefixColumn)
                            return prefix;
                        if (column === Ipv6RouteTableModel.MetricColumn)
                            return metric;

                        // Address and next hop share whatever is left.
                        const available = routeTable.width - prefix - metric - routeTable.columnSpacing * 3;
                        return Math.max(Kirigami.Units.gridUnit * 8, available / 2);
                    }

                    delegate: QQC2.TextField {
                        id: cell

                        required property int row
                        required property int column
                        required property var display

                        text: cell.display === undefined ? "" : cell.display

                        placeholderText: {
                            switch (cell.column) {
                            case Ipv6RouteTableModel.AddressColumn:
                                return i18nc("@info:placeholder", "2001:db8::");
                            case Ipv6RouteTableModel.PrefixColumn:
                                return i18nc("@info:placeholder IPv6 prefix length", "64");
                            case Ipv6RouteTableModel.NextHopColumn:
                                return i18nc("@info:placeholder optional field", "Optional");
                            default:
                                return "";
                            }
                        }

                        validator: {
                            if (cell.column === Ipv6RouteTableModel.PrefixColumn)
                                return prefixValidator;
                            if (cell.column === Ipv6RouteTableModel.MetricColumn)
                                return metricValidator;
                            return null;
                        }

                        onActiveFocusChanged: if (activeFocus)
                            routeSelection.setCurrentIndex(routeTable.model.index(cell.row, cell.column), ItemSelectionModel.ClearAndSelect)

                        onEditingFinished: {
                            if (cell.column === Ipv6RouteTableModel.PrefixColumn) {
                                const parsedPrefix = parseInt(text, 10);
                                const prefix = isNaN(parsedPrefix) ? 0 : Math.min(parsedPrefix, 128);
                                dialog.setting.routeModel.setRouteField(cell.row, cell.column, prefix);
                                text = Number(prefix).toFixed(0);
                            } else if (cell.column === Ipv6RouteTableModel.MetricColumn) {
                                const parsed = parseInt(text, 10);
                                const metric = isNaN(parsed) ? 0 : Math.min(parsed, 4294967295);
                                dialog.setting.routeModel.setRouteField(cell.row, cell.column, metric);
                                text = Number(metric).toFixed(0);
                            } else {
                                dialog.setting.routeModel.setRouteField(cell.row, cell.column, text);
                            }
                        }
                    }
                }
            }
        }

        RegularExpressionValidator {
            id: prefixValidator

            regularExpression: /^(1[01][0-9]|12[0-8]|[1-9][0-9]|[0-9])$/
        }

        RegularExpressionValidator {
            id: metricValidator

            regularExpression: /^\d{1,10}$/
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: Kirigami.Units.smallSpacing

            Item {
                Layout.fillWidth: true
            }

            QQC2.Button {
                text: i18nc("@action:button Insert a row", "Add")
                icon.name: "list-add"

                onClicked: dialog.setting.routeModel.addRoute()
            }

            QQC2.Button {
                text: i18nc("@action:button Remove a selected row", "Remove")
                icon.name: "list-remove"

                enabled: routeSelection.hasSelection

                onClicked: dialog.setting.routeModel.removeRoute(routeSelection.currentIndex.row)
            }

            Item {
                Layout.fillWidth: true
            }
        }

        QQC2.CheckBox {
            Layout.fillWidth: true
            Layout.topMargin: Kirigami.Units.largeSpacing

            text: i18n("Ignore automatically obtained routes")

            checked: dialog.setting.ignoreAutoRoutes
            onToggled: dialog.setting.ignoreAutoRoutes = checked

            hoverEnabled: true
            QQC2.ToolTip.visible: hovered
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
            QQC2.ToolTip.text: i18n("If enabled, automatically configured routes are ignored and only routes specified above are used")
        }

        QQC2.CheckBox {
            Layout.fillWidth: true

            text: i18n("Use only for resources on this connection")

            checked: dialog.setting.neverDefault
            onToggled: dialog.setting.neverDefault = checked

            hoverEnabled: true
            QQC2.ToolTip.visible: hovered
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
            QQC2.ToolTip.text: i18n("If enabled, this connection will never be used as the default network connection")
        }
    }
}
