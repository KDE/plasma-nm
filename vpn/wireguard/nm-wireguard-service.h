/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/* nm-wireguard-service - WireGuard integration with NetworkManager
 *
 *  Copyright 2018 Bruce Anderson <banderson19com@san.rr.com>
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#ifndef NM_WIREGUARD_SERVICE_H
#define NM_WIREGUARD_SERVICE_H

#define NM_DBUS_SERVICE_WIREGUARD    "org.freedesktop.NetworkManager.wireguard"
#define NM_DBUS_INTERFACE_WIREGUARD  "org.freedesktop.NetworkManager.wireguard"
#define NM_DBUS_PATH_WIREGUARD       "/org/freedesktop/NetworkManager/wireguard"

#define NM_WG_KEY_ADDR_IP4      "local-ip4"
#define NM_WG_KEY_ADDR_IP6      "local-ip6"
#define NM_WG_KEY_LISTEN_PORT   "local-listen-port"
#define NM_WG_KEY_PRIVATE_KEY   "local-private-key"
#define NM_WG_KEY_DNS           "connection-dns"
#define NM_WG_KEY_MTU           "connection-mtu"
#define NM_WG_KEY_TABLE         "connection_table"
#define NM_WG_KEY_PUBLIC_KEY    "peer-public-key"
#define NM_WG_KEY_ALLOWED_IPS   "peer-allowed-ips"
#define NM_WG_KEY_ENDPOINT      "peer-endpoint"
#define NM_WG_KEY_PRESHARED_KEY "peer-preshared-key"
#define NM_WG_KEY_FWMARK        "fwmark"
#define NM_WG_KEY_PRE_UP        "script-pre-up"
#define NM_WG_KEY_POST_UP       "script-post-up"
#define NM_WG_KEY_PRE_DOWN      "script-pre-down"
#define NM_WG_KEY_POST_DOWN     "script-post-down"
#define NM_WG_KEY_PERSISTENT_KEEPALIVE "peer-persistent-keep-alive"

#endif /* NM_WIREGUARD_SERVICE_H */
