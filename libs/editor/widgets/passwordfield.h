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

#include <QLineEdit>
#include <QComboBox>

#include <QVBoxLayout>

class Q_DECL_EXPORT PasswordField : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool passwordModeEnabled WRITE setPasswordModeEnabled)
public:
    enum PasswordOption {
        StoreForUser,
        StoreForAllUsers,
        AlwaysAsk,
        NotRequired
    };

    explicit PasswordField(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

    void setMaxLength(int maxLength);
    void setPasswordModeEnabled(bool passwordMode);
    void setPasswordOptionsEnabled(bool enable);
    void setPasswordNotRequiredEnabled(bool enable);

    PasswordOption passwordOption() const;
    void setPasswordOption(PasswordOption option);

    void setText(const QString &text);
    QString text() const;

private Q_SLOTS:
    void changePasswordOption(int index);
    void showToggleEchoModeAction(const QString &text);
    void toggleEchoMode();

Q_SIGNALS:
    void textChanged(const QString &text);
    void passwordOptionChanged(PasswordOption option);

private:
    PasswordOption m_currentPasswordOption;
    QVBoxLayout *m_layout;
    QLineEdit *m_passwordField;
    QComboBox *m_passwordOptionsMenu;
    QAction *m_toggleEchoModeAction;
};

#endif // PLASMA_NM_PASSWORD_FIELD_H
