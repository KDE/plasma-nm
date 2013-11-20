/*
    Copyright 2009 Will Stephenson <wstephenson@kde.org>
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

#include "modemmonitor.h"

#include <QDBusPendingReply>

#include <KLocale>
#include <KMessageBox>
#include <kdeversion.h>
#include <KDebug>

#include <ModemManagerQt/manager.h>
#ifdef MODEMMANAGERQT_ONE
#include <ModemManager/ModemManager.h>
#include <ModemManagerQt/modemdevice.h>
#include <ModemManagerQt/modem.h>
#include <ModemManagerQt/sim.h>
#else
#include <ModemManagerQt/modemgsmcardinterface.h>
#endif

#include "pindialog.h"

class ModemMonitorPrivate
{
public:
    QWeakPointer<PinDialog> dialog;
};

ModemMonitor::ModemMonitor(QObject * parent)
    :QObject(parent), d_ptr(new ModemMonitorPrivate)
{
    Q_D(ModemMonitor);
    d->dialog.clear();

    QObject::connect(ModemManager::notifier(), SIGNAL(modemAdded(QString)), SLOT(modemAdded(QString)));
#ifdef MODEMMANAGERQT_ONE
    foreach (const ModemManager::ModemDevice::Ptr &iface, ModemManager::modemDevices()) {
        modemAdded(iface->uni());
    }
#else
    foreach (const ModemManager::ModemInterface::Ptr &iface, ModemManager::modemInterfaces()) {
        modemAdded(iface->udi());
    }
#endif
}

ModemMonitor::~ModemMonitor()
{
    delete d_ptr;
}

#ifdef MODEMMANAGERQT_ONE
void ModemMonitor::modemAdded(const QString & udi)
{
    Q_D(ModemMonitor);

    ModemManager::ModemDevice::Ptr modemDevice = ModemManager::findModemDevice(udi);
    ModemManager::Modem::Ptr modem;
    if (modemDevice) {
        modem = modemDevice->interface(ModemManager::ModemDevice::ModemInterface).objectCast<ModemManager::Modem>();
    } else {
        return;
    }

    connect(modem.data(), SIGNAL(unlockRequiredChanged(MMModemLock)), SLOT(requestPin(MMModemLock)));

    if (d->dialog || (modem && modem->unlockRequired() == MM_MODEM_LOCK_NONE) || (modem && modem->unlockRequired() == MM_MODEM_LOCK_UNKNOWN)) {
        return;
    }

    if (modem) {
        // Using queued invocation to prevent kded stalling here until user enters the pin.
        QMetaObject::invokeMethod(modem.data(), "unlockRequiredChanged", Qt::QueuedConnection,
                                  Q_ARG(MMModemLock, modem->unlockRequired()));
    }
}

void ModemMonitor::requestPin(MMModemLock lock)
{
    Q_D(ModemMonitor);
    kDebug() << "unlockRequired == " << lock;
    if (lock == MM_MODEM_LOCK_NONE || lock == MM_MODEM_LOCK_UNKNOWN) {
        return;
    }

    ModemManager::Modem *modem = qobject_cast<ModemManager::Modem *>(sender());
    if (!modem) {
        return;
    }

    if (d->dialog) {
        kDebug() << "PinDialog already running";
        return;
    }

    if (lock == MM_MODEM_LOCK_SIM_PIN) {
        d->dialog = new PinDialog(modem, PinDialog::SimPin);
    } else if (lock == MM_MODEM_LOCK_SIM_PIN2) {
        d->dialog = new PinDialog(modem, PinDialog::SimPin2);
    } else if (lock == MM_MODEM_LOCK_SIM_PUK) {
        d->dialog = new PinDialog(modem, PinDialog::SimPuk);
    } else if (lock == MM_MODEM_LOCK_SIM_PUK2 ) {
        d->dialog = new PinDialog(modem, PinDialog::SimPuk);
    } else if (lock == MM_MODEM_LOCK_PH_SP_PIN) {
        d->dialog = new PinDialog(modem, PinDialog::ModemServiceProviderPin);
    } else if (lock == MM_MODEM_LOCK_PH_SP_PUK) {
        d->dialog = new PinDialog(modem, PinDialog::ModemServiceProviderPuk);
    } else if (lock == MM_MODEM_LOCK_PH_NET_PIN) {
        d->dialog = new PinDialog(modem, PinDialog::ModemNetworkPin);
    } else if (lock == MM_MODEM_LOCK_PH_NET_PUK) {
        d->dialog = new PinDialog(modem, PinDialog::ModemNetworkPuk);
    } else if (lock == MM_MODEM_LOCK_PH_SIM_PIN) {
        d->dialog = new PinDialog(modem, PinDialog::ModemPin);
    } else if (lock == MM_MODEM_LOCK_PH_CORP_PIN) {
        d->dialog = new PinDialog(modem, PinDialog::ModemCorporatePin);
    } else if (lock == MM_MODEM_LOCK_PH_CORP_PUK) {
        d->dialog = new PinDialog(modem, PinDialog::ModemCorporatePuk);
    } else if (lock == MM_MODEM_LOCK_PH_FSIM_PIN) {
        d->dialog = new PinDialog(modem, PinDialog::ModemPhFsimPin);
    } else if (lock == MM_MODEM_LOCK_PH_FSIM_PUK) {
        d->dialog = new PinDialog(modem, PinDialog::ModemPhFsimPuk);
    } else if (lock == MM_MODEM_LOCK_PH_NETSUB_PIN) {
        d->dialog = new PinDialog(modem, PinDialog::ModemNetworkSubsetPin);
    } else if (lock == MM_MODEM_LOCK_PH_NETSUB_PUK) {
        d->dialog = new PinDialog(modem, PinDialog::ModemNetworkSubsetPuk);
    }

    if (d->dialog.data()->exec() != QDialog::Accepted) {
        goto OUT;
    }

    kDebug() << "Sending unlock code";

    {
        ModemManager::Sim::Ptr sim;
        ModemManager::ModemDevice::Ptr modemDevice = ModemManager::findModemDevice(modem->uni());
        if (modemDevice && modemDevice->sim()) {
            sim = modemDevice->sim();
        }

        if (!sim) {
            return;
        }

        QDBusPendingCallWatcher *watcher = 0;

        if (d->dialog.data()->type() == PinDialog::SimPin ||
            d->dialog.data()->type() == PinDialog::SimPin2 ||
            d->dialog.data()->type() == PinDialog::ModemServiceProviderPin ||
            d->dialog.data()->type() == PinDialog::ModemNetworkPin ||
            d->dialog.data()->type() == PinDialog::ModemPin ||
            d->dialog.data()->type() == PinDialog::ModemCorporatePin ||
            d->dialog.data()->type() == PinDialog::ModemPhFsimPin ||
            d->dialog.data()->type() == PinDialog::ModemNetworkSubsetPin) {
            QDBusPendingCall reply = sim->sendPin(d->dialog.data()->pin());
            watcher = new QDBusPendingCallWatcher(reply, sim.data());
        } else if (d->dialog.data()->type() == PinDialog::SimPuk ||
            d->dialog.data()->type() == PinDialog::SimPuk2 ||
            d->dialog.data()->type() == PinDialog::ModemServiceProviderPuk ||
            d->dialog.data()->type() == PinDialog::ModemNetworkPuk ||
            d->dialog.data()->type() == PinDialog::ModemCorporatePuk ||
            d->dialog.data()->type() == PinDialog::ModemPhFsimPuk ||
            d->dialog.data()->type() == PinDialog::ModemNetworkSubsetPuk) {
            QDBusPendingCall reply = sim->sendPuk(d->dialog.data()->puk(), d->dialog.data()->pin());
            watcher = new QDBusPendingCallWatcher(reply, sim.data());
        }

        connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)), SLOT(onSendPinArrived(QDBusPendingCallWatcher*)));
    }

OUT:
    if(d->dialog) {
        d->dialog.data()->deleteLater();
    }
    d->dialog.clear();
}
#else
void ModemMonitor::modemAdded(const QString & udi)
{
    Q_D(ModemMonitor);
    ModemManager::ModemGsmCardInterface::Ptr modem;
    modem = ModemManager::findModemInterface(udi, ModemManager::ModemInterface::GsmCard).objectCast<ModemManager::ModemGsmCardInterface>();

    if (!modem) {
        return;
    }

    connect(modem.data(), SIGNAL(unlockRequiredChanged(QString)), SLOT(requestPin(QString)));

    if (d->dialog || modem->unlockRequired().isEmpty()) {
        return;
    }

#if 0 // TODO
    KNetworkManagerServicePrefs::self()->readConfig();
    if (KNetworkManagerServicePrefs::self()->askForGsmPin() != KNetworkManagerServicePrefs::OnModemDetection) {
        return;
    }
#endif

    // Using queued invocation to prevent kded stalling here until user enters the pin.
    QMetaObject::invokeMethod(modem.data(), "unlockRequiredChanged", Qt::QueuedConnection,
                              Q_ARG(QString, modem->unlockRequired()));
}

void ModemMonitor::requestPin(const QString & unlockRequired)
{
    Q_D(ModemMonitor);
    kDebug() << "unlockRequired == " << unlockRequired;
    if (unlockRequired.isEmpty()) {
        return;
    }

    ModemManager::ModemGsmCardInterface * modem = qobject_cast<ModemManager::ModemGsmCardInterface *>(sender());
    if (!modem) {
        return;
    }

    if (d->dialog) {
        kDebug() << "PinDialog already running";
        return;
    }

    if (unlockRequired == QLatin1String("sim-pin")) {
        d->dialog = new PinDialog(modem, PinDialog::Pin);
    } else if (unlockRequired == QLatin1String("sim-puk")) {
        d->dialog = new PinDialog(modem, PinDialog::PinPuk);
    } else {
        kWarning() << "Unhandled unlock request for '" << unlockRequired << "'";
        return;
    }

    if (d->dialog.data()->exec() != QDialog::Accepted) {
        goto OUT;
    }

    kDebug() << "Sending unlock code";

    {
        QDBusPendingCallWatcher *watcher = 0;

        if (d->dialog.data()->type() == PinDialog::Pin) {
            QDBusPendingCall reply = modem->sendPin(d->dialog.data()->pin());
            watcher = new QDBusPendingCallWatcher(reply, modem);
        } else if (d->dialog.data()->type() == PinDialog::PinPuk) {
            QDBusPendingCall reply = modem->sendPuk(d->dialog.data()->puk(), d->dialog.data()->pin());
            watcher = new QDBusPendingCallWatcher(reply, modem);
        }

        connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)), SLOT(onSendPinArrived(QDBusPendingCallWatcher*)));
    }

OUT:
    if(d->dialog) {
        d->dialog.data()->deleteLater();
    }
    d->dialog.clear();
}
#endif

void ModemMonitor::onSendPinArrived(QDBusPendingCallWatcher * watcher)
{
    QDBusPendingReply<> reply = *watcher;

    if (reply.isValid()) {
        // Automatically enabling this for cell phones with expensive data plans is not a good idea.
        //NetworkManager::setWwanEnabled(true);
    } else {
        KMessageBox::error(0, i18nc("Text in GSM PIN/PUK unlock error dialog", "Error unlocking modem: %1", reply.error().message()),
                           i18nc("Title for GSM PIN/PUK unlock error dialog", "PIN/PUK unlock error"));
    }

    watcher->deleteLater();
}
