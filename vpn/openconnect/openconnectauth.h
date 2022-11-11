/*
    SPDX-FileCopyrightText: 2011 Ilia Kats <ilia-kats@gmx.net>
    SPDX-FileCopyrightText: 2013 Lukáš Tinkl <ltinkl@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef OPENCONNECTAUTH_H
#define OPENCONNECTAUTH_H

#include "openconnectauthworkerthread.h"
#include "settingwidget.h"
#include "ui_openconnectauth.h"

#include <NetworkManagerQt/VpnSetting>

#include <QMutex>
#include <QString>
#include <QWaitCondition>

class QLayout;
struct openconnect_info;
struct oc_auth_form;

class OpenconnectAuthWidget : public SettingWidget
{
    Q_OBJECT
public:
    explicit OpenconnectAuthWidget(const NetworkManager::VpnSetting::Ptr &setting, const QStringList &hints, QWidget *parent = nullptr);
    ~OpenconnectAuthWidget() override;
    virtual void readSecrets();
    void readConfig();
    QVariantMap setting() const override;

private:
    void acceptDialog();
    void addFormInfo(const QString &, const QString &);
    void deleteAllFromLayout(QLayout *);

    // name/address: IP/domain name of the host (OpenConnect accepts both, so no difference here)
    // group: user group on the server
    using VPNHost = struct {
        QString name;
        QString group;
        QString address;
    };

    using Token = struct {
        oc_token_mode_t tokenMode;
        QByteArray tokenSecret;
    };

    Ui_OpenconnectAuth ui;
    NetworkManager::VpnSetting::Ptr m_setting;
    struct openconnect_info *vpninfo;
    NMStringMap secrets;
    NMStringMap tmpSecrets;
    QMutex mutex;
    QWaitCondition workerWaiting;
    OpenconnectAuthWorkerThread *worker;
    QList<VPNHost> hosts;
    bool userQuit;
    bool m_formGroupChanged;
    int cancelPipes[2];
    QList<QPair<QString, int>> serverLog;
    int passwordFormIndex;
    QByteArray tokenMode;
    Token token;

    enum LogLevels { Error = 0, Info, Debug, Trace };

private Q_SLOTS:
    void writeNewConfig(const QString &);
    void validatePeerCert(const QString &, const QString &, const QString &, bool *);
    void processAuthForm(struct oc_auth_form *);
    void updateLog(const QString &, const int &);
    void logLevelChanged(int);
    void formLoginClicked();
    void formGroupChanged();
    void workerFinished(const int &);
    void viewServerLogToggled(bool);
    void connectHost();
    void initTokens();
};

#endif // OPENCONNECTAUTH_H
