@echo off
REM **********************************************************************
REM
REM Copyright (c) 2003-2014 ZeroC, Inc. All rights reserved.
REM
REM This copy of Ice is licensed to you under the terms described in the
REM ICE_LICENSE file included in this distribution.
REM
REM **********************************************************************

REM .bat wrapper for building javascript demos. Assumes ICE_JS_HOME and
REM ICE_HOME are set.

if "%ICE_HOME%"=="" (
   echo Please set ICE_HOME to a valid Ice 3.5.1 installation
   goto :end
)

if "%ICE_JS_HOME%"=="" (
   echo Please set ICE_JS_HOME to a valid IceJS 0.1.0 installation
   goto :end
)

if NOT EXIST "%ICE_HOME%\slice\Ice\Identity.ice" (
   echo The directory specified by ICE_HOME is not a valid Ice 3.5.1 installation
   goto :end
)

if NOT EXIST "%ICE_JS_HOME%\lib\Ice.js" (
   echo The directory specified by ICE_JS_HOME is not a valid IceJS 0.1.0 installation
   goto :end
)

for /D %%G in ("*") do @(
	cd %%G
	build.bat
	cd %~dp0 )

:end
