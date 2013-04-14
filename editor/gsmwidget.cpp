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

#include <KLocalizedString>

#include <QtNetworkManager/settings/gsm.h>

#include "gsmwidget.h"
#include "ui_gsm.h"


GsmWidget::GsmWidget(const NetworkManager::Settings::Setting::Ptr &setting, QWidget* parent, Qt::WindowFlags f):
    SettingWidget(setting, parent, f),
    m_ui(new Ui::GsmWidget)
{
    m_ui->setupUi(this);

    // Network ID not supported yet in NM
    m_ui->labelNetworkId->setHidden(true);
    m_ui->networkId->setHidden(true);

    m_ui->type->addItem(i18nc("GSM network type", "Any"), NetworkManager::Settings::GsmSetting::Any);
    m_ui->type->addItem(i18n("3G Only (UMTS/HSPA)"), NetworkManager::Settings::GsmSetting::Only3G);
    m_ui->type->addItem(i18n("2G Only (GPRS/EDGE)"), NetworkManager::Settings::GsmSetting::GprsEdgeOnly);
    m_ui->type->addItem(i18n("Prefer 3G (UMTS/HSPA)"), NetworkManager::Settings::GsmSetting::Prefer3G);
    m_ui->type->addItem(i18n("Prefer 2G (GPRS/EDGE)"), NetworkManager::Settings::GsmSetting::Prefer2G);
    m_ui->type->addItem(i18n("Prefer 4G (LTE)"), NetworkManager::Settings::GsmSetting::Prefer4GLte);
    m_ui->type->addItem(i18n("4G Only (LTE)"), NetworkManager::Settings::GsmSetting::Only4GLte);

    connect(m_ui->cbShowPasswords, SIGNAL(toggled(bool)), SLOT(showPasswords(bool)));

    // TODO make the Change... button relaunch the wizard

    if (setting)
        loadConfig(setting);
}

GsmWidget::~GsmWidget()
{
}

void GsmWidget::loadConfig(const NetworkManager::Settings::Setting::Ptr &setting)
{
    NetworkManager::Settings::GsmSetting::Ptr gsmSetting = setting.staticCast<NetworkManager::Settings::GsmSetting>();
    const QString number = gsmSetting->number();
    if (!number.isEmpty())
        m_ui->number->setText(number);
    m_ui->username->setText(gsmSetting->username());
    m_ui->password->setText(gsmSetting->password());

    m_ui->apn->setText(gsmSetting->apn());
    m_ui->networkId->setText(gsmSetting->networkId());
    if (gsmSetting->networkType() != NetworkManager::Settings::GsmSetting::Any)
        m_ui->type->setCurrentIndex(m_ui->type->findData(static_cast<int>(gsmSetting->networkType())));
    m_ui->roaming->setChecked(!gsmSetting->homeOnly());
    m_ui->pin->setText(gsmSetting->pin());
}

QVariantMap GsmWidget::setting(bool agentOwned) const
{
    NetworkManager::Settings::GsmSetting gsmSetting;
    if (!m_ui->number->text().isEmpty())
        gsmSetting.setNumber(m_ui->number->text());
    if (!m_ui->username->text().isEmpty())
        gsmSetting.setUsername(m_ui->username->text());
    if (!m_ui->password->text().isEmpty())
        gsmSetting.setPassword(m_ui->password->text());

    if (agentOwned) {
        gsmSetting.setPasswordFlags(NetworkManager::Settings::Setting::AgentOwned);
    }

    if (!m_ui->apn->text().isEmpty())
        gsmSetting.setApn(m_ui->apn->text());
    if (!m_ui->networkId->text().isEmpty())
        gsmSetting.setNetworkId(m_ui->networkId->text());
    gsmSetting.setNetworkType(static_cast<NetworkManager::Settings::GsmSetting::NetworkType>(m_ui->type->itemData(m_ui->type->currentIndex()).toInt()));
    gsmSetting.setHomeOnly(!m_ui->roaming->isChecked());
    if (!m_ui->pin->text().isEmpty())
        gsmSetting.setPin(m_ui->pin->text());

    if (agentOwned) {
        gsmSetting.setPinFlags(NetworkManager::Settings::Setting::AgentOwned);
    }

    return gsmSetting.toMap();
}

void GsmWidget::showPasswords(bool show)
{
    m_ui->password->setPasswordMode(!show);
    m_ui->pin->setPasswordMode(!show);
}
