/*
    SPDX-FileCopyrightText: 2011 Ilia Kats <ilia-kats@gmx.net>
    SPDX-FileCopyrightText: 2013 Jan Grulich <jgrulich@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "advancedpermissionswidget.h"

#include <KAcceleratorManager>
#include <KLocalizedString>
#include <KUser>
#include <QList>

#define UserTagRole Qt::UserRole + 1

AdvancedPermissionsWidget::AdvancedPermissionsWidget(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);

    for (const KUser &user : KUser::allUsers()) {
        if (user.userId().nativeId() >= 1000 && user.loginName() != QLatin1String("nobody"))
            ui.availUsers->addTopLevelItem(constructItem(user));
    }
    setupCommon();
}

AdvancedPermissionsWidget::AdvancedPermissionsWidget(const QHash<QString, QString> &users, QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);

    for (const KUser &user : KUser::allUsers()) {
        const QString name = user.loginName();
        if (!users.contains(name) && user.userId().nativeId() >= 1000 && user.loginName() != QLatin1String("nobody"))
            ui.availUsers->addTopLevelItem(constructItem(user));
        else if (users.contains(name))
            ui.currentUsers->addTopLevelItem(constructItem(user, users.value(name)));
    }
    setupCommon();
}

AdvancedPermissionsWidget::~AdvancedPermissionsWidget()
{
    while (QTreeWidgetItem *item = ui.currentUsers->takeTopLevelItem(0)) {
        delete item;
    }
    while (QTreeWidgetItem *item = ui.availUsers->takeTopLevelItem(0)) {
        delete item;
    }
}

void AdvancedPermissionsWidget::setupCommon()
{
    connect(ui.arrowLeft, &QPushButton::clicked, this, &AdvancedPermissionsWidget::leftArrowClicked);
    connect(ui.arrowRight, &QPushButton::clicked, this, &AdvancedPermissionsWidget::rightArrowClicked);
    ui.availUsers->sortByColumn(FullName, Qt::AscendingOrder);
    ui.currentUsers->sortByColumn(FullName, Qt::AscendingOrder);
    ui.availUsers->setSortingEnabled(true);
    ui.currentUsers->setSortingEnabled(true);

    KAcceleratorManager::manage(this);
}

QTreeWidgetItem *AdvancedPermissionsWidget::constructItem(const KUser &user, const QString &itemData)
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
    auto item = new QTreeWidgetItem(data);
    item->setData(LoginName, UserTagRole, itemData);
    item->setToolTip(FullName, nametooltip);
    item->setToolTip(LoginName, user.loginName());
    return item;
}

QHash<QString, QString> AdvancedPermissionsWidget::currentUsers() const
{
    QHash<QString, QString> permissions;
    const int itemNumber = ui.currentUsers->topLevelItemCount();
    for (int i = 0; i < itemNumber; i++) {
        QTreeWidgetItem *item = ui.currentUsers->topLevelItem(i);
        QString username = item->data(LoginName, Qt::DisplayRole).toString();
        QString tags = item->data(LoginName, UserTagRole).toString();
        permissions.insert(username, tags);
    }
    return permissions;
}

void AdvancedPermissionsWidget::rightArrowClicked()
{
    for (QTreeWidgetItem *item : ui.availUsers->selectedItems()) {
        const int index = ui.availUsers->indexOfTopLevelItem(item);
        ui.availUsers->takeTopLevelItem(index);
        ui.currentUsers->addTopLevelItem(item);
    }
}

void AdvancedPermissionsWidget::leftArrowClicked()
{
    for (QTreeWidgetItem *item : ui.currentUsers->selectedItems()) {
        if (item->data(LoginName, Qt::DisplayRole) != KUser().loginName()) {
            const int index = ui.currentUsers->indexOfTopLevelItem(item);
            ui.currentUsers->takeTopLevelItem(index);
            ui.availUsers->addTopLevelItem(item);
        }
    }
}
