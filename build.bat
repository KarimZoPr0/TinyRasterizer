@echo off
setlocal enabledelayedexpansion
cd /D "%~dp0"

:: --- Build Configuration ----------------------------------------------------
set SDL2_DIR=%~dp0code\external\SDL2-x64
set BUILD_DIR=%~dp0build

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
set TEMP_DLL_NAME=libgame_dll.dll

if "%msvc%"=="1" (
    set CC=cl
    set CFLAGS=/I"%SDL2_DIR%\include" /I"code/external" /std:c17 /EHsc
    if "%debug%"=="1" (
        set CFLAGS=!CFLAGS! /Zi /DDEBUG
        set LDFLAGS=!LDFLAGS! /DEBUG
    )
    if "%release%"=="1" (
        set CFLAGS=!CFLAGS! /O2 /DNDEBUG
    )
    set OUT_FLAG=/Fe
) else if "%clang%"=="1" (
    set CC=clang
    set CFLAGS=-I"%SDL2_DIR%\include" -I"code/external" -std=c17
    if "%debug%"=="1" (
        set CFLAGS=!CFLAGS! -g -DDEBUG
    )
    if "%release%"=="1" (
        set CFLAGS=!CFLAGS! -O2 -DNDEBUG
    )
    set OUT_FLAG=-o
)

echo Building game DLL...
%CC% %CFLAGS% /LD code/game/game.c %OUT_FLAG%"%BUILD_DIR%\libgame_new.dll" %LDFLAGS% %DLL_LINK_FLAGS% >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo Failed to build game DLL. Check logs for details.
    ctime -end %TIMINGS_FILE% %ERRORLEVEL%
    exit /b 1
)

:: Safely rename the new DLL over the old one if it's not locked
move /Y "%BUILD_DIR%\libgame_new.dll" "%BUILD_DIR%\libgame.dll" >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo DLL is currently in use; will attempt replacement on next build.
)

:: Copy the DLL for hot reloading
copy "%BUILD_DIR%\libgame.dll" "%BUILD_DIR%\%TEMP_DLL_NAME%" >nul

:: Build the main application
echo Building main application...
%CC% %CFLAGS% code/main.c %OUT_FLAG%"%BUILD_DIR%\TinyRasterizer.exe" %LDFLAGS% >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo Failed to build main application.
    ctime -end %TIMINGS_FILE% %ERRORLEVEL%
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
