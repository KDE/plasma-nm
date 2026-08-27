import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts

import org.kde.plasma.networkmanagement.editorqml as PlasmaNMQ

ColumnLayout {
    id: root

    anchors.fill: parent

    readonly property string vpnServiceType: kcm.vpnServiceType

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

        Repeater {
            model: root.vpnServiceType === "org.freedesktop.NetworkManager.openvpn" ? 1 : 0

            QQC2.TabButton {
                text: i18n("IPv6")
            }
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

        Repeater {
            model: root.vpnServiceType === "org.freedesktop.NetworkManager.openvpn" ? 1 : 0

            Item {
                PlasmaNMQ.IPv6Settings {
                    anchors.fill: parent
                    setting: kcm.ipv6Settings
                }
            }
        }
    }
}
