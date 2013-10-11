/***************************************************************************
 *   Copyright (C) 2012 by Daniel Nicoletti                                *
 *   dantti12@gmail.com                                                    *
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

#include "NetworkKCM.h"
#include "ui_NetworkKCM.h"

#include <config.h>

#include "DeviceConnectionModel.h"
#include "DeviceConnectionSortFilterModel.h"
#include "DeviceConnectionDelegate.h"
#include "Description.h"

#include <NetworkManagerQt/Settings>

#include <KMessageBox>
#include <KGenericFactory>
#include <k4aboutdata.h>
#include <KFileDialog>
#include <KMimeType>
#include <KIcon>
#include <KUser>

#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusServiceWatcher>
#include <QItemSelectionModel>
#include <QTimer>
#include <QFileInfo>
#include <QStringBuilder>
#include <QSignalMapper>

#define DEVICE_PATH "device-path"

K_PLUGIN_FACTORY(NetworkKCMFactory, registerPlugin<NetworkKCM>();)
K_EXPORT_PLUGIN(NetworkKCMFactory("kcm_network"))

NetworkKCM::NetworkKCM(QWidget *parent, const QVariantList &args) :
    KCModule(parent, args),
    ui(new Ui::NetworkKCM)
{
    K4AboutData *aboutData;
    aboutData = new K4AboutData("kcm_network",
                               "kcm_network",
                               ki18n("Network settings"),
                               KDE_NETWORK_VERSION,
                               ki18n("Network settings"),
                               K4AboutData::License_GPL,
                               ki18n("(C) 2013 Daniel Nicoletti"));
    //setAboutData(aboutData);
    setButtons(NoAdditionalButton);

    ui->setupUi(this);
    ui->infoWidget->setPixmap(KTitleWidget::InfoMessage);

    ui->addProfileBt->setIcon(KIcon("list-add"));
    ui->removeConnectionBt->setIcon(KIcon("list-remove"));
    ui->advancedTB->setIcon(KIcon("applications-engineering"));

    DeviceConnectionDelegate *delegate = new DeviceConnectionDelegate(this);
    ui->devicesTV->setItemDelegate(delegate);

    m_deviceConnectionModel = new DeviceConnectionModel(this);
    connect(m_deviceConnectionModel, SIGNAL(parentAdded(QModelIndex)), this, SLOT(expandParent(QModelIndex)));
    connect(m_deviceConnectionModel, SIGNAL(layoutChanged()), this, SLOT(updateSelection()));

    m_deviceConnectionSortModel = new DeviceConnectionSortFilterModel(this);
    m_deviceConnectionSortModel->setSourceModel(m_deviceConnectionModel);
    // Connect this slot prior to defining the model
    // so we get a selection on the first item for free
    connect(m_deviceConnectionSortModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
            this, SLOT(showDescription()));
    // Set the source model then connect to the selection model to get updates
    ui->devicesTV->setModel(m_deviceConnectionSortModel);
    connect(ui->devicesTV->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(showDescription()));

    // make sure the screen is split on the half
    QList<int> sizes;
    sizes << width() / 2;
    sizes << width() / 2;
    ui->splitter->setSizes(sizes);

    connect(NetworkManager::notifier(), SIGNAL(networkingEnabledChanged(bool)),
            this, SLOT(setNetworkingEnabled(bool)));
    setNetworkingEnabled(NetworkManager::isNetworkingEnabled());

    connect(ui->showConnectionsCB, SIGNAL(toggled(bool)),
            m_deviceConnectionSortModel, SLOT(setShowInactiveConnections(bool)));
    connect(ui->showConnectionsCB, SIGNAL(toggled(bool)),
            ui->devicesTV, SLOT(expandAll()));
}

NetworkKCM::~NetworkKCM()
{
    delete ui;
}

void NetworkKCM::load()
{
    // Force the profile widget to get a proper height in case
    // the stacked widget is showing the info page first
    if (ui->stackedWidget->currentWidget() != ui->profile_page) {
        // This is highly needed otherwise the size get wrong on System Settings
        ui->stackedWidget->setCurrentWidget(ui->profile_page);
    }
    ui->devicesTV->setFocus();
    m_deviceConnectionModel->init();

    // align the tabbar to the list view
    int offset = ui->profile->innerHeight() - ui->devicesTV->viewport()->height();
    ui->offsetSpacer->changeSize(30, offset, QSizePolicy::Fixed, QSizePolicy::Fixed);

    // Make sure we have something selected
    showDescription();
}

void NetworkKCM::showDescription()
{
    QModelIndex index = currentIndex();
    if (!index.isValid()) {
        return;
    }

    if (ui->stackedWidget->currentWidget() != ui->profile_page) {
        ui->stackedWidget->setCurrentWidget(ui->profile_page);
    }

    bool enableRemoveConnection = false;
    if (index.data(DeviceConnectionModel::RoleIsDevice).toBool()) {
        ui->profile->setDevice(index.data(DeviceConnectionModel::RoleDeviceUNI).toString());
    } else if (index.data(DeviceConnectionModel::RoleIsConnection).toBool()) {
        ui->profile->setConnection(index.data(DeviceConnectionModel::RoleConnectionPath).toString());
        enableRemoveConnection =true;
    }
    ui->removeConnectionBt->setEnabled(enableRemoveConnection);
}

void NetworkKCM::updateSelection()
{
    QAbstractItemView *view = ui->devicesTV;
    QItemSelection selection = view->selectionModel()->selection();
    // Make sure we have an index selected
    if (selection.indexes().isEmpty()) {
        view->selectionModel()->select(view->model()->index(1, 0),
                                       QItemSelectionModel::SelectCurrent);
    }
}

void NetworkKCM::expandParent(const QModelIndex &index)
{
    ui->devicesTV->expand(m_deviceConnectionSortModel->mapFromSource(index));
}

void NetworkKCM::on_tabWidget_currentChanged(int index)
{
    if (index == 0 && ui->addProfileBt->menu() == 0) {
        // adds the menu to the Add Profile button
    } else if (index) {
        // Remove the menu from the buttom since we can
        // only add files anyway
        ui->addProfileBt->setMenu(0);
    }
}

void NetworkKCM::setNetworkingEnabled(bool enabled)
{
    if (enabled) {
        ui->networkingPB->setText(i18n("Disable networking"));
    } else {
        ui->networkingPB->setText(i18n("Enable networking"));
    }
}

void NetworkKCM::on_networkingPB_clicked()
{
    NetworkManager::setNetworkingEnabled(!NetworkManager::isNetworkingEnabled());
}

void NetworkKCM::on_removeConnectionBt_clicked()
{
    QModelIndex index = currentIndex();
    if (!index.data(DeviceConnectionModel::RoleIsConnection).toBool()) {
        return;
    }

    QString uni = index.data(DeviceConnectionModel::RoleConnectionPath).toString();
    NetworkManager::Connection::Ptr connection;
    connection = NetworkManager::findConnection(uni);
    if (!connection) {
        return;
    }

    int ret;
    ret = KMessageBox::questionYesNo(this,
                                     i18n("Are you sure you want to remove the connection: %1?", connection->name()),
                                     i18n("Remove Connection"));
    if (ret == KMessageBox::Yes && connection) {
        connection->remove();
    }
}

QModelIndex NetworkKCM::currentIndex() const
{
    QModelIndex ret;
    QAbstractItemView *view = ui->devicesTV;
    if (view->model()->rowCount() == 0) {
        if (ui->stackedWidget->currentWidget() != ui->info_page) {
            ui->stackedWidget->setCurrentWidget(ui->info_page);
        }

        if (ui->stackedWidget->currentIndex() == 0) {
            // Devices is empty
            ui->infoWidget->setText(i18n("You do not have any devices or connection registered"));
            ui->infoWidget->setComment(i18n("Make sure Network Manager is running"));
        }

        return ret;
    }

    QItemSelection selection;
    selection = view->selectionModel()->selection();
    // select the first index if the selection is not empty
    if (!selection.indexes().isEmpty()) {
        ret = selection.indexes().first();
    } else {
        ret = m_deviceConnectionSortModel->index(1, 0);
        view->selectionModel()->select(ret, QItemSelectionModel::Select);
    }

    return ret;
}

#include "NetworkKCM.moc"