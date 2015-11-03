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

#include "pppoewidget.h"
#include "ui_pppoe.h"

#include <NetworkManagerQt/PppoeSetting>

PppoeWidget::PppoeWidget(const NetworkManager::Setting::Ptr &setting, QWidget *parent, Qt::WindowFlags f)
    : SettingWidget(setting, parent, f)
    , m_ui(new Ui::PppoeWidget)
{
    m_ui->setupUi(this);

    KAcceleratorManager::manage(this);

    m_ui->password->setPasswordOptionsEnabled(true);
    m_ui->password->setPasswordOptionEnabled(PasswordField::NotRequired, true);

    connect(m_ui->username, &KLineEdit::textChanged, this, &PppoeWidget::slotWidgetChanged);
    connect(m_ui->password, &PasswordField::passwordOptionChanged, this, &PppoeWidget::slotWidgetChanged);

    if (setting && !setting->isNull()) {
        loadConfig(setting);
    }
}

PppoeWidget::~PppoeWidget()
{
    delete m_ui;
}

void PppoeWidget::loadConfig(const NetworkManager::Setting::Ptr &setting)
{
    NetworkManager::PppoeSetting::Ptr pppoeSetting = setting.staticCast<NetworkManager::PppoeSetting>();

    m_ui->service->setText(pppoeSetting->service());
    m_ui->username->setText(pppoeSetting->username());
    if (pppoeSetting->passwordFlags().testFlag(NetworkManager::Setting::None)) {
        m_ui->password->setPasswordOption(PasswordField::StoreForAllUsers);
    } else if (pppoeSetting->passwordFlags().testFlag(NetworkManager::Setting::AgentOwned)) {
        m_ui->password->setPasswordOption(PasswordField::StoreForUser);
    } else if (pppoeSetting->passwordFlags().testFlag(NetworkManager::Setting::NotSaved)) {
        m_ui->password->setPasswordOption(PasswordField::AlwaysAsk);
    } else {
        m_ui->password->setPasswordOption(PasswordField::NotRequired);
    }

    loadSecrets(setting);
}

void PppoeWidget::loadSecrets(const NetworkManager::Setting::Ptr &setting)
{
    NetworkManager::PppoeSetting::Ptr pppoeSetting = setting.staticCast<NetworkManager::PppoeSetting>();

    if (pppoeSetting) {
        const QString password = pppoeSetting->password();
        if (!password.isEmpty()) {
            m_ui->password->setText(password);
        }
    }
}

QVariantMap PppoeWidget::setting() const
{
    NetworkManager::PppoeSetting pppoeSetting;
    if (!m_ui->service->text().isEmpty()) {
        pppoeSetting.setService(m_ui->service->text());
    }

    if (!m_ui->username->text().isEmpty()) {
        pppoeSetting.setUsername(m_ui->username->text());
    }

    if (!m_ui->password->text().isEmpty()) {
        pppoeSetting.setPassword(m_ui->password->text());
    }

    if (m_ui->password->passwordOption() == PasswordField::StoreForAllUsers) {
        pppoeSetting.setPasswordFlags(NetworkManager::Setting::None);
    } else if (m_ui->password->passwordOption() == PasswordField::StoreForUser) {
        pppoeSetting.setPasswordFlags(NetworkManager::Setting::AgentOwned);
    } else if (m_ui->password->passwordOption() == PasswordField::AlwaysAsk) {
        pppoeSetting.setPasswordFlags(NetworkManager::Setting::NotSaved);
    } else {
        pppoeSetting.setPasswordFlags(NetworkManager::Setting::NotRequired);
    }

    return pppoeSetting.toMap();
}

bool PppoeWidget::isValid() const
{
    bool passwordUserValid = true;

    if (m_ui->password->passwordOption() == PasswordField::StoreForUser ||
        m_ui->password->passwordOption() == PasswordField::StoreForAllUsers) {
        passwordUserValid = !m_ui->username->text().isEmpty() && !m_ui->password->text().isEmpty();
    } else if (m_ui->password->passwordOption() == PasswordField::AlwaysAsk) {
        passwordUserValid = !m_ui->username->text().isEmpty();
    }

    return !m_ui->service->text().isEmpty() && passwordUserValid;
}
