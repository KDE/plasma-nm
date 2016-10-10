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
 * Copyright © 2012 Guido Günther <agx@sigxcpu.org>
 */

#ifndef NM_IODINE_SERVICE_DEFINES_H
#define NM_IODINE_SERVICE_DEFINES_H

#define NM_DBUS_SERVICE_IODINE    "org.freedesktop.NetworkManager.iodine"
#define NM_DBUS_INTERFACE_IODINE  "org.freedesktop.NetworkManager.iodine"
#define NM_DBUS_PATH_IODINE       "/org/freedesktop/NetworkManager/iodine"

#define NM_IODINE_KEY_TOPDOMAIN  "topdomain"
#define NM_IODINE_KEY_NAMESERVER "nameserver"
#define NM_IODINE_KEY_FRAGSIZE   "fragsize"
#define NM_IODINE_KEY_PASSWORD   "password"

#endif /* NM_IODINE_SERVICE_DEFINES_H */
