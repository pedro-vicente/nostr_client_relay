@echo off
perl Configure VC-WIN64A  --prefix=D:\ssl no-shared
pause
nmake
pause
nmake test 
pause
nmake install