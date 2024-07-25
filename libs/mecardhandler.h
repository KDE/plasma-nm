/*
 * SPDX-FileCopyrightText: 2024 Kai Uwe Broulik <kde@broulik.de>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#ifndef PLASMA_NM_MECARDHANDLER_H
#define PLASMA_NM_MECARDHANDLER_H

#include <QObject>
#include <QString>
#include <QVariantMap>

#include <qqmlregistration.h>

#include <QCoroCore>

class MeCardHandler : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QVariantMap meCard READ meCard WRITE setMeCard NOTIFY meCardChanged)

    Q_PROPERTY(QString ssid READ ssid NOTIFY ssidChanged)

public:
    explicit MeCardHandler(QObject *parent = nullptr);
    ~MeCardHandler() override;

    QString text() const;
    void setText(const QString &text);
    Q_SIGNAL void textChanged(const QString &text);

    QVariantMap meCard() const;
    void setMeCard(const QVariantMap &meCard);
    Q_SIGNAL void meCardChanged();

    QString ssid() const;
    Q_SIGNAL void ssidChanged();

    Q_INVOKABLE void reset();
    Q_INVOKABLE void activateConnection();

private:
    void setSsid(const QString &ssid);

    QCoro::Task<void> activateConnectionInternal();

    QString m_text;
    QVariantMap m_meCard;
};

#endif // PLASMA_NM_MECARDHANDLER_H
