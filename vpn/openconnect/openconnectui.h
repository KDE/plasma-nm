/*
Copyright 2011 Ilia Kats <ilia-kats@gmx.net>
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

#ifndef OPENCONNECT_UI_H
#define OPENCONNECT_UI_H

#include "vpnuiplugin.h"

#include <QVariant>
#include <KDialog>

class OpenconnectUiPlugin : public VpnUiPlugin
{
    Q_OBJECT
public:
    explicit OpenconnectUiPlugin(QObject * parent = 0, const QVariantList& = QVariantList());
    virtual ~OpenconnectUiPlugin();
    virtual SettingWidget * widget(const NetworkManager::VpnSetting::Ptr &setting, QWidget * parent = 0);
    virtual SettingWidget * askUser(const NetworkManager::VpnSetting::Ptr &setting, QWidget * parent = 0);
#if 0
    QString suggestedFileName(Knm::Connection *connection) const;
    QString supportedFileExtensions() const;
    QVariantList importConnectionSettings(const QString &fileName);
    bool exportConnectionSettings(Knm::Connection * connection, const QString &fileName);
    KDialog::ButtonCodes suggestAuthDialogButtons();
#endif
};

#endif //  OPENCONNECT_UI_H
