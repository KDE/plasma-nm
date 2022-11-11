/*
    SPDX-FileCopyrightText: 2016 Jan Grulich <jgrulich@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "iodineauth.h"

#include "nm-iodine-service.h"

#include <QString>

IodineAuthDialog::IodineAuthDialog(const NetworkManager::VpnSetting::Ptr &setting, const QStringList &hints, QWidget *parent)
    : SettingWidget(setting, hints, parent)
{
    ui.setupUi(this);
    m_setting = setting;

    KAcceleratorManager::manage(this);
}

QVariantMap IodineAuthDialog::setting() const
{
    NMStringMap secrets;
    QVariantMap secretData;

    if (!ui.le_password->text().isEmpty()) {
        secrets.insert(QLatin1String(NM_IODINE_KEY_PASSWORD), ui.le_password->text());
    }

    secretData.insert("secrets", QVariant::fromValue<NMStringMap>(secrets));
    return secretData;
}
