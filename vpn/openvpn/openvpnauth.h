/*
    SPDX-FileCopyrightText: 2011 Ilia Kats <ilia-kats@gmx.net>
    SPDX-FileCopyrightText: 2013 Lukáš Tinkl <ltinkl@redhat.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef OPENVPNAUTH_H
#define OPENVPNAUTH_H

#include <QFormLayout>

#include <NetworkManagerQt/VpnSetting>

#include "settingwidget.h"

class OpenVpnAuthWidget : public SettingWidget
{
    Q_OBJECT
public:
    explicit OpenVpnAuthWidget(const NetworkManager::VpnSetting::Ptr &setting, const QStringList &hints, QWidget *parent = nullptr);
    virtual void readSecrets();
    QVariantMap setting() const override;

private:
    void addPasswordField(const QString &labelText, const QString &password, const QString &secretKey, bool passwordMode = true);

    NetworkManager::VpnSetting::Ptr m_setting;
    QFormLayout *layout;
};

#endif // OPENVPNAUTH_H
