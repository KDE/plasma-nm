/*
Copyright 2010-2012 Lamarque Souza <lamarque@kde.org>
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

#include <QVBoxLayout>

#include <KLocale>
#include <KDebug>
#include <KGlobal>
#include <KIconLoader>
#include <Solid/Device>

#include <QtNetworkManager/modemdevice.h>

#include <QtModemManager/manager.h>
#include <QtModemManager/modeminterface.h>

#include "uiutils.h"

#include "mobileconnectionwizard.h"

#define NUMBER_OF_STATIC_ENTRIES 3

MobileConnectionWizard::MobileConnectionWizard(NetworkManager::Settings::ConnectionSettings::ConnectionType connectionType, QWidget * parent)
    : QWizard(parent)
{
    if (connectionType == NetworkManager::Settings::ConnectionSettings::Unknown) {
        mInitialMethodType = false;
    } else {
        mInitialMethodType = true;

        if (connectionType == NetworkManager::Settings::ConnectionSettings::Bluetooth) {
            mType = NetworkManager::Settings::ConnectionSettings::Gsm;
        } else {
            mType = connectionType;
        }
    }

    mProviders = new MobileProviders();
    setWindowTitle(i18nc("Mobile Connection Wizard", "New Mobile Broadband Connection"));
    addPage(createIntroPage());
    addPage(createCountryPage());
    addPage(createProvidersPage());
    addPage(createPlansPage());
    addPage(createConfirmPage());
    setOptions(QWizard::NoBackButtonOnStartPage);
}

MobileConnectionWizard::~MobileConnectionWizard()
{
    delete mProviders;
}

MobileProviders::ErrorCodes MobileConnectionWizard::getError()
{
    if (mProviders) {
        return mProviders->getError();
    }
    return MobileProviders::Success;
}

void MobileConnectionWizard::initializePage(int id)
{
    switch (id) {
    case 1: { // Country Page
        //QString country = KGlobal::locale()->country();
        if (country.isEmpty()) {
            country = mProviders->countryFromLocale();
        }
        if (country.isEmpty())
            mCountryList->setCurrentRow(0);
        else {
            QList<QListWidgetItem *> items = mCountryList->findItems(mProviders->getCountryName(country), Qt::MatchExactly);
            if (!items.empty()) {
                mCountryList->setCurrentItem(items.at(0));
            }
        }

        if (!mInitialMethodType) {
            NetworkManager::Device::Ptr iface = NetworkManager::findNetworkInterface(mDeviceComboBox->itemData(mDeviceComboBox->currentIndex()).toString());
            if (iface) {
                NetworkManager::ModemDevice::Ptr nmModemIface = iface.objectCast<NetworkManager::ModemDevice>();
                if (nmModemIface && UiUtils::modemSubType(nmModemIface->currentCapabilities()) == NetworkManager::ModemDevice::CdmaEvdo) {
                    mType = NetworkManager::Settings::ConnectionSettings::Cdma;
                } else {
                    mType = NetworkManager::Settings::ConnectionSettings::Gsm;
                }
                /* TODO: test for Lte */
            } else {
                mType = static_cast<NetworkManager::Settings::ConnectionSettings::ConnectionType>(mDeviceComboBox->itemData(mDeviceComboBox->currentIndex()).toUInt());
            }
        }

        if (mProviders->getError() != MobileProviders::Success) {
            accept();
        }
        break;
    }

    case 2: // Providers Page
        country = mCountryList->currentItem()->text();
        mProvidersList->clear();
        lineEditProvider->clear();
        radioAutoProvider->setChecked(true);

        switch (type()) {
        case NetworkManager::Settings::ConnectionSettings::Gsm:
            mProvidersList->insertItems(0, mProviders->getProvidersList(country, NetworkManager::Settings::ConnectionSettings::Gsm));
            break;
        case NetworkManager::Settings::ConnectionSettings::Cdma:
            mProvidersList->insertItems(0, mProviders->getProvidersList(country, NetworkManager::Settings::ConnectionSettings::Cdma));
            break;
        default:
            break;
        }
        mProvidersList->setCurrentRow(0);
        if (mProvidersList->count() > 0) {
            mProvidersList->setEnabled(true);
            radioAutoProvider->setEnabled(true);
            mProvidersList->setFocus();
        } else {
            mProvidersList->setEnabled(false);
            radioAutoProvider->setEnabled(false);
            radioManualProvider->setChecked(true);
            // TODO: this does not work, try reimplementing QWizardPage::isComplete()
            //button(QWizard::NextButton)->setEnabled(false);
        }
        break;

    case 3: // Plans Page
        disconnect(mPlanComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(slotEnablePlanEditBox(QString)));
        mPlanComboBox->clear();
        if (type() != NetworkManager::Settings::ConnectionSettings::Gsm) {
            goto OUT_3;
        }
        if (radioManualProvider->isChecked()) {
            mPlanComboBox->insertSeparator(1);
            mPlanComboBox->addItem(i18nc("Mobile Connection Wizard", "My plan is not listed..."));
            mPlanComboBox->setCurrentIndex(1);
            userApn->clear();

            if (lineEditProvider->text().isEmpty()) {
                lineEditProvider->setText(i18nc("Mobile Connection Wizard", "Unknown Provider"));
            }
        } else {
            if (mProvidersList->currentItem() != 0) {
                QStringList mApns = mProviders->getApns(mProvidersList->currentItem()->text());
                userApn->setText(mApns.at(0));
                mPlanComboBox->insertItems(0, mApns);
                mPlanComboBox->setItemText(0, i18nc("Mobile Connection Wizard", "Default"));
            }

            mPlanComboBox->insertSeparator(1);
            mPlanComboBox->addItem(i18nc("Mobile Connection Wizard", "My plan is not listed..."));
        }
