/***************************************************************************
 *   Copyright (C) 2013 by Daniel Nicoletti <dantti12@gmail.com>           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; see the file COPYING. If not, write to       *
 *   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,  *
 *   Boston, MA 02110-1301, USA.                                           *
 ***************************************************************************/

#include "AvailableConnectionsDelegate.h"
#include "AvailableConnectionsModel.h"

#include <QApplication>
#include <QPainter>
#include <QFontMetrics>

#include <KIcon>

#include <KDebug>

#define UNIVERSAL_PADDING 4

AvailableConnectionsDelegate::AvailableConnectionsDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

void AvailableConnectionsDelegate::paint(QPainter *painter,
                                         const QStyleOptionViewItem &option,
                                         const QModelIndex &index) const
{
    // For some reason some styles don't honor the views SelectionRectVisible
    // and I just hate that selection rect thing...
    QStyleOptionViewItemV4 opt(option);
    initStyleOption(&opt, index);
    if (opt.state & QStyle::State_HasFocus) {
        opt.state ^= QStyle::State_HasFocus;
    }

    QString name = index.data().toString();
    uint kind = index.data(AvailableConnectionsModel::RoleKinds).toUInt();
    int signal = index.data(AvailableConnectionsModel::RoleSignalStrength).toInt();
    int security = index.data(AvailableConnectionsModel::RoleSecurity).toBool();

    QStyle *style = opt.widget ? opt.widget->style() : QApplication::style();
    opt.viewItemPosition = QStyleOptionViewItemV4::OnlyOne;
    painter->save();
    style->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, opt.widget);
    painter->restore();

    QStyleOptionViewItemV4 optText(opt);
    optText.rect.setLeft(optText.rect.left() + UNIVERSAL_PADDING);

    if (kind & AvailableConnectionsModel::Network) {
        optText.rect.setWidth(optText.rect.width() - (opt.rect.height() + UNIVERSAL_PADDING) * 2);

        QStyleOptionViewItemV4 optSecurity(opt);
        optSecurity.rect.setLeft(optText.rect.right());
        optSecurity.rect.setWidth(opt.rect.height() + UNIVERSAL_PADDING);
        if (security) {
            painter->save();
            style->drawItemPixmap(painter,
                                  optSecurity.rect,
                                  Qt::AlignCenter,
                                  KIcon("object-locked").pixmap(optSecurity.rect.size()));
            painter->restore();
        }

        // TODO Find/Draw better icons
        QString iconName;
        if (signal < 13) {
            iconName = "network-wireless-connected-00";
        } else if (signal < 38) {
            iconName = "network-wireless-connected-25";
        } else if (signal < 63) {
            iconName = "network-wireless-connected-50";
        } else if (signal < 88) {
            iconName = "network-wireless-connected-75";
        } else {
            iconName = "network-wireless-connected-100";
        }

        QStyleOptionViewItemV4 optEmblem(opt);
        optEmblem.rect.setLeft(optSecurity.rect.right());
        optEmblem.rect.setWidth(opt.rect.height() + UNIVERSAL_PADDING);
        painter->save();
        style->drawItemPixmap(painter,
                              optEmblem.rect,
                              Qt::AlignCenter,
                              KIcon(iconName).pixmap(optEmblem.rect.size()));
        painter->restore();
    }

    painter->save();
    style->drawItemText(painter,
                        optText.rect,
                        Qt::AlignVCenter | Qt::AlignLeft,
                        optText.palette,
                        true,
                        optText.fontMetrics.elidedText(name, Qt::ElideRight, optText.rect.width()));
    painter->restore();
}

QSize AvailableConnectionsDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize ret = QStyledItemDelegate::sizeHint(option, index);

    ret.rheight() += UNIVERSAL_PADDING;

    return ret;
}
