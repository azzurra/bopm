#!/bin/sh
set -ex
autoreconf -i
./configure --enable-maintainer-mode
exit 0
