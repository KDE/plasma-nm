/*
    SPDX-FileCopyrightText: 2018 Martin Kacej <m.kacej@atlas.sk>
    SPDX-FileCopyrightText: 2020-2021 Devin Lin <espidev@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "cellularnetworksettings.h"

#include <KPluginFactory>

K_PLUGIN_CLASS_WITH_JSON(CellularNetworkSettings, "kcm_cellular_network.json")

CellularNetworkSettings::CellularNetworkSettings(QObject *parent, const KPluginMetaData &metaData)
    : KQuickConfigModule(parent, metaData)
{
    setButtons({});

    qmlRegisterType<InlineMessage>("cellularnetworkkcm", 1, 0, "InlineMessage");
}

QList<InlineMessage *> CellularNetworkSettings::messages()
{
    return m_messages;
}

void CellularNetworkSettings::addMessage(int type, const QString &msg)
{
    m_messages.push_back(new InlineMessage{this, static_cast<InlineMessage::Type>(type), msg});
    Q_EMIT messagesChanged();
}

void CellularNetworkSettings::removeMessage(int index)
{
    if (index >= 0 && index < m_messages.size()) {
        m_messages.removeAt(index);
        Q_EMIT messagesChanged();
    }
}

InlineMessage::InlineMessage(QObject *parent, Type type, QString message)
    : QObject{parent}
    , m_type{type}
    , m_message{message}
{
}

InlineMessage::Type InlineMessage::type()
{
    return m_type;
}

QString InlineMessage::message()
{
    return m_message;
}

#include "cellularnetworksettings.moc"
