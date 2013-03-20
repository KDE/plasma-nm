/*
    Copyright 2012-2013  Jan Grulich <jgrulich@redhat.com>

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

#include <QtNetworkManager/settings/ppp.h>

#include "pppwidget.h"
#include "ui_ppp.h"


PPPWidget::PPPWidget(NetworkManager::Settings::Setting * setting, QWidget* parent, Qt::WindowFlags f):
    SettingWidget(setting, parent, f),
    m_ui(new Ui::PPPWidget)
{
    m_ui->setupUi(this);

    if (setting)
        loadConfig(setting);
}

PPPWidget::~PPPWidget()
{
}

void PPPWidget::loadConfig(NetworkManager::Settings::Setting *setting)
{
    NetworkManager::Settings::PppSetting * pppSetting = static_cast<NetworkManager::Settings::PppSetting *>(setting);

    m_ui->eap->setChecked(!pppSetting->refuseEap());
    m_ui->pap->setChecked(!pppSetting->refusePap());
    m_ui->chap->setChecked(!pppSetting->refuseChap());
    m_ui->mschap->setChecked(!pppSetting->refuseMschap());
    m_ui->mschapv2->setChecked(!pppSetting->refuseMschapv2());

    m_ui->mppe->setChecked(pppSetting->requireMppe());
    m_ui->mppe128->setChecked(pppSetting->requireMppe128());
    m_ui->mppeStateful->setChecked(pppSetting->mppeStateful());

    m_ui->bsdComp->setChecked(!pppSetting->noBsdComp());
    m_ui->deflateComp->setChecked(!pppSetting->noDeflate());
    m_ui->tcpComp->setChecked(!pppSetting->noVjComp());
}

QVariantMap PPPWidget::setting() const
{
    NetworkManager::Settings::PppSetting pppSetting;

    pppSetting.setRefuseEap(!m_ui->eap->isChecked());
    pppSetting.setRefusePap(!m_ui->pap->isChecked());
    pppSetting.setRefuseChap(!m_ui->chap->isChecked());
    pppSetting.setRefuseMschap(!m_ui->mschap->isChecked());
    pppSetting.setRefuseMschapv2(!m_ui->mschapv2->isChecked());

    pppSetting.setRequireMppe(m_ui->mppe->isChecked());
    pppSetting.setRequireMppe128(m_ui->mppe128->isChecked());
    pppSetting.setMppeStateful(m_ui->mppeStateful->isChecked());

    pppSetting.setNoBsdComp(!m_ui->bsdComp->isChecked());
    pppSetting.setNoDeflate(!m_ui->deflateComp->isChecked());
    pppSetting.setNoVjComp(!m_ui->tcpComp->isChecked());

    return pppSetting.toMap();
}
