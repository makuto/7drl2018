#!/bin/sh

echo Building Base2.0...
# TODO: Fix this foolishness
cd libs/base2.0
./BuildDependencies_Release.sh || exit 1
jam -j4 && jam -j4 libBase20 || exit 1
cd ../../
echo Finished building Base2.0!
