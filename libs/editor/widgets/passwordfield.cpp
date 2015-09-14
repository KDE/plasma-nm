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

#include <KLocalizedString>

PasswordField::PasswordField(QWidget* parent)
    : KLineEdit(parent)
{
    toggleEchoModeAction = addAction(QIcon::fromTheme(QStringLiteral("visibility")), QLineEdit::TrailingPosition);
    toggleEchoModeAction->setVisible(false);
    toggleEchoModeAction->setToolTip(i18n("Change the visibility of the password"));
    connect(toggleEchoModeAction, &QAction::triggered, this, &PasswordField::toggleEchoMode);
    connect(this, &KLineEdit::textChanged, this, &PasswordField::showToggleEchoModeAction);
}

void PasswordField::showToggleEchoModeAction(const QString& text)
{
    toggleEchoModeAction->setVisible(!text.isEmpty());
}

void PasswordField::toggleEchoMode()
{
    if (echoMode() == QLineEdit::Password) {
        setEchoMode(QLineEdit::Normal);
        toggleEchoModeAction->setIcon(QIcon::fromTheme(QStringLiteral("hint")));
    } else if (echoMode() == QLineEdit::Normal) {
        setEchoMode(QLineEdit::Password);
        toggleEchoModeAction->setIcon(QIcon::fromTheme(QStringLiteral("visibility")));
    }
}
