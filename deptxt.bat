@echo off
echo !if exist ($(O)%1)   >> %2
echo !    include $(O)%1  >> %2
echo !endif               >> %2
echodot
