#!/usr/bin/env python
# **********************************************************************
#
# Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

import os, sys, fnmatch, re, getopt, atexit

sys.path.append(os.path.join(os.path.dirname(__file__), "..", "lib"))
from DistUtils import *
import FixUtil

#
# Files that will be excluded from all source distributions.
#
excludeFiles = [ \
    "expect.py", 
    "allDemos.py",
    "/certs/cakey.pem",
    "/distribution",
    "/protobuf",
    "/cs",
    "/eclipse ",
    "/php",
    "/vsaddin",
    "/demoscript",
    "/exclude.py",
    "/protobuf",
    "/diff",
    "/java",
    "/py",
    "/man",
    "/rb",
    "/distribution",
    "/perf",
    "/slice/Freeze",
    "/slice/Glacier2",
    "/slice/IceBox",
    "/slice/IceGrid",
    "/slice/IcePatch2",
    "/slice/IceStorm",
    "/vb",
    "/CHANGES",
    "/RELEASE_NOTES",
    "/allTests.py",
    "/allDemos.py",
    "/Makefile",
    "/Makefile.mak",
    "/README",
    "/cpp/allTests.py",
    "/cpp/README",
    "/cpp/CHANGES",
    "/cpp/Makefile",
    "/cpp/Makefile.mak",
    "/cpp/man",
    "/cpp/src/Makefile",
    "/cpp/src/Makefile.mak",
    "/cpp/src/slice2cpp",
    "/cpp/src/Freeze",
    "/cpp/src/FreezeScript",
    "/cpp/src/Slice",
    "/cpp/src/IceXML",
    "/cpp/src/IceDB",
    "/cpp/src/IcePatch2",
    "/cpp/src/IcePatch2Lib",
    "/cpp/src/IceStorm",
    "/cpp/src/IceStormLib",
    "/cpp/src/IceGrid",
    "/cpp/src/IceGridLib",
    "/cpp/src/IceBox",
    "/cpp/src/Glacier2",
    "/cpp/src/Glacier2Lib",
    "/cpp/include/Makefile",
    "/cpp/include/Makefile.mak",
    "/cpp/include/Freeze",
    "/cpp/include/Slice",
    "/cpp/include/IceXML",
    "/cpp/include/IcePatch2",
    "/cpp/include/IceStorm",
    "/cpp/include/IceGrid",
    "/cpp/include/IceBox",
    "/cpp/include/Glacier2",
    "/cpp/test/Makefile",
    "/cpp/test/Makefile.mak",
    "/cpp/test/Freeze",
    "/cpp/test/WinRT",
    "/cpp/test/FreezeScript",
    "/cpp/test/IceBox",
    "/cpp/test/IceStorm",
    "/cpp/test/Glacier2",
    "/cpp/test/IceGrid",
    "/cpp/test/IceUtil",
    "/cpp/test/Slice",
    "/cpp/demo/Makefile",
    "/cpp/demo/Makefile.mak",
    "/cpp/demo/Freeze",
    "/cpp/demo/WinRT",
    "/cpp/demo/FreezeScript",
    "/cpp/demo/IcePatch2",
    "/cpp/demo/Database",
    "/cpp/demo/book",
    "/cpp/demo/Ice/Makefile",
    "/cpp/demo/Ice/minimal",
    "/cpp/demo/Ice/converter",
    "/cpp/demo/Ice/plugin",
    "/cpp/demo/Ice/README",
    "/cpp/demo/Ice/hello",
    "/cpp/demo/Ice/multicast",
    "/cpp/demo/Ice/properties",
    "/cpp/demo/Ice/async",
    "/cpp/demo/Ice/interleaved",
    "/cpp/demo/Ice/nested",
    "/cpp/demo/Ice/session",
    "/cpp/demo/Ice/bidir",
    "/cpp/demo/Ice/invoke",
    "/cpp/demo/Ice/nrvo",
    "/cpp/demo/Ice/throughput",
    "/cpp/demo/Ice/callback",
    "/cpp/demo/Ice/latency",
    "/cpp/demo/Ice/optional",
    "/cpp/demo/Ice/value",
    "/cpp/demo/IceBox",
    "/cpp/demo/IceStorm",
    "/cpp/demo/Glacier2",
    "/cpp/demo/IceGrid",
    "/cpp/demo/IceUtil",
]

