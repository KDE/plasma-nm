/*
    SPDX-FileCopyrightText: 2015 Jan Grulich <jgrulich@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
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
        NotRequired,
    };

    explicit PasswordField(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

    void setMaxLength(int maxLength);
    void setPasswordModeEnabled(bool passwordMode);
    void setPasswordOptionsEnabled(bool enable);
    void setPasswordNotSavedEnabled(bool enable);
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
