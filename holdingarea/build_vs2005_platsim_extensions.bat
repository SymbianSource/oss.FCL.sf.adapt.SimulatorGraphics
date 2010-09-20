@ECHO OFF
mkdir build_vs2005_platsim_extensions
cd build_vs2005_platsim_extensions
cmake -DUSE_MINI_EGL:Bool=OFF -DPLATSIM_EXTENSIONS:Bool=ON %* -G "Visual Studio 8 2005" ../..
cd ..