#
# Windows files that will be excluded from Unix source distributions.
# 
excludeWindowsFiles = [ \
    "/vsaddin/",
    "*.rc",
    "*.sln",
    "*.csproj",
    "*.vbproj",
    "*.vcproj",
    "*.vcxproj",
    "*.vcxproj.filters",
    "Make*mak*",
    "Make.rules.msvc",
    ".depend.mak",
    "*.exe.config",
    "MSG00001.bin",
    "INSTALL.WINDOWS.icews",
    "/cpp/bin",
    "/cpp/test/WinRT",
    "/cpp/demo/**/generated",
    "/cpp/demo/**/MFC",
    "/**/winrt",
    "/cs/**/compact",
    "/cs/**/cf",
    "/cs/**/sl",
]

#
# Unix files that will be excluded from Windows source distributions.
# 
# Don't remove C++ source Makefile from the Windows distribution since
# the mingw build requires it.
#
excludeUnixFiles = [ \
    "INSTALL.LINUX.icews",
    "INSTALL.OSX.icews",
    "Makefile",
    "Makefile.icews",
    "*.sln",
    "/cpp/config/Make.rules",
    "/cpp/config/Make.rules.Linux",
    "/cpp/config/Make.rules.AIX",
    "/cpp/config/Make.rules.MINGW",
    "/cpp/config/Make.rules.Darwin",
    "/cpp/config/Make.rules.OSF1",
    "/cpp/config/Make.rules.FreeBSD",
    "/cpp/config/Make.rules.SunOS",
    "/cpp/config/Make.rules.HP-UX",
    "/cpp/config/Make.rules.icews",
    "/cpp/demo/Ice/MFC",
    "/cpp/demo/Ice/winrt",
    ".depend"
]

#
# Files from the top-level, cpp, java and cs config directories to include in the demo 
# source distribution config directory.
#
demoConfigFiles = [ \
    "Make.*", \
    "common.xml", \
]

#
# Files from the top-level certs directory to include in the demo distribution certs
# directory.
#
demoCertsFiles = [ \
    "*.jks", \
    "*.pem", \
    "*.pfx", \
    "makewinrtcerts.py", \
    "winrt", \
]

#
# Program usage.
#
def usage():
    print "Usage: " + sys.argv[0] + " [options] [tag]"
    print
    print "Options:"
    print "-h      Show this message."
    print "-v      Be verbose."
    print "-c DIR  Compare distribution to the one from DIR and"
    print "        save the result in the README file"

#
# Check arguments
#
try:
    opts, args = getopt.getopt(sys.argv[1:], "hvt:c:")
except getopt.GetoptError:
    print sys.argv[0] + ": unknown option"
    print
    usage()
    sys.exit(1)

tag = "HEAD"
if len(args) > 1:
    usage()
    sys.exit(1)
elif len(args) == 1:
    tag = args[0]

checkGitVersion() # Ensure we're using the right git version

verbose = 0
compareToDir = None
for (o, a) in opts:
    if o == "-h":
        usage()
        sys.exit(0)
    elif o == "-v":
        verbose = 1
    elif o == "-c":
        compareToDir = a

cwd = os.getcwd()
if compareToDir and not os.path.isabs(compareToDir):
    compareToDir = os.path.join(cwd, compareToDir)

gitRepoDir = os.path.join(os.getcwd(), os.path.dirname(__file__), "..", "..")

# Restore git attributes and core.autocrlf on exit.
def restore():
    os.chdir(gitRepoDir)
    os.system("git config --unset core.autocrlf")
    os.system("git checkout .gitattributes")

atexit.register(restore)

os.chdir(gitRepoDir)

#
# Get Ice versions.
#
config = open(os.path.join("config", "Make.common.rules.icews"), "r")
version = re.search("ICEWS_VERSION\s*=\s*([0-9\.b]*)", config.read()).group(1)
mmversion = re.search("([0-9]+\.[0-9b]+)[\.0-9]*", version).group(1)
libversion = mmversion.replace('.', '')

versions = (version, mmversion, libversion)
config.close()

#
# Remove any existing "dist-" directory and create a new one
# and sub-directories for the each source distribution.
#
distDir = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", "dist-" + tag.replace('/', '-')))
if os.path.exists(distDir):
    remove(distDir)
os.mkdir(distDir)

print "Creating " + version + " source distributions in " + distDir

demoDir = os.path.join(distDir, "IceWS-" + version + "-demos")
winDemoDir = os.path.join(distDir, "demos")
srcDir = os.path.join(distDir, "IceWS-" + version)
winSrcDir = os.path.join(distDir, "IceWS")
distFilesDir = os.path.join(distDir, "distfiles-" + version)
winDistFilesDir = os.path.join(distDir, "distfiles")
os.mkdir(demoDir)
os.mkdir(winDemoDir)

