/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/* NetworkManager-openswan -- openswan plugin for Network manager

    SPDX-License-Identifier: GPL-2.0-or-later

    (C) SPDX-FileCopyrightText: 2010 Red Hat Inc.
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
