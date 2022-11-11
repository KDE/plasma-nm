/*
    SPDX-FileCopyrightText: 2011 Ilia Kats <ilia-kats@gmx.net>
    SPDX-FileCopyrightText: 2013 Lukáš Tinkl <ltinkl@redhat.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "pptpauth.h"

#include "nm-pptp-service.h"

class PptpAuthWidgetPrivate
{
public:
};

PptpAuthWidget::PptpAuthWidget(const NetworkManager::VpnSetting::Ptr &setting, const QStringList &hints, QWidget *parent)
    : SettingWidget(setting, hints, parent)
{
    m_setting = setting;
    ui.setupUi(this);

    KAcceleratorManager::manage(this);
}

QVariantMap PptpAuthWidget::setting() const
{
    NMStringMap secrets;
    QVariantMap secretData;

    if (!ui.lePassword->text().isEmpty()) {
        secrets.insert(QLatin1String(NM_PPTP_KEY_PASSWORD), ui.lePassword->text());
    }

    secretData.insert("secrets", QVariant::fromValue<NMStringMap>(secrets));
    return secretData;
}
