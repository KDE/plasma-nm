/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
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
 * (C) Copyright 2005 - 2008 Red Hat, Inc. //krazy:exclude=copyright
 * (C) Copyright 2007 - 2008 Novell, Inc. //krazy:exclude=copyright
 */

#ifndef NM_STRONGSWAN_SERVICE_H
#define NM_STRONGSWAN_SERVICE_H

#define NM_DBUS_SERVICE_STRONGSWAN    "org.freedesktop.NetworkManager.strongswan"
#define NM_DBUS_INTERFACE_STRONGSWAN  "org.freedesktop.NetworkManager.strongswan"
#define NM_DBUS_PATH_STRONGSWAN       "/org/freedesktop/NetworkManager/strongswan"

#define NM_STRONGSWAN_GATEWAY "address"
#define NM_STRONGSWAN_CERTIFICATE "certificate"
#define NM_STRONGSWAN_USER "user"
#define NM_STRONGSWAN_METHOD "method"
#define NM_STRONGSWAN_USERKEY "userkey"
#define NM_STRONGSWAN_USERCERT "usercert"
#define NM_STRONGSWAN_SECRET "password"
#define NM_STRONGSWAN_SECRET_TYPE "secret_type"
#define NM_STRONGSWAN_INNERIP "virtual"
#define NM_STRONGSWAN_ENCAP "encap"
#define NM_STRONGSWAN_IPCOMP "ipcomp"

#define NM_STRONGSWAN_AUTH_KEY "key"
#define NM_STRONGSWAN_AUTH_AGENT "agent"
#define NM_STRONGSWAN_AUTH_SMARTCARD "smartcard"
#define NM_STRONGSWAN_AUTH_EAP "eap"

#define NM_STRONGSWAN_PW_TYPE_SAVE   "save"
#define NM_STRONGSWAN_PW_TYPE_ASK    "ask"
#define NM_STRONGSWAN_PW_TYPE_UNUSED "unused"

#endif /* NM_Strongswan_PLUGIN_H */
