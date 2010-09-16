
PlatSim rendering libraries

Requirements:
cmake 2.8 (http://www.cmake.org/cmake/resources/software.html)
Visual Studio 2005 or 2008 (tested with VS2008 Express)
llvm 2.7 (http://www.llvm.org/)


STRUCTURE:
  cmake:
        Holds cmake scripts for LLVM configuration

  llvm: 
        LLVM binaries (contains only binaries needed by libOpenVG).
        * if you want to use some other build/version of LLVM
          please modify CMakeLists.txt files accordingly.
  
  inc:  
        Common include files. EGL-VG interface, EGL-GLES interface,
        Khronos API includes etc.

  libEGL:
        EGL source code.

  libGLESv1:
        GLES wrapper source code.

  libGLESv2:
        GLES2 wrapper source code.   

  serialization: 
        PlatSim serialization source code.
 
  vg:
        OpenVG source code.

  bin:
        Built binaries will be placed here.

  lib:
        Include libraries will be placed here.
  
BUILDING:

 Windows:  
  - LLVM 2.7 is required by the OpenVG implementation. Use Visual Studio to
    compile the runtime library, and download binaries for the llvm-gcc 4.2.
    The libraries should be installed under [root]/llvm/llvm-2.7 (because this 
    is assumed by the libOpenVG build process), and gcc should be extracted 
    into [root]/llvm/llvm-gcc4.2-2.7-x86-mingw32 (default name of the 
    extracted .tar.bz2).

  - Run .bat file from top level directory in visual studio command prompt
     * build_vs[2005|2008].bat:
       - will create Visual Studio [2005|2008] solution with vanilla EGL
       - current serialization code won't build with this configuration

     * build_vs[2005|2008]_miniegl.bat:
       - will create Visual Studio [2005|2008] solution with MiniEGL
         from reference OpenVG
       - current serialization code won't build with this configuration

     * build_vs[2005|2008]_platsim_extensions.bat:
       - will create Visual Studio [2005|2008] solution with PlatSim
         specific extensions enabled in EGL library.
       - can be used with current serialization code.

  - Each of the .bat files will create own building directory. You will find
    Visual studio .sln file in that directory.
  
 Linux:
  \note Debugging on Linux requires gdb CVS 7.2 or newer.
  TODO
