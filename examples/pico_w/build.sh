#!/bin/bash

if [ -d "build" ]; then
  rm -rf build
fi

mkdir build
cd build

cmake ..
make -j
cd  ..

if [ -f "bluepad32_picow_example_app.uf2" ]; then
  rm bluepad32_picow_example_app.uf2
fi

if [ -f "build/bluepad32_picow_example_app.uf2" ]; then
  mv build/bluepad32_picow_example_app.uf2 .
else
  echo "Error: bluepad32_picow_example_app.uf2 was not found in the build directory, compiling probably failed ↑"
  exit 1
fi

echo "Build completed successfully. bluepad32_picow_example_app.uf2 has been updated."

