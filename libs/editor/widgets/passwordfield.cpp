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
#include <QActionGroup>
#include <QIcon>
#include <QMenu>

#include <KLocalizedString>
#include <KActionMenu>
#include <KAction>

PasswordField::PasswordField(QWidget *parent)
    : KLineEdit(parent)
    , m_currentPasswordOption(StoreForUser)
{
    QAction *action;
    QActionGroup *actionGroup = new QActionGroup(this);
    m_passwordOptionsMenu = new KActionMenu(this);
    m_passwordOptionsMenu->setIcon(QIcon::fromTheme(QStringLiteral("document-save")));
    m_passwordOptionsMenu->setToolTip(i18n("Store password for this user only (encrypted)"));
    m_passwordOptionsMenu->setActionGroup(actionGroup);

    action = new QAction(QIcon::fromTheme(QStringLiteral("document-save")), i18n("Store password for this user only (encrypted)"), actionGroup);
    action->setCheckable(true);
    action->setChecked(true);
    action->setData(StoreForUser);
    m_passwordOptionsMenu->addAction(action);

    action= new QAction(QIcon::fromTheme(QStringLiteral("document-save-all")), i18n("Store password and make it available for all users (not encrypted)"), actionGroup);
    action->setCheckable(true);
    action->setData(StoreForAllUsers);
    m_passwordOptionsMenu->addAction(action);

    action = new QAction(QIcon::fromTheme(QStringLiteral("dialog-messages")), i18n("Ask for this password every time"), actionGroup);
    action->setCheckable(true);
    action->setData(AlwaysAsk);
    m_passwordOptionsMenu->addAction(action);

    action = new QAction(QIcon::fromTheme(QStringLiteral("document-close")), i18n("This password is not required"), actionGroup);
    action->setCheckable(true);
    action->setVisible(false);
    action->setData(NotRequired);
    m_passwordOptionsMenu->addAction(action);

    connect(actionGroup, &QActionGroup::triggered, this, &PasswordField::changePasswordOption);

    m_toggleEchoModeAction = addAction(QIcon::fromTheme(QStringLiteral("visibility")), QLineEdit::TrailingPosition);
    m_toggleEchoModeAction->setVisible(false);
    m_toggleEchoModeAction->setToolTip(i18n("Change the visibility of the password"));
    connect(m_toggleEchoModeAction, &QAction::triggered, this, &PasswordField::toggleEchoMode);
    connect(this, &KLineEdit::textChanged, this, &PasswordField::showToggleEchoModeAction);
}

void PasswordField::setPasswordOptionsEnabled(bool enable)
{
    if (enable) {
        // Remove the "show password action" first so we are at the position we want
        removeAction(m_toggleEchoModeAction);
        addAction(m_passwordOptionsMenu,  QLineEdit::TrailingPosition);
        // Re-add the "show password action"
        addAction(m_toggleEchoModeAction,  QLineEdit::TrailingPosition);
    } else {
        removeAction(m_passwordOptionsMenu);
    }
}

void PasswordField::setPasswordOptionEnabled(PasswordOption option, bool enable)
{
    Q_FOREACH (QAction *action, m_passwordOptionsMenu->actionGroup()->actions()) {
        if ((PasswordOption)action->data().toUInt() == option) {
            action->setVisible(enable);
        }
    }
}

PasswordField::PasswordOption PasswordField::passwordOption() const
{
    return m_currentPasswordOption;
}

void PasswordField::setPasswordOption(PasswordField::PasswordOption option)
{
    Q_FOREACH (QAction *action, m_passwordOptionsMenu->actionGroup()->actions()) {
        if ((PasswordOption)action->data().toUInt() == option) {
            action->setChecked(true);
            m_passwordOptionsMenu->setIcon(action->icon());
            m_passwordOptionsMenu->setToolTip(action->toolTip());
            changePasswordOption(action);
        }
    }
}

void PasswordField::showToggleEchoModeAction(const QString &text)
{
    m_toggleEchoModeAction->setVisible(!text.isEmpty());
}

void PasswordField::toggleEchoMode()
{
    if (echoMode() == QLineEdit::Password) {
        setEchoMode(QLineEdit::Normal);
        m_toggleEchoModeAction->setIcon(QIcon::fromTheme(QStringLiteral("hint")));
    } else if (echoMode() == QLineEdit::Normal) {
        setEchoMode(QLineEdit::Password);
        m_toggleEchoModeAction->setIcon(QIcon::fromTheme(QStringLiteral("visibility")));
    }
}

void PasswordField::changePasswordOption(QAction *action)
{
    m_currentPasswordOption = (PasswordOption)action->data().toUInt();

    if (m_currentPasswordOption == PasswordField::NotRequired || m_currentPasswordOption == PasswordField::AlwaysAsk) {
        clear();
        setReadOnly(true);
    } else {
        setReadOnly(false);
    }

    m_passwordOptionsMenu->setIcon(action->icon());
    m_passwordOptionsMenu->setToolTip(action->toolTip());
    Q_EMIT passwordOptionChanged(m_currentPasswordOption);
}
