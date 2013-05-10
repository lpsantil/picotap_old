@ECHO OFF
REM Copyright (c) 2013, Louis P. Santillan <lpsantil@gmail.com>
REM All rights reserved.
REM
REM Redistribution and use in source and binary forms, with or without
REM modification, are permitted provided that the following conditions are met:
REM
REM 1. Redistributions of source code must retain the above copyright notice, this
REM    list of conditions and the following disclaimer.
REM 2. Redistributions in binary form must reproduce the above copyright notice,
REM    this list of conditions and the following disclaimer in the documentation
REM    and/or other materials provided with the distribution.
REM
REM THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
REM ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
REM WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
REM DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
REM ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
REM (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
REM LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
REM ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
REM (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
REM SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

REM ####################################################################

set CC=wcc
set CFLAGS=-otexan-bt=dos-ms-0
set LD=wcl
set SDIR=.\
set EXAMPLES=.\Examples
set EXDIR=%EXAMPLES%\C
set IDIR=-i=.\
set LIBS=taptime.obj dospopen.c
set PROG=tap
set LIB=libtap

set SRC=%SDIR%\libtap.c

REM ####################################################################

if "%1" == "clean" goto clean
if "%1" == "superclean" goto superclean
if "%1" == "ultraclean" goto ultraclean
if "%1" == "showconfig" goto showconfig
if "%1" == "help" goto help
if "%1" == "all" goto all

goto help

:all
        %CC% %CFLAGS% taptime.c
        %CC% %CFLAGS% dospopen.c
        %CC% %CFLAGS% libtap.c
        %LD% %CFLAGS% %EXDIR%\testexit.c %LIB%.obj -fe=%EXDIR%\testexit.exe
        %LD% %CFLAGS% %EXDIR%\testfail.c %LIB%.obj -fe=%EXDIR%\testfail.exe
        %LD% %CFLAGS% %EXDIR%\testover.c %LIB%.obj -fe=%EXDIR%\testover.exe
        %LD% %CFLAGS% %EXDIR%\testund.c %LIB%.obj -fe=%EXDIR%\testund.exe
        %LD% %CFLAGS% %EXDIR%\testinp.c %LIB%.obj -fe=%EXDIR%\testinp.exe
        %LD% %CFLAGS% %EXDIR%\test.c %LIB%.obj -fe=%EXDIR%\test.exe
        %LD% %CFLAGS% %PROG%.c %LIBS% -fe=%PROG%.exe
goto end

:clean
        del %PROG%.exe *.obj *.lib
goto end

:superclean
        call mkwcc clean
        del %EXDIR%\*.exe %EXDIR%\*.obj %EXDIR%\*.lib
goto end

:ultraclean
        call mkwcc superclean
goto end

:showconfig
        echo picotap Build Config
        echo    Build Settings (Change by modifying mkwcc.bat)
        echo       CC = %CC%
        echo       CFLAGS = %CFLAGS%
        echo       LIBS = %LIBS%
        echo       LIB = %LIB%
        echo       PROG = %PROG%
goto end

:help
        echo picotap Build methods
        echo    mkwcc clean: Remove tap, libtap and picotap object files
        echo    mkwcc superclean: clean + remove examples
        echo    mkwcc ultraclean: superclean + tests
        echo
        echo Build from scratch:
        echo    mkwcc all
        echo Find out what mkwcc.bat knows about your environment:
        echo    mkwcc showconfig


:end

