/*
    Copyright 2013-2017 Jan Grulich <jgrulich@redhat.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

import QtQuick 2.2
import org.kde.kcoreaddons 1.0 as KCoreAddons
import org.kde.kquickcontrolsaddons 2.0 as KQuickControlsAddons
import org.kde.plasma.components 2.0 as PlasmaComponents

Item {
    property QtObject dataEngine: null
    property string deviceName

    height: visible ? plotter.height + Math.round(units.gridUnit / 3) : 0

    Repeater {
        model: 5

        PlasmaComponents.Label {
            anchors {
                left: parent.left
                top: parent.top
                topMargin: Math.round(units.gridUnit / 3) + (index * plotter.height / 5)
            }
            height: paintedHeight
            font.pointSize: theme.smallestFont.pointSize
            text: KCoreAddons.Format.formatByteSize((plotter.maxValue * 1024) * (1 - index / 5))
        }
    }

    KQuickControlsAddons.Plotter {
        id: plotter

        // Joining two QList<foo> in QML/javascript doesn't seem to work so I'm getting maximum from both list separately
        readonly property int maxValue: Math.max(Math.max.apply(null, downloadPlotData.values), Math.max.apply(null, uploadPlotData.values))
        anchors {
            left: parent.left
            leftMargin: units.iconSizes.medium
            right: parent.right
            top: parent.top
            topMargin: Math.round(units.gridUnit / 2)
        }
        width: units.gridUnit * 20
        height: units.gridUnit * 8
        horizontalGridLineCount: 5

        dataSets: [
            KQuickControlsAddons.PlotData {
                id: downloadPlotData
                label: i18n("Download")
                color: theme.highlightColor
            },
            KQuickControlsAddons.PlotData {
                id: uploadPlotData
                label: i18n("Upload")
                color: cycle(theme.highlightColor, -180)
            }
        ]

        Connections {
            target: dataEngine;
            onNewData: {
                if (sourceName.indexOf("network/interfaces/" + deviceName) != 0) {
                    return;
                }
                var rx = dataEngine.data[dataEngine.downloadSource];
                var tx = dataEngine.data[dataEngine.uploadSource];
                if (rx === undefined || rx.value === undefined ||
                    tx === undefined || tx.value === undefined) {
                    return;
                }

                plotter.addSample([rx.value, tx.value]);
            }
        }
    }
}
