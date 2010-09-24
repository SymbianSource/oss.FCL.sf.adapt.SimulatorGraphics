@echo off

rem DOS batch file for building host-side libraries

rem Set default values
set USE_MINI_EGL=OFF
set PLATSIM_EXTENSIONS=ON
set VISUAL_STUDIO_VERSION=2005
set CMAKE_BUILD_TARGET=Release
set VISUAL_STUDIO_SOLUTION=OFF

:PARSECOMMANDLINE
IF '%1'=='/h' goto PRINTUSAGE
IF '%1'=='/H' goto PRINTUSAGE
IF '%1'=='/?' goto PRINTUSAGE
IF '%1'=='/miniegl' goto ENABLEMINIEGL
IF '%1'=='/MINIEGL' goto ENABLEMINIEGL
IF '%1'=='/noplatsim' goto DISABLEPLATSIM
IF '%1'=='/NOPLATSIM' goto DISABLEPLATSIM
IF '%1'=='/vs' goto SETVISUALSTUDIOVERSION
IF '%1'=='/VS' goto SETVISUALSTUDIOVERSION
IF '%1'=='/solution' goto ENABLESOLUTION
IF '%1'=='/SOLUTION' goto ENABLESOLUTION
IF '%1'=='/debug' goto ENABLEDEBUG
IF '%1'=='/DEBUG' goto ENABLEDEBUG

if "%VISUAL_STUDIO_SOLUTION%"=="ON" (
	set GENERATOR=Visual Studio 8 2005
	if '%VISUAL_STUDIO_VERSION%'=='2008' set GENERATOR=Visual Studio 9 2008
) else (
	set GENERATOR=NMake Makefiles
)

rem Print out options
echo.
echo USE_MINI_EGL          = %USE_MINI_EGL%
echo PLATSIM_EXTENSIONS    = %PLATSIM_EXTENSIONS%
echo VISUAL_STUDIO_VERSION = %VISUAL_STUDIO_VERSION%
echo GENERATOR             = %GENERATOR%
echo CMAKE_BUILD_TARGET    = %CMAKE_BUILD_TARGET%
echo.

rem Execute
echo on
rmdir /s /q build
mkdir build
cd build
cmake -DUSE_MINI_EGL:Bool=%USE_MINI_EGL% -DPLATSIM_EXTENSIONS:Bool=%PLATSIM_EXTENSIONS% -DVISUAL_STUDIO_VERSION:String=%VISUAL_STUDIO_VERSION% -DCMAKE_BUILD_TYPE=%CMAKE_BUILD_TARGET% -G "%GENERATOR%" ../..
@echo off
if "%GENERATOR%"=="NMake Makefiles" (
	echo on
	nmake
	@echo off
)
@echo off
cd ..

goto END

:PRINTUSAGE
echo Usage: build.bat [options]
echo Options:"
echo     [/H,/?]             print this message and exit
echo     [/MINIEGL]          build with miniEGL (default: disabled)
echo     [/VS VERSION]       set Visual Studio version
echo                         supported versions = 2005, 2008 (default: 2005)
echo     [/SOLUTION]         just generate VIsual Studio solution files
echo                             (default: generate NMake makefiles and build)
echo     [/DEBUG]            configure for debug build (default: release)
goto END

:ENABLEMINIEGL
set USE_MINI_EGL=ON
shift
goto PARSECOMMANDLINE

:DISABLEPLATSIM
set PLATSIM_EXTENSIONS=OFF
shift
goto PARSECOMMANDLINE

:SETVISUALSTUDIOVERSION
set VISUAL_STUDIO_VERSION=%2
shift
shift
goto PARSECOMMANDLINE

:ENABLESOLUTION
set VISUAL_STUDIO_SOLUTION=ON
shift
goto PARSECOMMANDLINE

:ENABLEDEBUG
set CMAKE_BUILD_TARGET=Debug
shift
goto PARSECOMMANDLINE

:END


