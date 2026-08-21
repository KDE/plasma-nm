/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>
    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef PLASMA_NM_IPV6_ROUTE_TABLE_MODEL_H
#define PLASMA_NM_IPV6_ROUTE_TABLE_MODEL_H

#include "plasmanm_editorqml_export.h"

#include <QAbstractTableModel>
#include <QList>
#include <QString>
#include <QtQmlIntegration/QtQmlIntegration>

struct Ipv6RouteEntry {
    Q_GADGET
    QML_VALUE_TYPE(ipv6RouteEntry)

    Q_PROPERTY(QString address MEMBER address)
    Q_PROPERTY(uint prefix MEMBER prefix)
    Q_PROPERTY(QString nextHop MEMBER nextHop)
    Q_PROPERTY(uint metric MEMBER metric)
public:
    QString address;
    uint prefix = 0;
    QString nextHop;
    uint metric = 0;
};

class PLASMANM_EDITORQML_EXPORT Ipv6RouteTableModel : public QAbstractTableModel
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

public:
    enum Column {
        AddressColumn = 0,
        PrefixColumn,
        NextHopColumn,
        MetricColumn,
        ColumnCount
    };
    Q_ENUM(Column)

    static constexpr uint MaxPrefixLength = 128;

    explicit Ipv6RouteTableModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    Q_INVOKABLE bool setRouteField(int row, int column, const QVariant &value);

    [[nodiscard]] QList<Ipv6RouteEntry> routes() const;
    void setRoutes(const QList<Ipv6RouteEntry> &routes);

    Q_INVOKABLE void addRoute();
    Q_INVOKABLE void removeRoute(int row);

Q_SIGNALS:
    void routesChanged();

private:
    QList<Ipv6RouteEntry> m_routes;
};

#endif // PLASMA_NM_IPV6_ROUTE_TABLE_MODEL_H
