@ECHO OFF
mkdir build_vs2005
cd build_vs2005
cmake -DUSE_MINI_EGL:Bool=OFF -DPLATSIM_EXTENSIONS:Bool=OFF %* -G "Visual Studio 8 2005" ..
cd ..

