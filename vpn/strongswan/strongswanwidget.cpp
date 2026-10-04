/*
    SPDX-FileCopyrightText: 2008 Will Stephenson <wstephenson@kde.org>
    SPDX-FileCopyrightText: 2010 Maurus Rohrer <maurus.rohrer@gmail.com>
    SPDX-FileCopyrightText: 2013 Lukas Tinkl <ltinkl@redhat.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "strongswanwidget.h"
#include "nm-strongswan-service.h"
#include "ui_strongswanprop.h"

#include <KLocalizedString>

#include <QString>
#include <QUrl>

class StrongswanSettingWidgetPrivate
{
public:
    Ui_StrongswanProp ui;
    NetworkManager::VpnSetting::Ptr setting;
    QString unknownMethod;
    NMStringMap preservedAuth;
    enum AuthType {
        PrivateKey = 0,
        SshAgent,
        Smartcard,
        Eap,
        EapTtls,
        Psk
    };
};

StrongswanSettingWidget::StrongswanSettingWidget(const NetworkManager::VpnSetting::Ptr &setting, QWidget *parent)
    : SettingWidget(setting, parent)
    , d_ptr(new StrongswanSettingWidgetPrivate)
{
    Q_D(StrongswanSettingWidget);
    d->ui.setupUi(this);

    d->setting = setting;

    // EAP-TTLS and PSK reuse the same fields as EAP — redirect stacked widget
    connect(d->ui.cmbMethod, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [d](int index) {
        if (index == StrongswanSettingWidgetPrivate::EapTtls || index == StrongswanSettingWidgetPrivate::Psk) {
            d->ui.swMethods->setCurrentIndex(StrongswanSettingWidgetPrivate::Eap);
        } else if (index > StrongswanSettingWidgetPrivate::Psk) {
            // Preserved unknown method, the certificate fields are the best guess
            d->ui.swMethods->setCurrentIndex(StrongswanSettingWidgetPrivate::PrivateKey);
        }
        d->ui.label_2->setText(index == StrongswanSettingWidgetPrivate::Psk ? i18n("Pre-shared key:") : i18n("User Password:"));
    });

    // Enables combo box for password
    d->ui.leUserPassword->setPasswordOptionsEnabled(true);
    d->ui.leUserPassword->setPasswordNotRequiredEnabled(true);

    // Connect for setting check
    watchChangedSetting();

    // Connect for validity check
    connect(d->ui.leGateway, &QLineEdit::textChanged, this, &StrongswanSettingWidget::slotWidgetChanged);
    connect(d->ui.cmbMethod, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &StrongswanSettingWidget::slotWidgetChanged);
    connect(d->ui.leUserPassword, &PasswordField::textChanged, this, &StrongswanSettingWidget::slotWidgetChanged);
    connect(d->ui.leUserPassword, &PasswordField::passwordOptionChanged, this, &StrongswanSettingWidget::slotWidgetChanged);
    connect(d->ui.proposal, &QGroupBox::toggled, this, &SettingWidget::settingChanged);

    KAcceleratorManager::manage(this);

    if (d->setting && !d->setting->isNull()) {
        loadConfig(d->setting);
    }

    // Report our natural height as minimum so the embedding KCM shows its
    // scroll bar early instead of squeezing the form
    ensurePolished();
    setMinimumHeight(sizeHint().height());
}

StrongswanSettingWidget::~StrongswanSettingWidget()
{
    delete d_ptr;
}

void StrongswanSettingWidget::loadConfig(const NetworkManager::Setting::Ptr &setting)
{
    Q_UNUSED(setting)
    Q_D(StrongswanSettingWidget);

    // General settings
    const NMStringMap dataMap = d->setting->data();
    // Gateway Address
    const QString gateway = dataMap[NM_STRONGSWAN_GATEWAY];
    if (!gateway.isEmpty()) {
        d->ui.leGateway->setText(gateway);
    }
    // Certificate
    d->ui.leGatewayCertificate->setUrl(QUrl::fromLocalFile(dataMap[NM_STRONGSWAN_CERTIFICATE]));

    // Remote Identity
    const QString rIdentity = dataMap[NM_STRONGSWAN_RIDENTITY];
    if (!rIdentity.isEmpty()) {
        d->ui.leRemoteIdentity->setText(rIdentity);
    }

    // Authentication
    const QString method = dataMap[NM_STRONGSWAN_METHOD];
    // Newer versions of the GNOME editor write method "cert" and store the
    // kind of private key in a separate "cert-source" data item
    const QString certSource = dataMap[NM_STRONGSWAN_CERT_SOURCE];
    const bool certMethod = method == QLatin1String(NM_STRONGSWAN_AUTH_CERT);
    if (method == QLatin1String(NM_STRONGSWAN_AUTH_KEY)
        || (certMethod && (certSource.isEmpty() || certSource == QLatin1String(NM_STRONGSWAN_CERT_SOURCE_FILE)))) {
        d->ui.cmbMethod->setCurrentIndex(StrongswanSettingWidgetPrivate::PrivateKey);
        d->ui.leAuthPrivatekeyCertificate->setUrl(QUrl::fromLocalFile(dataMap[NM_STRONGSWAN_USERCERT]));
        d->ui.leAuthPrivatekeyKey->setUrl(QUrl::fromLocalFile(dataMap[NM_STRONGSWAN_USERKEY]));
    } else if (method == QLatin1String(NM_STRONGSWAN_AUTH_AGENT) || (certMethod && certSource == QLatin1String(NM_STRONGSWAN_AUTH_AGENT))) {
        d->ui.cmbMethod->setCurrentIndex(StrongswanSettingWidgetPrivate::SshAgent);
        d->ui.leAuthSshCertificate->setUrl(QUrl::fromLocalFile(dataMap[NM_STRONGSWAN_USERCERT]));
    } else if (method == QLatin1String(NM_STRONGSWAN_AUTH_SMARTCARD) || (certMethod && certSource == QLatin1String(NM_STRONGSWAN_AUTH_SMARTCARD))) {
        d->ui.cmbMethod->setCurrentIndex(StrongswanSettingWidgetPrivate::Smartcard);
    } else if (method == QLatin1String(NM_STRONGSWAN_AUTH_EAP) || method == QLatin1String(NM_STRONGSWAN_AUTH_EAP_TTLS)
               || method == QLatin1String(NM_STRONGSWAN_AUTH_PSK)) {
        if (method == QLatin1String(NM_STRONGSWAN_AUTH_EAP_TTLS)) {
            d->ui.cmbMethod->setCurrentIndex(StrongswanSettingWidgetPrivate::EapTtls);
        } else if (method == QLatin1String(NM_STRONGSWAN_AUTH_PSK)) {
            d->ui.cmbMethod->setCurrentIndex(StrongswanSettingWidgetPrivate::Psk);
        } else {
            d->ui.cmbMethod->setCurrentIndex(StrongswanSettingWidgetPrivate::Eap);
        }
        d->ui.leUserName->setText(dataMap[NM_STRONGSWAN_USER]);

        // The editor asks NetworkManager for secrets only when a *-flags key
        // exists; connections written by other tools may lack it, so inject a
        // default to trigger the asynchronous secret fetch
        if (d->setting && !dataMap.contains(QLatin1String(NM_STRONGSWAN_POPTION))) {
            NMStringMap data = d->setting->data();
            data.insert(QLatin1String(NM_STRONGSWAN_POPTION), QString::number(NetworkManager::Setting::None));
            d->setting->setData(data);
        }

        PasswordField *passwordField = d->ui.leUserPassword;
        const NetworkManager::Setting::SecretFlags type = (NetworkManager::Setting::SecretFlags)dataMap[NM_STRONGSWAN_POPTION].toInt();
        if (type == NetworkManager::Setting::None) {
            passwordField->setPasswordOption(PasswordField::StoreForAllUsers);
        } else if (type.testFlag(NetworkManager::Setting::AgentOwned)) {
            passwordField->setPasswordOption(PasswordField::StoreForUser);
        } else if (type.testFlag(NetworkManager::Setting::NotSaved)) {
            passwordField->setPasswordOption(PasswordField::AlwaysAsk);
        } else if (type.testFlag(NetworkManager::Setting::NotRequired)) {
            passwordField->setPasswordOption(PasswordField::NotRequired);
        }
        // Strongswan always reads the password inside [vpn-secrets] section for global users
        NetworkManager::VpnSetting::Ptr vpnSetting = setting.staticCast<NetworkManager::VpnSetting>();
        const NMStringMap secrets = vpnSetting->secrets();
        d->ui.leUserPassword->setText(secrets.value(NM_STRONGSWAN_SECRET));
    } else if (!method.isEmpty()) {
        // Unknown authentication method (e.g. "eap-tls" written by another
        // editor): present it as-is and keep it on save unless the user
        // selects a different method
        d->unknownMethod = method;
        const QList<QString> authKeys = {
            QStringLiteral(NM_STRONGSWAN_USER),
            QStringLiteral(NM_STRONGSWAN_USERCERT),
            QStringLiteral(NM_STRONGSWAN_USERKEY),
            QStringLiteral(NM_STRONGSWAN_CERT_SOURCE),
            QStringLiteral(NM_STRONGSWAN_LIDENTITY),
            QStringLiteral(NM_STRONGSWAN_POPTION),
        };
        for (const QString &key : authKeys) {
            const QString value = dataMap.value(key);
            if (!value.isEmpty()) {
                d->preservedAuth.insert(key, value);
            }
        }
        d->ui.cmbMethod->addItem(i18nc("@item:inlistbox", "Unknown (%1)", method));
        d->ui.cmbMethod->setCurrentIndex(d->ui.cmbMethod->count() - 1);
        d->ui.leAuthPrivatekeyCertificate->setUrl(QUrl::fromLocalFile(dataMap[NM_STRONGSWAN_USERCERT]));
        d->ui.leAuthPrivatekeyKey->setUrl(QUrl::fromLocalFile(dataMap[NM_STRONGSWAN_USERKEY]));
    }

    // Settings
    d->ui.innerIP->setChecked(dataMap[NM_STRONGSWAN_INNERIP] == "yes");
    d->ui.udpEncap->setChecked(dataMap[NM_STRONGSWAN_ENCAP] == "yes");
    d->ui.ipComp->setChecked(dataMap[NM_STRONGSWAN_IPCOMP] == "yes");
    d->ui.proposal->setChecked(dataMap[NM_STRONGSWAN_PROPOSAL] == "yes");
    d->ui.ike->setText(dataMap[NM_STRONGSWAN_IKE]);
    d->ui.esp->setText(dataMap[NM_STRONGSWAN_ESP]);
}

void StrongswanSettingWidget::loadSecrets(const NetworkManager::Setting::Ptr &setting)
{
    Q_D(StrongswanSettingWidget);

    // The "password" secret holds the EAP password or the pre-shared key
    const int methodIndex = d->ui.cmbMethod->currentIndex();
    if (methodIndex != StrongswanSettingWidgetPrivate::Eap && methodIndex != StrongswanSettingWidgetPrivate::EapTtls
        && methodIndex != StrongswanSettingWidgetPrivate::Psk) {
        return;
    }
    const PasswordField::PasswordOption option = d->ui.leUserPassword->passwordOption();
    if (option != PasswordField::StoreForAllUsers && option != PasswordField::StoreForUser) {
        return;
    }

    const NetworkManager::VpnSetting::Ptr vpnSetting = setting.staticCast<NetworkManager::VpnSetting>();
    const QString password = vpnSetting->secrets().value(QLatin1String(NM_STRONGSWAN_SECRET));
    if (password.isEmpty()) {
        return;
    }
    d->ui.leUserPassword->setText(password);
    Q_EMIT validChanged(isValid());
}

QVariantMap StrongswanSettingWidget::setting() const
{
    Q_D(const StrongswanSettingWidget);

    NetworkManager::VpnSetting setting;
    setting.setServiceType(QLatin1String(NM_DBUS_SERVICE_STRONGSWAN));

    NMStringMap data;
    NMStringMap secretData;

    // General settings
    // Gateway
    if (!d->ui.leGateway->text().isEmpty()) {
        data.insert(NM_STRONGSWAN_GATEWAY, d->ui.leGateway->text());
    }

    const QString certificate = d->ui.leGatewayCertificate->url().toLocalFile();
    if (!certificate.isEmpty()) {
        data.insert(NM_STRONGSWAN_CERTIFICATE, certificate);
    }

    // Server Identity (Right Identity)
    if (!d->ui.leRemoteIdentity->text().isEmpty()) {
        data.insert(NM_STRONGSWAN_RIDENTITY, d->ui.leRemoteIdentity->text());
    }

    // Authentication
    if (!d->unknownMethod.isEmpty() && d->ui.cmbMethod->currentIndex() > StrongswanSettingWidgetPrivate::Psk) {
        // An unknown method was loaded and the user didn't pick a known one:
        // keep it together with its authentication data and secrets so that
        // saving doesn't break the connection
        data.insert(NM_STRONGSWAN_METHOD, d->unknownMethod);
        for (auto it = d->preservedAuth.cbegin(); it != d->preservedAuth.cend(); ++it) {
            data.insert(it.key(), it.value());
        }
        const QString userPrivateCertificate = d->ui.leAuthPrivatekeyCertificate->url().toLocalFile();
        if (!userPrivateCertificate.isEmpty()) {
            data.insert(NM_STRONGSWAN_USERCERT, userPrivateCertificate);
        }
        const QString userKey = d->ui.leAuthPrivatekeyKey->url().toLocalFile();
        if (!userKey.isEmpty()) {
            data.insert(NM_STRONGSWAN_USERKEY, userKey);
        }
        if (d->setting) {
            secretData = d->setting->secrets();
        }
    } else
        switch (d->ui.cmbMethod->currentIndex()) {
        case StrongswanSettingWidgetPrivate::PrivateKey: {
            data.insert(NM_STRONGSWAN_METHOD, NM_STRONGSWAN_AUTH_KEY);
            const QString userPrivateCertificate = d->ui.leAuthPrivatekeyCertificate->url().toLocalFile();
            if (!userPrivateCertificate.isEmpty()) {
                data.insert(NM_STRONGSWAN_USERCERT, userPrivateCertificate);
            }
            const QString userKey = d->ui.leAuthPrivatekeyKey->url().toLocalFile();
            if (!userKey.isEmpty()) {
                data.insert(NM_STRONGSWAN_USERKEY, userKey);
            }
            break;
        }
        case StrongswanSettingWidgetPrivate::SshAgent: {
            data.insert(NM_STRONGSWAN_METHOD, NM_STRONGSWAN_AUTH_AGENT);
            const QString userSshCertificate = d->ui.leAuthSshCertificate->url().toLocalFile();
            if (!userSshCertificate.isEmpty()) {
                data.insert(NM_STRONGSWAN_USERCERT, userSshCertificate);
            }
            break;
        }
        case StrongswanSettingWidgetPrivate::Smartcard:
            data.insert(NM_STRONGSWAN_METHOD, NM_STRONGSWAN_AUTH_SMARTCARD);
            break;
        case StrongswanSettingWidgetPrivate::EapTtls:
        case StrongswanSettingWidgetPrivate::Eap:
        case StrongswanSettingWidgetPrivate::Psk: {
            if (d->ui.cmbMethod->currentIndex() == StrongswanSettingWidgetPrivate::EapTtls) {
                data.insert(NM_STRONGSWAN_METHOD, NM_STRONGSWAN_AUTH_EAP_TTLS);
            } else if (d->ui.cmbMethod->currentIndex() == StrongswanSettingWidgetPrivate::Psk) {
                data.insert(NM_STRONGSWAN_METHOD, NM_STRONGSWAN_AUTH_PSK);
            } else {
                data.insert(NM_STRONGSWAN_METHOD, NM_STRONGSWAN_AUTH_EAP);
            }
            if (!d->ui.leUserName->text().isEmpty()) {
                data.insert(NM_STRONGSWAN_USER, d->ui.leUserName->text());
            }
            // flags for password
            PasswordField *passwordField = d->ui.leUserPassword;
            const PasswordField::PasswordOption option = passwordField->passwordOption();
            if (option == PasswordField::StoreForAllUsers) {
                data.insert(NM_STRONGSWAN_POPTION, QString::number(NetworkManager::Setting::None));
            } else if (option == PasswordField::StoreForUser) {
                data.insert(NM_STRONGSWAN_POPTION, QString::number(NetworkManager::Setting::AgentOwned));
            } else if (option == PasswordField::AlwaysAsk) {
                data.insert(NM_STRONGSWAN_POPTION, QString::number(NetworkManager::Setting::NotSaved));
            } else {
                data.insert(NM_STRONGSWAN_POPTION, QString::number(NetworkManager::Setting::NotRequired));
            }
            // Saving the password; keep an already stored secret when the field
            // was left empty (e.g. it could not be retrieved)
            const bool storeSecret = option == PasswordField::StoreForAllUsers || option == PasswordField::StoreForUser;
            if (storeSecret && d->setting && !d->setting->secrets().isEmpty()) {
                secretData = d->setting->secrets();
            }
            if (!d->ui.leUserPassword->text().isEmpty()) {
                secretData.insert(NM_STRONGSWAN_SECRET, d->ui.leUserPassword->text());
            }
            break;
        }
        }

    // Options
    data.insert(NM_STRONGSWAN_INNERIP, d->ui.innerIP->isChecked() ? "yes" : "no");
    data.insert(NM_STRONGSWAN_ENCAP, d->ui.udpEncap->isChecked() ? "yes" : "no");
    data.insert(NM_STRONGSWAN_IPCOMP, d->ui.ipComp->isChecked() ? "yes" : "no");
    if (d->ui.proposal->isChecked()) {
        data.insert(NM_STRONGSWAN_PROPOSAL, "yes");
        data.insert(NM_STRONGSWAN_IKE, d->ui.ike->text());
        data.insert(NM_STRONGSWAN_ESP, d->ui.esp->text());
    } else
        data.insert(NM_STRONGSWAN_PROPOSAL, "no");

    // save it all
    setting.setData(data);
    setting.setSecrets(secretData);

    return setting.toMap();
}

bool StrongswanSettingWidget::isValid() const
{
    Q_D(const StrongswanSettingWidget);
    if (d->ui.leGateway->text().isEmpty()) {
        return false;
    }
    // charon-nm refuses pre-shared keys shorter than 20 characters
    if (d->ui.cmbMethod->currentIndex() == StrongswanSettingWidgetPrivate::Psk) {
        const PasswordField::PasswordOption option = d->ui.leUserPassword->passwordOption();
        if ((option == PasswordField::StoreForAllUsers || option == PasswordField::StoreForUser) && d->ui.leUserPassword->text().length() < 20) {
            return false;
        }
    }
    return true;
}

#include "moc_strongswanwidget.cpp"
