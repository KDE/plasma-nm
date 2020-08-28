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

import QtQuick 2.4
import org.kde.kcoreaddons 1.0 as KCoreAddons
import org.kde.quickcharts 1.0 as QuickCharts
import org.kde.kirigami 2.12 as Kirigami
import org.kde.plasma.components 3.0 as PlasmaComponents3
import org.kde.plasma.core 2.0 as PlasmaCore

Item {
    property real rxBytes: 0
    property real txBytes: 0
    property alias interval: timer.interval

    height: visible ? plotter.height + plotter.anchors.topMargin + units.smallSpacing : 0

    QuickCharts.AxisLabels {
        anchors {
            right: plotter.left
            rightMargin: units.smallSpacing
            top: plotter.top
            bottom: plotter.bottom
        }
        constrainToBounds: false
        direction: QuickCharts.AxisLabels.VerticalBottomTop
        delegate:  PlasmaComponents3.Label { 
            text: KCoreAddons.Format.formatByteSize(QuickCharts.AxisLabels.label) + i18n("/s")
            font.pointSize: PlasmaCore.Theme.smallestFont.pointSize
        }
        source: QuickCharts.ChartAxisSource { 
            chart: plotter
            axis: QuickCharts.ChartAxisSource.YAxis
            itemCount: 5
        }
    }

    QuickCharts.GridLines {
        anchors.fill: plotter
        direction: QuickCharts.GridLines.Vertical
        minor.visible: false
        major.count: 3
        major.lineWidth: 1
        // Same calculation as Kirigami Separator
        major.color: Kirigami.ColorUtils.linearInterpolation(PlasmaCore.Theme.backgroundColor, PlasmaCore.Theme.textColor, 0.2)
    }
    QuickCharts.LineChart {
        id: plotter
        anchors {
            left: parent.left
            leftMargin: speedMetrics.width + units.smallSpacing * 2
            right: parent.right
            top: parent.top
            // Align plotter lines with labels.
            topMargin: speedMetrics.height / 2 + units.smallSpacing
        }
        height: units.gridUnit * 8
        smooth: true
        direction: QuickCharts.XYChart.ZeroAtEnd
        yRange {
            minimum: 100 * 1024
            increment: 100 * 1024
        }
        valueSources: [
            QuickCharts.ValueHistorySource {
                id: txSpeed
                maximumHistory: 40
            },
            QuickCharts.ValueHistorySource {
                id: rxSpeed
                maximumHistory: 40
            }
        ]
        colorSource: QuickCharts.ArraySource {
            array: colors.colors.reverse()
        }

        fillColorSource: QuickCharts.ArraySource  {
           array: colors.colors.reverse().map(color => Qt.lighter(color, 1.5))
        }

        QuickCharts.ColorGradientSource {
                id: colors
                baseColor:  PlasmaCore.Theme.highlightColor
                itemCount: 2
        }

        Timer {
            id: timer
            repeat: true
            running: parent.visible
            property real prevRxBytes
            property real prevTxBytes
            Component.onCompleted: {
                prevRxBytes = rxBytes
                prevTxBytes = txBytes
            }
            onTriggered: {
                rxSpeed.value = (rxBytes - prevRxBytes) * 1000 / interval
                txSpeed.value = (txBytes - prevTxBytes) * 1000 / interval
                prevRxBytes = rxBytes
                prevTxBytes = txBytes
            }
        }
    }

    TextMetrics {
        id: speedMetrics
        font.pointSize: theme.smallestFont.pointSize
        // Measure 888.8 KiB/s
        text: KCoreAddons.Format.formatByteSize(910131) + i18n("/s")
    }
}
