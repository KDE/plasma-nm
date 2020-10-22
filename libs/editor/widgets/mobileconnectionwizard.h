/*
Copyright 2010-2011 Lamarque Souza <lamarque@kde.org>
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

#ifndef PLASMA_NM_MOBILE_CONNECTION_WIZARD_H
#define PLASMA_NM_MOBILE_CONNECTION_WIZARD_H

#include <QWizardPage>
#include <QListWidget>
#include <QRadioButton>
#include <QLabel>

#include <KComboBox>
#include <KLineEdit>

#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/ConnectionSettings>

#include "mobileproviders.h"

class Q_DECL_EXPORT MobileConnectionWizard : public QWizard
{
Q_OBJECT
public:
    /*
     * Do not use NetworkManager::ConnectionSettings::Bluetooth here, use either NetworkManager::ConnectionSettings::Gsm
     * or NetworkManager::ConnectionSettings::Cdma.
     */
    explicit MobileConnectionWizard(NetworkManager::ConnectionSettings::ConnectionType connectionType = NetworkManager::ConnectionSettings::Unknown,
                                    QWidget * parent = nullptr);
    ~MobileConnectionWizard() override;

    /*
     * Returns the information to configure one connection from the last wizard run.
     * The format is:
     * for GSM connections: provider's name + QList of Gsm NetworkIds for that provider (can be an empty QList) + QMap with apn information
     * where apn information is: dial number + apn + apn name (optional) + username (optional) + password (optional) + QList of name servers (optional)
     *
     * for CDMA connections: provider's name + QMap with cdma information.
     * where cdma information is: name (optional) + username (optional) + password (optional) + list of sids (optional)
     */
    QVariantList args();

    NetworkManager::ConnectionSettings::ConnectionType type() const { return mType; }
    MobileProviders::ErrorCodes getError();

private Q_SLOTS:
    void introDeviceAdded(const QString &uni);
    void introDeviceRemoved(const QString &uni);
    void introStatusChanged(NetworkManager::Status);
    void slotEnablePlanEditBox(const QString & text);
    void slotEnableProviderEdit(bool enable);
    void slotCheckProviderEdit();
    void slotCheckProviderList();

private:
    QWizardPage * createIntroPage();
    QWizardPage * createCountryPage();
    QWizardPage * createProvidersPage();
    QWizardPage * createPlansPage();
    QWizardPage * createConfirmPage();
    void initializePage(int id) override;
    int nextId() const override;

    MobileProviders * mProviders;
    QString country;
    QString provider;
    QString apn;
    NetworkManager::ConnectionSettings::ConnectionType mType;
    bool mInitialMethodType;

    // Intro page
    KComboBox * mDeviceComboBox;
    void introAddInitialDevices();
    void introRemoveAllDevices();
    void introAddDevice(const NetworkManager::Device::Ptr &device);

    // Country page
    QListWidget * mCountryList;

    // Providers page
    QListWidget * mProvidersList;
    QRadioButton * radioAutoProvider;
    QRadioButton * radioManualProvider;
    KLineEdit * lineEditProvider;

    // Plan page
    KComboBox * mPlanComboBox;
    KLineEdit * userApn;

    // Confirm page
    QLabel * labelProvider;
    QLabel * labelPlanLabel;
    QLabel * labelPlan;
    QLabel * labelApn;
};
#endif // PLASMA_NM_MOBILE_CONNECTION_WIZARD_H