def createDistfilesDist(platform, whichDestDir):
    print "Creating " + platform + " distfiles git archive using " + tag + "...",
    sys.stdout.flush()
    os.mkdir(whichDestDir)
    os.system("git archive --worktree-attributes " + tag + ":distribution/ | ( cd " + whichDestDir + " && tar xfm - )")
    print "ok"

    os.chdir(whichDestDir)

    print "Walking through distribution to fix permissions, versions, etc...",
    sys.stdout.flush()

    fixVersion(os.path.join("bin", "makebindist.py"), *versions)

    #
    # Fix OS X installer files.
    #
    for root, dirnames, filenames in os.walk('src/mac/Ice'):
        for f in filenames:
            if fnmatch.fnmatch(f, "*.txt") or fnmatch.fnmatch(f, "*.xml") or fnmatch.fnmatch(f, "*.sh"):
                filepath = os.path.join(root, f)
                fixVersion(filepath, *versions)

    for root, dirnames, filenames in os.walk('.'):
        for f in filenames:
            filepath = os.path.join(root, f)
            # Fix version of README files
            if fnmatch.fnmatch(f, "README*"):
                fixVersion(filepath, *versions)
            fixFilePermission(filepath, verbose)
            
        for d in dirnames:
            os.chmod(os.path.join(root, d), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) # rwxr-xr-x

    print "ok"

def createSourceDist(platform, destDir):
    if platform == "UNIX":
        prefix = "IceWS-" + version
    else:
        prefix = "IceWS"

    print "Creating " + platform + " git archive using " + tag + "...",
    sys.stdout.flush()
    os.system("git archive --worktree-attributes --prefix=" + prefix + "/ " + tag + 
              " | ( cd " + destDir + " && tar xfm - )")
    print "ok"

    print "Walking through distribution to fix permissions, versions, etc...",
    sys.stdout.flush()
    
    current = os.getcwd()
    os.chdir(os.path.join(destDir, prefix))

    #fixVersion("RELEASE_NOTES", *versions)

    if os.path.exists(os.path.join("cpp", "config", "Make.rules")):
        fixMakeRules(os.path.join("cpp", "config", "Make.rules"))

    for root, dirnames, filenames in os.walk('.'):
        for f in filenames:
            filepath = os.path.join(root, f) 
            if fnmatch.fnmatch(f, "README*") or fnmatch.fnmatch(f, "INSTALL*") or fnmatch.fnmatch(f, "*.Addin"):
                fixVersion(filepath, *versions)
            if fnmatch.fnmatch(f, "*.y") or fnmatch.fnmatch(f, "*.l"):
                fixMakefileForFile(filepath)
            if fnmatch.fnmatch(f, "*.icews"):
                os.rename(filepath, os.path.join(root, f.replace(".icews", "")))
                filepath = os.path.join(root, f.replace(".icews", ""))
            if f == "Grammar.cpp":
                checkBisonVersion(filepath)
            if f == "Scanner.cpp":
                checkFlexVersion(filepath)
                
            if platform == "Windows":
                for name in ["README", "CHANGES", "LICENSE", "ICE_LICENSE", "RELEASE_NOTES"]:
                    if fnmatch.fnmatch(f, name) and not fnmatch.fnmatch(f, name + ".txt"):
                        os.rename(filepath, filepath + ".txt")
                        filepath = filepath + ".txt"
                        f = f + ".txt"

            fixFilePermission(filepath, verbose)

        for d in dirnames:
            os.chmod(os.path.join(root, d), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) # rwxr-xr-x

    if os.path.exists(os.path.join("cpp", "Makefile")):
        substitute(os.path.join("cpp", "Makefile"), [(re.compile(regexpEscape("Makefile.icews")), "Makefile")])
    if os.path.exists(os.path.join("cpp", "Makefile.mak")):
        substitute(os.path.join("cpp", "Makefile.mak"), [(re.compile(regexpEscape("Makefile.mak.icews")), 
                                                          "Makefile.mak")])

    os.chdir(current)
    print "ok"

def checkBisonVersion(filename):
    f = open(filename, "r")
    for line in f.readlines():
	if re.search("#define YYBISON_VERSION", line) and not re.search("2.4.1", line):
            print "Bison version mistmatch in `" + filename + "'"
            print "required Bison 2.4.1"
            print "Found " + line
            sys.exit(1)

