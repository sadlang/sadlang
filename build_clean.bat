@echo off
cd /d C:\s\s_language
REM Clean old build
if exist build (
    echo Cleaning old build directory...
    rmdir /s /q build
)

REM Create fresh build directory
echo Creating fresh build directory...
mkdir build
cd /d C:\s\s_language\build

REM Configure CMake
echo Configuring CMake...
"C:\Program Files\CMake\bin\cmake.exe" .. -G "Visual Studio 17 2022"

REM Build
echo Building project...
"C:\Program Files\CMake\bin\cmake.exe" --build . --config Debug

echo Build complete!
pause
