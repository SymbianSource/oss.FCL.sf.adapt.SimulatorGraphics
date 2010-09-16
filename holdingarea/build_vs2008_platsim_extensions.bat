@ECHO OFF
mkdir build_vs2008_platsim_extensions
cd build_vs2008_platsim_extensions
cmake -DUSE_MINI_EGL:Bool=OFF -DPLATSIM_EXTENSIONS:Bool=ON %* -G "Visual Studio 9 2008" ../..
cd ..