def checkFlexVersion(filename):
    f = open(filename, "r")
    for line in f.readlines():
	if re.search("#define YY_FLEX_MAJOR_VERSION", line) and not re.search("2", line):
            print "Flex version mistmatch in `" + filename + "'"
            print "required Flex 2.5"
            print "Found " + line
            sys.exit(1)

        if re.search("#define YY_FLEX_MINOR_VERSION", line) and not re.search("5", line):
            print "Flex version mistmatch in `" + filename + "'"
            print "required Flex 2.5"
            print "Found " + line
            sys.exit(1)

def fixGitAttributes(checkout, autocrlf, excludes):
    os.chdir(gitRepoDir)
    if checkout:
        os.system("git checkout .gitattributes")
    if autocrlf:
        os.system("git config core.autocrlf true")
    else:
        os.system("git config --unset core.autocrlf")

    file = ".gitattributes"

    origfile = file + ".orig"
    os.rename(file, origfile)

    oldFile = open(origfile, "r")
    newFile = open(file, "w")
    origLines = oldFile.readlines()

    doComment = 0
    doCheck = 0
    newLines = []
    for x in origLines:
        #
        # If the rule contains the target string, then
        # comment out this rule.
        #
        if autocrlf and x.find("text=auto") != -1:
            x = "#" + x
        newLines.append(x)

    if len(excludes) > 0:
        newLines.append("""
# THE FOLLOWING LINES WERE ADDED BY makedist.py
# DO NOT COMMIT
""")

        for e in excludes:
            newLines.append(e + " export-ignore\n")

    newFile.writelines(newLines)
    newFile.close()
    oldFile.close()
    os.remove(origfile)

###### UNIX distfiles 
excludeForDistFiles = [ "fixCopyright.py", "fixVersion.py", "makedist.py" ]
fixGitAttributes(True, False, excludeForDistFiles)
createDistfilesDist("UNIX", distFilesDir)

###### Windows distfiles 
fixGitAttributes(False, True, []) # No copy this time. Use the same .gitattributes file as the UNIX distfiles dist
createDistfilesDist("Windows", winDistFilesDir)

###### UNIX source code distribution
fixGitAttributes(True, False, excludeFiles + excludeWindowsFiles)
createSourceDist("UNIX", distDir)

###### Windows source code distribution
fixGitAttributes(True, True, excludeFiles + excludeUnixFiles)
createSourceDist("Windows", distDir)


#
# Consolidate demo, demo scripts distributions.
#
print "Consolidating demo distribution...",
os.chdir(srcDir)
sys.stdout.flush()

# Unix demo distribution
copy("ICE_LICENSE", demoDir)
copy(os.path.join(distFilesDir, "src", "ws", "README.DEMOS"), demoDir)

copyMatchingFiles(os.path.join("certs"), os.path.join(demoDir, "certs"), demoCertsFiles)
for d in ["", "cpp"]:
    copyMatchingFiles(os.path.join(d, "config"), os.path.join(demoDir, "config"), demoConfigFiles)

copy(os.path.join(distFilesDir, "src", "ws", "Make.rules"), os.path.join(demoDir, "config"), False)

# Consolidate demoscript and demo distribution with files from each language mapping
for d in os.listdir('.'):
    if os.path.isdir(d) and os.path.exists(os.path.join(d, "demo")):
        copy(os.path.join(d, "demo"), os.path.join(demoDir, getMappingDir("demo", d)))

configSubstituteExprs = [(re.compile(regexpEscape("../../certs")), "../certs")]
for root, dirnames, filesnames in os.walk(demoDir):
    for f in filesnames:
        if fnmatch.fnmatch(f, "config*"):
            substitute(os.path.join(root, f), configSubstituteExprs)

# Windows demo distribution
copy(os.path.join(winDistFilesDir, "src", "ws", "README.DEMOS.txt"), os.path.join(winDemoDir, "README.txt"))

copyMatchingFiles(os.path.join(winSrcDir, "certs"), os.path.join(winDemoDir, "certs"), demoCertsFiles)

os.mkdir(os.path.join(winDemoDir, "config"))

copy(os.path.join(winSrcDir, "config", "Make.common.rules.mak"), os.path.join(winDemoDir, "config"), False)
copy(os.path.join(winSrcDir, "cpp", "config", "Make.rules.msvc"), os.path.join(winDemoDir, "config"), False)

