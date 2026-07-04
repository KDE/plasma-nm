/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>
    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef PLASMA_NM_STRING_LIST_MODEL_H
#define PLASMA_NM_STRING_LIST_MODEL_H

#include "plasmanm_editorqml_export.h"

#include <QAbstractListModel>
#include <QStringList>
#include <qqmlregistration.h>

class PLASMANM_EDITORQML_EXPORT StringListModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QStringList stringList READ stringList WRITE setStringList NOTIFY stringListChanged)

public:
    explicit StringListModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    QStringList stringList() const;
    void setStringList(const QStringList &list);

    Q_INVOKABLE void append(const QString &value);
    Q_INVOKABLE void removeAt(int index);
    Q_INVOKABLE void moveUp(int index);
    Q_INVOKABLE void moveDown(int index);
    Q_INVOKABLE bool contains(const QString &value) const;

Q_SIGNALS:
    void stringListChanged();

private:
    QStringList m_items;
};

#endif // PLASMA_NM_STRING_LIST_MODEL_H