OUT_3:
        connect(mPlanComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(slotEnablePlanEditBox(QString)));
        break;

    case 4: // Confirm Page
        if (radioManualProvider->isChecked()) {
            labelProvider->setText("    " + lineEditProvider->text() + ", " + country);
            provider = lineEditProvider->text();
        } else {
            labelProvider->setText("    " + mProvidersList->currentItem()->text() + ", " + country);
            provider = mProvidersList->currentItem()->text();
        }

        if (type() == NetworkManager::Settings::ConnectionSettings::Cdma) {
            labelPlanLabel->hide();
            labelPlan->hide();
            labelApn->hide();
            userApn->clear();
            apn.clear();
        } else {
            labelPlanLabel->show();
            labelPlan->show();
            labelApn->show();

            if (mPlanComboBox->currentText() == i18nc("Mobile Connection Wizard", "My plan is not listed...")) {
                labelPlan->setText("    " + userApn->text());
                labelApn->setText("    " + i18nc("Mobile Connection Wizard", "APN") + ": " + userApn->text());
                apn = userApn->text();
            } else {
                int i = mPlanComboBox->currentIndex();
                i = i > 1 ? (i-1) : 0; // ignores separator's index (i == 1).

                QStringList mApns = mProviders->getApns(mProvidersList->currentItem()->text());
                labelPlan->setText("    " + mPlanComboBox->currentText());
                labelApn->setText("    " + i18nc("Mobile Connection Wizard", "APN") + ": " + mApns.at(i));
                apn = mApns.at(i);
            }
        }
        break;
    }
}

int MobileConnectionWizard::nextId() const
{
    // Providers page
    if (currentId() == 2 && type() != NetworkManager::Settings::ConnectionSettings::Gsm) {
        // Jumps to Confirm page instead of Plans page if type != Gsm.
        return 4;
    } else {
        return QWizard::nextId();
    }
}

QVariantList MobileConnectionWizard::args()
{
    QVariantList temp;

    switch (type()) {
    case NetworkManager::Settings::ConnectionSettings::Cdma:
        temp << provider << mProviders->getCdmaInfo(provider);
        break;

    case NetworkManager::Settings::ConnectionSettings::Gsm:
        temp << provider /*<< mProviders->getNetworkIds(provider)*/ << mProviders->getApnInfo(apn);
        break;

    default:
        break;
    }
    return temp;
}

/**********************************************************/
/* Intro page */
/**********************************************************/

