Const ForReading = 1, ForWriting = 2, ForAppending =8

Dim fso
Dim installDir
Dim registryFile
Dim vs2008File
Dim vs2010File
Dim icePropsFile
Dim iceVspropsFile
Dim files
Dim file
Dim fileName
Dim tmpFileName
Dim tmpFile
Dim nextLine
Dim paths

paths = Split(Session.Property("CustomActionData"), "|")

installDir = paths(0)
IceFIXPropsFile = paths(1) & "IceFIX.props"

files = Array(IceFIXPropsFile)

For Each fileName in files
    tmpFileName = fileName & ".tmp"

    Set fso = CreateObject("Scripting.FileSystemObject")
    Set file = fso.OpenTextFile(fileName, ForReading, True)
    Set tmpFile = fso.OpenTextFile(tmpFileName, ForWriting, True)

    Do Until file.AtEndOfStream
       nextLine = file.ReadLine
       tmpFile.WriteLine Replace(nextLine, "@installdir@\", installDir)
    Loop

    file.Close
    tmpFile.Close

    fso.DeleteFile fileName
    fso.MoveFile tmpFileName, fileName
Next
