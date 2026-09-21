/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Dialogs
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.plasma.networkmanagement.editorqml as PlasmaNMQ

Kirigami.Dialog {
    id: dialog

    required property var setting

    readonly property int certCheckDontVerify: 0
    readonly property int certCheckSubjectPartially: 4
    readonly property int tlsModeNone: 0
    readonly property int tlsModeAuth: 1
    readonly property int proxyNotRequired: 0
    readonly property int proxyHttp: 1

    parent: QQC2.Overlay.overlay

    title: i18nc("@title:window", "Advanced")

    standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel

    preferredWidth: Kirigami.Units.gridUnit * 34
    preferredHeight: Kirigami.Units.gridUnit * 30

    readonly property list<string> revertableProperties: ["useCustomPort", "customPort", "useCustomReneg", "customReneg", "useCompression", "compression", "useAsymCompression", "useTcp", "useVirtualDeviceType", "deviceType", "useVirtualDeviceName", "virtualDeviceName", "useMtu", "mtu", "useCustomFragmentSize", "customFragmentSize", "mssRestrict", "useMtuDisc", "mtuDisc", "randomizeRemoteHosts", "randomizeRemoteHostname", "connectTimeout", "allowPullFqdn", "pushPeerInfo", "ipv6TunLink", "usePingInterval", "pingInterval", "useExitRestartPing", "exitRestartPingMode", "exitRestartPing", "acceptAuthenticatedPackets", "useMaxRoutes", "maxRoutes", "cipher", "dataCiphers", "dataCiphersFallback", "disableCipherNegotiation", "useCustomCipherKey", "customCipherKey", "hmacAuth", "certCheckType", "subjectMatch", "verifyRemoteCertTls", "remoteCertTlsType", "verifyNsCertType", "nsCertType", "tlsMode", "tlsAuthKey", "tlsAuthDirection", "extraCerts", "tlsVersionMin", "tlsVersionMinOrHighest", "tlsVersionMax", "crlVerifyFile", "crlVerifyDir", "proxyType", "proxyServer", "proxyPort", "proxyRetry", "proxyUsername", "proxyPassword", "proxyPasswordOption"]

    property var previousState: null

    onOpened: {
        dialog.setting.probe();

        const state = {};
        for (const name of dialog.revertableProperties) {
            state[name] = dialog.setting[name];
        }
        dialog.previousState = state;
    }

    onRejected: {
        if (!dialog.previousState) {
            return;
        }

        for (const name of dialog.revertableProperties) {
            dialog.setting[name] = dialog.previousState[name];
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: Kirigami.Units.smallSpacing

        QQC2.TabBar {
            id: tabBar

            Layout.fillWidth: true

            QQC2.TabButton {
                text: i18n("General")
            }

            QQC2.TabButton {
                text: i18n("Security")
            }

            QQC2.TabButton {
                text: i18n("TLS Settings")
            }

            QQC2.TabButton {
                text: i18n("Proxies")
            }
        }

        StackLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true

            Layout.preferredHeight: Kirigami.Units.gridUnit * 24

            currentIndex: tabBar.currentIndex

            // General
            QQC2.ScrollView {
                id: generalView

                // Reporting no implicit height keeps this tab's contents from
                // deciding how tall the dialog is.
                implicitHeight: 0
                contentWidth: availableWidth

                Kirigami.FormLayout {
                    width: generalView.availableWidth

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: Kirigami.Units.largeSpacing

                        QQC2.CheckBox {
                            Layout.fillWidth: true

                            text: i18n("Use custom gateway port:")

                            checked: dialog.setting.useCustomPort
                            onToggled: dialog.setting.useCustomPort = checked
                        }

                        QQC2.SpinBox {
                            enabled: dialog.setting.useCustomPort

                            from: 1
                            to: 65535

                            value: dialog.setting.customPort
                            onValueModified: dialog.setting.customPort = value
                        }
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: Kirigami.Units.largeSpacing

                        QQC2.CheckBox {
                            Layout.fillWidth: true

                            text: i18n("Use custom renegotiation interval:")

                            checked: dialog.setting.useCustomReneg
                            onToggled: dialog.setting.useCustomReneg = checked
                        }

                        QQC2.SpinBox {
                            enabled: dialog.setting.useCustomReneg

                            from: 0
                            to: 2147483647

                            value: dialog.setting.customReneg
                            onValueModified: dialog.setting.customReneg = value
                        }
                    }

                    QQC2.CheckBox {
                        text: i18n("Use compression")

                        checked: dialog.setting.useCompression
                        onToggled: dialog.setting.useCompression = checked
                    }

                    QQC2.ComboBox {
                        Kirigami.FormData.label: i18n("Compression:")
                        Layout.fillWidth: true

                        enabled: dialog.setting.useCompression

                        model: [i18n("No"), i18n("LZO"), i18n("LZ4"), i18n("LZ4 v2"), i18n("Adaptive"), i18n("Automatic")]

                        currentIndex: dialog.setting.compression
                        onActivated: dialog.setting.compression = currentIndex
                    }

                    QQC2.CheckBox {
                        enabled: dialog.setting.useCompression

                        text: i18n("Use asymetric compression")

                        checked: dialog.setting.useAsymCompression
                        onToggled: dialog.setting.useAsymCompression = checked
                    }

                    QQC2.CheckBox {
                        text: i18n("Use a TCP connection")

                        checked: dialog.setting.useTcp
                        onToggled: dialog.setting.useTcp = checked
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: Kirigami.Units.largeSpacing

                        QQC2.CheckBox {
                            Layout.fillWidth: true

                            text: i18n("Set virtual device type:")

                            checked: dialog.setting.useVirtualDeviceType
                            onToggled: dialog.setting.useVirtualDeviceType = checked
                        }

                        QQC2.ComboBox {
                            enabled: dialog.setting.useVirtualDeviceType

                            model: [i18n("TUN"), i18n("TAP")]

                            currentIndex: dialog.setting.deviceType
                            onActivated: dialog.setting.deviceType = currentIndex
                        }
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: Kirigami.Units.largeSpacing

                        QQC2.CheckBox {
                            text: i18n("Set virtual device name:")

                            checked: dialog.setting.useVirtualDeviceName
                            onToggled: dialog.setting.useVirtualDeviceName = checked
                        }

                        QQC2.TextField {
                            Layout.fillWidth: true

                            enabled: dialog.setting.useVirtualDeviceName

                            text: dialog.setting.virtualDeviceName
                            onTextEdited: dialog.setting.virtualDeviceName = text
                        }
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: Kirigami.Units.largeSpacing

                        QQC2.CheckBox {
                            Layout.fillWidth: true

                            text: i18n("Use custom tunnel Maximum Transmission Unit (MTU):")

                            checked: dialog.setting.useMtu
                            onToggled: dialog.setting.useMtu = checked
                        }

                        QQC2.SpinBox {
                            enabled: dialog.setting.useMtu

                            from: 1
                            to: 65535

                            value: dialog.setting.mtu
                            onValueModified: dialog.setting.mtu = value
                        }
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: Kirigami.Units.largeSpacing

                        QQC2.CheckBox {
                            Layout.fillWidth: true

                            text: i18n("Use custom UDP fragment size:")

                            checked: dialog.setting.useCustomFragmentSize
                            onToggled: dialog.setting.useCustomFragmentSize = checked
                        }

                        QQC2.SpinBox {
                            enabled: dialog.setting.useCustomFragmentSize

                            from: 0
                            to: 65535

                            value: dialog.setting.customFragmentSize
                            onValueModified: dialog.setting.customFragmentSize = value
                        }
                    }

                    QQC2.CheckBox {
                        text: i18n("Restrict TCP maximum segment size (MSS)")

                        checked: dialog.setting.mssRestrict
                        onToggled: dialog.setting.mssRestrict = checked
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: Kirigami.Units.largeSpacing

                        QQC2.CheckBox {
                            Layout.fillWidth: true

                            text: i18n("Specify Path MTU discovery:")

                            checked: dialog.setting.useMtuDisc
                            onToggled: dialog.setting.useMtuDisc = checked
                        }

                        QQC2.ComboBox {
                            enabled: dialog.setting.useMtuDisc

                            model: [i18n("No"), i18n("Maybe"), i18n("Yes")]

                            currentIndex: dialog.setting.mtuDisc
                            onActivated: dialog.setting.mtuDisc = currentIndex
                        }
                    }

                    QQC2.CheckBox {
                        text: i18n("Randomize remote hosts")

                        checked: dialog.setting.randomizeRemoteHosts
                        onToggled: dialog.setting.randomizeRemoteHosts = checked
                    }

                    QQC2.CheckBox {
                        text: i18n("Prefix remote DNS name with random string")

                        checked: dialog.setting.randomizeRemoteHostname
                        onToggled: dialog.setting.randomizeRemoteHostname = checked
                    }

                    QQC2.SpinBox {
                        Kirigami.FormData.label: i18n("Connect timeout:")

                        from: 0
                        to: 1000000

                        textFromValue: (value, locale) => value === 0 ? i18n("None") : i18n("%1 s", value)
                        valueFromText: (text, locale) => text === i18n("None") ? 0 : Number.fromLocaleString(locale, text.replace(/[^0-9]/g, ""))

                        value: dialog.setting.connectTimeout
                        onValueModified: dialog.setting.connectTimeout = value
                    }

                    QQC2.CheckBox {
                        text: i18n("Allow pull of peer DNS names (FQDN)")

                        checked: dialog.setting.allowPullFqdn
                        onToggled: dialog.setting.allowPullFqdn = checked
                    }

                    QQC2.CheckBox {
                        text: i18n("push peer info")

                        checked: dialog.setting.pushPeerInfo
                        onToggled: dialog.setting.pushPeerInfo = checked
                    }

                    QQC2.CheckBox {
                        text: i18n("IPv6 tun link")

                        checked: dialog.setting.ipv6TunLink
                        onToggled: dialog.setting.ipv6TunLink = checked
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: Kirigami.Units.largeSpacing

                        QQC2.CheckBox {
                            Layout.fillWidth: true

                            text: i18n("Specify ping interval:")

                            checked: dialog.setting.usePingInterval
                            onToggled: dialog.setting.usePingInterval = checked
                        }

                        QQC2.SpinBox {
                            enabled: dialog.setting.usePingInterval

                            from: 1
                            to: 65535

                            value: dialog.setting.pingInterval
                            onValueModified: dialog.setting.pingInterval = value
                        }
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: Kirigami.Units.largeSpacing

                        QQC2.CheckBox {
                            Layout.fillWidth: true

                            text: i18n("Specify exit or restart ping:")

                            checked: dialog.setting.useExitRestartPing
                            onToggled: dialog.setting.useExitRestartPing = checked
                        }

                        QQC2.ComboBox {
                            enabled: dialog.setting.useExitRestartPing

                            model: [i18n("ping-exit"), i18n("ping-restart")]

                            currentIndex: dialog.setting.exitRestartPingMode
                            onActivated: dialog.setting.exitRestartPingMode = currentIndex
                        }

                        QQC2.SpinBox {
                            enabled: dialog.setting.useExitRestartPing

                            from: 1
                            to: 65535

                            value: dialog.setting.exitRestartPing
                            onValueModified: dialog.setting.exitRestartPing = value
                        }
                    }

                    QQC2.CheckBox {
                        text: i18n("Accept authenticated packets from any address (Float)")

                        checked: dialog.setting.acceptAuthenticatedPackets
                        onToggled: dialog.setting.acceptAuthenticatedPackets = checked
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: Kirigami.Units.largeSpacing

                        QQC2.CheckBox {
                            Layout.fillWidth: true

                            text: i18n("Specify max routes:")

                            checked: dialog.setting.useMaxRoutes
                            onToggled: dialog.setting.useMaxRoutes = checked
                        }

                        QQC2.SpinBox {
                            enabled: dialog.setting.useMaxRoutes

                            from: 0
                            to: 100000000

                            value: dialog.setting.maxRoutes
                            onValueModified: dialog.setting.maxRoutes = value
                        }
                    }
                }
            }

            // Security
            QQC2.ScrollView {
                id: securityView

                // Reporting no implicit height keeps this tab's contents from
                // deciding how tall the dialog is.
                implicitHeight: 0
                contentWidth: availableWidth

                Kirigami.FormLayout {
                    width: securityView.availableWidth

                    OpenvpnCipherField {
                        Kirigami.FormData.label: i18n("Cipher:")
                        Layout.fillWidth: true

                        ciphers: dialog.setting.availableCiphers

                        cipher: dialog.setting.cipher
                        onCipherEdited: cipher => dialog.setting.cipher = cipher
                    }

                    OpenvpnCipherField {
                        Kirigami.FormData.label: i18n("Data Ciphers:")
                        Layout.fillWidth: true

                        ciphers: dialog.setting.availableCiphers

                        cipher: dialog.setting.dataCiphers
                        onCipherEdited: cipher => dialog.setting.dataCiphers = cipher
                    }

                    OpenvpnCipherField {
                        Kirigami.FormData.label: i18n("Data Ciphers Fallback:")
                        Layout.fillWidth: true

                        ciphers: dialog.setting.availableCiphers

                        cipher: dialog.setting.dataCiphersFallback
                        onCipherEdited: cipher => dialog.setting.dataCiphersFallback = cipher
                    }

                    QQC2.CheckBox {
                        text: i18n("Disable cipher negotiation")

                        checked: dialog.setting.disableCipherNegotiation
                        onToggled: dialog.setting.disableCipherNegotiation = checked
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: Kirigami.Units.largeSpacing

                        QQC2.CheckBox {
                            Layout.fillWidth: true

                            text: i18n("Use custom size of cipher key:")

                            checked: dialog.setting.useCustomCipherKey
                            onToggled: dialog.setting.useCustomCipherKey = checked
                        }

                        QQC2.SpinBox {
                            enabled: dialog.setting.useCustomCipherKey

                            from: 1
                            to: 65535

                            value: dialog.setting.customCipherKey
                            onValueModified: dialog.setting.customCipherKey = value
                        }
                    }

                    QQC2.ComboBox {
                        Kirigami.FormData.label: i18n("HMAC Authentication:")
                        Layout.fillWidth: true

                        model: [i18n("Default"), i18n("None"), i18n("MD-4"), i18n("MD-5"), i18n("SHA-1"), i18n("SHA-224"), i18n("SHA-256"), i18n("SHA-384"), i18n("SHA-512"), i18n("RIPEMD-160")]

                        currentIndex: dialog.setting.hmacAuth
                        onActivated: dialog.setting.hmacAuth = currentIndex
                    }
                }
            }

            // TLS Settings
            QQC2.ScrollView {
                id: tlsView

                implicitHeight: 0
                contentWidth: availableWidth

                Kirigami.FormLayout {
                    width: tlsView.availableWidth

                    QQC2.ComboBox {
                        Kirigami.FormData.label: i18n("Server Certificate Check:")
                        Layout.fillWidth: true

                        model: {
                            const items = [i18n("Don't verify certificate identification"), i18n("Verify whole subject exactly"), i18n("Verify name exactly"), i18n("Verify name by prefix")];

                            if (dialog.setting.supportsLegacySubjectMatch) {
                                items.push(i18n("Verify subject partially (legacy mode, strongly discouraged)"));
                            }

                            return items;
                        }

                        currentIndex: dialog.setting.certCheckType
                        onActivated: dialog.setting.certCheckType = currentIndex
                    }

                    QQC2.TextField {
                        Kirigami.FormData.label: i18n("Subject Match:")
                        Layout.fillWidth: true

                        enabled: dialog.setting.certCheckType !== dialog.certCheckDontVerify

                        QQC2.ToolTip.text: i18n("Connect only to servers whose certificate matches the given subject.")
                        QQC2.ToolTip.visible: hovered
                        QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

                        text: dialog.setting.subjectMatch
                        onTextEdited: dialog.setting.subjectMatch = text
                    }

                    QQC2.CheckBox {
                        text: i18n("Verify peer (server) certificate usage signature")

                        checked: dialog.setting.verifyRemoteCertTls
                        onToggled: dialog.setting.verifyRemoteCertTls = checked
                    }

                    QQC2.ComboBox {
                        Kirigami.FormData.label: i18n("Remote peer certificate TLS type:")
                        Layout.fillWidth: true

                        enabled: dialog.setting.verifyRemoteCertTls

                        model: [i18n("Server"), i18n("Client")]

                        currentIndex: dialog.setting.remoteCertTlsType
                        onActivated: dialog.setting.remoteCertTlsType = currentIndex
                    }

                    QQC2.CheckBox {
                        text: i18n("Verify peer (server) certificate nsCertType designation")

                        checked: dialog.setting.verifyNsCertType
                        onToggled: dialog.setting.verifyNsCertType = checked
                    }

                    QQC2.ComboBox {
                        Kirigami.FormData.label: i18n("Remote peer certificate nsCert designation:")
                        Layout.fillWidth: true

                        enabled: dialog.setting.verifyNsCertType

                        model: [i18n("Server"), i18n("Client")]

                        currentIndex: dialog.setting.nsCertType
                        onActivated: dialog.setting.nsCertType = currentIndex
                    }

                    QQC2.ComboBox {
                        Kirigami.FormData.label: i18n("Mode:")
                        Layout.fillWidth: true

                        model: [i18n("None"), i18n("TLS-Auth"), i18n("TLS-Crypt"), i18n("TLS-Crypt v2")]

                        currentIndex: dialog.setting.tlsMode
                        onActivated: dialog.setting.tlsMode = currentIndex
                    }

                    RowLayout {
                        Kirigami.FormData.label: i18n("Key File:")
                        Layout.fillWidth: true
                        spacing: Kirigami.Units.smallSpacing

                        enabled: dialog.setting.tlsMode !== dialog.tlsModeNone

                        QQC2.TextField {
                            Layout.fillWidth: true

                            text: dialog.setting.tlsAuthKey
                            onTextEdited: dialog.setting.tlsAuthKey = text
                        }

                        QQC2.Button {
                            icon.name: "document-open"
                            onClicked: tlsAuthKeyDialog.open()
                        }
                    }

                    QQC2.ComboBox {
                        Kirigami.FormData.label: i18n("Key Direction:")
                        Layout.fillWidth: true

                        // Only TLS-Auth takes a direction.
                        enabled: dialog.setting.tlsMode === dialog.tlsModeAuth

                        model: [i18n("None"), i18n("Server (0)"), i18n("Client (1)")]

                        currentIndex: dialog.setting.tlsAuthDirection
                        onActivated: dialog.setting.tlsAuthDirection = currentIndex
                    }

                    RowLayout {
                        Kirigami.FormData.label: i18n("Extra certificates file:")
                        Layout.fillWidth: true
                        spacing: Kirigami.Units.smallSpacing

                        QQC2.TextField {
                            Layout.fillWidth: true

                            text: dialog.setting.extraCerts
                            onTextEdited: dialog.setting.extraCerts = text
                        }

                        QQC2.Button {
                            icon.name: "document-open"
                            onClicked: extraCertsDialog.open()
                        }
                    }

                    RowLayout {
                        Kirigami.FormData.label: i18n("TLS version minimum:")
                        Layout.fillWidth: true
                        spacing: Kirigami.Units.largeSpacing

                        QQC2.TextField {
                            Layout.fillWidth: true

                            text: dialog.setting.tlsVersionMin
                            onTextEdited: dialog.setting.tlsVersionMin = text
                        }

                        QQC2.CheckBox {
                            text: i18n("or highest")

                            checked: dialog.setting.tlsVersionMinOrHighest
                            onToggled: dialog.setting.tlsVersionMinOrHighest = checked
                        }
                    }

                    QQC2.TextField {
                        Kirigami.FormData.label: i18n("TLS version maximum:")
                        Layout.fillWidth: true

                        text: dialog.setting.tlsVersionMax
                        onTextEdited: dialog.setting.tlsVersionMax = text
                    }

                    RowLayout {
                        Kirigami.FormData.label: i18n("CRL verify file:")
                        Layout.fillWidth: true
                        spacing: Kirigami.Units.smallSpacing

                        QQC2.TextField {
                            Layout.fillWidth: true

                            text: dialog.setting.crlVerifyFile
                            onTextEdited: dialog.setting.crlVerifyFile = text
                        }

                        QQC2.Button {
                            icon.name: "document-open"
                            onClicked: crlVerifyFileDialog.open()
                        }
                    }

                    RowLayout {
                        Kirigami.FormData.label: i18n("CRL verify directory:")
                        Layout.fillWidth: true
                        spacing: Kirigami.Units.smallSpacing

                        QQC2.TextField {
                            Layout.fillWidth: true

                            text: dialog.setting.crlVerifyDir
                            onTextEdited: dialog.setting.crlVerifyDir = text
                        }

                        QQC2.Button {
                            icon.name: "folder-open"
                            onClicked: crlVerifyDirDialog.open()
                        }
                    }
                }
            }

            // Proxies
            QQC2.ScrollView {
                id: proxyView

                implicitHeight: 0
                contentWidth: availableWidth

                ColumnLayout {
                    width: proxyView.availableWidth
                    spacing: Kirigami.Units.largeSpacing

                    Kirigami.FormLayout {
                        Layout.fillWidth: true

                        QQC2.ComboBox {
                            Kirigami.FormData.label: i18n("Proxy Type:")
                            Layout.fillWidth: true

                            model: [i18n("Not Required"), i18n("HTTP"), i18n("SOCKS")]

                            currentIndex: dialog.setting.proxyType
                            onActivated: dialog.setting.proxyType = currentIndex
                        }

                        QQC2.Label {
                            Layout.fillWidth: true

                            text: i18n("Select this option if your organization requires the use of a proxy server to access the Internet.")
                            wrapMode: Text.WordWrap
                        }

                        QQC2.TextField {
                            Kirigami.FormData.label: i18n("Server Address:")
                            Layout.fillWidth: true

                            enabled: dialog.setting.proxyType !== dialog.proxyNotRequired

                            text: dialog.setting.proxyServer
                            onTextEdited: dialog.setting.proxyServer = text
                        }

                        QQC2.SpinBox {
                            Kirigami.FormData.label: i18n("Port:")

                            enabled: dialog.setting.proxyType !== dialog.proxyNotRequired

                            from: 0
                            to: 65535

                            value: dialog.setting.proxyPort
                            onValueModified: dialog.setting.proxyPort = value
                        }

                        QQC2.CheckBox {
                            enabled: dialog.setting.proxyType !== dialog.proxyNotRequired

                            text: i18n("Retry indefinitely when errors occur")

                            checked: dialog.setting.proxyRetry
                            onToggled: dialog.setting.proxyRetry = checked
                        }

                        QQC2.TextField {
                            Kirigami.FormData.label: i18n("Proxy Username:")
                            Layout.fillWidth: true

                            // Only HTTP proxies take credentials.
                            enabled: dialog.setting.proxyType === dialog.proxyHttp

                            text: dialog.setting.proxyUsername
                            onTextEdited: dialog.setting.proxyUsername = text
                        }
                    }

                    PlasmaNMQ.PasswordField {
                        Layout.fillWidth: true

                        enabled: dialog.setting.proxyType === dialog.proxyHttp

                        passwordLabel: i18n("Proxy Password:")
                        showPasswordOptions: true
                        showNotRequired: true

                        password: dialog.setting.proxyPassword
                        passwordOption: dialog.setting.proxyPasswordOption

                        onPasswordEdited: password => dialog.setting.proxyPassword = password
                        onPasswordOptionEdited: option => dialog.setting.proxyPasswordOption = option
                    }

                    Item {
                        Layout.fillHeight: true
                    }
                }
            }
        }
    }

    FileDialog {
        id: tlsAuthKeyDialog

        onAccepted: dialog.setting.tlsAuthKey = selectedFile
    }

    FileDialog {
        id: extraCertsDialog

        onAccepted: dialog.setting.extraCerts = selectedFile
    }

    FileDialog {
        id: crlVerifyFileDialog

        onAccepted: dialog.setting.crlVerifyFile = selectedFile
    }

    FolderDialog {
        id: crlVerifyDirDialog

        onAccepted: dialog.setting.crlVerifyDir = selectedFolder
    }
}
