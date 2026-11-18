@echo off

rem --------------------------
rem %1 = base input file name
rem %2 = input file dir
rem %3 = bin output dir
rem %4 = asm output dir
rem --------------------------

fxc /nologo /Op /Tfx_2_0 /Fc"%2/%1.asm" "%2/%1.fx"
fxc /nologo /Op /Tfx_2_0 /Fo"%3/%1.fxo" "%2/%1.fx"
