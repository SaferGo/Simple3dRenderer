#!/bin/bash

if [ !-d "build" ]; then
   mkdir build
fi

cd build
cmake ..
make -j4
./../bin/Simple3DRenderer
