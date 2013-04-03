/*
    Copyright 2013 Lukas Tinkl <ltinkl@redhat.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QDBusMetaType>

#include "nm-vpnc-service.h"
#include "vpncwidget.h"
#include "ui_vpnc.h"


VpncWidget::VpncWidget(NetworkManager::Settings::Setting * setting, QWidget* parent, Qt::WindowFlags f):
    SettingWidget(setting, parent, f),
    m_ui(new Ui::VpncWidget)
{
    qDBusRegisterMetaType<QStringMap>();

    m_setting = static_cast<NetworkManager::Settings::VpnSetting *>(setting);
    m_ui->setupUi(this);

    connect(m_ui->cboUserPasswordType, SIGNAL(currentIndexChanged(int)), SLOT(userPasswordTypeChanged(int)));
    connect(m_ui->cboGroupPasswordType, SIGNAL(currentIndexChanged(int)), SLOT(groupPasswordTypeChanged(int)));

    if (m_setting)
        loadConfig(setting);
}

VpncWidget::~VpncWidget()
{
}

void VpncWidget::loadConfig(NetworkManager::Settings::Setting *setting)
{
    Q_UNUSED(setting);

    const QStringMap data = m_setting->data();
    const QStringMap secrets = m_setting->secrets();

    const QString gateway = data.value(NM_VPNC_KEY_GATEWAY);
    if (!gateway.isEmpty())
        m_ui->gateway->setText(gateway);

    const QString user = data.value(NM_VPNC_KEY_XAUTH_USER);
    if (!user.isEmpty())
        m_ui->user->setText(user);

    const QString userPassword = secrets.value(NM_VPNC_KEY_XAUTH_PASSWORD);
    if (!userPassword.isEmpty())
        m_ui->userPassword->setText(userPassword);

    const NetworkManager::Settings::Setting::SecretFlags userPassType =
            static_cast<NetworkManager::Settings::Setting::SecretFlags>(data.value(NM_VPNC_KEY_XAUTH_PASSWORD"-flags").toInt());
    if (userPassType.testFlag(NetworkManager::Settings::Setting::NotSaved))
        m_ui->cboUserPasswordType->setCurrentIndex(0); // always ask
    else if (userPassType.testFlag(NetworkManager::Settings::Setting::NotRequired))
        m_ui->cboUserPasswordType->setCurrentIndex(2); // not required
    else
        m_ui->cboUserPasswordType->setCurrentIndex(1); // saved

    const QString groupName = data.value(NM_VPNC_KEY_ID);
    if (!groupName.isEmpty())
        m_ui->group->setText(groupName);

    const QString groupPassword = secrets.value(NM_VPNC_KEY_SECRET);
    if (!groupPassword.isEmpty())
        m_ui->groupPassword->setText(groupPassword);

    const NetworkManager::Settings::Setting::SecretFlags groupPassType =
            static_cast<NetworkManager::Settings::Setting::SecretFlags>(data.value(NM_VPNC_KEY_SECRET"-flags").toInt());
    if (groupPassType.testFlag(NetworkManager::Settings::Setting::NotSaved))
        m_ui->cboGroupPasswordType->setCurrentIndex(0); // always ask
    else if (groupPassType.testFlag(NetworkManager::Settings::Setting::NotRequired))
        m_ui->cboGroupPasswordType->setCurrentIndex(2); // not required
    else
        m_ui->cboGroupPasswordType->setCurrentIndex(1); // saved

    if (data.value(NM_VPNC_KEY_AUTHMODE) == QLatin1String("hybrid")) {
        m_ui->useHybridAuth->setChecked(true);
        m_ui->caFile->setUrl(KUrl(data.value(NM_VPNC_KEY_CA_FILE)));
    }
}

QVariantMap VpncWidget::setting() const
{

    return m_setting->toMap();
}

void VpncWidget::userPasswordTypeChanged(int index)
{
    if (index == 0 || index == 2) {
        m_ui->userPassword->setEnabled(false);
    }
    else {
        m_ui->userPassword->setEnabled(true);
    }
}

void VpncWidget::groupPasswordTypeChanged(int index)
{
    if (index == 0 || index == 2) {
        m_ui->groupPassword->setEnabled(false);
    }
    else {
        m_ui->groupPassword->setEnabled(true);
    }
}

void VpncWidget::showPasswords(bool show)
{
    m_ui->userPassword->setPasswordMode(!show);
    m_ui->groupPassword->setPasswordMode(!show);
}
