/*
    SPDX-FileCopyrightText: 2009 Will Stephenson <wstephenson@kde.org>
    SPDX-FileCopyrightText: 2010 Maurus Rohrer <maurus.rohrer@gmail.com>
    SPDX-FileCopyrightText: 2013 Lukas Tinkl <ltinkl@redhat.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef PLASMANM_STRONGSWAN_H
#define PLASMANM_STRONGSWAN_H

#include "vpnuiplugin.h"

#include <QVariant>

class Q_DECL_EXPORT StrongswanUiPlugin : public VpnUiPlugin
{
Q_OBJECT
public:
    explicit StrongswanUiPlugin(QObject * parent = nullptr, const QVariantList& = QVariantList());
    ~StrongswanUiPlugin() override;
    SettingWidget * widget(const NetworkManager::VpnSetting::Ptr &setting, QWidget * parent = nullptr) override;
    SettingWidget * askUser(const NetworkManager::VpnSetting::Ptr &setting, const QStringList &hints, QWidget * parent = nullptr) override;
    QString suggestedFileName(const NetworkManager::ConnectionSettings::Ptr &connection) const override;
    QString supportedFileExtensions() const override;

    NMVariantMapMap importConnectionSettings(const QString &fileName) override;
    bool exportConnectionSettings(const NetworkManager::ConnectionSettings::Ptr &connection, const QString &fileName) override;
};

#endif //  PLASMANM_STRONGSWAN_H
