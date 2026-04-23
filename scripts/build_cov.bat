@echo off
REM ============================================================
REM سكريبت بناء sad مع coverage flags داخل بيئة MSVC Developer
REM يُستدعى من PowerShell عبر: cmd /c build_cov.bat
REM ============================================================

call "C:\Program Files\Microsoft Visual Studio\18\Professional\VC\Auxiliary\Build\vcvars64.bat" > nul 2>&1

set COV_FLAGS=/EHsc -fprofile-instr-generate -fcoverage-mapping -D_ALLOW_COMPILER_AND_STL_VERSION_MISMATCH
set LINK_FLAGS=-fprofile-instr-generate
set ROOT=C:\s_lang\s-programming-language
set BUILD_COV=%ROOT%\build_cov
set CLANGCL=C:\llvm_dev\LLVM\bin\clang-cl.exe

echo [1/2] تهيئة CMake...
cmake -S "%ROOT%" -B "%BUILD_COV%" ^
    -G "NMake Makefiles" ^
    -DCMAKE_C_COMPILER="%CLANGCL%" ^
    -DCMAKE_CXX_COMPILER="%CLANGCL%" ^
    -DCMAKE_BUILD_TYPE=Debug ^
    "-DCMAKE_CXX_FLAGS=%COV_FLAGS%" ^
    "-DCMAKE_C_FLAGS=%COV_FLAGS%" ^
    "-DCMAKE_EXE_LINKER_FLAGS=%LINK_FLAGS%" ^
    -DBUILD_TESTS=OFF ^
    -DENABLE_LLVM_BACKEND=OFF

if errorlevel 1 (
    echo [خطا] فشل CMake configure
    exit /b 1
)

echo [2/2] بناء هدف sad...
cmake --build "%BUILD_COV%" --target sad

if errorlevel 1 (
    echo [خطا] فشل البناء
    exit /b 1
)

echo [نجاح] sad.exe بُني بنجاح
exit /b 0
