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

#include "cdmawidget.h"
#include "ui_cdma.h"

#include <NetworkManagerQt/CdmaSetting>

CdmaWidget::CdmaWidget(const NetworkManager::Setting::Ptr &setting, QWidget* parent, Qt::WindowFlags f):
    SettingWidget(setting, parent, f),
    m_ui(new Ui::CdmaWidget)
{
    m_ui->setupUi(this);

    connect(m_ui->cbShowPassword, SIGNAL(toggled(bool)), SLOT(showPassword(bool)));
    connect(m_ui->passwordStorage, SIGNAL(currentIndexChanged(int)), SLOT(passwordStorageChanged(int)));

    connect(m_ui->number, SIGNAL(textChanged(QString)), SLOT(slotWidgetChanged()));
    connect(m_ui->password, SIGNAL(textChanged(QString)), SLOT(slotWidgetChanged()));
    connect(m_ui->passwordStorage, SIGNAL(currentIndexChanged(int)), SLOT(slotWidgetChanged()));
    connect(m_ui->username, SIGNAL(textChanged(QString)), SLOT(slotWidgetChanged()));

    KAcceleratorManager::manage(this);

    if (setting)
        loadConfig(setting);
}

CdmaWidget::~CdmaWidget()
{
    delete m_ui;
}

void CdmaWidget::loadConfig(const NetworkManager::Setting::Ptr &setting)
{
    NetworkManager::CdmaSetting::Ptr cdmaSetting = setting.staticCast<NetworkManager::CdmaSetting>();
    const QString number = cdmaSetting->number();
    if (!number.isEmpty())
        m_ui->number->setText(number);
    m_ui->username->setText(cdmaSetting->username());
    if ((!cdmaSetting->password().isEmpty() && cdmaSetting->passwordFlags().testFlag(NetworkManager::Setting::None)) ||
        cdmaSetting->passwordFlags().testFlag(NetworkManager::Setting::AgentOwned)) {
        m_ui->passwordStorage->setCurrentIndex(SettingWidget::EnumPasswordStorageType::Store);
        m_ui->password->setText(cdmaSetting->password());
    } else if (cdmaSetting->passwordFlags().testFlag(NetworkManager::Setting::NotSaved)) {
        m_ui->passwordStorage->setCurrentIndex(SettingWidget::EnumPasswordStorageType::AlwaysAsk);
    } else {
        m_ui->passwordStorage->setCurrentIndex(SettingWidget::EnumPasswordStorageType::NotRequired);
    }
}

QVariantMap CdmaWidget::setting(bool agentOwned) const
{
    NetworkManager::CdmaSetting cdmaSetting;
    if (!m_ui->number->text().isEmpty())
        cdmaSetting.setNumber(m_ui->number->text());
    if (!m_ui->username->text().isEmpty())
        cdmaSetting.setUsername(m_ui->username->text());
    if (!m_ui->password->text().isEmpty())
        cdmaSetting.setPassword(m_ui->password->text());

    if (m_ui->passwordStorage->currentIndex() == SettingWidget::EnumPasswordStorageType::Store) {
        if (agentOwned) {
            cdmaSetting.setPasswordFlags(NetworkManager::Setting::AgentOwned);
        }
    } else if (m_ui->passwordStorage->currentIndex() == SettingWidget::EnumPasswordStorageType::AlwaysAsk) {
        cdmaSetting.setPasswordFlags(NetworkManager::Setting::NotSaved);
    } else {
        cdmaSetting.setPasswordFlags(NetworkManager::Setting::NotRequired);
    }

    return cdmaSetting.toMap();
}

void CdmaWidget::showPassword(bool show)
{
    m_ui->password->setPasswordMode(!show);
}

void CdmaWidget::passwordStorageChanged(int index)
{
    m_ui->password->setEnabled(index == SettingWidget::EnumPasswordStorageType::Store);
}

bool CdmaWidget::isValid() const
{
    bool passwordUserValid = true;

    if (m_ui->passwordStorage->currentIndex() == SettingWidget::EnumPasswordStorageType::Store) {
        passwordUserValid = !m_ui->username->text().isEmpty() && !m_ui->password->text().isEmpty();
    } else if (m_ui->passwordStorage->currentIndex() == SettingWidget::EnumPasswordStorageType::AlwaysAsk) {
        passwordUserValid = !m_ui->username->text().isEmpty();
    }

    return !m_ui->number->text().isEmpty() && passwordUserValid;
}
