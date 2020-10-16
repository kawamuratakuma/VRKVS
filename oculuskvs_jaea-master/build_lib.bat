@echo off
REM =========================================
REM  build scrit for Lib/
REM =========================================

if EXIST CMakeLibs_GL.conf (GOTO build_imgui)

REM CMakeLibs_GL.conf is not found!!
echo CMakeLibs_GL.conf���Ȃ����߁ALib���r���h�ł��܂���B
exit 1

:build_imgui
cd Lib\imgui
echo Lib/imgui���r���h���܂��B
echo "  - (1/2) cmake"
cmake -G "NMake Makefiles"
if ERRORLEVEL 1 goto err_build
echo "  - (2/2) nmake"
nmake clean
nmake
if ERRORLEVEL 1 goto err_build

cd ..
echo "Lib���r���h���܂��B"
echo "  - (1/2) kvsmake"
kvsmake -G
if not EXIST Makefile.vc.kvs goto err_build
echo "  - (2/2) nmake"
nmake /F Makefile.vc.kvs clean
nmake /F Makefile.vc.kvs lib
if ERRORLEVEL 1 goto err_build

echo "Lib�̃r���h�ɐ������܂����B"
cd ..
exit /b 0

:err_build
echo "���s���܂����B"
exit /b 1

