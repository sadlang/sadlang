@echo off
REM Kill any processes that might be locking the build
taskkill /F /IM cmake.exe >nul 2>&1
taskkill /F /IM msbuild.exe >nul 2>&1

timeout /t 2

REM Clean the build directory
echo Cleaning build directory...
cd /d C:\s\s_language
for /d %%x in (build*) do (
    echo Removing %%x...
    rmdir /s /q %%x 2>nul
)

REM Create fresh build directory
echo Creating new build...
mkdir build
cd build

REM Configure
echo Configuring CMake...
"C:\Program Files\CMake\bin\cmake.exe" .. -G "Visual Studio 17 2022" -DBUILD_TESTS=ON

if errorlevel 1 (
    echo CMake configuration failed!
    pause
    exit /b 1
)

REM Build
echo Building...
"C:\Program Files\CMake\bin\cmake.exe" --build . --config Debug --parallel 4

if errorlevel 1 (
    echo Build failed!
    pause
    exit /b 1
)

echo.
echo Build successful!
cd bin\Debug
dir *.exe
pause
