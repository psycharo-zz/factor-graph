@echo off
rem MSSDK71OPTS.BAT
rem
rem    Compile and link options used for building MEX-files
rem    using the Microsoft Windows Software Development Kit.
rem
rem    $Revision: 1.1.6.2 $  $Date: 2011/03/09 05:35:27 $
rem    Copyright 2010 The MathWorks, Inc.
rem
rem StorageVersion: 1.0
rem C++keyFileName: MSSDK71OPTS.BAT
rem C++keyName: Microsoft SDK
rem C++keyManufacturer: Microsoft
rem C++keyVersion: 7.1
rem C++keyLanguage: C++
rem
rem ********************************************************************
rem General parameters
rem ********************************************************************@echo off

rem Compile and link options used for building MEX-files using Cygwin,
rem for the 64-bit version of MATLAB.
rem
rem It makes the assumption that you installed Cygwin in C:\CYGWIN,
rem and that you installed either the "mingw64-x86_64-gcc" package.
rem
rem This file should be renamed to "mexopts.bat" and copied to:
rem C:\Documents and Settings\<Username>\Application Data\MathWorks\MATLAB\<MATLAB version>\
rem
rem This file only supports C code. It can easily be changed to compile C++
rem code by replacing "gcc" by "g++" in COMPILER and LINKER, and by adding
rem "-static-libstdc++" to LINKFLAGS.
rem You will also need the "mingw64-x86_64-gcc-g++" package.
rem
rem Initial version by Michel Juillard, revised by Sebastien Villemot.

rem Copyright (C) 2009-2010 Dynare Team
rem
rem This file is part of Dynare.
rem
rem Dynare is free software: you can redistribute it and/or modify
rem it under the terms of the GNU General Public License as published by
rem the Free Software Foundation, either version 3 of the License, or
rem (at your option) any later version.
rem
rem Dynare is distributed in the hope that it will be useful,
rem but WITHOUT ANY WARRANTY; without even the implied warranty of
rem MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
rem GNU General Public License for more details.
rem
rem You should have received a copy of the GNU General Public License
rem along with Dynare.  If not, see <http://www.gnu.org/licenses/>.

rem ********************************************************************
rem General parameters
rem ********************************************************************

set MATLAB=%MATLAB%
set PATH=%PATH%;c:\cygwin\bin
set MW_TARGET_ARCH=win64

rem ********************************************************************
rem Compiler parameters
rem ********************************************************************
set COMPILER=x86_64-w64-mingw32-gcc
set COMPFLAGS=-c -fexceptions "-I%MATLAB%\extern\include" -std=c++0x
set OPTIMFLAGS=-O3
set DEBUGFLAGS=-g -Wall
set NAME_OBJECT=-o 

rem ********************************************************************
rem Linker parameters
rem ********************************************************************
set PRELINK_CMDS1=echo EXPORTS > mex.def & echo mexFunction >> mex.def
set LIBLOC="%MATLAB%\bin\win64"
set LINKER=x86_64-w64-mingw32-gcc
set LINKFLAGS= -static-libgcc -shared mex.def -L%LIBLOC% 
set LINKFLAGSPOST= -lmex -lmx -lmwlapack -lmwblas -lstdc++
set LINKOPTIMFLAGS=-O3
set LINKDEBUGFLAGS= -g -Wall
set LINK_FILE=
set LINK_LIB=
set NAME_OUTPUT=-o "%OUTDIR%%MEX_NAME%%MEX_EXT%"
set RSP_FILE_INDICATOR=
set POSTLINK_CMDS1=del mex.def