QWizardPage * MobileConnectionWizard::createIntroPage()
{
    QWizardPage *page = new QWizardPage();
    page->setTitle(i18nc("Mobile Connection Wizard", "Set up a Mobile Broadband Connection"));
    QVBoxLayout *layout = new QVBoxLayout;

    QLabel *label = new QLabel(i18nc("Mobile Connection Wizard", "This assistant helps you easily set up a mobile broadband connection to a cellular (3G) network."));
    label->setWordWrap(true);
    layout->addWidget(label);

    label = new QLabel('\n' + i18nc("Mobile Connection Wizard", "You will need the following information:"));
    layout->addWidget(label);

    label = new QLabel(QString("  . %1\n  . %2\n  . %3").
                       arg(i18nc("Mobile Connection Wizard", "Your broadband provider's name")).
                       arg(i18nc("Mobile Connection Wizard", "Your broadband billing plan name")).
                       arg(i18nc("Mobile Connection Wizard", "(in some cases) Your broadband billing plan APN (Access Point Name)")));
    layout->addWidget(label);

    if (!mInitialMethodType) {
        label = new QLabel('\n' + i18nc("Mobile Connection Wizard", "Create a connection for &this mobile broadband device:"));
        layout->addWidget(label);

        mDeviceComboBox = new KComboBox();
        mDeviceComboBox->addItem(i18nc("Mobile Connection Wizard", "Any GSM device"));
        mDeviceComboBox->setItemData(0, NetworkManager::Settings::ConnectionSettings::Gsm);
        mDeviceComboBox->addItem(i18nc("Mobile Connection Wizard", "Any CDMA device"));
        mDeviceComboBox->setItemData(1, NetworkManager::Settings::ConnectionSettings::Cdma);
        mDeviceComboBox->insertSeparator(NUMBER_OF_STATIC_ENTRIES-1);
        label->setBuddy(mDeviceComboBox);
        layout->addWidget(mDeviceComboBox);

        QObject::connect(NetworkManager::notifier(), SIGNAL(deviceAdded(QString)),
                         this, SLOT(introDeviceAdded(QString)));
        QObject::connect(NetworkManager::notifier(), SIGNAL(deviceRemoved(QString)),
                         this, SLOT(introDeviceRemoved(QString)));
        QObject::connect(NetworkManager::notifier(), SIGNAL(statusChanged(NetworkManager::Status)),
                         this, SLOT(introStatusChanged(NetworkManager::Status)));

        introAddInitialDevices();
    }

    page->setLayout(layout);

    return page;
}

void MobileConnectionWizard::introAddDevice(const NetworkManager::Device::Ptr &device)
{
    QString desc;

    ModemManager::ModemInterface::Ptr modem = ModemManager::findModemInterface(device->udi(), ModemManager::ModemInterface::GsmCard);
    if (modem) {
        if (modem->enabled()) {
            desc.append(modem->getInfo().manufacturer);
            desc.append(" ");
            desc.append(modem->getInfo().model);
        } else {
            QString deviceName = modem->masterDevice();
            foreach (const Solid::Device &d, Solid::Device::allDevices()) {
                if (d.udi().contains(deviceName, Qt::CaseInsensitive)) {
                    deviceName = d.product();
                    if (!deviceName.startsWith(d.vendor())) {
                        deviceName = d.vendor() + ' ' + deviceName;
                    }
                    desc.append(deviceName);
                    break;
                }
            }
        }
    }

    NetworkManager::ModemDevice::Ptr nmModemIface = device.objectCast<NetworkManager::ModemDevice>();
    if (!nmModemIface) {
        return;
    }

    if (UiUtils::modemSubType(nmModemIface->currentCapabilities()) == NetworkManager::ModemDevice::GsmUmts) {
        if (desc.isEmpty()) {
            desc.append(i18nc("Mobile Connection Wizard", "Installed GSM device"));
        }
    } else if (UiUtils::modemSubType(nmModemIface->currentCapabilities()) == NetworkManager::ModemDevice::CdmaEvdo) {
        if (desc.isEmpty()) {
            desc.append(i18nc("Mobile Connection Wizard", "Installed CDMA device"));
        }
    } else {
        return;
    }

    mDeviceComboBox->addItem(desc, device->uni());

    if (mDeviceComboBox->count() == NUMBER_OF_STATIC_ENTRIES) {
        mDeviceComboBox->setCurrentIndex(0);
    } else {
        mDeviceComboBox->setCurrentIndex(NUMBER_OF_STATIC_ENTRIES);
    }
}

void MobileConnectionWizard::introDeviceAdded(const QString &uni)
{
    introAddDevice(NetworkManager::findNetworkInterface(uni));
}

