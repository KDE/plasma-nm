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
    qRegisterMetaType<MMModemLock>();

    ModemManager::ModemDevice::Ptr modemDevice = ModemManager::findModemDevice(udi);
    ModemManager::Modem::Ptr modemInterface;
    ModemManager::Sim::Ptr sim;
    if (modemDevice) {
        modemInterface = modemDevice->interface(ModemManager::ModemDevice::ModemInterface).objectCast<ModemManager::Modem>();
        if (modemInterface) {
            sim = ModemManager::findSim(modemInterface->simPath());
        }
    } else {
        return;
    }

    connect(sim.data(), SIGNAL(unlockRequiredChanged(MMModemLock)), SLOT(requestPin(MMModemLock)));

    if (d->dialog || (modemInterface && modemInterface->unlockRequired() == MM_MODEM_LOCK_NONE) || (modemInterface && modemInterface->unlockRequired() == MM_MODEM_LOCK_UNKNOWN)) {
        return;
    }

    if (modemInterface) {
        // Using queued invocation to prevent kded stalling here until user enters the pin.
        QMetaObject::invokeMethod(modemInterface.data(), "unlockRequiredChanged", Qt::QueuedConnection,
                                Q_ARG(MMModemLock, modemInterface->unlockRequired()));
    }
}

void ModemMonitor::requestPin(MMModemLock lock)
{
    Q_D(ModemMonitor);
    kDebug() << "unlockRequired == " << lock;
    if (lock == MM_MODEM_LOCK_NONE || lock == MM_MODEM_LOCK_UNKNOWN) {
        return;
    }

    ModemManager::Sim * sim = qobject_cast<ModemManager::Sim *>(sender());
    if (!sim) {
        return;
    }

    if (d->dialog) {
        kDebug() << "PinDialog already running";
        return;
    }

    ModemManager::Modem::Ptr modem;

    foreach (ModemManager::ModemDevice::Ptr modemDevice, ModemManager::modemDevices()) {
        if (modemDevice) {
            ModemManager::Modem::Ptr modemTmp = modemDevice->interface(ModemManager::ModemDevice::ModemInterface).objectCast<ModemManager::Modem>();

            if (modem && modem->simPath() == sim->uni()) {
                modem = modemTmp;
                break;
            }
        }
    }

    if (modem) {
        if (lock == MM_MODEM_LOCK_SIM_PIN) {
            d->dialog = new PinDialog(modem.data(), PinDialog::Pin);
        } else if (lock == MM_MODEM_LOCK_SIM_PUK) {
            d->dialog = new PinDialog(modem.data(), PinDialog::PinPuk);
        } else {
            // TODO handle other lock types?
            kWarning() << "Unhandled unlock request for '" << lock << "'";
            return;
        }
    }

    if (d->dialog.data()->exec() != QDialog::Accepted) {
        goto OUT;
    }

    kDebug() << "Sending unlock code";

    {
        QDBusPendingCallWatcher *watcher = 0;

        if (d->dialog.data()->type() == PinDialog::Pin) {
            QDBusPendingCall reply = sim->sendPin(d->dialog.data()->pin());
            watcher = new QDBusPendingCallWatcher(reply, sim);
        } else if (d->dialog.data()->type() == PinDialog::PinPuk) {
            QDBusPendingCall reply = sim->sendPuk(d->dialog.data()->puk(), d->dialog.data()->pin());
            watcher = new QDBusPendingCallWatcher(reply, sim);
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
