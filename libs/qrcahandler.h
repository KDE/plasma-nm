/*
 * SPDX-FileCopyrightText: 2025 Kai Uwe Broulik <kde@broulik.de>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#ifndef PLASMA_NM_QRCAHANDLER_H
#define PLASMA_NM_QRCAHANDLER_H

#include <QObject>
#include <QString>
#include <QVariantMap>

#include <qqmlregistration.h>

#include <KService>

class QrcaHandler : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(bool available READ available NOTIFY availableChanged)

public:
    explicit QrcaHandler(QObject *parent = nullptr);
    ~QrcaHandler() override;

    bool available() const;
    Q_SIGNAL void availableChanged();

    Q_INVOKABLE void launch();

private:
    void update();

    bool m_available = false;
};

#endif // PLASMA_NM_QRCAHANDLER_H