copy(os.path.join(winDistFilesDir, "src", "ws", "Make.rules.mak"), os.path.join(winDemoDir, "config"), False)

# Consolidate demo distribution with files from each language mapping
for sd in os.listdir(winSrcDir):
    d = os.path.join(winSrcDir, sd)
    if os.path.isdir(d) and os.path.exists(os.path.join(d, "demo")):
        copy(os.path.join(d, "demo"), os.path.join(winDemoDir, getMappingDir("demo", sd)))

rmFiles = []

projectSubstituteExprs = [(re.compile(re.escape('"README"')), '"README.txt"'),
                          (re.compile(re.escape("PublicKeyToken=1f998c50fec78381")), "PublicKeyToken=cdd571ade22f2f16"),
                          (re.compile(re.escape("..\\..\\..\\..\\..\\certs\\winrt\\cacert.pem")), 
                           "..\\..\\..\\..\\certs\\winrt\\cacert.pem")]

for root, dirnames, filesnames in os.walk(winDemoDir):
    for f in filesnames:

        if fnmatch.fnmatch(f, "README") and not fnmatch.fnmatch(f, "README.txt"):
            os.rename(os.path.join(root, f), os.path.join(root, f + ".txt"))
        elif fnmatch.fnmatch(f, "config*"):
            substitute(os.path.join(root, f), configSubstituteExprs)

        for m in [ "*.vcproj", "*.vcxproj", "*.vcxproj.filters", "*.csproj", "*.vbproj" ]:
            if fnmatch.fnmatch(f, m):
                FixUtil.fileMatchAndReplace(os.path.join(root, f), projectSubstituteExprs, False)

        for m in [ "Makefile", ".depend", "*.exe.config" ]:
            if fnmatch.fnmatch(f, m):
                rmFiles.append(os.path.join(root[len(winDemoDir) + 1:], f))
        
for d in ["democs", "demovb"]:
    for root, dirnames, filesnames in os.walk(os.path.join(winDemoDir, d)):
        for f in filesnames:
            for m in [ "Makefile.mak", ".depend.mak" ]:
                if fnmatch.fnmatch(f, m):
                    rmFiles.append(os.path.join(root[len(winDemoDir) + 1:], f))

for f in rmFiles: remove(os.path.join(winDemoDir, f))

print "ok"


#
# Copy CHANGES and RELEASE_NOTES
#
#copy(os.path.join(srcDir, "CHANGES"), os.path.join(distDir, "IceWS-" + version + "-CHANGES"))
#copy(os.path.join(srcDir, "RELEASE_NOTES"), os.path.join(distDir, "IceWS-" + version + "-RELEASE_NOTES"))

#
# Everything should be clean now, we can create the source distributions archives
# 
print "Archiving..."
sys.stdout.flush()
os.chdir(distDir)
for d in [srcDir, demoDir, distFilesDir]:
    tarArchive(d, verbose)

for (dir, archiveDir) in [(winDistFilesDir, "distfiles-" + version)]:
    zipArchive(dir, verbose, archiveDir)
os.rename(os.path.join(distDir, "distfiles.zip"), os.path.join(distDir, "distfiles-" + version + ".zip"))

for (dir, archiveDir) in [(winSrcDir, "IceWS-" + version)]:
    zipArchive(dir, verbose, archiveDir)
os.rename(os.path.join(distDir, "IceWS.zip"), os.path.join(distDir, "IceWS-" + version + ".zip"))

for (dir, archiveDir) in [(winDemoDir, "IceWS-" + version + "-demos")]:
    zipArchive(dir, verbose, archiveDir)
os.rename(os.path.join(distDir, "demos.zip"), os.path.join(distDir, "IceWS-" + version + "-demos.zip"))

#
# Write source distribution report in README file.
#
writeSrcDistReport("IceWS", version, tag, compareToDir, 
                   [(srcDir + ".tar.gz", srcDir), 
                    (demoDir + ".tar.gz", demoDir),
                    (os.path.join(distDir, "distfiles-" + version + ".zip"), winDistFilesDir),
                    (os.path.join(distDir, "IceWS-" + version + ".zip"), winSrcDir),
                    (os.path.join(distDir, "IceWS-" + version + "-demos.zip"), winDemoDir),
                    ])

#
# Done.
#
print "Cleaning up...",
sys.stdout.flush()
remove(srcDir)
remove(winSrcDir)
remove(demoDir)
remove(winDemoDir)
remove(distFilesDir)
remove(winDistFilesDir)
print "ok"
