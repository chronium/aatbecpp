#!/bin/sh

set -e

meson compile -C builddir

if ! type "gcovr" > /dev/null; then
  python3 -m pip install gcovr
fi

builddir/run_tests && :

gcovr --exclude tests --exclude extern --exclude lang.cpp --html -o builddir/coverage.html
open builddir/coverage.html