#! /usr/bin/env bash
# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: 2020 Tobias Fella <fella@posteo.de>
#
$EXTRACTRC `find . -name '*.ui' -or -name '*.rc'` >> rc.cpp || exit 11
$XGETTEXT  `find -name '*.cpp'` -o $podir/accessibilityinspector.pot
rm -rf rc.cpp
