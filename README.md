Plasma-nm
========================

Plasma applet written in QML for managing network connections

Dependencies:
-------------
  * networkmanager-qt
  * NetworkManager 0.9.10 and newer

Optional dependencies:
---------------------
  * modemmanager-qt
    - requires ModemManager 1.0.0 and newer as runtime dependency
    - Plasma-nm is compiled with ModemManager support by default when modemmanager-qt is found,
      when you want to explicitly disable ModemManager support, use `-DDISABLE_MODEMMANAGER_SUPPORT=true` cmake parameter.

  * openconnect
    - if you want to build the OpenConnect VPN plugin

  * NetworkManager-fortisslvpn|iodine|l2tp|libreswan|openconnect|openvpn|pptp|ssh|sstp|strongswan|vpnc
    - these are runtime dependencies for VPN plugins

Compiling:
----------
**Recommended method**
The best way to develop for KDE is to use [kdesrc-build](https://kdesrc-build.kde.org/), a KDE supported tool chain to manage installation and compilation of KDE applications and dependencies. Please refer to the following guide when developing, building, and testing `plasma-nm`:

https://community.kde.org/Get_Involved/development#Iterating_on_a_single_project

**Expert method (quick and dirty)**
:warning: **The following method is for experienced developers. developers should use the above mentioned recommmended method when developing for KDE. This expert method will install files on your system that may not align with your specific OS best practices and may cause issues.** :warning:

```sh
  mkdir build
  cd build
  cmake ../ -DCMAKE_INSTALL_PREFIX=/usr [-DDISABLE_MODEMMANAGER_SUPPORT=true]
  make
  # As root:
  make install
```

BUGS:
-----
Submit bugs and feature requests to KDE bugzilla, product plasma-nm:

https://bugs.kde.org/describecomponents.cgi?product=plasma-nm


NetworkManager specification:
------------------------------
https://developer.gnome.org/NetworkManager/stable/
https://developer.gnome.org/NetworkManager/stable/ref-settings.html
https://www.freedesktop.org/software/ModemManager/api/latest/
