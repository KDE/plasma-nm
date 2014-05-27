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
 * (C) Copyright 2005 - 2012 Red Hat, Inc.
 * (C) Copyright 2007 - 2008 Novell, Inc.
 */

#ifndef NM_VPNC_SERVICE_H
#define NM_VPNC_SERVICE_H

#define NM_DBUS_SERVICE_VPNC    "org.freedesktop.NetworkManager.vpnc"
#define NM_DBUS_INTERFACE_VPNC  "org.freedesktop.NetworkManager.vpnc"
#define NM_DBUS_PATH_VPNC       "/org/freedesktop/NetworkManager/vpnc"

#define NM_VPNC_KEY_GATEWAY "IPSec gateway"
#define NM_VPNC_KEY_ID "IPSec ID"
#define NM_VPNC_KEY_SECRET "IPSec secret"
#define NM_VPNC_KEY_XAUTH_USER "Xauth username"
#define NM_VPNC_KEY_XAUTH_PASSWORD "Xauth password"
#define NM_VPNC_KEY_DOMAIN "Domain"
#define NM_VPNC_KEY_DHGROUP "IKE DH Group"
#define NM_VPNC_KEY_PERFECT_FORWARD "Perfect Forward Secrecy"
#define NM_VPNC_KEY_VENDOR "Vendor"
#define NM_VPNC_KEY_APP_VERSION "Application Version"
#define NM_VPNC_KEY_SINGLE_DES "Enable Single DES"
#define NM_VPNC_KEY_NO_ENCRYPTION "Enable no encryption"
#define NM_VPNC_KEY_NAT_TRAVERSAL_MODE "NAT Traversal Mode"
#define NM_VPNC_KEY_DPD_IDLE_TIMEOUT "DPD idle timeout (our side)"
#define NM_VPNC_KEY_CISCO_UDP_ENCAPS_PORT "Cisco UDP Encapsulation Port"
#define NM_VPNC_KEY_LOCAL_PORT "Local Port"
#define NM_VPNC_KEY_AUTHMODE "IKE Authmode"
#define NM_VPNC_KEY_CA_FILE "CA-File"

#define NM_VPNC_NATT_MODE_NATT        "natt"
#define NM_VPNC_NATT_MODE_NONE        "none"
#define NM_VPNC_NATT_MODE_NATT_ALWAYS "force-natt"
#define NM_VPNC_NATT_MODE_CISCO       "cisco-udp"

#define NM_VPNC_DHGROUP_DH1 "dh1"
#define NM_VPNC_DHGROUP_DH2 "dh2"
#define NM_VPNC_DHGROUP_DH5 "dh5"

#define NM_VPNC_PFS_SERVER "server"
#define NM_VPNC_PFS_NOPFS  "nopfs"
#define NM_VPNC_PFS_DH1    "dh1"
#define NM_VPNC_PFS_DH2    "dh2"
#define NM_VPNC_PFS_DH5    "dh5"

#define NM_VPNC_VENDOR_CISCO     "cisco"
#define NM_VPNC_VENDOR_NETSCREEN "netscreen"

#endif /* NM_VPNC_SERVICE_H */
