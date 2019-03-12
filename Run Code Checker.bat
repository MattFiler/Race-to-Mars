@echo ///////////// EXECUTING CODE CHECK! /////////////
@echo To run this tool you must have CPPCheck installed.
@echo --------------------------------------------------
"F:\Portable Apps\Cppcheck\cppcheck.exe" --quiet --enable=all --error-exitcode=1 --force --inline-suppr --suppressions-list=.cppcheck --std=c++14 --language=c++ -I src main.cpp src/*
pause