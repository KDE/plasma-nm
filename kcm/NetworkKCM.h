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

#ifndef NETWORK_KCM_H
#define NETWORK_KCM_H

#include <KCModule>
#include <KTitleWidget>

#include <QMenu>
#include <QStackedLayout>
#include <QSortFilterProxyModel>
#include <QDBusObjectPath>

typedef QPair<QString, QDBusObjectPath> KindAndPath;

namespace Ui {
    class NetworkKCM;
}
class DeviceConnectionModel;
class DeviceConnectionSortFilterModel;
class NetworkKCM : public KCModule
{
    Q_OBJECT
public:
    NetworkKCM(QWidget *parent, const QVariantList &args);
    ~NetworkKCM();

public Q_SLOTS:
    void load();

private Q_SLOTS:
    void showDescription();
    void updateSelection();
    void expandParent(const QModelIndex &index);
    void on_tabWidget_currentChanged(int index);
    void setNetworkingEnabled(bool enabled);
    void on_networkingPB_clicked();
    void on_removeConnectionBt_clicked();

private:
    QModelIndex currentIndex() const;

    Ui::NetworkKCM *ui;
    DeviceConnectionSortFilterModel *m_deviceConnectionSortModel;
    DeviceConnectionModel *m_deviceConnectionModel;
    QStackedLayout *m_stackedLayout;
    QWidget *m_noPrinter;
    QWidget *m_serverError;
    KTitleWidget *m_serverErrorW;
    int m_lastError;
    QSortFilterProxyModel *m_profilesFilter;
    QHash<QString, KindAndPath> m_profileFiles;

    QAction *m_addAction;
    QAction *m_removeAction;
    QAction *m_configureAction;
};

#endif // NETWORK_KCM_H
