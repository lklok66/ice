REM @ECHO OFF
REM This script must be called with the following arguments:
REM
REM %1 = %VS71COMNTOOLS% or %VS80COMNTOOLS%
REM %2 = <policy file>
REM %3 = $(SolutionDir)
REM
call %1\vsvars32.bat
al.exe /link:"%2" /out:"%2.dll" /keyfile:"%3\config\IcecsKey.snk"
move "%2.dll" "%3\bin"
copy "%2" "%3\bin"
