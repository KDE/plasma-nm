/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef PLASMA_NM_WIREGUARD_PEER_MODEL_H
#define PLASMA_NM_WIREGUARD_PEER_MODEL_H

#include "plasmanm_editorqml_export.h"

#include <NetworkManagerQt/WireguardSetting>

#include <QAbstractListModel>

class WireGuardKeyValidator;

class PLASMANM_EDITORQML_EXPORT WireguardPeerModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
    Q_PROPERTY(bool valid READ isValid NOTIFY validChanged)

public:
    enum PresharedKeyOption {
        StoreForUser = 0,
        StoreForAllUsers,
        NotRequired,
    };
    Q_ENUM(PresharedKeyOption)

    enum Roles {
        PublicKeyRole = Qt::UserRole + 1,
        PresharedKeyRole,
        PresharedKeyOptionRole,
        AllowedIpsRole,
        EndpointAddressRole,
        EndpointPortRole,
        PersistentKeepaliveRole,
    };

    explicit WireguardPeerModel(QObject *parent = nullptr);
    ~WireguardPeerModel() override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    QHash<int, QByteArray> roleNames() const override;

    NMVariantMapList peers() const;
    void setPeers(const NMVariantMapList &peers);

    void loadSecrets(const NMVariantMapList &peers);

    bool isValid() const;

    Q_INVOKABLE void addPeer();
    Q_INVOKABLE void removePeer(int row);

    Q_INVOKABLE void beginEdit();
    Q_INVOKABLE void revertEdit();

Q_SIGNALS:
    void countChanged();
    void validChanged();
    void changed();

private:
    bool isKeyValid(const QString &key) const;

    WireGuardKeyValidator *const m_keyValidator;

    NMVariantMapList m_peers;
    NMVariantMapList m_snapshot;
};

#endif // PLASMA_NM_WIREGUARD_PEER_MODEL_H
