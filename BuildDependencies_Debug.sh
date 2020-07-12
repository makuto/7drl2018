#!/bin/sh

echo "Building Base2.0 (and by extension, SFML)..."
# TODO: Fix this foolishness
cd libs/base2.0
./BuildDependencies_Debug.sh || exit 1
jam -j4 -sDEBUG_BUILD=true && jam -j4 -sDEBUG_BUILD=true libBase20 || exit 1
cd ../../
echo "Finished building Base2.0!"
