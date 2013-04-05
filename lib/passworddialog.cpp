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

#include "passworddialog.h"

PasswordDialog::PasswordDialog(const NetworkManager::Settings::Setting::Ptr &setting, const QStringList &neededSecrets, const QString &ssid, QWidget *parent) :
    QDialog(parent), m_ui(new Ui::PasswordDialog), m_neededSecrets(neededSecrets)
{
    m_ui->setupUi(this);
    m_ui->labelIcon->setPixmap(QIcon::fromTheme("dialog-password").pixmap(32));

    if (neededSecrets.isEmpty()) {
        qWarning() << Q_FUNC_INFO << "list of secrets is empty!!!";
    }

    if (setting->type() == NetworkManager::Settings::Setting::WirelessSecurity || setting->type() == NetworkManager::Settings::Setting::Security8021x) {
        m_ui->labelText->setText(i18n("For accessing the wireless network '%1' you need to provide a password below", ssid));
    } else {
        m_ui->labelText->setText(i18n("Please provide a password below"));
    }

    connect(m_ui->showPassword, SIGNAL(toggled(bool)), this, SLOT(showPassword(bool)));
    connect(m_ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(m_ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

PasswordDialog::~PasswordDialog()
{
    delete m_ui;
}

QVariantMap PasswordDialog::secrets() const
{
    QVariantMap result;

    if (!m_ui->password->text().isEmpty() && !m_neededSecrets.isEmpty())
        result.insert(m_neededSecrets.first(), m_ui->password->text());

    return result;
}

void PasswordDialog::showPassword(bool show)
{
    m_ui->password->setPasswordMode(!show);
}
