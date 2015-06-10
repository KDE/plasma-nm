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

#ifndef PLASMA_NM_HWADDRCOMBOBOX_H
#define PLASMA_NM_HWADDRCOMBOBOX_H

#include <QComboBox>

#include <NetworkManagerQt/Device>

class Q_DECL_EXPORT HwAddrComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit HwAddrComboBox(QWidget *parent = 0);

    void init(const NetworkManager::Device::Type &deviceType, const QString &address);

    bool isValid() const;
    QString hwAddress() const;

Q_SIGNALS:
    void hwAddressChanged();

private Q_SLOTS:
    void slotEditTextChanged(const QString &);
    void slotCurrentIndexChanged(int);

private:
    void addAddressToCombo(const NetworkManager::Device::Ptr &device);
    QVariant hwAddressFromDevice(const NetworkManager::Device::Ptr &device);
    QString m_initialAddress;
    bool m_dirty;
};

#endif // PLASMA_NM_HWADDRCOMBOBOX_H
