/*
    Copyright 2014 Jan Grulich <jgrulich@redhat.com>
    Copyright 2014 Lukáš Tinkl <ltinkl@redhat.com>

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

#include "editoridentitymodel.h"
#include "networkmodel.h"
#include "networkmodelitem.h"
#include "uiutils.h"

#include <QIcon>
#include <QFont>

#include <KLocalizedString>
#include <KIcon>

EditorIdentityModel::EditorIdentityModel(QObject* parent)
    : QIdentityProxyModel(parent)
{
    NetworkModel * baseModel = new NetworkModel(this);
    setSourceModel(baseModel);
}

EditorIdentityModel::~EditorIdentityModel()
{
}

Qt::ItemFlags EditorIdentityModel::flags(const QModelIndex& index) const
{
    const QModelIndex mappedProxyIndex = index.sibling(index.row(), 0);
    return QIdentityProxyModel::flags(mappedProxyIndex) | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

int EditorIdentityModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);

    return 2;
}

QVariant EditorIdentityModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case 0:
            return i18n("Connection name");
        case 1:
            return i18n("Last used");
        }
    }

    return QIdentityProxyModel::headerData(section, orientation, role);
}

QVariant EditorIdentityModel::data(const QModelIndex& index, int role) const
{
    const QModelIndex sourceIndex = sourceModel()->index(index.row(), 0);
    const QString connectionName = sourceModel()->data(sourceIndex, NetworkModel::NameRole).toString();
    const QString lastUsed = sourceModel()->data(sourceIndex, NetworkModel::LastUsedDateOnlyRole).toString();
    const bool isActivated = (NetworkManager::ActiveConnection::State) sourceModel()->data(sourceIndex, NetworkModel::ConnectionStateRole).toInt()
                             == NetworkManager::ActiveConnection::Activated;
    NetworkManager::ConnectionSettings::ConnectionType type = static_cast<NetworkManager::ConnectionSettings::ConnectionType>(sourceModel()->data(sourceIndex, NetworkModel::TypeRole).toInt());
    QString tooltip;
    QString iconName = UiUtils::iconAndTitleForConnectionSettingsType(type, tooltip);
    const int column = index.column();

    if (role == Qt::DisplayRole) {
        switch (column) {
        case 0:
            if (type == NetworkManager::ConnectionSettings::Infiniband ||
                type == NetworkManager::ConnectionSettings::Bond ||
                type == NetworkManager::ConnectionSettings::Bridge ||
#if NM_CHECK_VERSION(0, 9, 9)
                type == NetworkManager::ConnectionSettings::Team ||
#endif
                type == NetworkManager::ConnectionSettings::Vlan) {
                return QString(connectionName + " (" + NetworkManager::ConnectionSettings::typeAsString(type) + ")");
            }
            return connectionName;
        case 1:
            return lastUsed;
        }
    } else if (role == Qt::DecorationRole) {
        if (column == 0) {
            return KIcon(iconName);
        }
    } else if (role == Qt::ToolTipRole) {
        if (column == 0)
            return tooltip;
    } else if (role == Qt::FontRole) {
        if (column == 0 && isActivated) {
            QFont f;
            f.setBold(true);
            return f;
        }
    } else {
        return sourceModel()->data(index, role);
    }

    return QVariant();
}

QModelIndex EditorIdentityModel::index(int row, int column, const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return createIndex(row, column);
}

QModelIndex EditorIdentityModel::mapToSource(const QModelIndex& proxyIndex) const
{
    if (proxyIndex.column() > 0) {
        return QModelIndex();
    }

    return QIdentityProxyModel::mapToSource(proxyIndex);
}
