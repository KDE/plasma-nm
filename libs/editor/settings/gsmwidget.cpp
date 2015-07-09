/*
    Copyright 2013, 2014 Lukas Tinkl <ltinkl@redhat.com>

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

#include "gsmwidget.h"
#include "ui_gsm.h"

#include <KLocalizedString>

#include <NetworkManagerQt/GsmSetting>

GsmWidget::GsmWidget(const NetworkManager::Setting::Ptr &setting, QWidget* parent, Qt::WindowFlags f):
    SettingWidget(setting, parent, f),
    m_ui(new Ui::GsmWidget)
{
    m_ui->setupUi(this);

    // Network ID not supported yet in NM
    m_ui->labelNetworkId->setHidden(true);
    m_ui->networkId->setHidden(true);

    m_ui->type->addItem(i18nc("GSM network type", "Any"), NetworkManager::GsmSetting::Any);
    m_ui->type->addItem(i18n("3G Only (UMTS/HSPA)"), NetworkManager::GsmSetting::Only3G);
    m_ui->type->addItem(i18n("2G Only (GPRS/EDGE)"), NetworkManager::GsmSetting::GprsEdgeOnly);
    m_ui->type->addItem(i18n("Prefer 3G (UMTS/HSPA)"), NetworkManager::GsmSetting::Prefer3G);
    m_ui->type->addItem(i18n("Prefer 2G (GPRS/EDGE)"), NetworkManager::GsmSetting::Prefer2G);
    m_ui->type->addItem(i18n("Prefer 4G (LTE)"), NetworkManager::GsmSetting::Prefer4GLte);
    m_ui->type->addItem(i18n("4G Only (LTE)"), NetworkManager::GsmSetting::Only4GLte);

    connect(m_ui->cbShowPasswords, &QCheckBox::toggled, this, &GsmWidget::showPasswords);
    connect(m_ui->pinStorage, static_cast<void (KComboBox::*)(int)>(&KComboBox::currentIndexChanged), this, &GsmWidget::pinStorageChanged);
    connect(m_ui->passwordStorage, static_cast<void (KComboBox::*)(int)>(&KComboBox::currentIndexChanged), this, &GsmWidget::passwordStorageChanged);

    connect(m_ui->apn, &KLineEdit::textChanged, this, &GsmWidget::slotWidgetChanged);
    connect(m_ui->password, &KLineEdit::textChanged, this, &GsmWidget::slotWidgetChanged);
    connect(m_ui->pin, &KLineEdit::textChanged, this, &GsmWidget::slotWidgetChanged);
    connect(m_ui->username, &KLineEdit::textChanged, this, &GsmWidget::slotWidgetChanged);
    connect(m_ui->passwordStorage, static_cast<void (KComboBox::*)(int)>(&KComboBox::currentIndexChanged), this, &GsmWidget::slotWidgetChanged);
    connect(m_ui->pinStorage, static_cast<void (KComboBox::*)(int)>(&KComboBox::currentIndexChanged), this, &GsmWidget::slotWidgetChanged);

    KAcceleratorManager::manage(this);

    if (setting)
        loadConfig(setting);
}

GsmWidget::~GsmWidget()
{
    delete m_ui;
}

void GsmWidget::loadConfig(const NetworkManager::Setting::Ptr &setting)
{
    NetworkManager::GsmSetting::Ptr gsmSetting = setting.staticCast<NetworkManager::GsmSetting>();

    const QString number = gsmSetting->number();
    if (!number.isEmpty())
        m_ui->number->setText(number);
    m_ui->username->setText(gsmSetting->username());
    if (gsmSetting->passwordFlags().testFlag(NetworkManager::Setting::None) ||
        gsmSetting->passwordFlags().testFlag(NetworkManager::Setting::AgentOwned)) {
        m_ui->passwordStorage->setCurrentIndex(SettingWidget::EnumPasswordStorageType::Store);
    } else if (gsmSetting->passwordFlags().testFlag(NetworkManager::Setting::NotSaved)) {
        m_ui->passwordStorage->setCurrentIndex(SettingWidget::EnumPasswordStorageType::AlwaysAsk);
    } else {
        m_ui->passwordStorage->setCurrentIndex(SettingWidget::EnumPasswordStorageType::NotRequired);
    }
    m_ui->apn->setText(gsmSetting->apn());
    m_ui->networkId->setText(gsmSetting->networkId());
    if (gsmSetting->networkType() != NetworkManager::GsmSetting::Any)
        m_ui->type->setCurrentIndex(m_ui->type->findData(static_cast<int>(gsmSetting->networkType())));
    m_ui->roaming->setChecked(!gsmSetting->homeOnly());
    if (gsmSetting->pinFlags() == NetworkManager::Setting::None ||
        gsmSetting->pinFlags() == NetworkManager::Setting::AgentOwned) {
        m_ui->pinStorage->setCurrentIndex(SettingWidget::EnumPasswordStorageType::Store);
    } else if (gsmSetting->pinFlags() == NetworkManager::Setting::NotSaved) {
        m_ui->pinStorage->setCurrentIndex(SettingWidget::EnumPasswordStorageType::AlwaysAsk);
    } else {
        m_ui->pinStorage->setCurrentIndex(SettingWidget::EnumPasswordStorageType::NotRequired);
    }

    loadSecrets(setting);
}

void GsmWidget::loadSecrets(const NetworkManager::Setting::Ptr &setting)
{
    NetworkManager::GsmSetting::Ptr gsmSetting = setting.staticCast<NetworkManager::GsmSetting>();

    if (gsmSetting) {
        const QString password = gsmSetting->password();
        if (!password.isEmpty()) {
            m_ui->password->setText(password);
        }

        const QString pin = gsmSetting->pin();
        if (!pin.isEmpty()) {
            m_ui->pin->setText(pin);
        }
    }
}

QVariantMap GsmWidget::setting(bool agentOwned) const
{
    NetworkManager::GsmSetting gsmSetting;
    if (!m_ui->number->text().isEmpty())
        gsmSetting.setNumber(m_ui->number->text());
    if (!m_ui->username->text().isEmpty())
        gsmSetting.setUsername(m_ui->username->text());
    if (!m_ui->password->text().isEmpty())
        gsmSetting.setPassword(m_ui->password->text());
    if (m_ui->passwordStorage->currentIndex() == SettingWidget::EnumPasswordStorageType::Store) {
        if (agentOwned) {
            gsmSetting.setPasswordFlags(NetworkManager::Setting::AgentOwned);
        }
    } else if (m_ui->passwordStorage->currentIndex() == SettingWidget::EnumPasswordStorageType::AlwaysAsk) {
        gsmSetting.setPasswordFlags(NetworkManager::Setting::NotSaved);
    } else {
        gsmSetting.setPasswordFlags(NetworkManager::Setting::NotRequired);
    }

    if (!m_ui->apn->text().isEmpty())
        gsmSetting.setApn(m_ui->apn->text());
    if (!m_ui->networkId->text().isEmpty())
        gsmSetting.setNetworkId(m_ui->networkId->text());
    gsmSetting.setNetworkType(static_cast<NetworkManager::GsmSetting::NetworkType>(m_ui->type->itemData(m_ui->type->currentIndex()).toInt()));
    gsmSetting.setHomeOnly(!m_ui->roaming->isChecked());
    if (!m_ui->pin->text().isEmpty())
        gsmSetting.setPin(m_ui->pin->text());
    if (m_ui->pinStorage->currentIndex() == SettingWidget::EnumPasswordStorageType::Store) {
        if (agentOwned) {
            gsmSetting.setPinFlags(NetworkManager::Setting::AgentOwned);
        }
    } else if (m_ui->pinStorage->currentIndex() == SettingWidget::EnumPasswordStorageType::AlwaysAsk) {
        gsmSetting.setPinFlags(NetworkManager::Setting::NotSaved);
    } else {
        gsmSetting.setPinFlags(NetworkManager::Setting::NotRequired);
    }

    return gsmSetting.toMap();
}

void GsmWidget::showPasswords(bool show)
{
    m_ui->password->setPasswordMode(!show);
    m_ui->pin->setPasswordMode(!show);
}

void GsmWidget::pinStorageChanged(int index)
{
    m_ui->pin->setEnabled(index == SettingWidget::EnumPasswordStorageType::Store);
}

void GsmWidget::passwordStorageChanged(int index)
{
    m_ui->password->setEnabled(index == SettingWidget::EnumPasswordStorageType::Store);
}

bool GsmWidget::isValid() const
{
    bool passwordUserValid = true;
    bool pinValid = true;

    if (m_ui->passwordStorage->currentIndex() == SettingWidget::EnumPasswordStorageType::Store) {
        passwordUserValid = !m_ui->username->text().isEmpty() && !m_ui->password->text().isEmpty();
    } else if (m_ui->passwordStorage->currentIndex() == SettingWidget::EnumPasswordStorageType::AlwaysAsk) {
        passwordUserValid = !m_ui->username->text().isEmpty();
    }

    if (m_ui->pinStorage->currentIndex() == SettingWidget::EnumPasswordStorageType::Store) {
        pinValid = !m_ui->pin->text().isEmpty();
    }

    return !m_ui->apn->text().isEmpty() && passwordUserValid && pinValid;
}
