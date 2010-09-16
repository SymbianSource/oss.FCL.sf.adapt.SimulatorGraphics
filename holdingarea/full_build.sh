#! /bin/bash

set -e

EGL_CONFIG=-DUSE_MINI_EGL:Bool=ON

mkdir -p x86Debug; cd x86Debug; cmake .. $EGL_CONFIG -DCMAKE_BUILD_TYPE=Debug && make; cd ..
#mkdir -p x86Release && cd x86Release && cmake .. $EGL_CONFIG -DCMAKE_BUILD_TYPE=Release && make && cd ..
# Profiling build is the same as release (optimizations, etc.), but with debug symbols
#mkdir -p x86Profile && cd x86Profile && cmake .. $EGL_CONFIG -DCMAKE_BUILD_TYPE=RelWithDebInfo && make && cd ..
