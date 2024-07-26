/*
    SPDX-FileCopyrightText: 2010 Marco Martin <notmart@gmail.com>
    SPDX-FileCopyrightText: 2016 Jan Grulich <jgrulich@redhat.com>
    SPDX-FileCopyrightText: 2020 George Vogiatzis <gvgeo@protonmail.com>
    SPDX-FileCopyrightText: 2024 ivan tkachenko <me@ratijas.tk>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

import QtQuick
import org.kde.kirigami as Kirigami
import org.kde.ksvg as KSvg

Kirigami.Padding {
    id: separatorLine

    width: {
        const view = ListView.view;
        return view ? view.width - view.leftMargin - view.rightMargin : undefined;
    }

    // Sections have spacing above but not below. Will use double spacing below.
    topPadding: Kirigami.Units.smallSpacing
    bottomPadding: Kirigami.Units.largeSpacing
    horizontalPadding: Kirigami.Units.gridUnit

    contentItem: KSvg.SvgItem {
        imagePath: "widgets/line"
        elementId: "horizontal-line"
    }
}
