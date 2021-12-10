/*
    SPDX-FileCopyrightText: 2010-2011 Lamarque Souza <lamarque@kde.org>
    SPDX-FileCopyrightText: 2013 Lukas Tinkl <ltinkl@redhat.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef PLASMA_NM_MOBILE_CONNECTION_WIZARD_H
#define PLASMA_NM_MOBILE_CONNECTION_WIZARD_H

#include <QLabel>
#include <QListWidget>
#include <QRadioButton>
#include <QWizardPage>

#include <KComboBox>
#include <KLineEdit>

#include <NetworkManagerQt/ConnectionSettings>
#include <NetworkManagerQt/Manager>

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
                                    QWidget *parent = nullptr);
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

    NetworkManager::ConnectionSettings::ConnectionType type() const
    {
        return mType;
    }
    MobileProviders::ErrorCodes getError();

private Q_SLOTS:
    void introDeviceAdded(const QString &uni);
    void introDeviceRemoved(const QString &uni);
    void introStatusChanged(NetworkManager::Status);
    void slotEnablePlanEditBox(int index);
    void slotEnableProviderEdit(bool enable);
    void slotCheckProviderEdit();
    void slotCheckProviderList();

private:
    QWizardPage *createIntroPage();
    QWizardPage *createCountryPage();
    QWizardPage *createProvidersPage();
    QWizardPage *createPlansPage();
    QWizardPage *createConfirmPage();
    void initializePage(int id) override;
    int nextId() const override;

    MobileProviders *mProviders;
    QString country;
    QString provider;
    QString apn;
    NetworkManager::ConnectionSettings::ConnectionType mType;
    bool mInitialMethodType;

    // Intro page
    KComboBox *mDeviceComboBox;
    void introAddInitialDevices();
    void introRemoveAllDevices();
    void introAddDevice(const NetworkManager::Device::Ptr &device);

    // Country page
    QListWidget *mCountryList;

    // Providers page
    QListWidget *mProvidersList;
    QRadioButton *radioAutoProvider;
    QRadioButton *radioManualProvider;
    KLineEdit *lineEditProvider;

    // Plan page
    KComboBox *mPlanComboBox;
    KLineEdit *userApn;

    // Confirm page
    QLabel *labelProvider;
    QLabel *labelPlanLabel;
    QLabel *labelPlan;
    QLabel *labelApn;
};
#endif // PLASMA_NM_MOBILE_CONNECTION_WIZARD_H
