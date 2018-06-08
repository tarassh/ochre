#!/usr/bin/env bash

set -ex

rm -f *.abi
rm -f *.was*
eosiocpp -o ochre.wast ochre.cpp
eosiocpp -g ochre.abi ochre.cpp

cleos set contract ochre ../ochre -p ochre