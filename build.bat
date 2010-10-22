@echo off

rem DOS batch file for building host-side libraries

if '%EPOCROOT%'=='' goto PRINTUSAGE

rem Set default values
set SIMULATOR_EXTENSIONS=ON
set TOOLCHAIN_VARIANT=vs2005
set CMAKE_BUILD_TARGET=Release
set VISUAL_STUDIO_SOLUTION=OFF
set BUILD=ON
set EPOCROOTX=%EPOCROOT:\=/%

:PARSECOMMANDLINE
IF '%1'=='/h' goto PRINTUSAGE
IF '%1'=='/H' goto PRINTUSAGE
IF '%1'=='/?' goto PRINTUSAGE
IF '%1'=='/nosimext' goto DISABLE_SIMULATOR_EXTENSIONS
IF '%1'=='/NOSIMEXT' goto DISABLE_SIMULATOR_EXTENSIONS
IF '%1'=='/vs' goto SETVISUALSTUDIOVERSION
IF '%1'=='/VS' goto SETVISUALSTUDIOVERSION
IF '%1'=='/solution' goto ENABLESOLUTION
IF '%1'=='/SOLUTION' goto ENABLESOLUTION
IF '%1'=='/debug' goto ENABLEDEBUG
IF '%1'=='/DEBUG' goto ENABLEDEBUG
IF '%1'=='/nobuild' goto DISABLEBUILD
IF '%1'=='/NOBUILD' goto DISABLEBUILD

if "%VISUAL_STUDIO_SOLUTION%"=="ON" (
	set GENERATOR=Visual Studio 8 2005
	if '%TOOLCHAIN_VARIANT%'=='vs2008' set GENERATOR=Visual Studio 9 2008
) else (
	set GENERATOR=NMake Makefiles
)

rem Print out options
echo.
echo SIMULATOR_EXTENSIONS = %SIMULATOR_EXTENSIONS%
echo TOOLCHAIN_VARIANT    = %TOOLCHAIN_VARIANT%
echo GENERATOR            = %GENERATOR%
echo CMAKE_BUILD_TARGET   = %CMAKE_BUILD_TARGET%
echo BUILD                = %BUILD%
echo EPOCROOT (modified)  = %EPOCROOTX%
echo.

rem Execute
echo on
rmdir /s /q build
mkdir build
cd build
cmake -DEPOCROOT=%EPOCROOTX% -DSIMULATOR_EXTENSIONS:Bool=%SIMULATOR_EXTENSIONS% -DTOOLCHAIN_VARIANT:String=%TOOLCHAIN_VARIANT% -DCMAKE_BUILD_TYPE=%CMAKE_BUILD_TARGET% -G "%GENERATOR%" ..
@echo off
if "%BUILD%"=="ON" (
if "%GENERATOR%"=="NMake Makefiles" (
	echo on
	nmake
	@echo off
)
)
@echo off
cd ..

goto END

:PRINTUSAGE
echo Usage: build.bat [options]
echo Options:"
echo     [/H,/?]             print this message and exit
echo     [/VS VERSION]       set Visual Studio version
echo                         supported versions = 2005, 2008 (default: 2005)
echo     [/SOLUTION]         just generate VIsual Studio solution files
echo                             (default: generate NMake makefiles and build)
echo     [/DEBUG]            configure for debug build (default: release)
echo     [/NOBUILD]          call cmake but do not call nmake
echo                             note: has no effect if /SOLUTION is specified
echo.
echo Note: EPOCROOT must be defined to be the directory containing the epoc32 tree.
goto END

:DISABLE_SIMULATOR_EXTENSIONS
set SIMULATOR_EXTENSIONS=OFF
shift
goto PARSECOMMANDLINE

:SETVISUALSTUDIOVERSION
set TOOLCHAIN_VARIANT=vs%2
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

:DISABLEBUILD
set BUILD=OFF
shift
goto PARSECOMMANDLINE

:END
