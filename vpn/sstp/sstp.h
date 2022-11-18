/*
    SPDX-FileCopyrightText: 2015 Jan Grulich <jgrulich@redhat.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef PLASMA_NM_SSTP_H
#define PLASMA_NM_SSTP_H

#include "vpnuiplugin.h"

class Q_DECL_EXPORT SstpUiPlugin : public VpnUiPlugin
{
    Q_OBJECT
public:
    explicit SstpUiPlugin(QObject *parent = nullptr, const QVariantList & = QVariantList());
    ~SstpUiPlugin() override;
    SettingWidget *widget(const NetworkManager::VpnSetting::Ptr &setting, QWidget *parent = nullptr) override;
    SettingWidget *askUser(const NetworkManager::VpnSetting::Ptr &setting, const QStringList &hints, QWidget *parent = nullptr) override;
    QString suggestedFileName(const NetworkManager::ConnectionSettings::Ptr &connection) const override;

    NMVariantMapMap importConnectionSettings(const QString &fileName) override;
    bool exportConnectionSettings(const NetworkManager::ConnectionSettings::Ptr &connection, const QString &fileName) override;
};

#endif //  PLASMA_NM_SSTP_H
