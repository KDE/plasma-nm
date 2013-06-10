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

#include "l2tppppwidget.h"
#include "ui_l2tpppp.h"
#include "nm-l2tp-service.h"

#include <QDebug>
#include <KLocalizedString>

L2tpPPPWidget::L2tpPPPWidget(const NetworkManager::VpnSetting::Ptr &setting, QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::L2tpPppWidget)
{
    m_ui->setupUi(this);

    setWindowTitle(i18n("L2TP PPP Options"));

    loadConfig(setting);
}

L2tpPPPWidget::~L2tpPPPWidget()
{
    delete m_ui;
}

void L2tpPPPWidget::loadConfig(const NetworkManager::VpnSetting::Ptr &setting)
{
    // General settings
    const NMStringMap dataMap = setting->data();

    // Authentication options
    const QString yesString = QLatin1String("yes");
    bool refuse_pap = (dataMap[NM_L2TP_KEY_REFUSE_PAP] == yesString);
    bool refuse_chap = (dataMap[NM_L2TP_KEY_REFUSE_CHAP] == yesString);
    bool refuse_mschap = (dataMap[NM_L2TP_KEY_REFUSE_MSCHAP] == yesString);
    bool refuse_mschapv2 = (dataMap[NM_L2TP_KEY_REFUSE_MSCHAPV2] == yesString);
    bool refuse_eap = (dataMap[NM_L2TP_KEY_REFUSE_EAP] == yesString);

    QListWidgetItem * item = 0;
    item = m_ui->listWidget->item(0); // PAP
    item->setCheckState(refuse_pap ? Qt::Unchecked : Qt::Checked);
    item = m_ui->listWidget->item(1); // CHAP
    item->setCheckState(refuse_chap ? Qt::Unchecked : Qt::Checked);
    item = m_ui->listWidget->item(2); // MSCHAP
    item->setCheckState(refuse_mschap ? Qt::Unchecked : Qt::Checked);
    item = m_ui->listWidget->item(3); // MSCHAPv2
    item->setCheckState(refuse_mschapv2 ? Qt::Unchecked : Qt::Checked);
    item = m_ui->listWidget->item(4); // EAP
    item->setCheckState(refuse_eap ? Qt::Unchecked : Qt::Checked);

    // Cryptography and compression
    const bool mppe = (dataMap[NM_L2TP_KEY_REQUIRE_MPPE] == yesString);
    const bool mppe40 = (dataMap[NM_L2TP_KEY_REQUIRE_MPPE_40] == yesString);
    const bool mppe128 = (dataMap[NM_L2TP_KEY_REQUIRE_MPPE_128] == yesString);
    const bool mppe_stateful = (dataMap[NM_L2TP_KEY_MPPE_STATEFUL] == yesString);

    if (mppe || mppe40 || mppe128) { // If MPPE is use
        m_ui->gbMPPE->setChecked(mppe || mppe40 || mppe128);
        if (mppe128) {
            m_ui->cbMPPECrypto->setCurrentIndex(1); // 128 bit
        }
        else if (mppe40) {
            m_ui->cbMPPECrypto->setCurrentIndex(2); // 40 bit
        }
        else {
            m_ui->cbMPPECrypto->setCurrentIndex(0); // Any
        }
        m_ui->cbstatefulEncryption->setChecked(mppe_stateful);
    }

    const bool nobsd = (dataMap[NM_L2TP_KEY_NOBSDCOMP] == yesString);
    m_ui->cbBSD->setChecked(!nobsd);

    const bool nodeflate = (dataMap[NM_L2TP_KEY_NODEFLATE] == yesString);
    m_ui->cbdeflate->setChecked(!nodeflate);

    const bool novjcomp = (dataMap[NM_L2TP_KEY_NO_VJ_COMP] == yesString);
    m_ui->cbTCPheaders->setChecked(!novjcomp);

    const bool nopcomp = (dataMap[NM_L2TP_KEY_NO_PCOMP] == yesString);
    m_ui->cbCompressionNegotiation->setChecked(!nopcomp);

    const bool noaccomp = (dataMap[NM_L2TP_KEY_NO_ACCOMP] == yesString);
    m_ui->cbAddressControlCompression->setChecked(!noaccomp);

    // Echo
    const int lcp_echo_interval = QString(dataMap[NM_L2TP_KEY_LCP_ECHO_INTERVAL]).toInt();
    m_ui->cbsendEcho->setChecked(lcp_echo_interval > 0);
}

NMStringMap L2tpPPPWidget::setting() const
{
    NMStringMap result;

    QListWidgetItem * item = 0;
    item = m_ui->listWidget->item(0); // PAP
    const QString yesString = QLatin1String("yes");
    if (item->checkState() == Qt::Unchecked)
        result.insert(NM_L2TP_KEY_REFUSE_PAP, yesString);
    item = m_ui->listWidget->item(1); // CHAP
    if (item->checkState() == Qt::Unchecked)
        result.insert(NM_L2TP_KEY_REFUSE_CHAP, yesString);
    item = m_ui->listWidget->item(2); // MSCHAP
    if (item->checkState() == Qt::Unchecked)
        result.insert(NM_L2TP_KEY_REFUSE_MSCHAP, yesString);
    item = m_ui->listWidget->item(3); // MSCHAPv2
    if (item->checkState() == Qt::Unchecked)
        result.insert(NM_L2TP_KEY_REFUSE_MSCHAPV2, yesString);
    item = m_ui->listWidget->item(4); // EAP
    if (item->checkState() == Qt::Unchecked)
        result.insert(NM_L2TP_KEY_REFUSE_EAP, yesString);

    // Cryptography and compression
    if (m_ui->gbMPPE->isChecked()) {
        int index = m_ui->cbMPPECrypto->currentIndex();

        switch (index) {
            case 0: // "Any"
                result.insert(NM_L2TP_KEY_REQUIRE_MPPE, yesString);
                break;
            case 1: // "128 bit"
                result.insert(NM_L2TP_KEY_REQUIRE_MPPE_128, yesString);
                break;
            case 2: // "40 bit"
                result.insert(NM_L2TP_KEY_REQUIRE_MPPE_40, yesString);
                break;
        }

        if (m_ui->cbstatefulEncryption->isChecked()) {
            result.insert(NM_L2TP_KEY_MPPE_STATEFUL, yesString);
        }
    }

    if (!m_ui->cbBSD->isChecked()) {
        result.insert(NM_L2TP_KEY_NOBSDCOMP, yesString);

    }
    if (!m_ui->cbdeflate->isChecked()) {
        result.insert(NM_L2TP_KEY_NODEFLATE, yesString);
    }

    if (!m_ui->cbTCPheaders->isChecked()) {
        result.insert(NM_L2TP_KEY_NO_VJ_COMP, yesString);
    }

    if (!m_ui->cbCompressionNegotiation->isChecked()) {
        result.insert(NM_L2TP_KEY_NO_PCOMP, yesString);
    }

    if (!m_ui->cbAddressControlCompression->isChecked()) {
        result.insert(NM_L2TP_KEY_NO_ACCOMP, yesString);
    }

    // Echo
    if (m_ui->cbsendEcho->isChecked()) {
        result.insert(NM_L2TP_KEY_LCP_ECHO_FAILURE, "5");
        result.insert(NM_L2TP_KEY_LCP_ECHO_INTERVAL, "30");
    }

    qDebug() << "RESULT - " << result;
    return result;
}
