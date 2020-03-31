/*
    Copyright 2015 Jan Grulich <jgrulich@redhat.com>

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

#include "passwordfield.h"

#include <QAction>
#include <QIcon>

#include <KAuthorized>
#include <KLocalizedString>
#include <KWallet>

PasswordField::PasswordField(QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f)
    , m_currentPasswordOption(StoreForUser)
{
    m_layout = new QVBoxLayout(this);
    // The widget will be already in layout, thus reset content margins
    // to align it with the rest of widgets
    m_layout->setContentsMargins(0, 0, 0, 0);

    m_passwordField = new QLineEdit(this);
    connect(m_passwordField, &QLineEdit::textChanged, this, &PasswordField::textChanged);

    if (KAuthorized::authorize(QStringLiteral("lineedit_reveal_password"))) {
        m_toggleEchoModeAction = m_passwordField->addAction(QIcon::fromTheme(QStringLiteral("visibility")), QLineEdit::TrailingPosition);
        m_toggleEchoModeAction->setVisible(false);
        m_toggleEchoModeAction->setToolTip(i18n("Change the visibility of the password"));
        connect(m_passwordField, &QLineEdit::textChanged, this, &PasswordField::showToggleEchoModeAction);
        connect(m_toggleEchoModeAction, &QAction::triggered, this, &PasswordField::toggleEchoMode);
    }

    m_layout->addWidget(m_passwordField);

    m_passwordOptionsMenu = new QComboBox(this);
    m_passwordOptionsMenu->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLength);

    m_passwordOptionsMenu->addItem(QIcon::fromTheme(QStringLiteral("document-save")), i18n("Store password for this user only (encrypted)"), StoreForUser);
    m_passwordOptionsMenu->addItem(QIcon::fromTheme(QStringLiteral("document-save-all")), i18n("Store password for all users (not encrypted)"), StoreForAllUsers);
    m_passwordOptionsMenu->addItem(QIcon::fromTheme(QStringLiteral("dialog-messages")), i18n("Ask for this password every time"), AlwaysAsk);
    // Do not add by default
    // m_passwordOptionsMenu->addItem(QIcon::fromTheme(QStringLiteral("document-close")), i18n("This password is not required"), NotRequired);

    if (KWallet::Wallet::isEnabled()) {
        m_passwordOptionsMenu->setCurrentIndex(0);
    } else {
        m_passwordOptionsMenu->setCurrentIndex(1);
        m_currentPasswordOption = StoreForAllUsers;
    }

    connect(m_passwordOptionsMenu, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &PasswordField::changePasswordOption);

    // Disable by default
    m_passwordOptionsMenu->setVisible(false);
    // m_layout->addWidget(m_passwordOptionsMenu);

    setLayout(m_layout);
}

void PasswordField::setMaxLength(int maxLength)
{
    m_passwordField->setMaxLength(maxLength);
}

void PasswordField::setPasswordModeEnabled(bool enable)
{
    m_passwordField->setEchoMode(enable ? QLineEdit::Password : QLineEdit::Normal);
}

void PasswordField::setPasswordOptionsEnabled(bool enable)
{
    if (enable) {
        m_layout->addWidget(m_passwordOptionsMenu);
        m_passwordOptionsMenu->setVisible(true);
    } else {
        m_layout->removeWidget(m_passwordOptionsMenu);
        m_passwordOptionsMenu->setVisible(false);
    }
}

void PasswordField::setPasswordNotSavedEnabled(bool enable)
{
    if (enable) {
        const int index = m_passwordOptionsMenu->findData(AlwaysAsk);
        if (index == -1) {
            m_passwordOptionsMenu->addItem(QIcon::fromTheme(QStringLiteral("dialog-messages")), i18n("Ask for this password every time"), AlwaysAsk);
        }
    } else {
        const int index = m_passwordOptionsMenu->findData(AlwaysAsk);
        if (index != -1) {
            m_passwordOptionsMenu->removeItem(index);
        }
    }
}

void PasswordField::setPasswordNotRequiredEnabled(bool enable)
{
    if (enable) {
        const int index = m_passwordOptionsMenu->findData(NotRequired);
        if (index == -1) {
            m_passwordOptionsMenu->addItem(QIcon::fromTheme(QStringLiteral("document-close")), i18n("This password is not required"), NotRequired);
        }
    } else {
        const int index = m_passwordOptionsMenu->findData(NotRequired);
        if (index != -1) {
            m_passwordOptionsMenu->removeItem(index);
        }
    }
}

void PasswordField::setText(const QString &text)
{
    m_passwordField->setText(text);
}

QString PasswordField::text() const
{
    return m_passwordField->text();
}

PasswordField::PasswordOption PasswordField::passwordOption() const
{
    return m_currentPasswordOption;
}

void PasswordField::setPasswordOption(PasswordField::PasswordOption option)
{
    const int index = m_passwordOptionsMenu->findData(option);
    if (index != -1) {
        m_passwordOptionsMenu->setCurrentIndex(index);
    }
}

void PasswordField::showToggleEchoModeAction(const QString &text)
{
    m_toggleEchoModeAction->setVisible(!text.isEmpty());
}

void PasswordField::toggleEchoMode()
{
    if (m_passwordField->echoMode() == QLineEdit::Password) {
        m_passwordField->setEchoMode(QLineEdit::Normal);
        m_toggleEchoModeAction->setIcon(QIcon::fromTheme(QStringLiteral("hint")));
    } else if (m_passwordField->echoMode() == QLineEdit::Normal) {
        m_passwordField->setEchoMode(QLineEdit::Password);
        m_toggleEchoModeAction->setIcon(QIcon::fromTheme(QStringLiteral("visibility")));
    }
}

void PasswordField::changePasswordOption(int index)
{
    Q_UNUSED(index);

    m_currentPasswordOption = (PasswordOption)m_passwordOptionsMenu->currentData().toUInt();

    if (m_currentPasswordOption == PasswordField::NotRequired || m_currentPasswordOption == PasswordField::AlwaysAsk) {
        m_passwordField->clear();
        m_passwordField->setDisabled(true);
    } else {
        m_passwordField->setEnabled(true);
    }

    Q_EMIT passwordOptionChanged(m_currentPasswordOption);
}
