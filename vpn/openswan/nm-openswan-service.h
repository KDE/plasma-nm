/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/* NetworkManager-openswan -- openswan plugin for Network manager
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
 * (C) Copyright 2010 Red Hat, Inc.
 */

#ifndef NM_OPENSWAN_SERVICE_H
#define NM_OPENSWAN_SERVICE_H

#define NM_DBUS_SERVICE_OPENSWAN    "org.freedesktop.NetworkManager.openswan"
#define NM_DBUS_INTERFACE_OPENSWAN  "org.freedesktop.NetworkManager.openswan"
#define NM_DBUS_PATH_OPENSWAN       "/org/freedesktop/NetworkManager/openswan"

#define NM_OPENSWAN_RIGHT  "right"
#define NM_OPENSWAN_LEFTID "leftid"
#define NM_OPENSWAN_PSK_VALUE "pskvalue"
#define NM_OPENSWAN_PSK_INPUT_MODES "pskinputmodes"
#define NM_OPENSWAN_LEFTXAUTHUSER "leftxauthusername"
#define NM_OPENSWAN_XAUTH_PASSWORD "xauthpassword"
#define NM_OPENSWAN_XAUTH_PASSWORD_INPUT_MODES "xauthpasswordinputmodes"
#define NM_OPENSWAN_DOMAIN "Domain"
#define NM_OPENSWAN_DHGROUP "dhgroup"
#define NM_OPENSWAN_PFSGROUP "pfsgroup"
#define NM_OPENSWAN_DPDTIMEOUT "dpdtimeout"
#define NM_OPENSWAN_IKE  "ike"
#define NM_OPENSWAN_ESP  "esp"

#define NM_OPENSWAN_PW_TYPE_SAVE   "save"
#define NM_OPENSWAN_PW_TYPE_ASK    "ask"
#define NM_OPENSWAN_PW_TYPE_UNUSED "unused"

#endif /* NM_OPENSWAN_SERVICE_H */
