@echo off
REM Final clean build with tests disabled to get working interpreter
cd /d C:\s\s_language\build

echo Cleaning CMake cache...
del /f /q CMakeCache.txt CMakeUserPresets.json .cmake* >nul 2>&1

echo Reconfiguring CMake without tests...
"C:\Program Files\CMake\bin\cmake.exe" .. -G "Visual Studio 17 2022" -DBUILD_TESTS=OFF -DBUILD_EXAMPLES=OFF -DBUILD_PKG_MANAGER=OFF

if errorlevel 1 (
    echo CMake configuration failed!
    pause
    exit /b 1
)

echo.
echo Building Sad interpreter...
"C:\Program Files\CMake\bin\cmake.exe" --build . --config Debug --parallel 4 --target sad

if errorlevel 1 (
    echo Build failed!
    pause
    exit /b 1
)

echo.
echo Build successful!
if exist bin\Debug\sad.exe (
    echo.
    echo Sad interpreter created: bin\Debug\sad.exe
    dir /s bin\Debug\sad.exe
)

pause
