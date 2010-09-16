@ECHO OFF
mkdir build_vs2008_miniegl
cd build_vs2008_miniegl
cmake -DUSE_MINI_EGL:Bool=ON %* -G "Visual Studio 9 2008" ..
cd ..
