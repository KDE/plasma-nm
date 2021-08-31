/*
 *   Copyright 2018 Martin Kacej <m.kacej@atlas.sk>
 *             2020-2021 Devin Lin <espidev@gmail.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */


#pragma once

#include <QSharedPointer>

#include <KQuickAddons/ConfigModule>

#include "mobileproviders.h"
#include "modem.h"
#include "modemdetails.h"
#include "sim.h"

#include <NetworkManagerQt/ConnectionSettings>
#include <NetworkManagerQt/GsmSetting>
#include <NetworkManagerQt/CdmaSetting>
#include <NetworkManagerQt/ModemDevice>
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/Settings>

#include <ModemManagerQt/Manager>
#include <ModemManagerQt/GenericTypes>
#include <ModemManagerQt/ModemDevice>

class Sim;
class Modem;

class InlineMessage : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int type READ type NOTIFY typeChanged)
    Q_PROPERTY(QString message READ message NOTIFY messageChanged)

public:
    enum Type { Information, Positive, Warning, Error };

    InlineMessage(QObject *parent = nullptr, Type type = Information, QString message = "");

    Type type();
    QString message();

Q_SIGNALS:
    void typeChanged();
    void messageChanged();

private:
    Type m_type;
    QString m_message;
};

class CellularNetworkSettings : public KQuickAddons::ConfigModule
{
    Q_OBJECT
    Q_PROPERTY(bool modemFound READ modemFound NOTIFY modemFoundChanged)
    Q_PROPERTY(Modem *selectedModem READ selectedModem NOTIFY selectedModemChanged)
    Q_PROPERTY(QList<Sim *> sims READ sims NOTIFY simsChanged)
    Q_PROPERTY(QList<InlineMessage *> messages READ messages NOTIFY messagesChanged)

public:
    CellularNetworkSettings(QObject *parent, const QVariantList &args);

    static CellularNetworkSettings *instance();

    Modem *selectedModem();
    QList<Modem *> modems();
    QList<Sim *> sims();
    
    bool modemFound();

    QList<InlineMessage *> messages();
    void addMessage(InlineMessage::Type type, QString msg);
    Q_INVOKABLE void removeMessage(int index);

Q_SIGNALS:
    void modemFoundChanged();
    void selectedModemChanged();
    void simsChanged();
    void messagesChanged();

private:
    void fillSims();
    
    QList<Modem *> m_modemList;
    QList<Sim *> m_simList;
    
    MobileProviders *m_providers;

    QList<InlineMessage *> m_messages;

    static CellularNetworkSettings *staticInst;
};
