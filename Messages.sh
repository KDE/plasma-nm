#!/usr/bin/env bash

$EXTRACTRC `find applet -name '*.ui'` >> rc.cpp
$XGETTEXT rc.cpp `find applet -name '*.cpp'` `find libs -name '*.cpp' | grep -v 'libs/editor'` -o $podir/plasma_applet_org.kde.plasma.networkmanagement.pot
$XGETTEXT `find applet -name '*.qml'` -j -L Java -o $podir/plasma_applet_org.kde.plasma.networkmanagement.pot
rm -f rc.cpp
