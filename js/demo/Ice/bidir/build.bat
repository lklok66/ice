@echo off
REM **********************************************************************
REM
REM Copyright (c) 2003-2014 ZeroC, Inc. All rights reserved.
REM
REM This copy of Ice is licensed to you under the terms described in the
REM ICE_LICENSE file included in this distribution.
REM
REM **********************************************************************

REM .bat wrapper for buidling javascript demos. Assumes ICE_JS_HOME and
REM ICE_HOME are set.

@echo Building %~dp0

@echo "%ICE_JS_HOME%\bin\slice2js" -I"%ICE_HOME%\slice" Callback.ice
"%ICE_JS_HOME%\bin\slice2js" -I"%ICE_HOME%\slice" Callback.ice
