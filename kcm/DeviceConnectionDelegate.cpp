/*
 *   Copyright (C) 2007 Ivan Cukic <ivan.cukic+kde@gmail.com>
 *   Copyright (C) 2008-2010 Daniel Nicoletti <dantti12@gmail.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library/Lesser General Public License
 *   version 2, or (at your option) any later version, as published by the
 *   Free Software Foundation
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library/Lesser General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "DeviceConnectionDelegate.h"
#include "DeviceConnectionModel.h"

#include <cmath>

#include <QtGui/QPainter>

#include <QDebug>
#include <KIconLoader>

#define FAV_ICON_SIZE 24
#define EMBLEM_ICON_SIZE 8
#define UNIVERSAL_PADDING 4
#define FADE_LENGTH 16
#define MAIN_ICON_SIZE 32
#define DROPDOWN_PADDING 2
// #define DROPDOWN_SEPARATOR_HEIGHT 32

DeviceConnectionDelegate::DeviceConnectionDelegate(QObject *parent)
: QStyledItemDelegate(parent)
{
}

int DeviceConnectionDelegate::calcItemHeight(const QStyleOptionViewItem &option) const
{
    // Painting main column
    QStyleOptionViewItem local_option_title(option);
    QStyleOptionViewItem local_option_normal(option);

    local_option_normal.font.setPointSize(local_option_normal.font.pointSize() - 1);

    int textHeight = QFontInfo(local_option_title.font).pixelSize() + QFontInfo(local_option_normal.font).pixelSize();
    return qMax(textHeight, MAIN_ICON_SIZE) + 2 * UNIVERSAL_PADDING;
}


void DeviceConnectionDelegate::paint(QPainter *painter,
        const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (!index.isValid()){
        return;
    } else if (index.data(DeviceConnectionModel::RoleIsDevice).toBool() == false) {
        // For some reason some styles don't honor the views SelectionRectVisible
            // and I just hate that selection rect thing...
            QStyleOptionViewItemV4 opt(option);
            if (opt.state & QStyle::State_HasFocus) {
                opt.state ^= QStyle::State_HasFocus;
            }
        QStyledItemDelegate::paint(painter, opt, index);
        return;
    }

    QStyleOptionViewItemV4 opt(option);
    QStyle *style = opt.widget ? opt.widget->style() : QApplication::style();
    style->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, opt.widget);

    int left = option.rect.left();
    int top = option.rect.top();
    int width = option.rect.width();

    bool leftToRight = (painter->layoutDirection() == Qt::LeftToRight);

    // selects the mode to paint the icon based on the info field
    QIcon::Mode iconMode;
    if (option.state & QStyle::State_MouseOver) {
        iconMode = QIcon::Active;
    } else {
        iconMode = QIcon::Normal;
    }

    QColor foregroundColor = (option.state.testFlag(QStyle::State_Selected))?
        option.palette.color(QPalette::HighlightedText):option.palette.color(QPalette::Text);

    // Painting main column
    QStyleOptionViewItem local_option_title(option);
    QStyleOptionViewItem local_option_normal(option);

    local_option_normal.font.setPointSize(local_option_normal.font.pointSize() - 1);
//    local_option_title.font.setBold(index.data(PrinterModel::DestIsDefault).toBool());

    QPixmap pixmap(option.rect.size());
    pixmap.fill(Qt::transparent);
    QPainter p(&pixmap);
    p.translate(-option.rect.topLeft());

    QLinearGradient gradient;

    // Painting

    // Text
    int textInner = 2 * UNIVERSAL_PADDING + MAIN_ICON_SIZE;
    const int itemHeight = calcItemHeight(option);

    QString status = index.data(DeviceConnectionModel::RoleState).toString();
    QString description = index.data(Qt::DisplayRole).toString();

    p.setPen(foregroundColor);
    p.setFont(local_option_title.font);
    p.drawText(left + (leftToRight ? textInner : 0),
                top,
                width - textInner,
                itemHeight / 2,
                Qt::AlignBottom | Qt::AlignLeft,
                description);

    p.setFont(local_option_normal.font);
    p.drawText(left + (leftToRight ? textInner : 0) + 10,
                (top + itemHeight / 2) + 1,
                width - textInner,
                itemHeight / 2,
                Qt::AlignTop | Qt::AlignLeft,
                status);

    // Main icon
    QIcon icon = index.data(Qt::DecorationRole).value<QIcon>();
    // if we have a package that mean we
    // can try to get a better icon
    icon.paint(&p,
               leftToRight ? left + UNIVERSAL_PADDING : left + width - UNIVERSAL_PADDING - MAIN_ICON_SIZE,
               top + UNIVERSAL_PADDING,
               MAIN_ICON_SIZE,
               MAIN_ICON_SIZE,
               Qt::AlignCenter,
               iconMode);

    // Counting the number of emblems for this item
    int emblemCount = 1;

    // Gradient part of the background - fading of the text at the end
    if (leftToRight) {
        gradient = QLinearGradient(left + width - UNIVERSAL_PADDING - FADE_LENGTH, 0,
                left + width - UNIVERSAL_PADDING, 0);
        gradient.setColorAt(0, Qt::white);
        gradient.setColorAt(1, Qt::transparent);
    } else {
        gradient = QLinearGradient(left + UNIVERSAL_PADDING, 0,
                left + UNIVERSAL_PADDING + FADE_LENGTH, 0);
        gradient.setColorAt(0, Qt::transparent);
        gradient.setColorAt(1, Qt::white);
    }

    QRect paintRect = option.rect;
    p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
    p.fillRect(paintRect, gradient);

    if (leftToRight) {
        gradient.setStart(left + width
                - emblemCount * (UNIVERSAL_PADDING + EMBLEM_ICON_SIZE) - FADE_LENGTH, 0);
        gradient.setFinalStop(left + width
                - emblemCount * (UNIVERSAL_PADDING + EMBLEM_ICON_SIZE), 0);
    } else {
        gradient.setStart(left + UNIVERSAL_PADDING
                + emblemCount * (UNIVERSAL_PADDING + EMBLEM_ICON_SIZE), 0);
        gradient.setFinalStop(left + UNIVERSAL_PADDING
                + emblemCount * (UNIVERSAL_PADDING + EMBLEM_ICON_SIZE) + FADE_LENGTH, 0);
    }
    paintRect.setHeight(UNIVERSAL_PADDING + MAIN_ICON_SIZE / 2);
    p.fillRect(paintRect, gradient);

    painter->drawPixmap(option.rect.topLeft(), pixmap);
}

QSize DeviceConnectionDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.data(DeviceConnectionModel::RoleIsDevice).toBool() == false) {
        return QStyledItemDelegate::sizeHint(option, index);
    }

    return QSize(index.data(Qt::SizeHintRole).toSize().width(), calcItemHeight(option));
}

#include "DeviceConnectionDelegate.moc"
