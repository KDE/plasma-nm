/*
    Copyright 2012 Lamarque V. Souza <lamarque@kde.org>
    Copyright 2013 Jan Grulich <jgrulich@redhat.com>

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

#include "detailkeyseditor.h"
#include "ui_detailkeyswidget.h"

#include <KLocalizedString>
#include <KDebug>
#include <QList>

#include <KConfigDialogManager>

#define DetailTagRole Qt::UserRole + 1

class DetailKeysEditorPrivate
{
public:
    Ui_DetailKeysWidget ui;
};

DetailKeysEditor::DetailKeysEditor(QWidget *parent)
    : QWidget(parent), d_ptr(new DetailKeysEditorPrivate)
{
    Q_D(DetailKeysEditor);
    d->ui.setupUi(this);

    setContentsMargins(QMargins());
    d->ui.gridLayout->setSpacing(0);

    loadAllDetailElements();

    QMap<QString, QPair<QString, QString> >::const_iterator it = m_allDetailsElements.constBegin();
    while (it != m_allDetailsElements.constEnd()) {
        d->ui.availDetails->addTopLevelItem(constructItem(it.key()));
        ++it;
    }

    setupCommon();

    KConfigDialogManager::changedMap()->insert("DetailKeysEditor", SIGNAL(currentDetailsChanged(const QStringList &)));
}

DetailKeysEditor::~DetailKeysEditor()
{
    Q_D(DetailKeysEditor);
    d->ui.currentDetails->clear();
    d->ui.availDetails->clear();
    delete d_ptr;
}

void DetailKeysEditor::loadSelectedElements(const QStringList& keys)
{
    Q_D(DetailKeysEditor);

    foreach (const QString & key, keys) {
        for (int i = 0; i < d->ui.availDetails->topLevelItemCount(); i++) {
            QTreeWidgetItem * item = d->ui.availDetails->topLevelItem(i);
            if (key == item->data(DetailKeysEditor::Key, DetailTagRole).toString()) {
                int index = d->ui.availDetails->indexOfTopLevelItem(item);
                d->ui.availDetails->takeTopLevelItem(index);
                d->ui.currentDetails->addTopLevelItem(item);
                break;
            }
        }
    }
}

void DetailKeysEditor::loadAllDetailElements()
{
    m_allDetailsElements.insert("interface:type", QPair<QString, QString>(i18n("Type"), i18n("Interface type")));
    m_allDetailsElements.insert("interface:status", QPair<QString, QString>(i18n("Connection State"), i18n("Connection state")));
    m_allDetailsElements.insert("interface:driver", QPair<QString, QString>(i18n("Driver"), i18n("Device driver")));
    m_allDetailsElements.insert("interface:bitrate", QPair<QString, QString>(i18n("Connection Speed"), i18n("Connection speed")));
    m_allDetailsElements.insert("interface:name", QPair<QString, QString>(i18n("System Name"), i18n("System Name")));
    m_allDetailsElements.insert("interface:hardwareaddress", QPair<QString, QString>(i18n("MAC Address"), i18n("Hardware address")));

    // IPv4
    m_allDetailsElements.insert("ipv4:address", QPair<QString, QString>(i18n("IPv4 Address"), i18n("IP version 4 address")));
//    m_allDetailsElements.insert("ipv4:nameservers", QPair<QString, QString>(i18n("IPv4 Name Servers"), i18n("IP version 4 Name servers")));
//    m_allDetailsElements.insert("ipv4:domains", QPair<QString, QString>(i18n("IPv4 Domains"), i18n("IPv version 4 DNS domain names")));
    m_allDetailsElements.insert("ipv4:gateway", QPair<QString, QString>(i18n("IPv4 Gateway"), i18n("IP version 4 default gateway")));

    // IPv6
    m_allDetailsElements.insert("ipv6:address", QPair<QString, QString>(i18n("IPv6 Address"), i18n("IP version 6 address")));
//    m_allDetailsElements.insert("ipv6:nameservers", QPair<QString, QString>(i18n("IPv6 Name Servers"), i18n("IP version 6 Name servers")));
//    m_allDetailsElements.insert("ipv6:domains", QPair<QString, QString>(i18n("IPv6 Domains"), i18n("IPv version 6 DNS domain names")));
    m_allDetailsElements.insert("ipv6:gateway", QPair<QString, QString>(i18n("IPv6 Gateway"), i18n("IP version 6 default gateway")));

    // Wireless
    m_allDetailsElements.insert("wireless:ssid", QPair<QString, QString>(i18n("Access Point (SSID)"), i18n("Wireless Access Point's SSID")));
    m_allDetailsElements.insert("wireless:signal", QPair<QString, QString>(i18n("Signal Strength"), i18n("Wireless Access Point's signal strength")));
    m_allDetailsElements.insert("wireless:accesspoint", QPair<QString, QString>(i18n("Access Point (MAC)"), i18n("Wireless Access Point's Hardware Address")));
    m_allDetailsElements.insert("wireless:band", QPair<QString, QString>(i18n("Wireless Band"), i18n("Wireless Frequency Band")));
    m_allDetailsElements.insert("wireless:channel", QPair<QString, QString>(i18n("Wireless Channel"), i18n("Wireless Frequency Channel")));

    // Mobile Broadband
    m_allDetailsElements.insert("mobile:operator", QPair<QString, QString>(i18n("Mobile Operator"), i18n("Mobile Operator's Name")));
    m_allDetailsElements.insert("mobile:quality", QPair<QString, QString>(i18n("Mobile Signal Quality"), i18n("Mobile Signal Quality")));
    m_allDetailsElements.insert("mobile:technology", QPair<QString, QString>(i18n("Mobile Access Technology"), i18n("Mobile Access Technology")));
    m_allDetailsElements.insert("mobile:band", QPair<QString, QString>(i18n("Mobile Frequency Band"), i18n("Mobile Frequency Band")));
    m_allDetailsElements.insert("mobile:mode", QPair<QString, QString>(i18n("Mobile Allowed Mode"), i18n("Mobile Allowed Mode")));
    m_allDetailsElements.insert("mobile:unlock", QPair<QString, QString>(i18n("Mobile Unlock Required"), i18n("Mobile Master Device")));
    m_allDetailsElements.insert("mobile:imei", QPair<QString, QString>(i18n("Mobile Device IMEI"), i18n("Mobile Device Serial Number")));
    m_allDetailsElements.insert("mobile:imsi", QPair<QString, QString>(i18n("Mobile Device IMSI"), i18n("Mobile Device Identity Number")));

    // Others
    m_allDetailsElements.insert("bluetooth:name", QPair<QString, QString>(i18n("Bluetooth Name"), i18n("Bluetooth name of the device")));
    m_allDetailsElements.insert("vpn:plugin", QPair<QString, QString>(i18n("VPN Plugin"), i18n("VPN plugin type")));
    m_allDetailsElements.insert("vpn:banner", QPair<QString, QString>(i18n("VPN Banner"), i18n("VPN connection banner")));
}

void DetailKeysEditor::setupCommon()
{
    Q_D(DetailKeysEditor);
    connect(d->ui.arrowUp, SIGNAL(clicked()), this, SLOT(upArrowClicked()));
    connect(d->ui.arrowLeft, SIGNAL(clicked()), this, SLOT(leftArrowClicked()));
    connect(d->ui.arrowRight, SIGNAL(clicked()), this, SLOT(rightArrowClicked()));
    connect(d->ui.arrowDown, SIGNAL(clicked()), this, SLOT(downArrowClicked()));
    d->ui.availDetails->sortByColumn(DetailKeysEditor::Name, Qt::AscendingOrder);
    d->ui.availDetails->setSortingEnabled(true);
}

QTreeWidgetItem * DetailKeysEditor::constructItem(const QString & key)
{
    QStringList data;
    data << m_allDetailsElements[key].first;
    QTreeWidgetItem *item = new QTreeWidgetItem(data);
    item->setData(DetailKeysEditor::Key, DetailTagRole, key);
    item->setToolTip(DetailKeysEditor::Name, m_allDetailsElements[key].second);
    return item;
}

QStringList DetailKeysEditor::currentDetails()
{
    Q_D(DetailKeysEditor);
    QStringList details;
    int itemCount = d->ui.currentDetails->topLevelItemCount();
    for (int i = 0; i < itemCount; i++) {
        QTreeWidgetItem *item = d->ui.currentDetails->topLevelItem(i);
        details << item->data(DetailKeysEditor::Key, DetailTagRole).toString();
    }
    return details;
}

void DetailKeysEditor::upArrowClicked()
{
    Q_D(DetailKeysEditor);

    foreach (QTreeWidgetItem *item, d->ui.currentDetails->selectedItems()) {
        int index = d->ui.currentDetails->indexOfTopLevelItem(item);
        if (index > 0) {
            d->ui.currentDetails->takeTopLevelItem(index);
            d->ui.currentDetails->insertTopLevelItem(index-1, item);
            item->setSelected(true);
        }
    }

    Q_EMIT currentDetailsChanged(currentDetails());
}

void DetailKeysEditor::downArrowClicked()
{
    Q_D(DetailKeysEditor);

    if (d->ui.currentDetails->selectedItems().isEmpty()) {
        return;
    }

    QList<QTreeWidgetItem *> list = d->ui.currentDetails->selectedItems();
    for (int i = list.size()-1; i >= 0; --i) {
        QTreeWidgetItem * item = list[i];
        int index = d->ui.currentDetails->indexOfTopLevelItem(item);
        d->ui.currentDetails->takeTopLevelItem(index);
        d->ui.currentDetails->insertTopLevelItem(index+1, item);
        item->setSelected(true);
    }

    Q_EMIT currentDetailsChanged(currentDetails());
}

void DetailKeysEditor::rightArrowClicked()
{
    Q_D(DetailKeysEditor);
    foreach (QTreeWidgetItem *item, d->ui.availDetails->selectedItems()) {
        int index = d->ui.availDetails->indexOfTopLevelItem(item);
        d->ui.availDetails->takeTopLevelItem(index);
        d->ui.currentDetails->addTopLevelItem(item);
    }

    Q_EMIT currentDetailsChanged(currentDetails());
}

void DetailKeysEditor::leftArrowClicked()
{
    Q_D(DetailKeysEditor);
    foreach (QTreeWidgetItem *item, d->ui.currentDetails->selectedItems()) {
        int index = d->ui.currentDetails->indexOfTopLevelItem(item);
        d->ui.currentDetails->takeTopLevelItem(index);
        d->ui.availDetails->addTopLevelItem(item);
    }

    Q_EMIT currentDetailsChanged(currentDetails());
}
