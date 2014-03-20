#!/usr/bin/env bash

$EXTRACTRC `find settings/details -name '*.ui' -o -name '*.rc'` `find applet -name '*.ui'` >> rc.cpp
$XGETTEXT rc.cpp `find applet -name '*.cpp'` `find libs -name '*.cpp' | grep -v 'libs/editor'` `find settings/details -name '*.cpp'` -o $podir/plasma_applet_org.kde.networkmanagement.pot
$XGETTEXT `find applet -name '*.qml'` -j -L Java -o $podir/plasma_applet_org.kde.networkmanagement.pot
rm -f rc.cpp
