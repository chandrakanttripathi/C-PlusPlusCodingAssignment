@echo off
setlocal enabledelayedexpansion

REM -----------------------------
REM Configure your Visual Studio
REM -----------------------------
REM Update this path to match your VS installation
set VS_DEV_CMD="C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"

if not exist %VS_DEV_CMD% (
    echo ERROR: Cannot find VsDevCmd.bat at %VS_DEV_CMD%
    exit /b 1
)

echo Initializing Visual Studio build environment...
call %VS_DEV_CMD%

REM -----------------------------
REM Output folder
REM -----------------------------
if exist build (
    echo Cleaning old build folder...
    rmdir /s /q build
)

set BUILD_DIR=build
if not exist %BUILD_DIR% mkdir %BUILD_DIR%

cd %BUILD_DIR%
set BUILD_OBJ_DIR=obj
if not exist %BUILD_OBJ_DIR% mkdir %BUILD_OBJ_DIR%
cd ..
REM -----------------------------
REM Compiler & Flags
REM -----------------------------
set CL_FLAGS=/std:c++17 /EHsc /W4 /MD
set INCLUDE_DIRS=/I common
set LIBS=User32.lib Advapi32.lib

REM -----------------------------
REM Build NumberService
REM -----------------------------
echo.
echo === Building NumberService ===
cl %CL_FLAGS% %INCLUDE_DIRS% ^
    NumberService\ThreadPool.cpp ^
    NumberService\NumberStore.cpp ^
	NumberService\NamedPipeServer.cpp ^
	NumberService\ServerRequestHandler.cpp ^
    NumberService\main.cpp ^
	/Fo:build\obj\ ^
    /Fe%BUILD_DIR%\NumberService.exe ^
    /link %LIBS%

if errorlevel 1 (
    echo ERROR: Failed to build NumberService
    exit /b 1
)

REM -----------------------------
REM Build NumberCli
REM -----------------------------
echo.
echo === Building NumberCli ===
cl %CL_FLAGS% %INCLUDE_DIRS% ^
    NumberCli\NamedPipeClient.cpp ^
    NumberCli\ClientRequestHandler.cpp ^
    NumberCli\main.cpp ^
	/Fo:build\obj\ ^
    /Fe%BUILD_DIR%\NumberCli.exe ^
    /link %LIBS%

if errorlevel 1 (
    echo ERROR: Failed to build NumberCli
    exit /b 1
)

echo.
echo Build completed successfully!
echo Executables are in %BUILD_DIR%\

endlocal
pause
