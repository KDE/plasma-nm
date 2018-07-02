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

#include "l2tpwidget.h"
#include "l2tpadvancedwidget.h"
#include "l2tppppwidget.h"
#include "ui_l2tp.h"
#include "nm-l2tp-service.h"

#include <NetworkManagerQt/Setting>

#include <QPointer>
#include <QDBusMetaType>

L2tpWidget::L2tpWidget(const NetworkManager::VpnSetting::Ptr &setting, QWidget* parent, Qt::WindowFlags f)
    : SettingWidget(setting, parent, f)
    , m_ui(new Ui::L2tpWidget)
    , m_setting(setting)
{
    qDBusRegisterMetaType<NMStringMap>();

    m_ui->setupUi(this);

    m_ui->password->setPasswordOptionsEnabled(true);

    connect(m_ui->btnIPSecSettings, &QPushButton::clicked, this, &L2tpWidget::showAdvanced);
    connect(m_ui->btnPPPSettings, &QPushButton::clicked, this, &L2tpWidget::showPpp);

    // Connect for setting check
    watchChangedSetting();

    // Connect for validity check
    connect(m_ui->gateway, &QLineEdit::textChanged, this, &L2tpWidget::slotWidgetChanged);
    connect(m_ui->cbUseCertificate, &QCheckBox::stateChanged, this, &L2tpWidget::certStateChanged);

    KAcceleratorManager::manage(this);

    if (setting && !setting->isNull()) {
        loadConfig(setting);
    }
}

L2tpWidget::~L2tpWidget()
{
    m_tmpAdvancedSetting.clear();
    m_tmpPppSetting.clear();
    delete m_ui;
}

void L2tpWidget::loadConfig(const NetworkManager::Setting::Ptr &setting)
{
    Q_UNUSED(setting);

    const NMStringMap data = m_setting->data();

    if (data.contains(NM_L2TP_KEY_GATEWAY)) {
        m_ui->gateway->setText(data.value(NM_L2TP_KEY_GATEWAY));
    }

    if (data.contains(NM_L2TP_KEY_USER)) {
        m_ui->username->setText(data.value(NM_L2TP_KEY_USER));
    }

    const NetworkManager::Setting::SecretFlags userPassType = static_cast<NetworkManager::Setting::SecretFlags>(data.value(NM_L2TP_KEY_PASSWORD"-flags").toInt());
    if (userPassType.testFlag(NetworkManager::Setting::None)) {
        m_ui->password->setPasswordOption(PasswordField::StoreForAllUsers);
    } else if (userPassType.testFlag(NetworkManager::Setting::AgentOwned)) {
        m_ui->password->setPasswordOption(PasswordField::StoreForUser);
    } else {
        m_ui->password->setPasswordOption(PasswordField::AlwaysAsk);
    }

    if (data.contains(NM_L2TP_KEY_DOMAIN)) {
        m_ui->domain->setText(data.value(NM_L2TP_KEY_DOMAIN));
    }

    if (data.contains(NM_L2TP_KEY_CERT_CA)) {
        m_ui->urCACertificate->setText(data.value(NM_L2TP_KEY_CERT_CA));
    }

    if (data.contains(NM_L2TP_KEY_CERT_PUB)) {
        m_ui->urCertificate->setText(data.value(NM_L2TP_KEY_CERT_PUB));
    }

    if (data.contains(NM_L2TP_KEY_CERT_KEY)) {
        m_ui->urPrivateKey->setText(data.value(NM_L2TP_KEY_CERT_KEY));
    }

    if (data.value(NM_L2TP_KEY_USE_CERT) == QLatin1String("yes")) {
        m_ui->cbUseCertificate->setChecked(true);
    }

    loadSecrets(setting);
}

void L2tpWidget::loadSecrets(const NetworkManager::Setting::Ptr &setting)
{
    NetworkManager::VpnSetting::Ptr vpnSetting = setting.staticCast<NetworkManager::VpnSetting>();

    if (vpnSetting) {
        const NMStringMap secrets = vpnSetting->secrets();
        const QString userPassword = secrets.value(NM_L2TP_KEY_PASSWORD);
        if (!userPassword.isEmpty()) {
            m_ui->password->setText(userPassword);
        }
    }
}

