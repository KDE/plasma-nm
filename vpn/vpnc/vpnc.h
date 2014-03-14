/*
    Copyright 2008 Will Stephenson <wstephenson@kde.org>
    Copyright 2011-2012 Rajeesh K Nambiar <rajeeshknambiar@gmail.com>
    Copyright 2011-2012 Lamarque V. Souza <lamarque@kde.org>
    Copyright 2013 Lukas Tinkl <ltinkl@redhat.com>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of
    the License or (at your option) version 3 or any later version
    accepted by the membership of KDE e.V. (or its successor approved
    by the membership of KDE e.V.), which shall act as a proxy
    defined in Section 14 of version 3 of the license.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef PLASMA_NM_VPNC_H
#define PLASMA_NM_VPNC_H

#include "vpnuiplugin.h"
#include "plasmanm_export.h"

#include <QVariant>

#include <KProcess>
#include <KConfigGroup>

class VpncUiPluginPrivate: public QObject
{
    Q_OBJECT
public:
    VpncUiPluginPrivate();
    ~VpncUiPluginPrivate();
    QString readStringKeyValue(const KConfigGroup & configGroup, const QString & key);
    KProcess * ciscoDecrypt;
    QString decryptedPasswd;

protected Q_SLOTS:
    void gotCiscoDecryptOutput();
    void ciscoDecryptError(QProcess::ProcessError pError);
    void ciscoDecryptFinished(int exitCode, QProcess::ExitStatus exitStatus);
};

class PLASMA_NM_EXPORT VpncUiPlugin : public VpnUiPlugin
{
    Q_OBJECT

public:
    explicit VpncUiPlugin(QObject * parent = 0, const QVariantList& = QVariantList());
    virtual ~VpncUiPlugin();
    virtual SettingWidget * widget(const NetworkManager::VpnSetting::Ptr &setting, QWidget * parent = 0);
    virtual SettingWidget * askUser(const NetworkManager::VpnSetting::Ptr &setting, QWidget * parent = 0);

    QString suggestedFileName(const NetworkManager::ConnectionSettings::Ptr &connection) const;
    QString supportedFileExtensions() const;
    NMVariantMapMap importConnectionSettings(const QString &fileName);
    bool exportConnectionSettings(const NetworkManager::ConnectionSettings::Ptr &connection, const QString &fileName);
};

#endif // PLASMA_NM_VPNC_H
