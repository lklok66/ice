const HKEY_LOCAL_MACHINE = &H80000002

Set shell = CreateObject("WScript.Shell")
Set env = shell.Environment("Process")

x86 = True
If  StrComp(env("PROCESSOR_ARCHITECTURE"), "AMD64") = 0 OR StrComp(env("PROCESSOR_ARCHITEW6432"), "AMD64") = 0 Then
       x86 = False
End If

Set  out = WScript.StdOut

Set fso = CreateObject("Scripting.FileSystemObject")

strComputer = "."
Set registryObj = GetObject("winmgmts:{impersonationLevel=impersonate}!\\" &_
 strComputer & "\root\default:StdRegProv")
 
'
' Read Ice-3.4.2 intall dir from registry keyword
'
installKeyPath = "SOFTWARE\ZeroC\Ice 3.4.2"
installKeyName = "InstallDir"
registryObj.GetStringValue  HKEY_LOCAL_MACHINE,installKeyPath,installKeyName,installDir

'
' Check if we found Ice-3.4.2 installation directory
'
If TypeName(installDir) = "Null" Then
    WScript.StdOut.WriteLine "Ice-3.4.2 installation not detected terminating"
    Wscript.Quit
End If

'
' Creat target directories if not exists
'
If Not fso.FolderExists(installDir & "bin\sl") Then
    fso.CreateFolder(installDir & "bin\sl")
End If

'
' Install Silverlight Assemblies
'
dllFiles = Array("Glacier2", "Ice", "IceGrid", "IcePatch2", "IceStorm")
extensions = Array(".dll", ".pdb", ".xml")
For Each fileName in dllFiles
    For Each ext in extensions
        out.WriteLine "copy: bin\sl\" & fileName & ext & " to: " & installDir & "bin\sl\"
        fso.CopyFile "bin\sl\" & fileName & ext, installDir & "bin\sl\"
    Next
Next

'
' Install policy server
'
out.WriteLine "copy: bin\policyserver.exe" & " to: " & installDir & "bin\policyserver.exe"
fso.CopyFile "bin\policyserver.exe", installDir & "bin\policyserver.exe"
out.WriteLine "copy: bin\cf\policyserver.exe" & " to: " & installDir & "bin\cf\policyserver.exe"
fso.CopyFile "bin\cf\policyserver.exe", installDir & "bin\cf\policyserver.exe"

'
' Install Visual Studio Add-in
'
WScript.StdOut.WriteLine "copy: vsaddin\IceVisualStudioAddin-VS2010.dll " & " to: " & installDir & "bin\sl\" & fileName
fso.CopyFile "vsaddin\IceVisualStudioAddin-VS2010.dll", installDir & "vsaddin\IceVisualStudioAddin-VS2010.dll"

If x86 = True Then
    assembliesKeyPath = "SOFTWARE\Microsoft\Microsoft SDKs\Silverlight\v5.0\AssemblyFoldersEx\ZeroC"
Else
    assembliesKeyPath = "SOFTWARE\Wow6432Node\Microsoft\Microsoft SDKs\Silverlight\v5.0\AssemblyFoldersEx\ZeroC"
End If

assembliesKeyValue = installDir & "\bin\sl"

registryObj.CreateKey HKEY_LOCAL_MACHINE,assembliesKeyPath
registryObj.SetStringValue HKEY_LOCAL_MACHINE,assembliesKeyPath,"",assembliesKeyValue

'
' Install ICESL_LICENSE, README and RELEASE_NOTES
'
If Not fso.FolderExists(installDir & "silverlight") Then
    fso.CreateFolder(installDir & "silverlight")
End If
out.WriteLine "copy: ICE_LICENSE.txt" & " to: " & installDir & "silverlight\ICE_LICENSE.txt"
fso.CopyFile "ICE_LICENSE.txt", installDir & "silverlight\ICE_LICENSE.txt"
out.WriteLine "copy: README.txt" & " to: " & installDir & "silverlight\README.txt"
fso.CopyFile "README.txt", installDir & "silverlight\README.txt"
out.WriteLine "copy: RELEASE_NOTES.txt" & " to: " & installDir & "silverlight\RELEASE_NOTES.txt"
fso.CopyFile "RELEASE_NOTES.txt", installDir & "silverlight\RELEASE_NOTES.txt"

WScript.StdOut.WriteLine "Ice for Silverlight installation terminated ok"