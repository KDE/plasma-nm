/*
    Copyright 2013 Jan Grulich <jgrulich@redhat.com>

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

#include "openswanwidget.h"
#include "ui_openswan.h"
#include "nm-openswan-service.h"

#include <NetworkManagerQt/Setting>

#include <QDBusMetaType>

OpenswanWidget::OpenswanWidget(const NetworkManager::VpnSetting::Ptr &setting, QWidget* parent, Qt::WindowFlags f)
    : SettingWidget(setting, parent, f)
    , m_ui(new Ui::OpenswanWidget)
    , m_setting(setting)
{
    qDBusRegisterMetaType<NMStringMap>();

    m_ui->setupUi(this);

    connect(m_ui->cbUsernamePasswordMode, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &OpenswanWidget::userPasswordTypeChanged);
    connect(m_ui->cbGroupPasswordMode, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &OpenswanWidget::groupPasswordTypeChanged);

    connect(m_ui->gateway, &QLineEdit::textChanged, this, &OpenswanWidget::slotWidgetChanged);
    connect(m_ui->groupname, &QLineEdit::textChanged, this, &OpenswanWidget::slotWidgetChanged);

    KAcceleratorManager::manage(this);

    if (m_setting) {
        loadConfig(setting);
    }
}

OpenswanWidget::~OpenswanWidget()
{
    delete m_ui;
}

void OpenswanWidget::loadConfig(const NetworkManager::Setting::Ptr &setting)
{
    Q_UNUSED(setting);

    const NMStringMap data = m_setting->data();

    const QString gateway = data.value(NM_OPENSWAN_RIGHT);
    if (!gateway.isEmpty()) {
        m_ui->gateway->setText(gateway);
    }

    const QString groupName = data.value(NM_OPENSWAN_LEFTID);
    if (!groupName.isEmpty()) {
        m_ui->groupname->setText(groupName);
    }

    const QString userPasswordMode = data.value(NM_OPENSWAN_XAUTH_PASSWORD_INPUT_MODES);
    if (userPasswordMode == NM_OPENSWAN_PW_TYPE_SAVE) {
        m_ui->cbUsernamePasswordMode->setCurrentIndex(0);
    } else if (userPasswordMode == NM_OPENSWAN_PW_TYPE_ASK) {
        m_ui->cbUsernamePasswordMode->setCurrentIndex(1);
    } else if (userPasswordMode == NM_OPENSWAN_PW_TYPE_UNUSED) {
        m_ui->cbUsernamePasswordMode->setCurrentIndex(2);
    }

    const QString groupPasswordMode = data.value(NM_OPENSWAN_PSK_INPUT_MODES);
    if (groupPasswordMode == NM_OPENSWAN_PW_TYPE_SAVE) {
        m_ui->cbGroupPasswordMode->setCurrentIndex(0);
    } else if (groupPasswordMode == NM_OPENSWAN_PW_TYPE_ASK) {
        m_ui->cbGroupPasswordMode->setCurrentIndex(1);
    } else if (groupPasswordMode == NM_OPENSWAN_PW_TYPE_UNUSED) {
        m_ui->cbGroupPasswordMode->setCurrentIndex(2);
    }

    const QString username = data.value(NM_OPENSWAN_LEFTXAUTHUSER);
    if (!username.isEmpty()) {
        m_ui->username->setText(username);
    }

    const QString phase1 = data.value(NM_OPENSWAN_IKE);
    if (!phase1.isEmpty()) {
        m_ui->phase1->setText(phase1);
    }

    const QString phase2 = data.value(NM_OPENSWAN_ESP);
    if (!phase2.isEmpty()) {
        m_ui->phase2->setText(phase2);
    }

    const QString domain = data.value(NM_OPENSWAN_DOMAIN);
    if (!domain.isEmpty()) {
        m_ui->domain->setText(domain);
    }

    loadSecrets(setting);
}

void OpenswanWidget::loadSecrets(const NetworkManager::Setting::Ptr &setting)
{
    NetworkManager::VpnSetting::Ptr vpnSetting = setting.staticCast<NetworkManager::VpnSetting>();

    if (vpnSetting) {
        const NMStringMap secrets = vpnSetting->secrets();

        const QString userPassword = secrets.value(NM_OPENSWAN_XAUTH_PASSWORD);
        if (!userPassword.isEmpty()) {
            m_ui->userPassword->setText(userPassword);
        }

        const QString groupPassword = secrets.value(NM_OPENSWAN_PSK_VALUE);
        if (!groupPassword.isEmpty()) {
            m_ui->groupPassword->setText(groupPassword);
        }
    }
}

QVariantMap OpenswanWidget::setting(bool agentOwned) const
{
    NetworkManager::VpnSetting setting;
    setting.setServiceType(QLatin1String(NM_DBUS_SERVICE_OPENSWAN));
    NMStringMap data;
    NMStringMap secrets;

    if (!m_ui->gateway->text().isEmpty()) {
        data.insert(NM_OPENSWAN_RIGHT, m_ui->gateway->text());
    }

    if (!m_ui->groupname->text().isEmpty()) {
        data.insert(NM_OPENSWAN_LEFTID, m_ui->groupname->text());
    }

    if (!m_ui->userPassword->text().isEmpty()) {
        secrets.insert(NM_OPENSWAN_XAUTH_PASSWORD, m_ui->userPassword->text());
    }

    const int usernamePasswordMode = m_ui->cbUsernamePasswordMode->currentIndex();
    if (usernamePasswordMode == 0) {
        data.insert(NM_OPENSWAN_XAUTH_PASSWORD_INPUT_MODES, NM_OPENSWAN_PW_TYPE_SAVE);
        if (agentOwned) {
            data.insert(NM_OPENSWAN_XAUTH_PASSWORD"-flags", QString::number(NetworkManager::Setting::AgentOwned));
        } else {
            data.insert(NM_OPENSWAN_XAUTH_PASSWORD"-flags", QString::number(NetworkManager::Setting::None));
        }
    } else if (usernamePasswordMode == 1) {
        data.insert(NM_OPENSWAN_XAUTH_PASSWORD_INPUT_MODES, NM_OPENSWAN_PW_TYPE_ASK);
        data.insert(NM_OPENSWAN_XAUTH_PASSWORD"-flags", QString::number(NetworkManager::Setting::NotSaved));
    } else {
        data.insert(NM_OPENSWAN_XAUTH_PASSWORD_INPUT_MODES, NM_OPENSWAN_PW_TYPE_UNUSED);
        data.insert(NM_OPENSWAN_XAUTH_PASSWORD"-flags", QString::number(NetworkManager::Setting::NotRequired));
    }

    if (!m_ui->groupPassword->text().isEmpty()) {
        secrets.insert(NM_OPENSWAN_PSK_VALUE, m_ui->groupPassword->text());
    }

    const int groupPasswordMode = m_ui->cbGroupPasswordMode->currentIndex();
    if (groupPasswordMode == 0) {
        data.insert(NM_OPENSWAN_PSK_INPUT_MODES, NM_OPENSWAN_PW_TYPE_SAVE);
        if (agentOwned) {
            data.insert(NM_OPENSWAN_PSK_VALUE"-flags", QString::number(NetworkManager::Setting::AgentOwned));
        } else {
            data.insert(NM_OPENSWAN_PSK_VALUE"-flags", QString::number(NetworkManager::Setting::None));
        }
    } else if (groupPasswordMode == 1) {
        data.insert(NM_OPENSWAN_PSK_INPUT_MODES, NM_OPENSWAN_PW_TYPE_ASK);
        data.insert(NM_OPENSWAN_PSK_VALUE"-flags", QString::number(NetworkManager::Setting::NotSaved));
    } else {
        data.insert(NM_OPENSWAN_PSK_INPUT_MODES, NM_OPENSWAN_PW_TYPE_UNUSED);
        data.insert(NM_OPENSWAN_PSK_VALUE"-flags", QString::number(NetworkManager::Setting::NotRequired));
    }

    if (!m_ui->username->text().isEmpty()) {
        data.insert(NM_OPENSWAN_LEFTXAUTHUSER, m_ui->username->text());
    }

    if (!m_ui->phase1->text().isEmpty()) {
        data.insert(NM_OPENSWAN_IKE, m_ui->phase1->text());
    }

    if (!m_ui->phase2->text().isEmpty()) {
        data.insert(NM_OPENSWAN_ESP, m_ui->phase2->text());
    }

    if (!m_ui->domain->text().isEmpty()) {
        data.insert(NM_OPENSWAN_DOMAIN, m_ui->domain->text());
    }

    setting.setData(data);
    setting.setSecrets(secrets);
    return setting.toMap();
}

void OpenswanWidget::userPasswordTypeChanged(int index)
{
    if (index == 1 || index == 2) {
        m_ui->userPassword->setEnabled(false);
    } else {
        m_ui->userPassword->setEnabled(true);
    }
}

void OpenswanWidget::groupPasswordTypeChanged(int index)
{
    if (index == 1 || index == 2) {
        m_ui->groupPassword->setEnabled(false);
    } else {
        m_ui->groupPassword->setEnabled(true);
    }
}

bool OpenswanWidget::isValid() const
{
    return !m_ui->gateway->text().isEmpty() && !m_ui->groupname->text().isEmpty();
}
