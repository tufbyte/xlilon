@echo off
rem ==========================================
rem Xlilon Build Script (Windows / MSVC)
rem ==========================================

rem 1. Create the build directory if it doesn't exist
if not exist build mkdir build

rem 2. Move into the build directory
pushd build

rem 3. Compile!
rem -----------------------------------------------------------------------
rem Compiler Flags:
rem  -nologo    : Don't print the copyright banner on startup.
rem  -Zi        : Generate debug info (.pdb) so you can debug the code.
rem  -FC        : Use full path in error messages (helpful for VS Code/Vim).
rem  -W4        : Enable Level 4 warnings (strict, catches many bugs).
rem  -std:c++17 : Use C++17 standard (needed if we use std::span later).
rem
rem Source Files:
rem  ..\src\platform\win32_runner.cpp : The OS entry point.
rem  ..\src\xlilon.cpp                : The library implementation.
rem
rem Linker (/link):
rem  user32.lib : Required for CreateWindow, PeekMessage, etc.
rem  gdi32.lib  : Required for StretchDIBits (blitting pixels).
rem -----------------------------------------------------------------------

echo [BUILD] Compiling Xlilon...

cl -nologo -Zi -FC -W4 -std:c++17 ^
   ..\src\platform\win32_runner.cpp ^
   ..\src\xlilon.cpp ^
   /Fe:xlilon.exe ^
   /link user32.lib gdi32.lib

rem 4. Exit the build directory
popd

echo [BUILD] Finished.
