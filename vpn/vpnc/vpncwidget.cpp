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

#include "vpncwidget.h"
#include "vpncadvancedwidget.h"
#include "ui_vpnc.h"
#include "nm-vpnc-service.h"

#include <QDBusMetaType>
#include <QDebug>

VpncWidget::VpncWidget(const NetworkManager::VpnSetting::Ptr &setting, QWidget* parent, Qt::WindowFlags f):
    SettingWidget(setting, parent, f),
    m_ui(new Ui::VpncWidget),
    m_setting(setting)
{
    qDBusRegisterMetaType<NMStringMap>();

    m_ui->setupUi(this);

    connect(m_ui->cboUserPasswordType, SIGNAL(currentIndexChanged(int)), SLOT(userPasswordTypeChanged(int)));
    connect(m_ui->cboGroupPasswordType, SIGNAL(currentIndexChanged(int)), SLOT(groupPasswordTypeChanged(int)));
    connect(m_ui->cbShowPasswords, SIGNAL(toggled(bool)), SLOT(showPasswords(bool)));
    connect(m_ui->btnAdvanced, SIGNAL(clicked()), SLOT(showAdvanced()));

    connect(m_ui->gateway, SIGNAL(textChanged(QString)), SLOT(slotWidgetChanged()));

    KAcceleratorManager::manage(this);

    if (m_setting)
        loadConfig(setting);
}

VpncWidget::~VpncWidget()
{
    m_tmpSetting.clear();
    delete m_ui;
}

void VpncWidget::loadConfig(const NetworkManager::Setting::Ptr &setting)
{
    Q_UNUSED(setting);

    const NMStringMap data = m_setting->data();
    const NMStringMap secrets = m_setting->secrets();

    const QString gateway = data.value(NM_VPNC_KEY_GATEWAY);
    if (!gateway.isEmpty())
        m_ui->gateway->setText(gateway);

    const QString user = data.value(NM_VPNC_KEY_XAUTH_USER);
    if (!user.isEmpty())
        m_ui->user->setText(user);

    const QString userPassword = secrets.value(NM_VPNC_KEY_XAUTH_PASSWORD);
    if (!userPassword.isEmpty())
        m_ui->userPassword->setText(userPassword);

    const NetworkManager::Setting::SecretFlags userPassType =
            static_cast<NetworkManager::Setting::SecretFlags>(data.value(NM_VPNC_KEY_XAUTH_PASSWORD"-flags").toInt());
    if (userPassType.testFlag(NetworkManager::Setting::NotSaved))
        m_ui->cboUserPasswordType->setCurrentIndex(0); // always ask
    else if (userPassType.testFlag(NetworkManager::Setting::NotRequired))
        m_ui->cboUserPasswordType->setCurrentIndex(2); // not required
    else
        m_ui->cboUserPasswordType->setCurrentIndex(1); // saved

    const QString groupName = data.value(NM_VPNC_KEY_ID);
    if (!groupName.isEmpty())
        m_ui->group->setText(groupName);

    const QString groupPassword = secrets.value(NM_VPNC_KEY_SECRET);
    if (!groupPassword.isEmpty())
        m_ui->groupPassword->setText(groupPassword);

    const NetworkManager::Setting::SecretFlags groupPassType =
            static_cast<NetworkManager::Setting::SecretFlags>(data.value(NM_VPNC_KEY_SECRET"-flags").toInt());
    if (groupPassType.testFlag(NetworkManager::Setting::NotSaved))
        m_ui->cboGroupPasswordType->setCurrentIndex(0); // always ask
    else if (groupPassType.testFlag(NetworkManager::Setting::NotRequired))
        m_ui->cboGroupPasswordType->setCurrentIndex(2); // not required
    else
        m_ui->cboGroupPasswordType->setCurrentIndex(1); // saved

    if (data.value(NM_VPNC_KEY_AUTHMODE) == QLatin1String("hybrid")) {
        m_ui->useHybridAuth->setChecked(true);
        m_ui->caFile->setUrl(KUrl(data.value(NM_VPNC_KEY_CA_FILE)));
    }
}

