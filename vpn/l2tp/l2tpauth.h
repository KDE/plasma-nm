/*
    SPDX-FileCopyrightText: 2011 Ilia Kats <ilia-kats@gmx.net>
    SPDX-FileCopyrightText: 2013 Lukáš Tinkl <ltinkl@redhat.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef PLASMA_NM_L2TP_AUTH_H
#define PLASMA_NM_L2TP_AUTH_H

#include <QFormLayout>

#include <NetworkManagerQt/VpnSetting>

#include "settingwidget.h"

class L2tpAuthWidget : public SettingWidget
{
    Q_OBJECT
public:
    explicit L2tpAuthWidget(const NetworkManager::VpnSetting::Ptr &setting, const QStringList &hints, QWidget *parent = nullptr);
    virtual void readSecrets();
    QVariantMap setting() const override;

private:
    NetworkManager::VpnSetting::Ptr m_setting;
    QFormLayout *layout;
};

#endif // PLASMA_NM_L2TP_AUTH_H
