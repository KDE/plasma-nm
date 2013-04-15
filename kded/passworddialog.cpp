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
#include "ui_passworddialog.h"

#include <KIcon>
#include <KDebug>

PasswordDialog::PasswordDialog(const NetworkManager::Settings::Setting::Ptr &setting, const QStringList &neededSecrets, const QString &ssid, QWidget *parent) :
    KDialog(parent), ui(new Ui::PasswordDialog), m_neededSecrets(neededSecrets)
{
    ui->setupUi(mainWidget());
    setWindowIcon(KIcon("dialog-password"));
    // TODO fix this for high DPI
    ui->labelIcon->setPixmap(KIcon("dialog-password").pixmap(32));

    if (neededSecrets.isEmpty()) {
        qWarning() << Q_FUNC_INFO << "list of secrets is empty!!!";
    }

    if (setting->type() == NetworkManager::Settings::Setting::WirelessSecurity || setting->type() == NetworkManager::Settings::Setting::Security8021x) {
        ui->labelText->setText(i18n("For accessing the wireless network '%1' you need to provide a password below", ssid));
    } else {
        ui->labelText->setText(i18n("Please provide a password below"));
    }

    ui->password->setPasswordMode(true);
    ui->password->setFocus();
    connect(ui->showPassword, SIGNAL(toggled(bool)), this, SLOT(showPassword(bool)));
}

PasswordDialog::~PasswordDialog()
{
    delete ui;
}

QVariantMap PasswordDialog::secrets() const
{
    QVariantMap result;

    if (!ui->password->text().isEmpty() && !m_neededSecrets.isEmpty())
        result.insert(m_neededSecrets.first(), ui->password->text());

    return result;
}

void PasswordDialog::showPassword(bool show)
{
    ui->password->setPasswordMode(!show);
}