void MobileConnectionWizard::introDeviceRemoved(const QString &uni)
{
    int index = mDeviceComboBox->findData(uni);

    mDeviceComboBox->removeItem(index);

    if (mDeviceComboBox->count() == NUMBER_OF_STATIC_ENTRIES) {
        mDeviceComboBox->setCurrentIndex(0);
        if (currentId() > 0) {
            close();
        }
    } else {
        mDeviceComboBox->setCurrentIndex(NUMBER_OF_STATIC_ENTRIES);
    }
}

void MobileConnectionWizard::introStatusChanged(NetworkManager::Status status)
{
    switch (status) {
    case NetworkManager::Unknown:
    case NetworkManager::Asleep:
    case NetworkManager::Disconnected:
    case NetworkManager::Disconnecting:
        introRemoveAllDevices();
        break;
    case NetworkManager::Connecting:
    case NetworkManager::ConnectedLinkLocal:
    case NetworkManager::ConnectedSiteOnly:
    case NetworkManager::Connected:
        introAddInitialDevices();
        break;
    }
}

void MobileConnectionWizard::introAddInitialDevices()
{
    foreach(const NetworkManager::Device::Ptr & n, NetworkManager::networkInterfaces()) {
        introAddDevice(n);
    }

    if (mDeviceComboBox->count() == NUMBER_OF_STATIC_ENTRIES) {
        mDeviceComboBox->setCurrentIndex(0);
    } else {
        mDeviceComboBox->setCurrentIndex(NUMBER_OF_STATIC_ENTRIES);
    }
}

void MobileConnectionWizard::introRemoveAllDevices()
{
    mDeviceComboBox->clear();
    mDeviceComboBox->addItem(i18nc("Mobile Connection Wizard", "Any GSM device"));
    mDeviceComboBox->setItemData(0, NetworkManager::Settings::ConnectionSettings::Gsm);
    mDeviceComboBox->addItem(i18nc("Mobile Connection Wizard", "Any CDMA device"));
    mDeviceComboBox->setItemData(1, NetworkManager::Settings::ConnectionSettings::Cdma);
    mDeviceComboBox->insertSeparator(NUMBER_OF_STATIC_ENTRIES-1);
    mDeviceComboBox->setCurrentIndex(0);
}

/**********************************************************/
/* Country page */
/**********************************************************/

QWizardPage * MobileConnectionWizard::createCountryPage()
{
    QWizardPage *page = new QWizardPage();
    page->setTitle(i18nc("Mobile Connection Wizard", "Choose your Provider's Country"));
    QVBoxLayout *layout = new QVBoxLayout;

    QLabel *label = new QLabel(i18nc("Mobile Connection Wizard", "Country List:"));
    layout->addWidget(label);

    mCountryList = new QListWidget();
    mCountryList->addItem(i18nc("Mobile Connection Wizard", "My country is not listed"));
    mCountryList->insertItems(1, mProviders->getCountryList());
    layout->addWidget(mCountryList);

    page->setLayout(layout);

    return page;
}

/**********************************************************/
/* Providers page */
/**********************************************************/

QWizardPage * MobileConnectionWizard::createProvidersPage()
{
    QWizardPage *page = new QWizardPage();
    page->setTitle(i18nc("Mobile Connection Wizard", "Choose your Provider"));
    QVBoxLayout *layout = new QVBoxLayout;

    radioAutoProvider = new QRadioButton(i18nc("Mobile Connection Wizard", "Select your provider from a &list:"));
    radioAutoProvider->setChecked(true);
    layout->addWidget(radioAutoProvider);

    mProvidersList = new QListWidget();
    connect(mProvidersList, SIGNAL(itemSelectionChanged()), this, SLOT(slotCheckProviderList()));
    connect(mProvidersList, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(slotCheckProviderList()));
    layout->addWidget(mProvidersList);

    radioManualProvider = new QRadioButton(i18nc("Mobile Connection Wizard", "I can't find my provider and I wish to enter it &manually:"));
    layout->addWidget(radioManualProvider);
    connect(radioManualProvider, SIGNAL(toggled(bool)), this, SLOT(slotEnableProviderEdit(bool)));

    lineEditProvider = new KLineEdit();
    layout->addWidget(lineEditProvider);
    connect(lineEditProvider, SIGNAL(textEdited(QString)), this, SLOT(slotCheckProviderEdit()));

    page->setLayout(layout);

    return page;
}

