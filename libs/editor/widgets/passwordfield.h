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

#ifndef PLASMA_NM_PASSWORD_FIELD_H
#define PLASMA_NM_PASSWORD_FIELD_H

#include <KActionMenu>
#include <KLineEdit>

class Q_DECL_EXPORT PasswordField : public KLineEdit
{
    Q_OBJECT
public:
    enum PasswordOption {
        StoreForUser,
        StoreForAllUsers,
        AlwaysAsk,
        NotRequired
    };

    explicit PasswordField(QWidget *parent = 0);

    void setPasswordOptionsEnabled(bool enable);
    void setPasswordOptionEnabled(PasswordOption option, bool enable);

    PasswordOption passwordOption() const;
    void setPasswordOption(PasswordOption option);
private Q_SLOTS:
    void changePasswordOption(QAction *action);
    void showToggleEchoModeAction(const QString &text);
    void toggleEchoMode();

Q_SIGNALS:
    void passwordOptionChanged(PasswordOption option);

private:
    PasswordOption m_currentPasswordOption;
    KActionMenu *m_passwordOptionsMenu;
    QAction *m_toggleEchoModeAction;
};

#endif // PLASMA_NM_PASSWORD_FIELD_H