QVariantMap VpncWidget::setting(bool agentOwned) const
{
    NetworkManager::VpnSetting setting;
    setting.setServiceType(QLatin1String(NM_DBUS_SERVICE_VPNC));
    NMStringMap data;
    if (!m_tmpSetting.isNull()) {
        data = m_tmpSetting->data();
    }
    NMStringMap secrets;

    if (!m_ui->gateway->text().isEmpty())
        data.insert(NM_VPNC_KEY_GATEWAY, m_ui->gateway->text());

    if (!m_ui->user->text().isEmpty())
        data.insert(NM_VPNC_KEY_XAUTH_USER, m_ui->user->text());

    if (m_ui->userPassword->isEnabled() && !m_ui->userPassword->text().isEmpty())
        secrets.insert(NM_VPNC_KEY_XAUTH_PASSWORD, m_ui->userPassword->text());

    const int userPasswordTypeIndex =  m_ui->cboUserPasswordType->currentIndex();
    if (userPasswordTypeIndex == 0) { // always ask
        data.insert(NM_VPNC_KEY_XAUTH_PASSWORD"-flags", QString::number(NetworkManager::Setting::NotSaved));
    } else if (userPasswordTypeIndex == 2) { // not required
        data.insert(NM_VPNC_KEY_XAUTH_PASSWORD"-flags", QString::number(NetworkManager::Setting::NotRequired));
    } else { // none
        if (agentOwned) {
            data.insert(NM_VPNC_KEY_XAUTH_PASSWORD"-flags", QString::number(NetworkManager::Setting::AgentOwned));
        } else {
            data.insert(NM_VPNC_KEY_XAUTH_PASSWORD"-flags", QString::number(NetworkManager::Setting::None));
        }
    }

    if (!m_ui->group->text().isEmpty())
        data.insert(NM_VPNC_KEY_ID, m_ui->group->text());

    if (m_ui->groupPassword->isEnabled() && !m_ui->groupPassword->text().isEmpty())
        secrets.insert(NM_VPNC_KEY_SECRET, m_ui->groupPassword->text());

    const int groupPasswordTypeIndex =  m_ui->cboGroupPasswordType->currentIndex();
    if (groupPasswordTypeIndex == SettingWidget::EnumPasswordStorageType::AlwaysAsk) {
        data.insert(NM_VPNC_KEY_SECRET"-flags", QString::number(NetworkManager::Setting::NotSaved));
    } else if (groupPasswordTypeIndex == SettingWidget::EnumPasswordStorageType::NotRequired) {
        data.insert(NM_VPNC_KEY_SECRET"-flags", QString::number(NetworkManager::Setting::NotRequired));
    } else { // none
        if (agentOwned) {
            data.insert(NM_VPNC_KEY_SECRET"-flags", QString::number(NetworkManager::Setting::AgentOwned));
        } else {
            data.insert(NM_VPNC_KEY_SECRET"-flags", QString::number(NetworkManager::Setting::None));
        }
    }

    if (m_ui->useHybridAuth->isChecked() && !m_ui->caFile->url().isEmpty()) {
        data.insert(NM_VPNC_KEY_AUTHMODE, "hybrid");
        data.insert(NM_VPNC_KEY_CA_FILE, m_ui->caFile->url().url());
    }

    setting.setData(data);
    setting.setSecrets(secrets);
    return setting.toMap();
}

void VpncWidget::userPasswordTypeChanged(int index)
{
    m_ui->userPassword->setEnabled(index == SettingWidget::EnumPasswordStorageType::Store);
}

void VpncWidget::groupPasswordTypeChanged(int index)
{
    m_ui->groupPassword->setEnabled(index == SettingWidget::EnumPasswordStorageType::Store);
}

void VpncWidget::showPasswords(bool show)
{
    m_ui->userPassword->setPasswordMode(!show);
    m_ui->groupPassword->setPasswordMode(!show);
}

void VpncWidget::showAdvanced()
{
    QPointer<VpncAdvancedWidget> adv;
    if (m_tmpSetting.isNull()) {
        adv = new VpncAdvancedWidget(m_setting, this);
    } else {
        adv = new VpncAdvancedWidget(m_tmpSetting, this);
    }
    if (adv->exec() == QDialog::Accepted) {
        NMStringMap advData = adv->setting();
        if (!advData.isEmpty()) {
            if (m_tmpSetting.isNull()) {
                m_tmpSetting = NetworkManager::VpnSetting::Ptr(new NetworkManager::VpnSetting);
            }
            m_tmpSetting->setData(advData);
        }
    }

    if (adv) {
        adv->deleteLater();
    }
}

bool VpncWidget::isValid() const
{
    return !m_ui->gateway->text().isEmpty();
}
