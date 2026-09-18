/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami

RowLayout {
    id: root

    property int seconds: 0

    signal secondsEdited(int seconds)

    readonly property int hoursPart: Math.floor(root.seconds / 3600)
    readonly property int minutesPart: Math.floor((root.seconds % 3600) / 60)
    readonly property int secondsPart: root.seconds % 60

    function edited(hours: int, minutes: int, secs: int): void {
        root.secondsEdited(hours * 3600 + minutes * 60 + secs);
    }

    spacing: Kirigami.Units.smallSpacing

    QQC2.SpinBox {
        from: 0
        to: 23

        textFromValue: (value, locale) => String(value).padStart(2, "0")

        value: root.hoursPart
        onValueModified: root.edited(value, root.minutesPart, root.secondsPart)
    }

    QQC2.Label {
        text: i18nc("separator between hours and minutes", ":")
    }

    QQC2.SpinBox {
        from: 0
        to: 59

        textFromValue: (value, locale) => String(value).padStart(2, "0")

        value: root.minutesPart
        onValueModified: root.edited(root.hoursPart, value, root.secondsPart)
    }

    QQC2.Label {
        text: i18nc("separator between minutes and seconds", ":")
    }

    QQC2.SpinBox {
        from: 0
        to: 59

        textFromValue: (value, locale) => String(value).padStart(2, "0")

        value: root.secondsPart
        onValueModified: root.edited(root.hoursPart, root.minutesPart, value)
    }

    Item {
        Layout.fillWidth: true
    }
}