QVariantMap L2tpWidget::setting() const
{
    NetworkManager::VpnSetting setting;
    setting.setServiceType(QLatin1String(NM_DBUS_SERVICE_L2TP));
    NMStringMap data;
    if (!m_tmpAdvancedSetting.isNull()) {
        data = m_tmpAdvancedSetting->data();
    } else {
        // retrieve the settings if the dialog has not been opened
        QScopedPointer<L2tpAdvancedWidget> adv(new L2tpAdvancedWidget(m_setting, nullptr));
        data = adv->setting();
    }

    if (!m_tmpPppSetting.isNull()) {
        data.unite(m_tmpPppSetting->data());
    } else {
        // retrieve the settings if the dialog has not been opened
        QScopedPointer<L2tpPPPWidget> ppp(new L2tpPPPWidget(m_setting, nullptr));
        data.unite(ppp->setting());
    }

    NMStringMap secrets;

    if (!m_ui->gateway->text().isEmpty()) {
        data.insert(NM_L2TP_KEY_GATEWAY, m_ui->gateway->text());
    }

    if (m_ui->cbUseCertificate->isChecked()) {
        data.insert(NM_L2TP_KEY_USE_CERT, "yes");

        if (!m_ui->urCACertificate->text().isEmpty()) {
            data.insert(NM_L2TP_KEY_CERT_CA, m_ui->urCACertificate->text());
        }

        if (!m_ui->urCertificate->text().isEmpty()) {
            data.insert(NM_L2TP_KEY_CERT_PUB, m_ui->urCertificate->text());
        }

        if (!m_ui->urPrivateKey->text().isEmpty()) {
            data.insert(NM_L2TP_KEY_CERT_KEY, m_ui->urPrivateKey->text());
        }

        data.insert(NM_L2TP_KEY_PASSWORD"-flags", QString::number(NetworkManager::Setting::NotRequired));

    } else {

        if (!m_ui->username->text().isEmpty()) {
            data.insert(NM_L2TP_KEY_USER, m_ui->username->text());
        }

        if (m_ui->password->isEnabled() && !m_ui->password->text().isEmpty()) {
            secrets.insert(NM_L2TP_KEY_PASSWORD, m_ui->password->text());
        }

        switch (m_ui->password->passwordOption()) {
        case PasswordField::StoreForAllUsers:
            data.insert(NM_L2TP_KEY_PASSWORD"-flags", QString::number(NetworkManager::Setting::None));
            break;
        case PasswordField::StoreForUser:
            data.insert(NM_L2TP_KEY_PASSWORD"-flags", QString::number(NetworkManager::Setting::AgentOwned));
            break;
        default:
            data.insert(NM_L2TP_KEY_PASSWORD"-flags", QString::number(NetworkManager::Setting::NotSaved));
        };

        if (!m_ui->domain->text().isEmpty()) {
            data.insert(NM_L2TP_KEY_DOMAIN, m_ui->domain->text());
        }
    }

    setting.setData(data);
    setting.setSecrets(secrets);
    return setting.toMap();
}


void L2tpWidget::userPasswordTypeChanged(int index)
{
    m_ui->password->setEnabled(index == SettingWidget::EnumPasswordStorageType::Store);
}

void L2tpWidget::showAdvanced()
{
    QPointer<L2tpAdvancedWidget> adv;
    if (m_tmpAdvancedSetting.isNull()) {
        adv = new L2tpAdvancedWidget(m_setting, this);
    } else {
        adv = new L2tpAdvancedWidget(m_tmpAdvancedSetting, this);
    }
    connect(adv.data(), &L2tpAdvancedWidget::accepted,
            [adv, this] () {
                NMStringMap advData = adv->setting();
                if (!advData.isEmpty()) {
                    if (m_tmpAdvancedSetting.isNull()) {
                        m_tmpAdvancedSetting = NetworkManager::VpnSetting::Ptr(new NetworkManager::VpnSetting);
                    }
                    m_tmpAdvancedSetting->setData(advData);
                }
            });
    connect(adv.data(), &L2tpAdvancedWidget::finished,
            [adv] () {
                if (adv) {
                    adv->deleteLater();
                }
            });
    adv->setModal(true);
    adv->show();
}

void L2tpWidget::showPpp()
{
    QPointer<L2tpPPPWidget> adv;
    if (m_tmpPppSetting.isNull()) {
        adv = new L2tpPPPWidget(m_setting, this);
    } else {
        adv = new L2tpPPPWidget(m_tmpPppSetting, this);
    }
    connect(adv.data(), &L2tpPPPWidget::accepted,
            [adv, this] () {
                NMStringMap advData = adv->setting();
                if (!advData.isEmpty()) {
                    if (m_tmpPppSetting.isNull()) {
                        m_tmpPppSetting = NetworkManager::VpnSetting::Ptr(new NetworkManager::VpnSetting);
                    }
                    m_tmpPppSetting->setData(advData);
                }
            });
    connect(adv.data(), &L2tpPPPWidget::finished,
            [adv] () {
                if (adv) {
                    adv->deleteLater();
                }
            });
    adv->setModal(true);
    adv->show();
}

bool L2tpWidget::isValid() const
{
    return !m_ui->gateway->text().isEmpty();
}

void L2tpWidget::certStateChanged()
{
    if (m_ui->cbUseCertificate->isChecked()) {
        m_ui->urCACertificate->setEnabled(true);
        m_ui->urCertificate->setEnabled(true);
        m_ui->urPrivateKey->setEnabled(true);
        m_ui->username->setEnabled(false);
        m_ui->password->setEnabled(false);
        m_ui->domain->setEnabled(false);
    } else {
        m_ui->urCACertificate->setEnabled(false);
        m_ui->urCertificate->setEnabled(false);
        m_ui->urPrivateKey->setEnabled(false);
        m_ui->username->setEnabled(true);
        m_ui->password->setEnabled(true);
        m_ui->domain->setEnabled(true);
    }
}
