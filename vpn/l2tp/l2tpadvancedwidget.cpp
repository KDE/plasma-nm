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

#include "l2tpadvancedwidget.h"
#include "ui_l2tpadvanced.h"
#include "nm-l2tp-service.h"

#include <QDebug>
#include <KLocalizedString>

L2tpAdvancedWidget::L2tpAdvancedWidget(const NetworkManager::VpnSetting::Ptr &setting, QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::L2tpAdvancedWidget)
{
    m_ui->setupUi(this);

    setWindowTitle(i18n("L2TP IPsec Options"));

    loadConfig(setting);
}

L2tpAdvancedWidget::~L2tpAdvancedWidget()
{
    delete m_ui;
}

void L2tpAdvancedWidget::loadConfig(const NetworkManager::VpnSetting::Ptr &setting)
{
    if (setting->data().value(NM_L2TP_KEY_IPSEC_ENABLE).toInt()) {
        m_ui->cbEnableTunnelToHost->setChecked(true);
        m_ui->gatewayId->setText(setting->data().value(NM_L2TP_KEY_IPSEC_GATEWAY_ID));
        m_ui->groupName->setText(setting->data().value(NM_L2TP_KEY_IPSEC_GROUP_NAME));
        m_ui->presharedKey->setText(setting->data().value(NM_L2TP_KEY_IPSEC_PSK));
    } else {
        m_ui->cbEnableTunnelToHost->setChecked(false);
    }
}

NMStringMap L2tpAdvancedWidget::setting() const
{
    NMStringMap result;

    if (m_ui->cbEnableTunnelToHost->isChecked()) {
        result.insert(NM_L2TP_KEY_IPSEC_ENABLE, "1");

        if (!m_ui->gatewayId->text().isEmpty()) {
            result.insert(NM_L2TP_KEY_IPSEC_GATEWAY_ID, m_ui->gatewayId->text());
        }

        if (!m_ui->groupName->text().isEmpty()) {
            result.insert(NM_L2TP_KEY_IPSEC_GROUP_NAME, m_ui->groupName->text());
        }

        if (!m_ui->presharedKey->text().isEmpty()) {
            result.insert(NM_L2TP_KEY_IPSEC_PSK, m_ui->presharedKey->text());
        }
    } else {
        result.insert(NM_L2TP_KEY_IPSEC_ENABLE, "0");
    }

    return result;
}
