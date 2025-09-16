@echo off
setlocal enabledelayedexpansion
cd /D "%~dp0"

:: --- Build Configuration ----------------------------------------------------
set SDL2_DIR=%~dp0code\external\SDL2-x64
set BUILD_DIR=%~dp0build

set TIMINGS_FILE=%BASE_DIR%timings.ctm

rem Path to ctime.exe (assumed to be in the base directory)
set CTIME=%BASE_DIR%ctime.exe

:: Ensure the build directory exists
if not exist "%BUILD_DIR%" (
    mkdir "%BUILD_DIR%"
)

:: --- Argument Parsing -------------------------------------------------------
for %%a in (%*) do set "%%a=1"
if not "%msvc%"=="1" if not "%clang%"=="1" set msvc=1
if not "%release%"=="1" set debug=1
if "%debug%"=="1"   set release=0 && echo [Debug mode enabled]
if "%release%"=="1" set debug=0 && echo [Release mode enabled]
if "%msvc%"=="1"    set clang=0 && echo [Using MSVC compiler]
if "%clang%"=="1"   set msvc=0 && echo [Using Clang compiler]

:: --- Compiler/Linker Settings -----------------------------------------------
set CC=
set CFLAGS=
set LDFLAGS=/link /LIBPATH:"%SDL2_DIR%\lib" SDL2.lib SDL2main.lib
set DLL_LINK_FLAGS=/DLL /EXPORT:game_update_and_render
set OUT_FLAG=

if "%msvc%"=="1" (
    set CC=cl
    set CFLAGS=/I"%SDL2_DIR%\include" /I"code/external" /std:c11 /EHsc
    if "%debug%"=="1" (
        set CFLAGS=!CFLAGS! /Od /Zi /DDEBUG
    )
    if "%release%"=="1" (
        set CFLAGS=!CFLAGS! /O2 /DNDEBUG
    )
    set OUT_FLAG=/Fe
) else if "%clang%"=="1" (
    set CC=clang
    set CFLAGS=-I"%SDL2_DIR%\include" -I"code/external" -std=c11
    if "%debug%"=="1" (
        set CFLAGS=!CFLAGS! -O0 -g -DDEBUG
    )
    if "%release%"=="1" (
        set CFLAGS=!CFLAGS! -O2 -DNDEBUG
    )
    set OUT_FLAG=-o
)

rem ===============================
rem Begin Timing with ctime
rem ===============================
echo Starting timing...
call "%CTIME%" -begin "%TIMINGS_FILE%"
if errorlevel 1 (
    echo ERROR: Failed to start timing with ctime.
    exit /b 1
)


echo Building game DLL...
%CC% %CFLAGS% /LD code/game/game.c %OUT_FLAG%"%BUILD_DIR%\libgame_new.dll" %LDFLAGS% %DLL_LINK_FLAGS%
if %ERRORLEVEL% neq 0 (
    echo Failed to build game DLL.
    exit /b 1
)

rem ===============================
rem End Timing with ctime
rem ===============================
echo Ending timing...
call "%CTIME%" -end "%TIMINGS_FILE%" %BUILD_ERROR%
if errorlevel 1 (
    echo ERROR: Failed to end timing with ctime.
    exit /b 1
)

:: Safely rename the new DLL over the old one if it's not locked
move /Y "%BUILD_DIR%\libgame_new.dll" "%BUILD_DIR%\libgame.dll"

echo Building main application...
%CC% %CFLAGS% code/main.c %OUT_FLAG%"%BUILD_DIR%\TinyRasterizer.exe" %LDFLAGS%
if %ERRORLEVEL% neq 0 (
    echo Failed to build main application.
    exit /b 1
)

:: --- Copy SDL Runtime -------------------------------------------------------
copy "%SDL2_DIR%\bin\SDL2.dll" "%BUILD_DIR%\" >nul

:: --- Done -------------------------------------------------------------------
echo Build successful!

:: Run the application if "run" argument is passed
if "%run%"=="1" (
    echo Running the application...
    pushd "%BUILD_DIR%"
    TinyRasterizer.exe
    popd
)

endlocal