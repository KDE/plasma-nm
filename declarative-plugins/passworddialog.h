#ifndef PASSWORDDIALOG_H
#define PASSWORDDIALOG_H

#include <QtNetworkManager/generic-types.h>
#include <QtNetworkManager/settings/connection.h>

#include <QDialog>

#include "ui_passworddialog.h"

class PasswordDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PasswordDialog(NetworkManager::Settings::Setting * setting, const QStringList & neededSecrets,
                            const QString & ssid = QString(), QWidget *parent = 0);
    ~PasswordDialog();
    QVariantMap secrets() const;

private:
    Ui::PasswordDialog * m_ui;
    QStringList m_neededSecrets;
};

#endif // PASSWORDDIALOG_H
