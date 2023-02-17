#!/bin/bash

mkdir build
cd build

cmake3 ../
make -j2

cd ../

cp -v data.txt build/src/client/
cp -v data.txt build/src/server/

cp -v configs/client_config.conf build/src/client/
cp -v configs/server_config.conf build/src/server/

echo ""

echo "================START======================="
echo "./server -c server_config.conf"
echo "./client -c client_config.conf"
echo "============================================"
