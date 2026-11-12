/*
    SPDX-FileCopyrightText: 2016 Jan Grulich <jgrulich@redhat.com>
    SPDX-FileCopyrightText: 2026 Peder Bergan <pederbe@users.noreply.github.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

import org.kde.kirigami as Kirigami

Kirigami.PromptDialog {
    property string connectionName
    property string connectionPath

    standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel
    title: i18ndc("plasmanetworkmanagement-libs", "@title:window", "Forget Connection")
    subtitle: i18ndc("plasmanetworkmanagement-libs", "@info", "Forget the connection “%1”?",
        connectionName.replace(/&/g, "&amp;").replace(/</g, "&lt;").replace(/>/g, "&gt;"))
}
