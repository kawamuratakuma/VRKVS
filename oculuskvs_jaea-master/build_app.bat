@echo off
REM
REM  build Lib and App/*
REM

if "%1"=="" (
	echo "usage: build_app.bat <APP_DIR>"
	exit /b 0
)

cd Lib
nmake /F Makefile.vc.kvs clean lib
if ERRORLEVEL 1 (
	echo "ERROR when build Lib/"
	cd ..
	exit /b 1
)

cd "..\App\%1"
kvsmake -G
nmake /F Makefile.vc.kvs clean build
if ERRORLEVEL 1 (
	echo "ERROR when build App\%1"
	cd ..\..
	exit /b 1
)
cd ..\..

echo "Build finished successfully."
exit /b 0

