@echo off
echodot
if not exist (%1) exit /b
echo ### > %1
touch %1 /t000000 > NUL
