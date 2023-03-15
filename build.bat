@echo off

md build
cd build

cmake .. -G "MinGW Makefiles"
mingw32-make

pause