/*
    Copyright (c) 2013 Lukas Tinkl <ltinkl@redhat.com>

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

#include "security802-1x.h"
#include "ui_802-1x.h"
#include "editlistdialog.h"
#include "listvalidator.h"

#include <KAcceleratorManager>
#include <KLocalizedString>

#include <QtCrypto>

Security8021x::Security8021x(const NetworkManager::Security8021xSetting::Ptr &setting, bool wifiMode, QWidget *parent) :
    QWidget(parent),
    m_setting(setting),
    m_ui(new Ui::Security8021x)
{
    m_ui->setupUi(this);

    m_ui->auth->setItemData(0, NetworkManager::Security8021xSetting::EapMethodMd5);
    m_ui->auth->setItemData(1, NetworkManager::Security8021xSetting::EapMethodTls);
    m_ui->auth->setItemData(2, NetworkManager::Security8021xSetting::EapMethodLeap);
    m_ui->auth->setItemData(3, NetworkManager::Security8021xSetting::EapMethodFast);
    m_ui->auth->setItemData(4, NetworkManager::Security8021xSetting::EapMethodTtls);
    m_ui->auth->setItemData(5, NetworkManager::Security8021xSetting::EapMethodPeap);

    if (wifiMode) {
        m_ui->auth->removeItem(m_ui->auth->findData(NetworkManager::Security8021xSetting::EapMethodMd5)); // MD 5
        m_ui->stackedWidget->removeWidget(m_ui->md5Page);
    } else {
        m_ui->auth->removeItem(m_ui->auth->findData(NetworkManager::Security8021xSetting::EapMethodLeap)); // LEAP
        m_ui->stackedWidget->removeWidget(m_ui->leapPage);
    }

    connect(m_ui->btnTlsAltSubjectMatches, &QPushButton::clicked, this, &Security8021x::altSubjectMatchesButtonClicked);
    connect(m_ui->btnTlsConnectToServers, &QPushButton::clicked, this, &Security8021x::connectToServersButtonClicked);

    // Those signals are monitor for setting validation
    connect(m_ui->auth, static_cast<void (KComboBox::*)(int)>(&KComboBox::currentIndexChanged), this, &Security8021x::widgetChanged);
    connect(m_ui->md5UserName, &KLineEdit::textChanged, this, &Security8021x::widgetChanged);
    connect(m_ui->md5Password, &KLineEdit::textChanged, this, &Security8021x::widgetChanged);
    connect(m_ui->cbAskMd5Password, &QCheckBox::stateChanged, this, &Security8021x::widgetChanged);
    connect(m_ui->tlsIdentity, &KLineEdit::textChanged, this, &Security8021x::widgetChanged);
    connect(m_ui->tlsCACert, &KUrlRequester::textChanged, this, &Security8021x::widgetChanged);
    connect(m_ui->tlsUserCert, &KUrlRequester::textChanged, this, &Security8021x::widgetChanged);
    connect(m_ui->tlsPrivateKey, &KUrlRequester::textChanged, this, &Security8021x::widgetChanged);
    connect(m_ui->tlsPrivateKeyPassword, &KLineEdit::textChanged, this, &Security8021x::widgetChanged);
    connect(m_ui->leapUsername, &KLineEdit::textChanged, this, &Security8021x::widgetChanged);
    connect(m_ui->leapPassword, &KLineEdit::textChanged, this, &Security8021x::widgetChanged);
    connect(m_ui->fastAllowPacProvisioning, &QCheckBox::stateChanged, this, &Security8021x::widgetChanged);
    connect(m_ui->pacFile, &KUrlRequester::textChanged, this, &Security8021x::widgetChanged);
    connect(m_ui->fastUsername, &KLineEdit::textChanged, this, &Security8021x::widgetChanged);
    connect(m_ui->fastPassword, &KLineEdit::textChanged, this, &Security8021x::widgetChanged);
    connect(m_ui->cbAskFastPassword, &QCheckBox::stateChanged, this, &Security8021x::widgetChanged);
    connect(m_ui->ttlsCACert, &KUrlRequester::textChanged, this, &Security8021x::widgetChanged);
    connect(m_ui->ttlsUsername, &KLineEdit::textChanged, this, &Security8021x::widgetChanged);
    connect(m_ui->ttlsPassword, &KLineEdit::textChanged, this, &Security8021x::widgetChanged);
    connect(m_ui->cbAskTtlsPassword, &QCheckBox::stateChanged, this, &Security8021x::widgetChanged);
    connect(m_ui->peapCACert, &KUrlRequester::textChanged, this, &Security8021x::widgetChanged);
    connect(m_ui->peapUsername, &KLineEdit::textChanged, this, &Security8021x::widgetChanged);
    connect(m_ui->peapPassword, &KLineEdit::textChanged, this, &Security8021x::widgetChanged);
    connect(m_ui->cbAskPeapPassword, &QCheckBox::stateChanged, this, &Security8021x::widgetChanged);

    KAcceleratorManager::manage(this);
    connect(m_ui->stackedWidget, &QStackedWidget::currentChanged, this, &Security8021x::currentAuthChanged);

    altSubjectValidator = new QRegExpValidator(QRegExp(QLatin1String("^(DNS:[a-zA-Z0-9_-]+\\.[a-zA-Z0-9_.-]+|EMAIL:[a-zA-Z0-9._-]+@[a-zA-Z0-9_-]+\\.[a-zA-Z0-9_.-]+|URI:[a-zA-Z0-9.+-]+:.+|)$")), this);
    serversValidator = new QRegExpValidator(QRegExp(QLatin1String("^[a-zA-Z0-9_-]+\\.[a-zA-Z0-9_.-]+$")), this);

    ListValidator *altSubjectListValidator = new ListValidator(this);
    altSubjectListValidator->setInnerValidator(altSubjectValidator);
    m_ui->leTlsSubjectMatch->setValidator(altSubjectListValidator);

    ListValidator *serverListValidator = new ListValidator(this);
    serverListValidator->setInnerValidator(serversValidator);
    m_ui->leTlsConnectToServers->setValidator(serverListValidator);

    if (m_setting) {
        loadConfig();
    }
}

Security8021x::~Security8021x()
{
    delete m_ui;
}

void Security8021x::loadSecrets(const NetworkManager::Security8021xSetting::Ptr &setting)
{
    const QString password = setting->password();
    const QList<NetworkManager::Security8021xSetting::EapMethod> eapMethods = m_setting->eapMethods();

    if (!password.isEmpty()) {
        if (eapMethods.contains(NetworkManager::Security8021xSetting::EapMethodMd5)) {
            m_ui->md5Password->setText(setting->password());
        } else if (eapMethods.contains(NetworkManager::Security8021xSetting::EapMethodLeap)) {
            m_ui->leapPassword->setText(setting->password());
        } else if (eapMethods.contains(NetworkManager::Security8021xSetting::EapMethodFast)) {
            m_ui->fastPassword->setText(setting->password());
        } else if (eapMethods.contains(NetworkManager::Security8021xSetting::EapMethodTtls)) {
            m_ui->ttlsPassword->setText(setting->password());
        } else if (eapMethods.contains(NetworkManager::Security8021xSetting::EapMethodPeap)) {
            m_ui->peapPassword->setText(setting->password());
        }
    }

    if (eapMethods.contains(NetworkManager::Security8021xSetting::EapMethodTls)) {
        const QString privateKeyPassword = setting->privateKeyPassword();
        if (!privateKeyPassword.isEmpty()) {
            m_ui->tlsPrivateKeyPassword->setText(setting->privateKeyPassword());
        }
    }
}

void Security8021x::loadConfig()
{
    const QList<NetworkManager::Security8021xSetting::EapMethod> eapMethods = m_setting->eapMethods();
    const NetworkManager::Security8021xSetting::AuthMethod phase2AuthMethod = m_setting->phase2AuthMethod();
    const bool notSavedPassword = m_setting->passwordFlags() & NetworkManager::Setting::NotSaved;

    if (eapMethods.contains(NetworkManager::Security8021xSetting::EapMethodMd5)) {
        m_ui->auth->setCurrentIndex(m_ui->auth->findData(NetworkManager::Security8021xSetting::EapMethodMd5));
        m_ui->md5UserName->setText(m_setting->identity());
        m_ui->cbAskMd5Password->setChecked(notSavedPassword);
    } else if (eapMethods.contains(NetworkManager::Security8021xSetting::EapMethodTls)) {
        QStringList servers;
        m_ui->auth->setCurrentIndex(m_ui->auth->findData(NetworkManager::Security8021xSetting::EapMethodTls));
        m_ui->tlsIdentity->setText(m_setting->identity());
        m_ui->tlsUserCert->setUrl(QUrl::fromLocalFile(m_setting->clientCertificate()));
        m_ui->tlsCACert->setUrl(QUrl::fromLocalFile(m_setting->caCertificate()));
        m_ui->leTlsSubjectMatch->setText(m_setting->subjectMatch());
        m_ui->leTlsAlternativeSubjectMatches->setText(m_setting->altSubjectMatches().join(QLatin1String(", ")));
        Q_FOREACH (const QString &match, m_setting->altSubjectMatches()) {
            if (match.startsWith(QLatin1String("DNS:"))) {
                servers.append(match.right(match.length()-4));
            }
        }
        m_ui->leTlsConnectToServers->setText(servers.join(QLatin1String(", ")));
        m_ui->tlsPrivateKey->setUrl(QUrl::fromLocalFile(m_setting->privateKey()));
    } else if (eapMethods.contains(NetworkManager::Security8021xSetting::EapMethodLeap)) {
        m_ui->auth->setCurrentIndex(m_ui->auth->findData(NetworkManager::Security8021xSetting::EapMethodLeap));
        m_ui->leapUsername->setText(m_setting->identity());
    } else if (eapMethods.contains(NetworkManager::Security8021xSetting::EapMethodFast)) {
        m_ui->auth->setCurrentIndex(m_ui->auth->findData(NetworkManager::Security8021xSetting::EapMethodFast));
        m_ui->fastAnonIdentity->setText(m_setting->anonymousIdentity());
        m_ui->fastAllowPacProvisioning->setChecked((int)m_setting->phase1FastProvisioning() > 0);
        m_ui->pacMethod->setCurrentIndex(m_setting->phase1FastProvisioning() - 1);
        m_ui->pacFile->setUrl(QUrl::fromLocalFile(m_setting->pacFile()));
        if (phase2AuthMethod == NetworkManager::Security8021xSetting::AuthMethodGtc) {
            m_ui->fastInnerAuth->setCurrentIndex(0);
        } else {
            m_ui->fastInnerAuth->setCurrentIndex(1);
        }
        m_ui->fastUsername->setText(m_setting->identity());
        m_ui->cbAskFastPassword->setChecked(notSavedPassword);
    } else if (eapMethods.contains(NetworkManager::Security8021xSetting::EapMethodTtls)) {
        m_ui->auth->setCurrentIndex(m_ui->auth->findData(NetworkManager::Security8021xSetting::EapMethodTtls));
        m_ui->ttlsAnonIdentity->setText(m_setting->anonymousIdentity());
        m_ui->ttlsCACert->setUrl(QUrl::fromLocalFile(m_setting->caCertificate()));
        if (phase2AuthMethod == NetworkManager::Security8021xSetting::AuthMethodPap) {
            m_ui->ttlsInnerAuth->setCurrentIndex(0);
        } else if (phase2AuthMethod == NetworkManager::Security8021xSetting::AuthMethodMschap) {
            m_ui->ttlsInnerAuth->setCurrentIndex(1);
        } else if (phase2AuthMethod == NetworkManager::Security8021xSetting::AuthMethodMschapv2) {
            m_ui->ttlsInnerAuth->setCurrentIndex(2);
        } else if (phase2AuthMethod == NetworkManager::Security8021xSetting::AuthMethodChap) {
            m_ui->ttlsInnerAuth->setCurrentIndex(3);
        }
        m_ui->ttlsUsername->setText(m_setting->identity());
        m_ui->cbAskTtlsPassword->setChecked(notSavedPassword);
    } else if (eapMethods.contains(NetworkManager::Security8021xSetting::EapMethodPeap)) {
        m_ui->auth->setCurrentIndex(m_ui->auth->findData(NetworkManager::Security8021xSetting::EapMethodPeap));
        m_ui->peapAnonIdentity->setText(m_setting->anonymousIdentity());
        m_ui->peapCACert->setUrl(QUrl::fromLocalFile(m_setting->caCertificate()));
        m_ui->peapVersion->setCurrentIndex(m_setting->phase1PeapVersion() + 1);
        if (phase2AuthMethod == NetworkManager::Security8021xSetting::AuthMethodMschapv2) {
            m_ui->peapInnerAuth->setCurrentIndex(0);
        } else if (phase2AuthMethod == NetworkManager::Security8021xSetting::AuthMethodMd5) {
            m_ui->peapInnerAuth->setCurrentIndex(1);
        } else if (phase2AuthMethod == NetworkManager::Security8021xSetting::AuthMethodGtc) {
            m_ui->peapInnerAuth->setCurrentIndex(2);
        }
        m_ui->peapUsername->setText(m_setting->identity());
        m_ui->cbAskPeapPassword->setChecked(notSavedPassword);
    }

    loadSecrets(m_setting);
}

QVariantMap Security8021x::setting(bool agentOwned) const
{
    NetworkManager::Security8021xSetting setting;

    NetworkManager::Security8021xSetting::EapMethod method =
            static_cast<NetworkManager::Security8021xSetting::EapMethod>(m_ui->auth->itemData(m_ui->auth->currentIndex()).toInt());

    setting.setEapMethods(QList<NetworkManager::Security8021xSetting::EapMethod>() << method);

    if (method == NetworkManager::Security8021xSetting::EapMethodMd5) {
        if (!m_ui->md5UserName->text().isEmpty()) {
            setting.setIdentity(m_ui->md5UserName->text());
        }

        if (m_ui->cbAskMd5Password->isChecked()) {
            setting.setPasswordFlags(NetworkManager::Setting::NotSaved);
        } else if (!m_ui->md5Password->text().isEmpty()) {
            setting.setPassword(m_ui->md5Password->text());
        }

        if (agentOwned && !m_ui->cbAskMd5Password->isChecked()) {
            setting.setPasswordFlags(NetworkManager::Setting::AgentOwned);
        }
    } else if (method == NetworkManager::Security8021xSetting::EapMethodTls) {
        if (!m_ui->tlsIdentity->text().isEmpty()) {
            setting.setIdentity(m_ui->tlsIdentity->text());
        }

        if (m_ui->tlsUserCert->url().isValid()) {
            setting.setClientCertificate(m_ui->tlsUserCert->url().toString().toUtf8().append('\0'));
        }

        if (m_ui->tlsCACert->url().isValid()) {
            setting.setCaCertificate(m_ui->tlsCACert->url().toString().toUtf8().append('\0'));
        }

        QStringList altsubjectmatches = m_ui->leTlsAlternativeSubjectMatches->text().remove(QLatin1Char(' ')).split(QLatin1Char(','), QString::SkipEmptyParts);
        Q_FOREACH (const QString &match, m_ui->leTlsConnectToServers->text().remove(QLatin1Char(' ')).split(QLatin1Char(','), QString::SkipEmptyParts)) {
            const QString tempstr = QLatin1String("DNS:") + match;
            if (!altsubjectmatches.contains(tempstr)) {
                altsubjectmatches.append(tempstr);
            }
        }
        setting.setSubjectMatch(m_ui->leTlsSubjectMatch->text());
        setting.setAltSubjectMatches(altsubjectmatches);

        if (m_ui->tlsPrivateKey->url().isValid()) {
            setting.setPrivateKey(m_ui->tlsPrivateKey->url().toString().toUtf8().append('\0'));
        }

        if (!m_ui->tlsPrivateKeyPassword->text().isEmpty()) {
            setting.setPrivateKeyPassword(m_ui->tlsPrivateKeyPassword->text());
        }

        QCA::Initializer init;
        QCA::ConvertResult convRes;

        // Try if the private key is in pkcs12 format bundled with client certificate
        if (QCA::isSupported("pkcs12")) {
            QCA::KeyBundle keyBundle = QCA::KeyBundle::fromFile(m_ui->tlsPrivateKey->url().path(), m_ui->tlsPrivateKeyPassword->text().toUtf8(), &convRes);
            // Set client certificate to the same path as private key
            if (convRes == QCA::ConvertGood && keyBundle.privateKey().canDecrypt()) {
                setting.setClientCertificate(m_ui->tlsPrivateKey->url().toString().toUtf8().append('\0'));
            }
        }

        if (agentOwned) {
            setting.setPrivateKeyPasswordFlags(NetworkManager::Setting::AgentOwned);
        }
    } else if (method == NetworkManager::Security8021xSetting::EapMethodLeap) {
        if (!m_ui->leapUsername->text().isEmpty()) {
            setting.setIdentity(m_ui->leapUsername->text());
        }

        if (!m_ui->leapPassword->text().isEmpty()) {
            setting.setPassword(m_ui->leapPassword->text());
        }

        if (agentOwned) {
            setting.setPasswordFlags(NetworkManager::Setting::AgentOwned);
        }
    } else if (method == NetworkManager::Security8021xSetting::EapMethodFast) {
        if (!m_ui->fastAnonIdentity->text().isEmpty()) {
            setting.setAnonymousIdentity(m_ui->fastAnonIdentity->text());
        }

        if (!m_ui->fastAllowPacProvisioning->isChecked()) {
            setting.setPhase1FastProvisioning(NetworkManager::Security8021xSetting::FastProvisioningDisabled);
        } else {
            setting.setPhase1FastProvisioning(static_cast<NetworkManager::Security8021xSetting::FastProvisioning>(m_ui->pacMethod->currentIndex() + 1));
        }

        if (m_ui->pacFile->url().isValid()) {
            setting.setPacFile(QFile::encodeName(m_ui->pacFile->url().toLocalFile()));
        }

        if (m_ui->fastInnerAuth->currentIndex() == 0) {
            setting.setPhase2AuthMethod(NetworkManager::Security8021xSetting::AuthMethodGtc);
        } else {
            setting.setPhase2AuthMethod(NetworkManager::Security8021xSetting::AuthMethodMschapv2);
        }

        if (!m_ui->fastUsername->text().isEmpty()) {
            setting.setIdentity(m_ui->fastUsername->text());
        }

        if (m_ui->cbAskFastPassword->isChecked()) {
            setting.setPasswordFlags(NetworkManager::Setting::NotSaved);
        } else if (!m_ui->fastPassword->text().isEmpty()) {
            setting.setPassword(m_ui->fastPassword->text());
        }

        if (agentOwned && !m_ui->cbAskFastPassword->isChecked()) {
            setting.setPasswordFlags(NetworkManager::Setting::AgentOwned);
        }
    } else if (method == NetworkManager::Security8021xSetting::EapMethodTtls) {
        if (!m_ui->ttlsAnonIdentity->text().isEmpty()) {
            setting.setAnonymousIdentity(m_ui->ttlsAnonIdentity->text());
        }

        if (m_ui->ttlsCACert->url().isValid()) {
            setting.setCaCertificate(m_ui->ttlsCACert->url().toString().toUtf8().append('\0'));
        }

        const int innerAuth = m_ui->ttlsInnerAuth->currentIndex();
        if (innerAuth == 0) {
            setting.setPhase2AuthMethod(NetworkManager::Security8021xSetting::AuthMethodPap);
        } else if (innerAuth == 1) {
            setting.setPhase2AuthMethod(NetworkManager::Security8021xSetting::AuthMethodMschap);
        } else if (innerAuth == 2) {
            setting.setPhase2AuthMethod(NetworkManager::Security8021xSetting::AuthMethodMschapv2);
        } else if (innerAuth == 3) {
            setting.setPhase2AuthMethod(NetworkManager::Security8021xSetting::AuthMethodChap);
        }

        if (!m_ui->ttlsUsername->text().isEmpty()) {
            setting.setIdentity(m_ui->ttlsUsername->text());
        }

        if (m_ui->cbAskTtlsPassword->isChecked()) {
            setting.setPasswordFlags(NetworkManager::Setting::NotSaved);
        } else if (!m_ui->ttlsPassword->text().isEmpty()) {
            setting.setPassword(m_ui->ttlsPassword->text());
        }

        if (agentOwned && !m_ui->cbAskTtlsPassword->isChecked()) {
            setting.setPasswordFlags(NetworkManager::Setting::AgentOwned);
        }
    } else if (method == NetworkManager::Security8021xSetting::EapMethodPeap) {
        if (!m_ui->peapAnonIdentity->text().isEmpty()) {
            setting.setAnonymousIdentity(m_ui->peapAnonIdentity->text());
        }

        if (m_ui->peapCACert->url().isValid()) {
            setting.setCaCertificate(m_ui->peapCACert->url().toString().toUtf8().append('\0'));
        }

        setting.setPhase1PeapVersion(static_cast<NetworkManager::Security8021xSetting::PeapVersion>(m_ui->peapVersion->currentIndex() - 1));
        const int innerAuth = m_ui->peapInnerAuth->currentIndex();
        if (innerAuth == 0) {
            setting.setPhase2AuthMethod(NetworkManager::Security8021xSetting::AuthMethodMschapv2);
        } else if (innerAuth == 1) {
            setting.setPhase2AuthMethod(NetworkManager::Security8021xSetting::AuthMethodMd5);
        } else if (innerAuth == 2) {
            setting.setPhase2AuthMethod(NetworkManager::Security8021xSetting::AuthMethodGtc);
        }

        if (m_ui->cbAskPeapPassword->isChecked()) {
            setting.setPasswordFlags(NetworkManager::Setting::NotSaved);
        } else if (!m_ui->peapPassword->text().isEmpty()) {
            setting.setPassword(m_ui->peapPassword->text());
        }

        if (!m_ui->peapUsername->text().isEmpty()) {
            setting.setIdentity(m_ui->peapUsername->text());
        }

        if (agentOwned && !m_ui->cbAskPeapPassword->isChecked()) {
            setting.setPasswordFlags(NetworkManager::Setting::AgentOwned);
        }
    }

    return setting.toMap();
}

void Security8021x::altSubjectMatchesButtonClicked()
{
    QPointer<EditListDialog> editor = new EditListDialog(this);

    editor->setItems(m_ui->leTlsSubjectMatch->text().remove(QLatin1Char(' ')).split(QLatin1Char(','), QString::SkipEmptyParts));
    editor->setWindowTitle(i18n("Alternative Subject Matches"));
    editor->setToolTip(i18n("<qt>This entry must be one of:<ul><li>DNS: &lt;name or ip address&gt;</li><li>EMAIL: &lt;email&gt;</li><li>URI: &lt;uri, e.g. http://www.kde.org&gt;</li></ul></qt>"));
    editor->setValidator(altSubjectValidator);

    connect(editor.data(), &QDialog::accepted,
            [editor, this] () {
                m_ui->leTlsSubjectMatch->setText(editor->items().join(QLatin1String(", ")));
            });
    connect(editor.data(), &QDialog::finished,
            [editor] () {
                if (editor) {
                    editor->deleteLater();
                }
            });
    editor->setModal(true);
    editor->show();
}

void Security8021x::connectToServersButtonClicked()
{
    QPointer<EditListDialog> editor = new EditListDialog(this);

    editor->setItems(m_ui->leTlsConnectToServers->text().remove(QLatin1Char(' ')).split(QLatin1Char(','), QString::SkipEmptyParts));
    editor->setWindowTitle(i18n("Connect to these servers only"));
    editor->setValidator(serversValidator);

    connect(editor.data(), &QDialog::accepted,
            [editor, this] () {
                m_ui->leTlsConnectToServers->setText(editor->items().join(QLatin1String(", ")));
            });
    connect(editor.data(), &QDialog::finished,
            [editor] () {
                if (editor) {
                    editor->deleteLater();
                }
            });
    editor->setModal(true);
    editor->show();
}

bool Security8021x::isValid() const
{
    NetworkManager::Security8021xSetting::EapMethod method =
            static_cast<NetworkManager::Security8021xSetting::EapMethod>(m_ui->auth->itemData(m_ui->auth->currentIndex()).toInt());

    if (method == NetworkManager::Security8021xSetting::EapMethodMd5) {
        return !m_ui->md5UserName->text().isEmpty() && (!m_ui->md5Password->text().isEmpty() || m_ui->cbAskMd5Password->isChecked());
    } else if (method == NetworkManager::Security8021xSetting::EapMethodTls) {
        if (m_ui->tlsIdentity->text().isEmpty()) {
            return false;
        }

        if (!m_ui->tlsPrivateKey->url().isValid()) {
            return false;
        }

        if (m_ui->tlsPrivateKeyPassword->text().isEmpty()) {
            return false;
        }

        QCA::Initializer init;
        QCA::ConvertResult convRes;

        // Try if the private key is in pkcs12 format bundled with client certificate
        if (QCA::isSupported("pkcs12")) {
            QCA::KeyBundle keyBundle = QCA::KeyBundle::fromFile(m_ui->tlsPrivateKey->url().path(), m_ui->tlsPrivateKeyPassword->text().toUtf8(), &convRes);
            // We can return the result of decryption when we managed to import the private key
            if (convRes == QCA::ConvertGood) {
                return keyBundle.privateKey().canDecrypt();
            }
        }

        // If the private key is not in pkcs12 format, we need client certificate to be set
        if (!m_ui->tlsUserCert->url().isValid()) {
            return false;
        }

        // Try if the private key is in PEM format and return the result of decryption if we managed to open it
        QCA::PrivateKey key = QCA::PrivateKey::fromPEMFile(m_ui->tlsPrivateKey->url().path(), m_ui->tlsPrivateKeyPassword->text().toUtf8(), &convRes);
        if (convRes == QCA::ConvertGood) {
            return key.canDecrypt();
        }

        // TODO Try other formats (DER - mainly used in Windows)
        // TODO Validate other certificates??
    } else if (method == NetworkManager::Security8021xSetting::EapMethodLeap) {
        return !m_ui->leapUsername->text().isEmpty() && !m_ui->leapPassword->text().isEmpty();
    } else if (method == NetworkManager::Security8021xSetting::EapMethodFast) {
        if (!m_ui->fastAllowPacProvisioning->isChecked() && !m_ui->pacFile->url().isValid()) {
            return false;
        }
        return !m_ui->fastUsername->text().isEmpty() && (!m_ui->fastPassword->text().isEmpty() || m_ui->cbAskFastPassword->isChecked());
    } else if (method == NetworkManager::Security8021xSetting::EapMethodTtls) {
         return !m_ui->ttlsUsername->text().isEmpty() && (!m_ui->ttlsPassword->text().isEmpty() || m_ui->cbAskTtlsPassword->isChecked());
    } else if (method == NetworkManager::Security8021xSetting::EapMethodPeap) {
         return !m_ui->peapUsername->text().isEmpty() && (!m_ui->peapPassword->text().isEmpty() || m_ui->cbAskPeapPassword->isChecked());
    }

    return true;
}

void Security8021x::currentAuthChanged(int index)
{
    Q_UNUSED(index);
    KAcceleratorManager::manage(m_ui->stackedWidget->currentWidget());
}