void MobileConnectionWizard::slotEnableProviderEdit(bool checked)
{
    if (checked) {
        lineEditProvider->setFocus();
    } else {
        mProvidersList->setFocus();
    }
}

void MobileConnectionWizard::slotCheckProviderEdit()
{
    radioManualProvider->setChecked(true);
    // TODO: this does not work, try reimplementing QWizardPage::isComplete()
    //button(QWizard::NextButton)->setEnabled(true);
}

void MobileConnectionWizard::slotCheckProviderList()
{
    radioAutoProvider->setChecked(true);
    lineEditProvider->clear();
}

/**********************************************************/
/* Plan page */
/**********************************************************/

QWizardPage * MobileConnectionWizard::createPlansPage()
{
    QWizardPage *page = new QWizardPage();
    page->setTitle(i18nc("Mobile Connection Wizard", "Choose your Billing Plan"));
    QBoxLayout *layout = new QBoxLayout(QBoxLayout::TopToBottom);

    QLabel *label = new QLabel(i18nc("Mobile Connection Wizard", "&Select your plan:"));
    layout->addWidget(label);

    mPlanComboBox = new KComboBox();
    label->setBuddy(mPlanComboBox);
    layout->addWidget(mPlanComboBox);

    label = new QLabel('\n' + i18nc("Mobile Connection Wizard", "Selected plan &APN (Access Point Name):"));
    layout->addWidget(label);

    userApn = new KLineEdit();
    userApn->setEnabled(false);
    label->setBuddy(userApn);
    layout->addWidget(userApn);

    QHBoxLayout *layout2 = new QHBoxLayout();
    label = new QLabel();
    label->setPixmap(KIconLoader::global()->loadIcon("dialog-warning", KIconLoader::Dialog));
    layout2->addWidget(label, 0, Qt::AlignTop);
    label = new QLabel(i18nc("Mobile Connection Wizard", "Warning: Selecting an incorrect plan may result in billing issues for your broadband account or may prevent connectivity.\n\nIf you are unsure of your plan please ask your provider for your plan's APN."));
    label->setWordWrap(true);
    layout2->addWidget(label);
    layout->addWidget(new QLabel(""));
    layout->addLayout(layout2);

    page->setLayout(layout);

    return page;
}

void MobileConnectionWizard::slotEnablePlanEditBox(const QString & text)
{
    if (type() != NetworkManager::Settings::ConnectionSettings::Gsm) {
        return;
    }
    if (text == i18nc("Mobile Connection Wizard", "My plan is not listed...")) {
        userApn->clear();
        userApn->setEnabled(true);
    } else {
        if (mProvidersList->currentItem() != 0) {
            int i = mPlanComboBox->currentIndex();
            if (i>0) i=i-1; // Skiping the separator (i==1)
            QStringList mApns = mProviders->getApns(mProvidersList->currentItem()->text());
            userApn->setText(mApns.at(i));
        }
        userApn->setEnabled(false);
    }
}

/**********************************************************/
/* Confirm page */
/**********************************************************/

QWizardPage * MobileConnectionWizard::createConfirmPage()
{
    QWizardPage *page = new QWizardPage();
    page->setTitle(i18nc("Mobile Connection Wizard", "Confirm Mobile Broadband Settings"));
    QVBoxLayout *layout = new QVBoxLayout;

    QLabel *label = new QLabel(i18nc("Mobile Connection Wizard", "Your mobile broadband connection is configured with the following settings:"));
    label->setWordWrap(true);
    layout->addWidget(label);

    label = new QLabel('\n' + i18nc("Mobile Connection Wizard", "Your Provider:"));
    layout->addWidget(label);
    labelProvider = new QLabel();
    layout->addWidget(labelProvider);

    labelPlanLabel = new QLabel('\n' + i18nc("Mobile Connection Wizard", "Your Plan:"));
    layout->addWidget(labelPlanLabel);
    labelPlan = new QLabel();
    layout->addWidget(labelPlan);
    labelApn = new QLabel();
    labelApn->setEnabled(false);
    layout->addWidget(labelApn);

    page->setLayout(layout);

    return page;
}
