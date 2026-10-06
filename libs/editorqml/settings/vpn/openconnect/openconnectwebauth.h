/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef PLASMA_NM_OPENCONNECT_WEBAUTH_QML_H
#define PLASMA_NM_OPENCONNECT_WEBAUTH_QML_H

#include "plasmanm_editorqml_export.h"

#include <QObject>
#include <QPointer>
#include <QStringList>

class QWebEngineWebAuthUxRequest;

class PLASMANM_EDITORQML_EXPORT OpenconnectWebAuth : public QObject
{
    Q_OBJECT

    Q_PROPERTY(State state READ state NOTIFY changed)
    Q_PROPERTY(QString heading READ heading NOTIFY changed)
    Q_PROPERTY(QString description READ description NOTIFY changed)

    // SelectAccount
    Q_PROPERTY(QStringList userNames READ userNames NOTIFY changed)

    // CollectPin
    Q_PROPERTY(bool confirmPinRequired READ confirmPinRequired NOTIFY changed)
    Q_PROPERTY(QString pinError READ pinError NOTIFY changed)

    // Which buttons the current state calls for
    Q_PROPERTY(bool canAccept READ canAccept NOTIFY changed)
    Q_PROPERTY(QString acceptText READ acceptText NOTIFY changed)
    Q_PROPERTY(bool canRetry READ canRetry NOTIFY changed)
    Q_PROPERTY(QString cancelText READ cancelText NOTIFY changed)

public:
    enum State {
        Inactive = 0,
        SelectAccount,
        CollectPin,
        FinishTokenCollection,
        RequestFailed
    };
    Q_ENUM(State)

    explicit OpenconnectWebAuth(QObject *parent = nullptr);
    ~OpenconnectWebAuth() override;

    void setRequest(QWebEngineWebAuthUxRequest *request);

    State state() const;
    QString heading() const;
    QString description() const;
    QStringList userNames() const;
    bool confirmPinRequired() const;
    QString pinError() const;
    bool canAccept() const;
    QString acceptText() const;
    bool canRetry() const;
    QString cancelText() const;

    Q_INVOKABLE void acceptAccount(const QString &userName);

    Q_INVOKABLE void acceptPin(const QString &pin);

    Q_INVOKABLE void cancel();
    Q_INVOKABLE void retry();

Q_SIGNALS:
    void changed();

    void activeChanged(bool active);

private:
    void onStateChanged();

    QPointer<QWebEngineWebAuthUxRequest> m_request;
};

#endif // PLASMA_NM_OPENCONNECT_WEBAUTH_QML_H
