/*
    Copyright 2013 Jan Grulich <jgrulich@redhat.com>
    Copyright 2013 Lukas Tinkl <ltinkl@redhat.com>
    Copyright 2013 by Daniel Nicoletti <dantti12@gmail.com>

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

#ifndef PLASMA_NM_SECRET_AGENT_H
#define PLASMA_NM_SECRET_AGENT_H

#include <QtNetworkManager/secretagent.h>

#include <kdemacros.h>

namespace KWallet {
class Wallet;
}

class PasswordDialog;
class SecretsRequest {
public:
    enum Type {
        GetSecrets,
        SaveSecrets,
        DeleteSecrets
    };
    SecretsRequest(Type _type) :
        type(_type),
        flags(NetworkManager::SecretAgent::None),
        dialog(0)
    {}
    inline bool operator==(const QString &other) const {
        return callId == other;
    }
    Type type;
    QString callId;
    NMVariantMapMap connection;
    QDBusObjectPath connection_path;
    QString setting_name;
    QStringList hints;
    NetworkManager::SecretAgent::GetSecretsFlags flags;
    QDBusMessage message;
    PasswordDialog *dialog;
};

class KDE_EXPORT SecretAgent : public NetworkManager::SecretAgent
{
    Q_OBJECT
public:
    explicit SecretAgent(QObject* parent = 0);
    virtual ~SecretAgent();

public Q_SLOTS:
    virtual NMVariantMapMap GetSecrets(const NMVariantMapMap&, const QDBusObjectPath&, const QString&, const QStringList&, uint);
    virtual void SaveSecrets(const NMVariantMapMap&, const QDBusObjectPath&);
    virtual void DeleteSecrets(const NMVariantMapMap &, const QDBusObjectPath &);
    virtual void CancelGetSecrets(const QDBusObjectPath &, const QString &);

private Q_SLOTS:
    void dialogAccepted();
    void dialogRejected();
    void killDialogs();
    void walletOpened(bool success);
    void walletClosed();

private:
    void processNext(bool ignoreWallet = false);
    /**
     * @brief processGetSecrets requests
     * @param request the request we are processing
     * @param ignoreWallet true if the code should avoid Wallet
     * nomally if it failed to open
     * @return true if the item was processed
     */
    bool processGetSecrets(SecretsRequest &request, bool ignoreWallet) const;
    bool processSaveSecrets(SecretsRequest &request, bool ignoreWallet) const;
    bool processDeleteSecrets(SecretsRequest &request, bool ignoreWallet) const;
    /**
     * @brief useWallet checks if the KWallet system is enabled
     * and tries to open it async.
     * @return return true if the method should use the wallet,
     * the caller MUST always check if the wallet is opened.
     */
    bool useWallet() const;
    void sendSecrets(const NMVariantMapMap &secrets, const QDBusMessage &message) const;

    mutable KWallet::Wallet *m_wallet;
    mutable PasswordDialog *m_dialog;
    QList<SecretsRequest> m_calls;
};

#endif // PLASMA_NM_SECRET_AGENT_H
