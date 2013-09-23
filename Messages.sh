#!/usr/bin/env bash

$EXTRACTRC `find declarative-plugins -name '*.ui' -o -name '*.rc'` `find settings/details -name '*.ui' -o -name '*.rc'` >> rc.cpp
$XGETTEXT rc.cpp `find applet -name '*.cpp'` `find declarative-plugins -name '*.cpp'` `find lib -name '*.cpp' | grep -v 'lib/editor'` `find settings/details -name '*.cpp'` -o $podir/plasma_applet_org.kde.networkmanagement.pot
$XGETTEXT `find applet -name '*.qml'` -j -L Java -o $podir/plasma_applet_org.kde.networkmanagement.pot
rm -f rc.cpp
