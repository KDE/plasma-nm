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
      when you want to explicitly disable ModemManager support, use -DDISABLE_MODEMMANAGER_SUPPORT=true cmake parameter.

  * openconnect
    - if you want to build the OpenConnect VPN plugin

  * NetworkManager-fortisslvpn|iodine|l2tp|openconnect|openswan|openvpn|pptp|ssh|sstp|strongswan|vpnc
    - these are runtime dependencies for VPN plugins

Compiling:
----------
  mkdir build
  cd build
  cmake ../ -DCMAKE_INSTALL_PREFIX=/usr [-DDISABLE_MODEMMANAGER_SUPPORT=true]
  make
  # As root:
  make install


BUGS:
-----
Submit bugs and feature requests to KDE bugzilla, product plasma-nm:

https://bugs.kde.org/describecomponents.cgi?product=plasma-nm


NetworkManager specification:
------------------------------
https://developer.gnome.org/NetworkManager/stable/
https://developer.gnome.org/NetworkManager/stable/ref-settings.html
https://www.freedesktop.org/software/ModemManager/api/latest/
