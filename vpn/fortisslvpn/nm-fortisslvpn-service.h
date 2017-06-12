/* nm-fortisslvpn-service - SSLVPN integration with NetworkManager
 *
 * Lubomir Rintel <lkundrak@v3.sk>
 * Dan Williams <dcbw@redhat.com>
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
 * (C) Copyright 2008 Red Hat, Inc.
 * (C) Copyright 2015 Lubomir Rintel
 */

#ifndef __NM_SERVICE_DEFINES_H__
#define __NM_SERVICE_DEFINES_H__

/* For the NM <-> VPN plugin service */
#define NM_DBUS_SERVICE_FORTISSLVPN    "org.freedesktop.NetworkManager.fortisslvpn"
#define NM_DBUS_INTERFACE_FORTISSLVPN  "org.freedesktop.NetworkManager.fortisslvpn"
#define NM_DBUS_PATH_FORTISSLVPN       "/org/freedesktop/NetworkManager/fortisslvpn"

#define NM_FORTISSLVPN_KEY_GATEWAY           "gateway"
#define NM_FORTISSLVPN_KEY_USER              "user"
#define NM_FORTISSLVPN_KEY_PASSWORD          "password"
#define NM_FORTISSLVPN_KEY_CA                "ca"
#define NM_FORTISSLVPN_KEY_CERT              "cert"
#define NM_FORTISSLVPN_KEY_KEY               "key"
#define NM_FORTISSLVPN_KEY_TRUSTED_CERT      "trusted-cert"

#endif /* __NM_SERVICE_DEFINES_H__ */
