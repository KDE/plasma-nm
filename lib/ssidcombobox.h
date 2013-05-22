/*
    Copyright 2013 Lukas Tinkl <ltinkl@redhat.com>
    Copyright 2013 Jan Grulich <jgrulich@redhat.com>

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

#ifndef PLASMA_NM_SSIDCOMBOBOX_H
#define PLASMA_NM_SSIDCOMBOBOX_H

#include <KComboBox>

#include <kdemacros.h>

#include <NetworkManagerQt/Device>
#include <NetworkManagerQt/WirelessNetwork>

class KDE_EXPORT SsidComboBox : public KComboBox
{
    Q_OBJECT
public:
    explicit SsidComboBox(QWidget *parent = 0);

    void init(const QString & ssid);

    QString ssid() const;

private slots:
    void editTextChanged(const QString &);
    void currentIndexChanged(int);

private:
    void addSsidsToCombo(const QList<NetworkManager::WirelessNetwork::Ptr> &networks);
    QString m_initialSsid;
    bool m_dirty;
};

#endif // PLASMA_NM_SSIDCOMBOBOX_H
