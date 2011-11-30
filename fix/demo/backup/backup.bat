@echo off
rem ********************************************************************
rem
rem Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
rem
rem This copy of Ice FIX is licensed to you under the terms described in the
rem ICE_FIX_LICENSE file included in this distribution.
rem
rem ********************************************************************

if "%1"=="full" goto fullBackup
if "%1"=="incremental" goto incrementalBackup
echo Usage: %0 {full^|incremental}
exit /b 1

:fullBackup
echo Performing full backup
echo ====== Checkpoint ======
db_checkpoint -1 -h db-tp1

if exist hotbackup echo ====== Preserve (move) previous hotbackup directory ===== && (if exist hotbackup.1 rmdir /s /q hotbackup.1) && move hotbackup hotbackup.1

echo ====== Retrieve list of old logs ======
db_archive -h db-tp1 > oldlogs.txt

echo ====== Run db_hotbackup ======
db_hotbackup -b hotbackup -v -D -h db-tp1
if errorlevel 1 exit /b %errorlevel%

echo ===== Remove old logs =====
for /f %%i in (oldlogs.txt) do del /q db-tp1\logs\%%i && echo db-tp1\logs\%%i deleted
del /q oldLogs.txt
exit /b 0

:incrementalBackup
echo Performing incremental backup
if exist hotbackup echo ====== Preserve (copy) previous hotbackup directory ===== && (if exist hotbackup.1 rmdir /s /q hotbackup.1) && xcopy /s /q hotbackup hotbackup.1\
echo ====== Run db_hotbackup -c -u (log archival) ======
db_hotbackup -c -u -b hotbackup -v -D -h db-tp1
if errorlevel 1 exit /b %errorlevel%
exit /b 0

