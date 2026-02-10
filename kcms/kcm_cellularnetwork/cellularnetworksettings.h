/*
    SPDX-FileCopyrightText: 2018 Martin Kacej <m.kacej@atlas.sk>
    SPDX-FileCopyrightText: 2020-2021 Devin Lin <espidev@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <KQuickConfigModule>

class InlineMessage : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int type READ type NOTIFY typeChanged)
    Q_PROPERTY(QString message READ message NOTIFY messageChanged)

public:
    enum Type {
        Information,
        Positive,
        Warning,
        Error,
    };
    Q_ENUM(Type)

    InlineMessage(QObject *parent = nullptr, Type type = Information, QString message = QString());

    Type type();
    QString message();

Q_SIGNALS:
    void typeChanged();
    void messageChanged();

private:
    Type m_type;
    QString m_message;
};

class CellularNetworkSettings : public KQuickConfigModule
{
    Q_OBJECT
    Q_PROPERTY(QList<InlineMessage *> messages READ messages NOTIFY messagesChanged)

public:
    CellularNetworkSettings(QObject *parent, const KPluginMetaData &metaData);

    QList<InlineMessage *> messages();
    Q_INVOKABLE void addMessage(int type, const QString &msg);
    Q_INVOKABLE void removeMessage(int index);

Q_SIGNALS:
    void messagesChanged();

private:
    QList<InlineMessage *> m_messages;
};
