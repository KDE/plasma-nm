/*
    Copyright 2011 Ilia Kats <ilia-kats@gmx.net>
    Copyright 2013 Ilia Kats <jgrulich@redhat.com>

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

#include "advancedpermissionswidget.h"
#include "ui_advancedpermissionswidget.h"

#include <KUser>
#include <KLocalizedString>
#include <KDebug>
#include <QList>

#define UserTagRole Qt::UserRole + 1

class AdvancedPermissionsWidgetPrivate
{
public:
    Ui_AdvancedPermissions ui;
};

AdvancedPermissionsWidget::AdvancedPermissionsWidget(QWidget *parent):
    QWidget(parent),
    d_ptr(new AdvancedPermissionsWidgetPrivate)
{
    Q_D(AdvancedPermissionsWidget);
    d->ui.setupUi(this);

    foreach (const KUser &user, KUser::allUsers()) {
        if (user.uid() >= 1000 && user.loginName() != QLatin1String("nobody"))
            d->ui.availUsers->addTopLevelItem(constructItem(user));
    }
    setupCommon();
}

AdvancedPermissionsWidget::AdvancedPermissionsWidget(const QHash<QString,QString> &users, QWidget *parent):
    QWidget(parent),
    d_ptr(new AdvancedPermissionsWidgetPrivate)
{
    Q_D(AdvancedPermissionsWidget);
    d->ui.setupUi(this);

    foreach (const KUser &user, KUser::allUsers()) {
        const QString name = user.loginName();
        if (!users.contains(name) && user.uid() >= 1000 && user.loginName() != QLatin1String("nobody"))
            d->ui.availUsers->addTopLevelItem(constructItem(user));
        else if (users.contains(name))
            d->ui.currentUsers->addTopLevelItem(constructItem(user, users.value(name)));
    }
    setupCommon();
}

AdvancedPermissionsWidget::~AdvancedPermissionsWidget()
{
    Q_D(AdvancedPermissionsWidget);
    while (QTreeWidgetItem *item = d->ui.currentUsers->takeTopLevelItem(0)) {
        delete item;
    }
    while (QTreeWidgetItem *item = d->ui.availUsers->takeTopLevelItem(0)) {
        delete item;
    }
    delete d_ptr;
}

void AdvancedPermissionsWidget::setupCommon()
{
    Q_D(AdvancedPermissionsWidget);
    connect(d->ui.arrowLeft, SIGNAL(clicked()), this, SLOT(leftArrowClicked()));
    connect(d->ui.arrowRight, SIGNAL(clicked()), this, SLOT(rightArrowClicked()));
    d->ui.availUsers->sortByColumn(FullName, Qt::AscendingOrder);
    d->ui.currentUsers->sortByColumn(FullName, Qt::AscendingOrder);
    d->ui.availUsers->setSortingEnabled(true);
    d->ui.currentUsers->setSortingEnabled(true);
}

QTreeWidgetItem * AdvancedPermissionsWidget::constructItem(const KUser &user, const QString &itemData)
{
    QStringList data;
    QString name = user.property(KUser::FullName).toString();
    QString nametooltip;
    if (name.isEmpty()) {
        name = i18nc("@item:intable shortcut for Not Available", "N/A");
        nametooltip = i18nc("@info:tooltip real user name is not available", "Not Available");
    } else {
        nametooltip = name;
    }
    data << name << user.loginName();
    QTreeWidgetItem *item = new QTreeWidgetItem(data);
    item->setData(LoginName, UserTagRole, itemData);
    item->setToolTip(FullName, nametooltip);
    item->setToolTip(LoginName, user.loginName());
    return item;
}

QHash<QString, QString> AdvancedPermissionsWidget::currentUsers()
{
    Q_D(AdvancedPermissionsWidget);
    QHash<QString, QString> permissions;
    const int itemNumber = d->ui.currentUsers->topLevelItemCount();
    for (int i = 0; i < itemNumber; i++) {
        QTreeWidgetItem *item = d->ui.currentUsers->topLevelItem(i);
        QString username = item->data(LoginName, Qt::DisplayRole).toString();
        QString tags = item->data(LoginName, UserTagRole).toString();
        permissions.insert(username, tags);
    }
    return permissions;
}

void AdvancedPermissionsWidget::rightArrowClicked()
{
    Q_D(AdvancedPermissionsWidget);
    foreach (QTreeWidgetItem *item, d->ui.availUsers->selectedItems()) {
        const int index = d->ui.availUsers->indexOfTopLevelItem(item);
        d->ui.availUsers->takeTopLevelItem(index);
        d->ui.currentUsers->addTopLevelItem(item);
    }
}

void AdvancedPermissionsWidget::leftArrowClicked()
{
    Q_D(AdvancedPermissionsWidget);
    foreach (QTreeWidgetItem *item, d->ui.currentUsers->selectedItems()) {
        if (item->data(LoginName, Qt::DisplayRole) != KUser().loginName()) {
            const int index = d->ui.currentUsers->indexOfTopLevelItem(item);
            d->ui.currentUsers->takeTopLevelItem(index);
            d->ui.availUsers->addTopLevelItem(item);
        }
    }
}
