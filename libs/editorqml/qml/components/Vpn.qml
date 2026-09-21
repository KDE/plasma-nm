import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts

import org.kde.plasma.networkmanagement.editorqml as PlasmaNMQ

ColumnLayout {
    id: root

    anchors.fill: parent

    readonly property string vpnServiceType: kcm.vpnServiceType

    readonly property bool supportsIPv6: root.vpnServiceType === "org.freedesktop.NetworkManager.openvpn"

    function showStatusTab(): void {
        tabBar.currentIndex = 0;
    }

    QQC2.TabBar {
        id: tabBar

        Layout.fillWidth: true

        QQC2.TabButton {
            text: i18n("Status")
        }

        QQC2.TabButton {
            text: i18n("General")
        }

        QQC2.TabButton {
            text: i18n("VPN")
        }

        QQC2.TabButton {
            text: i18n("IPv4")
        }

        QQC2.TabButton {
            text: i18n("IPv6")

            visible: root.supportsIPv6
            width: visible ? implicitWidth : 0
        }
    }

    StackLayout {
        Layout.fillWidth: true
        Layout.fillHeight: true

        currentIndex: tabBar.currentIndex

        Item {
            PlasmaNMQ.ConnectionStatusForm {
                anchors.fill: parent
                connectionStatus: kcm.connectionStatus
            }
        }

        Item {
            PlasmaNMQ.GeneralSettings {
                anchors.fill: parent
                setting: kcm.generalSettings
            }
        }

        Item {
            Loader {
                id: vpnPageLoader

                anchors.fill: parent

                readonly property var vpnPage: {
                    switch (root.vpnServiceType) {
                    case "org.freedesktop.NetworkManager.ssh":
                        return {
                            url: "../vpn/ssh/Ssh.qml",
                            setting: kcm.vpnSshSetting
                        };
                    case "org.freedesktop.NetworkManager.sstp":
                        return {
                            url: "../vpn/sstp/Sstp.qml",
                            setting: kcm.vpnSstpSetting
                        };
                    case "org.freedesktop.NetworkManager.vpnc":
                        return {
                            url: "../vpn/vpnc/Vpnc.qml",
                            setting: kcm.vpnVpncSetting
                        };
                    case "org.freedesktop.NetworkManager.fortisslvpn":
                        return {
                            url: "../vpn/fortisslvpn/Fortisslvpn.qml",
                            setting: kcm.vpnFortisslvpnSetting
                        };
                    case "org.freedesktop.NetworkManager.iodine":
                        return {
                            url: "../vpn/iodine/Iodine.qml",
                            setting: kcm.vpnIodineSetting
                        };
                    case "org.freedesktop.NetworkManager.libreswan":
                        return {
                            url: "../vpn/libreswan/Libreswan.qml",
                            setting: kcm.vpnLibreswanSetting
                        };
                    case "org.freedesktop.NetworkManager.strongswan":
                        return {
                            url: "../vpn/strongswan/Strongswan.qml",
                            setting: kcm.vpnStrongswanSetting
                        };
                    case "org.freedesktop.NetworkManager.l2tp":
                        return {
                            url: "../vpn/l2tp/L2tp.qml",
                            setting: kcm.vpnL2tpSetting
                        };
                    case "org.freedesktop.NetworkManager.pptp":
                        return {
                            url: "../vpn/pptp/Pptp.qml",
                            setting: kcm.vpnPptpSetting
                        };
                    case "org.freedesktop.NetworkManager.openvpn":
                        return {
                            url: "../vpn/openvpn/Openvpn.qml",
                            setting: kcm.vpnOpenvpnSetting
                        };
                    default:
                        return null;
                    }
                }

                function loadPage(): void {
                    if (!vpnPageLoader.vpnPage) {
                        vpnPageLoader.source = "";
                        return;
                    }

                    vpnPageLoader.setSource(vpnPageLoader.vpnPage.url, {
                        setting: vpnPageLoader.vpnPage.setting
                    });
                }

                onVpnPageChanged: vpnPageLoader.loadPage()
                Component.onCompleted: vpnPageLoader.loadPage()
            }
        }

        Item {
            PlasmaNMQ.IPv4Settings {
                anchors.fill: parent
                setting: kcm.ipv4Settings
            }
        }

        Loader {
            Layout.fillWidth: true
            Layout.fillHeight: true

            active: root.supportsIPv6

            sourceComponent: Component {
                PlasmaNMQ.IPv6Settings {
                    setting: kcm.ipv6Settings
                }
            }
        }
    }
}
