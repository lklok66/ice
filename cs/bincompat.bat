cd src\Ice
C:\WINDOWS\Microsoft.NET\Framework\v1.1.4322\al.exe /link:policy.2.1.icecs /out:policy.2.1.icecs.dll /keyfile:IcecsKey.snk
C:\WINDOWS\Microsoft.NET\Framework\v1.1.4322\gacutil.exe -i policy.2.1.icecs.dll
cd ..\Glacier2
C:\WINDOWS\Microsoft.NET\Framework\v1.1.4322\al.exe /link:policy.2.1.glacier2cs /out:policy.2.1.glacier2cs.dll /keyfile:Glacier2csKey.snk
C:\WINDOWS\Microsoft.NET\Framework\v1.1.4322\gacutil.exe -i policy.2.1.glacier2cs.dll
cd ..\IcePack
C:\WINDOWS\Microsoft.NET\Framework\v1.1.4322\al.exe /link:policy.2.1.icepackcs /out:policy.2.1.icepackcs.dll /keyfile:IcePackcsKey.snk
C:\WINDOWS\Microsoft.NET\Framework\v1.1.4322\gacutil.exe -i policy.2.1.icepackcs.dll
cd ..\IcePatch2
C:\WINDOWS\Microsoft.NET\Framework\v1.1.4322\al.exe /link:policy.2.1.icepatch2cs /out:policy.2.1.icepatch2cs.dll /keyfile:IcePatch2csKey.snk
C:\WINDOWS\Microsoft.NET\Framework\v1.1.4322\gacutil.exe -i policy.2.1.icepatch2cs.dll
cd ..\IceStorm
C:\WINDOWS\Microsoft.NET\Framework\v1.1.4322\al.exe /link:policy.2.1.icestormcs /out:policy.2.1.icestormcs.dll /keyfile:IceStormcsKey.snk
C:\WINDOWS\Microsoft.NET\Framework\v1.1.4322\gacutil.exe -i policy.2.1.icestormcs.dll

cd ..\..
