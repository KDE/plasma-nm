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

#ifndef PLASMA_NM_OPENVPN_ADVANCED_WIDGET_H
#define PLASMA_NM_OPENVPN_ADVANCED_WIDGET_H

#include <QDialog>
#include <QProcess>

#include <NetworkManagerQt/VpnSetting>

namespace Ui
{
class OpenVpnAdvancedWidget;
}

class QComboBox;
class QLineEdit;
class OpenVpnAdvancedWidget : public QDialog
{
    Q_OBJECT
public:
    explicit OpenVpnAdvancedWidget(const NetworkManager::VpnSetting::Ptr &setting, QWidget *parent = 0);
    ~OpenVpnAdvancedWidget();
    void init();

    NetworkManager::VpnSetting::Ptr setting() const;

private Q_SLOTS:
    void gotOpenVpnOutput();
    void openVpnError(QProcess::ProcessError);
    void openVpnFinished(int, QProcess::ExitStatus);

    void proxyPasswordStorageChanged(int);
    void proxyTypeChanged(int);

private:
    void loadConfig();
    void setPasswordType(QLineEdit *edit, int type);
    void fillOnePasswordCombo(QComboBox *combo, NetworkManager::Setting::SecretFlags type);
    uint handleOnePasswordType(const QComboBox *combo, const QString & key, NMStringMap & data) const;
    Ui::OpenVpnAdvancedWidget * m_ui;
    class Private;
    Private * const d;
};

#endif // PLASMA_NM_OPENVPN_ADVANCED_WIDGET_H
