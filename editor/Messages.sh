#! /usr/bin/env bash
$EXTRACTRC `find . -name "*.ui" -o -name "*.rc"` >> rc.cpp
$XGETTEXT `find . -name "*.cpp"` -o $podir/kde-nm-connection-editor.pot
rm -f rc.cpp
