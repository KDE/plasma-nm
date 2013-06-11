/* -*- Mode: C; tab-width: 5; indent-tabs-mode: t; c-basic-offset: 5 -*- */
/* NetworkManager -- Network link manager
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
 *   Copyright © 2008 - 2009 Intel Corporation. //krazy:exclude=copyright
 *
 * Based on nm-openconnect-vpnc.h:
 *   Copyright © 2005 - 2008 Red Hat, Inc. //krazy:exclude=copyright
 *   Copyright © 2007 - 2008 Novell, Inc. //krazy:exclude=copyright
 */

#ifndef NM_OPENCONNECT_SERVICE_H
#define NM_OPENCONNECT_SERVICE_H

#define NM_DBUS_SERVICE_OPENCONNECT    "org.freedesktop.NetworkManager.openconnect"
#define NM_DBUS_INTERFACE_OPENCONNECT  "org.freedesktop.NetworkManager.openconnect"
#define NM_DBUS_PATH_OPENCONNECT       "/org/freedesktop/NetworkManager/openconnect"

#define NM_OPENCONNECT_KEY_GATEWAY "gateway"
#define NM_OPENCONNECT_KEY_COOKIE "cookie"
#define NM_OPENCONNECT_KEY_GWCERT "gwcert"
#define NM_OPENCONNECT_KEY_AUTHTYPE "authtype"
#define NM_OPENCONNECT_KEY_USERCERT "usercert"
#define NM_OPENCONNECT_KEY_CACERT "cacert"
#define NM_OPENCONNECT_KEY_PRIVKEY "userkey"
#define NM_OPENCONNECT_KEY_MTU "mtu"
#define NM_OPENCONNECT_KEY_PEM_PASSPHRASE_FSID "pem_passphrase_fsid"
#define NM_OPENCONNECT_KEY_PROXY "proxy"
#define NM_OPENCONNECT_KEY_CSD_ENABLE "enable_csd_trojan"
#define NM_OPENCONNECT_KEY_CSD_WRAPPER "csd_wrapper"

#define NM_OPENCONNECT_USER "nm-openconnect"

#endif /* NM_OPENCONNECT_PLUGIN_H */
