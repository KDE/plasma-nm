/*
    SPDX-FileCopyrightText: 2013 Jan Grulich <jgrulich@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef PLASMA_NM_L2TP_H
#define PLASMA_NM_L2TP_H

#include "vpnuiplugin.h"

#include <QVariant>

class Q_DECL_EXPORT L2tpUiPlugin : public VpnUiPlugin
{
    Q_OBJECT
public:
    explicit L2tpUiPlugin(QObject * parent = nullptr, const QVariantList& = QVariantList());
    ~L2tpUiPlugin() override;
    SettingWidget * widget(const NetworkManager::VpnSetting::Ptr &setting, QWidget * parent = nullptr) override;
    SettingWidget * askUser(const NetworkManager::VpnSetting::Ptr &setting, QWidget * parent = nullptr) override;

    QString suggestedFileName(const NetworkManager::ConnectionSettings::Ptr &connection) const override;
    QString supportedFileExtensions() const override;
    NMVariantMapMap importConnectionSettings(const QString &fileName) override;
    bool exportConnectionSettings(const NetworkManager::ConnectionSettings::Ptr &connection, const QString &fileName) override;
};

#endif // PLASMA_NM_L2TP_H
