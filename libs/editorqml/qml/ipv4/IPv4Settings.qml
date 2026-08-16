/*
      SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

      SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.plasma.networkmanagement.editorqml

ColumnLayout {
    id: root

    required property IPv4Setting setting

    spacing: Kirigami.Units.largeSpacing

    Kirigami.FormLayout {
        id: form

        Layout.fillWidth: true

        QQC2.ComboBox {
            Kirigami.FormData.label: i18n("Method:")
            Layout.fillWidth: true

            model: [i18nc("@item:inlistbox IPv4 method", "Automatic"), i18nc("@item:inlistbox IPv4 method", "Automatic (Only addresses)"), i18nc("@item:inlistbox IPv4 method", "Link-Local"), i18nc("@item:inlistbox like in use Manual configuration", "Manual"), i18nc("@item:inlistbox IPv4 method", "Shared to other computers"), i18nc("@item:inlistbox like in this setting is Disabled", "Disabled")]

            currentIndex: root.setting.method
            onActivated: root.setting.method = currentIndex

            hoverEnabled: true
            QQC2.ToolTip.visible: hovered
            QQC2.ToolTip.delay: Kirigami.Units.humanMoment
            QQC2.ToolTip.text: i18n("How to configure the IPv4 address:\n• Automatic: obtain address and DNS automatically via DHCP\n• Automatic (Only addresses): obtain address via DHCP but configure DNS manually\n• Link-Local: use automatic link-local addressing (169.254.x.x)\n• Manual: enter a static IP address\n• Shared: turn this computer into a router to share internet with other devices\n• Disabled: turn off IPv4")
        }

        RowLayout {
            Kirigami.FormData.label: root.setting.dnsLabel
            Layout.fillWidth: true
            spacing: Kirigami.Units.smallSpacing

            enabled: root.setting.dnsEnabled

            QQC2.TextField {
                id: dnsField

                Layout.fillWidth: true
                Layout.minimumWidth: Kirigami.Units.gridUnit * 14

                text: root.setting.dns
                onTextEdited: root.setting.dns = text

                hoverEnabled: true
                QQC2.ToolTip.visible: hovered
                QQC2.ToolTip.delay: Kirigami.Units.humanMoment
                QQC2.ToolTip.text: i18n("Use this field to specify the IP address(es) of one or more DNS servers. Use ',' to separate entries.")
            }

            QQC2.Button {
                icon.name: "document-properties"

                onClicked: {
                    dnsDialog.addresses = dnsField.text.length > 0 ? dnsField.text.split(",").map(s => s.trim()).filter(s => s.length > 0) : [];
                    dnsDialog.open();
                }

                hoverEnabled: true
                QQC2.ToolTip.visible: hovered
                QQC2.ToolTip.delay: Kirigami.Units.humanMoment
                QQC2.ToolTip.text: i18n("Edit the list of DNS servers")
            }
        }

        RowLayout {
            Kirigami.FormData.label: i18nc("@label:textbox", "Search Domains:")
            Layout.fillWidth: true
            spacing: Kirigami.Units.smallSpacing

            enabled: root.setting.dnsEnabled

            QQC2.TextField {
                id: dnsSearchField

                Layout.fillWidth: true
                Layout.minimumWidth: Kirigami.Units.gridUnit * 14

                text: root.setting.dnsSearch
                onTextEdited: root.setting.dnsSearch = text

                hoverEnabled: true
                QQC2.ToolTip.visible: hovered
                QQC2.ToolTip.delay: Kirigami.Units.humanMoment
                QQC2.ToolTip.text: i18n("Use this field to specify one or more DNS domains to search. Use ',' to separate entries.")
            }

            QQC2.Button {
                icon.name: "document-properties"

                onClicked: {
                    searchDomainDialog.addresses = dnsSearchField.text.length > 0 ? dnsSearchField.text.split(",").map(s => s.trim()).filter(s => s.length > 0) : [];
                    searchDomainDialog.open();
                }

                hoverEnabled: true
                QQC2.ToolTip.visible: hovered
                QQC2.ToolTip.delay: Kirigami.Units.humanMoment
                QQC2.ToolTip.text: i18n("Edit the list of DNS domains being searched")
            }
        }

        QQC2.TextField {
            Kirigami.FormData.label: i18n("DHCP Client ID:")
            Layout.fillWidth: true

            enabled: root.setting.dhcpClientIdEnabled

            text: root.setting.dhcpClientId
            onTextEdited: root.setting.dhcpClientId = text

            hoverEnabled: true
            QQC2.ToolTip.visible: hovered
            QQC2.ToolTip.delay: Kirigami.Units.humanMoment
            QQC2.ToolTip.text: i18n("Use this field to specify the DHCP client ID which is a string sent to the DHCP server to identify the local machine that the DHCP server may use to customize the DHCP lease and options.")
        }
        QQC2.SpinBox {
            Kirigami.FormData.label: i18n("Route Metric:")
            Layout.fillWidth: true

            from: -1
            to: 2147483647
            editable: true

            value: root.setting.routeMetric
            onValueModified: root.setting.routeMetric = value

            textFromValue: (value, locale) => Number(value).toLocaleString(locale, 'f', 0)

            valueFromText: (text, locale) => {
                const digits = text.replace(/[^0-9-]/g, "");
                const parsed = parseInt(digits, 10);

                if (isNaN(parsed))
                    return -1;

                return Math.max(-1, Math.min(parsed, 2147483647));
            }

            hoverEnabled: true

            QQC2.ToolTip.visible: hovered
            QQC2.ToolTip.delay: Kirigami.Units.humanMoment
            QQC2.ToolTip.text: i18n("A score that helps choose the best path for data packets to reach their destination, based on factors like distance, delay, and reliability.\n\nDefault value -1 means that NetworkManager gets to choose and manage the actual metric of the route.")
        }
    }

    Kirigami.Heading {
        Layout.fillWidth: true

        text: i18n("Addresses")
        level: 2
        font.weight: Font.Bold

        visible: root.setting.addressTableEnabled
    }

    RowLayout {
        Layout.fillWidth: true
        Layout.fillHeight: true
        spacing: Kirigami.Units.largeSpacing

        visible: root.setting.addressTableEnabled

        QQC2.Frame {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumHeight: Kirigami.Units.gridUnit * 7
            Layout.minimumWidth: Kirigami.Units.gridUnit * 20

            padding: 0

            GridLayout {
                anchors.fill: parent

                columns: 2
                rowSpacing: 0
                columnSpacing: 0

                Item {
                    Layout.preferredWidth: addressVerticalHeader.width
                    Layout.preferredHeight: addressHorizontalHeader.height
                }

                QQC2.HorizontalHeaderView {
                    id: addressHorizontalHeader

                    Layout.fillWidth: true

                    syncView: addressTable
                    clip: true
                }

                QQC2.VerticalHeaderView {
                    id: addressVerticalHeader

                    Layout.fillHeight: true

                    syncView: addressTable
                    clip: true
                }

                TableView {
                    id: addressTable

                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    clip: true
                    rowSpacing: 1
                    columnSpacing: 1

                    model: root.setting.addressModel

                    selectionModel: ItemSelectionModel {
                        id: addressSelection

                        model: addressTable.model
                    }

                    onWidthChanged: Qt.callLater(addressTable.forceLayout)

                    columnWidthProvider: function (column) {
                        if (column === RouteTableModel.MetricColumn)
                            return 0;

                        const available = addressTable.width - addressTable.columnSpacing * 2;
                        return Math.max(Kirigami.Units.gridUnit * 6, available / 3);
                    }

                    delegate: QQC2.TextField {
                        id: addressCell

                        required property int row
                        required property int column
                        required property var display

                        text: addressCell.display === undefined ? "" : addressCell.display

                        placeholderText: {
                            switch (addressCell.column) {
                            case RouteTableModel.AddressColumn:
                                return i18nc("@info:placeholder", "10.0.0.0");
                            case RouteTableModel.NetmaskColumn:
                                return i18nc("@info:placeholder", "255.255.255.0");
                            default:
                                return i18nc("@info:placeholder optional field", "Optional");
                            }
                        }

                        onActiveFocusChanged: if (activeFocus)
                            addressSelection.setCurrentIndex(addressTable.model.index(addressCell.row, addressCell.column), ItemSelectionModel.ClearAndSelect)

                        onEditingFinished: {
                            root.setting.addressModel.setRouteField(addressCell.row, addressCell.column, text);
                            if (addressCell.column === RouteTableModel.AddressColumn)
                                root.setting.suggestNetmaskForAddress(addressCell.row);
                        }
                    }
                }
            }
        }

        ColumnLayout {
            Layout.alignment: Qt.AlignTop
            spacing: Kirigami.Units.smallSpacing

            QQC2.Button {
                Layout.fillWidth: true
                text: i18n("Add")
                icon.name: "list-add"

                onClicked: root.setting.addressModel.addRoute()
            }

            QQC2.Button {
                Layout.fillWidth: true
                text: i18n("Remove")
                icon.name: "list-remove"

                enabled: addressSelection.hasSelection

                onClicked: root.setting.addressModel.removeRoute(addressSelection.currentIndex.row)
            }
        }
    }

    QQC2.CheckBox {
        Layout.fillWidth: true

        text: i18n("IPv4 is required for this connection")

        checked: root.setting.ipv4Required
        onToggled: root.setting.ipv4Required = checked

        hoverEnabled: true
        QQC2.ToolTip.visible: hovered
        QQC2.ToolTip.delay: Kirigami.Units.humanMoment
        QQC2.ToolTip.text: i18n("Allows the connection to complete if IPv4 configuration fails but IPv6 configuration succeeds")
    }

    Item {
        Layout.fillHeight: true
    }

    RowLayout {
        Layout.fillWidth: true
        spacing: Kirigami.Units.smallSpacing

        Item {
            Layout.fillWidth: true
        }

        QQC2.Button {
            text: i18nc("@action:button", "Advanced…")

            onClicked: advancedDialog.open()
        }

        QQC2.Button {
            text: i18nc("@action:button", "Routes…")

            enabled: root.setting.routesEnabled

            onClicked: routesDialog.open()
        }
    }

    IPv4Advance {
        id: advancedDialog

        setting: root.setting
    }

    IPv4Routes {
        id: routesDialog

        setting: root.setting
    }

    DnsList {
        id: dnsDialog

        onAccepted: root.setting.dns = addresses.join(",")
    }
    SearchDomainList {
        id: searchDomainDialog

        onAccepted: root.setting.dnsSearch = addresses.join(",")
    }
}
