@echo off
REM **********************************************************************
REM
REM Copyright (c) 2003-2014 ZeroC, Inc. All rights reserved.
REM
REM This copy of Ice is licensed to you under the terms described in the
REM ICE_LICENSE file included in this distribution.
REM
REM **********************************************************************

REM .bat wrapper for buidling javascript demos. Assumes ICEJS_HOME is set.

@echo Building %~dp0

@echo "%ICEJS_HOME%\bin\slice2js" -I"%ICEJS_HOME%\slice" Chat.ice
"%ICEJS_HOME%\bin\slice2js" -I"%ICEJS_HOME%\slice" Chat.ice
@echo "%ICEJS_HOME%\bin\slice2js" -I"%ICEJS_HOME%\slice" -I. ChatSession.ice
"%ICEJS_HOME%\bin\slice2js" -I"%ICEJS_HOME%\slice" -I. ChatSession.ice
