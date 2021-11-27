/*
    SPDX-FileCopyrightText: 2013 Lukas Tinkl <ltinkl@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef PLASMA_NM_HWADDRCOMBOBOX_H
#define PLASMA_NM_HWADDRCOMBOBOX_H

#include <QComboBox>

#include <NetworkManagerQt/Device>

class Q_DECL_EXPORT HwAddrComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit HwAddrComboBox(QWidget *parent = nullptr);

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
    bool m_dirty = false;
};

#endif // PLASMA_NM_HWADDRCOMBOBOX_H
