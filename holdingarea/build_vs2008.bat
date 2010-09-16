@ECHO OFF
mkdir build_vs2008
cd build_vs2008
cmake -DUSE_MINI_EGL:Bool=OFF -DPLATSIM_EXTENSIONS:Bool=OFF %* -G "Visual Studio 9 2008" ..
cd ..
