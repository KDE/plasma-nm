/*
    SPDX-FileCopyrightText: 2013-2017 Jan Grulich <jgrulich@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

import QtQuick 2.4
import QtQuick.Layouts 1.4
import org.kde.coreaddons 1.0 as KCoreAddons
import org.kde.quickcharts 1.0 as QuickCharts
import org.kde.quickcharts.controls 1.0 as QuickChartControls
import org.kde.plasma.components 3.0 as PlasmaComponents3
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.kirigami 2.20 as Kirigami

ColumnLayout {
    property alias downloadSpeed: download.value
    property alias uploadSpeed: upload.value

    spacing: Kirigami.Units.largeSpacing

    Item {
        Layout.fillWidth: true
        implicitHeight: plotter.height + speedMetrics.height

        QuickCharts.AxisLabels {
            anchors {
                right: plotter.left
                rightMargin: Kirigami.Units.smallSpacing
                top: plotter.top
                bottom: plotter.bottom
            }
            constrainToBounds: false
            direction: QuickCharts.AxisLabels.VerticalBottomTop
            delegate:  PlasmaComponents3.Label {
                text: KCoreAddons.Format.formatByteSize(QuickCharts.AxisLabels.label) + i18n("/s")
                font: Kirigami.Theme.smallFont
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
            major.color: Kirigami.ColorUtils.linearInterpolation(Kirigami.Theme.backgroundColor, Kirigami.Theme.textColor, 0.4)
        }
        QuickCharts.LineChart {
            id: plotter
            anchors {
                left: parent.left
                leftMargin: speedMetrics.width + Kirigami.Units.smallSpacing
                right: parent.right
                top: parent.top
                // Align plotter lines with labels.
                topMargin: speedMetrics.height / 2 + Kirigami.Units.smallSpacing
            }
            height: Kirigami.Units.gridUnit * 8
            smooth: true
            direction: QuickCharts.XYChart.ZeroAtEnd
            yRange {
                minimum: 100 * 1024
                increment: 100 * 1024
            }
            valueSources: [
                QuickCharts.HistoryProxySource {
                    source: QuickCharts.SingleValueSource {
                        id: upload
                    }
                    maximumHistory: 40
                    fillMode: QuickCharts.HistoryProxySource.FillFromStart
                },
                QuickCharts.HistoryProxySource {
                    source: QuickCharts.SingleValueSource {
                        id: download
                    }
                    maximumHistory: 40
                    fillMode: QuickCharts.HistoryProxySource.FillFromStart
                }
            ]
            nameSource: QuickCharts.ArraySource {
                array: [i18n("Upload"), i18n("Download")]
            }
            colorSource: QuickCharts.ArraySource {
                // Array.reverse() mutates the array but colors.colors is read-only.
                array: [colors.colors[1], colors.colors[0]]
            }
            fillColorSource: QuickCharts.ArraySource  {
                array: plotter.colorSource.array.map(color => Qt.lighter(color, 1.5))
            }
            QuickCharts.ColorGradientSource {
                id: colors
                baseColor:  Kirigami.Theme.highlightColor
                itemCount: 2
            }
        }
        TextMetrics {
            id: speedMetrics
            font: Kirigami.Theme.smallFont
            // Measure 888.8 KiB/s
            text: KCoreAddons.Format.formatByteSize(910131) + i18n("/s")
        }
    }
    QuickChartControls.Legend {
        chart: plotter
        Layout.leftMargin: Kirigami.Units.smallSpacing
        spacing: Kirigami.Units.largeSpacing
        delegate: RowLayout {
            spacing: Kirigami.Units.smallSpacing
            Rectangle {
                color: model.color
                width: Kirigami.Units.smallSpacing
                height: legendLabel.height
            }
            PlasmaComponents3.Label {
                id: legendLabel
                font: Kirigami.Theme.smallFont
                text: model.name
            }
        }
    }
}
