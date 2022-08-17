#! /bin/sh

set -e

meson compile -C builddir
echo "----------------------------------------"
builddir/lang "$*"