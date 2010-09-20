@ECHO OFF
mkdir build_vs2005_miniegl
cd build_vs2005_miniegl
cmake -DUSE_MINI_EGL:Bool=ON %* -G "Visual Studio 8 2005" ..
cd ..
