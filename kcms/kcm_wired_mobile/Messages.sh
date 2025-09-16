#!/usr/bin/env bash
# SPDX-FileCopyrightText: 2025 Sebastian Kügler <sebas@kde.org>
# SPDX-License-Identifier: LGPL-2.0-or-later
$XGETTEXT `find . -name \*.cpp -o -name \*.qml` -o $podir/kcm_mobile_wired.pot
