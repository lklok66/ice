#!/usr/bin/env python
# **********************************************************************
#
# Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

import os, sys, fnmatch, re, getopt

sys.path.append(os.path.join(os.path.dirname(__file__), "distribution", "lib"))
from DistUtils import *

#
# This is an explicit list of some IceE specific files to remove. The
# other files are all removed by reversing the below list.
#
filesToRemove = [
    "./cpp/config/Make.rules",
    "./cpp/config/Make.rules.mak",
    "./cpp/config/convertssl.py",
    "./cpp/config/findSliceFiles.py",
    "./cpp/config/glacier2router.cfg",
    "./cpp/config/ice_ca.cnf",
    "./cpp/config/icegridnode.cfg",
    "./cpp/config/icegridregistry.cfg",
    "./cpp/config/icegrid-slice.3.1.ice.gz",
    "./cpp/config/icegrid-slice.3.2.ice.gz",
    "./cpp/config/icegrid-slice.3.3.ice.gz",
    "./cpp/config/makegitignore.py",
    "./cpp/config/templates.xml",
    "./cpp/config/upgradeicegrid.py"
]

# List of files & subdirectories to keep, all others are removed.
filesToKeep = [
    "./ICEE_LICENSE",
    "./LICENSE",
    "./CHANGES.ICEE",
    "./README.ICEE",
    "./RELEASE_NOTES.ICEE",
    "./Makefile.icee",
    "./Makefile.mak.icee",
    "./config/Make.common.rules.mak.icee",
    "./config/Make.common.rules.icee",
    "./config/TestUtil.py",
    "./cpp/Makefile.mak",
    "./cpp/Makefile",
    "./cpp/config",
    "./cpp/include/IceUtil",
    "./cpp/include/Slice",
    "./cpp/src/Makefile.mak",
    "./cpp/src/Makefile",
    "./cpp/src/IceUtil",
    "./cpp/src/Slice",
    "./cpp/src/slice2cppe",
    "./cpp/bin",
    "./cpp/lib",
    "./cppe",
    "./slice/IceE",
    "./scripts"
]

def pathInList(p, l):
    for f in l:
        # Slower, but more accurate.
	#if os.path.samefile(p, f):
        if p == f:
            return True
    return False

# This takes a list of files to keep, and generates from that a list
# of files to remove.
def genRemoveList(l):
    files = []
    dirs = []
    for root, dirnames, filenames in os.walk('.'):
        mod = []
        for d in dirnames:
            if pathInList(os.path.join(root, d), filesToKeep):
                mod.append(d)
	for e in mod:
	    del dirnames[dirnames.index(e)]

        for f in filenames:
            if not pathInList(os.path.join(root, f), filesToKeep):
                files.append(os.path.join(root, f))

        for f in dirnames:
            dirs.append(os.path.join(root, f))
    dirs.reverse()
    files.extend(dirs)
    return files

#
# Program usage.
#
def usage():
    print "Usage: " + sys.argv[0] + " [options] tag"
    print
    print "Options:"
    print "-h      Show this message."
    print "-v      Be verbose."
    print "-c DIR  Compare distribution to the one from DIR and"

#
# Check arguments
#
verbose = 0
tag = "HEAD"
compareToDir = None

try:
    opts, args = getopt.getopt(sys.argv[1:], "hvc:k:")
except getopt.GetoptError:
    usage()
    sys.exit(1)

for o, a in opts:
    if o == "-h":
        usage()
        sys.exit(0)
    elif o == "-v":
        verbose = 1
    elif o == "-c":
        compareToDir = a

if len(args) > 1:
    usage()
    sys.exit(1)

if len(args) == 1:
    tag = args[0]

cwd = os.getcwd()
os.chdir(os.path.dirname(__file__))

#
# Get IceE version.
#
config = open(os.path.join("cppe", "include", "IceE", "Config.h"), "r")
version = re.search("ICEE_STRING_VERSION \"([0-9\.]*)\"", config.read()).group(1)

#
# Remove any existing "disticee-" directory and create a new one
# and sub-directories for the each source distribution.
#
distDir = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "disticee-" + tag.replace('/', '-')))
if os.path.exists(distDir):
    remove(distDir)
os.mkdir(distDir)

print "Creating " + version + " source distributions in " + distDir

demoscriptDir = os.path.join(distDir, "IceE-" + version + "-demo-scripts")
srcDir = os.path.join(distDir, "IceE-" + version)
os.mkdir(demoscriptDir)

#
# Extract the sources with git archive using the given tag.
#
print "Creating git archive using " + tag + "...",
sys.stdout.flush()
os.system("git archive --prefix=IceE-" + version + "/ " + tag + " | ( cd " + distDir + " && tar xfm - )")
print "ok"

os.chdir(os.path.join(srcDir))

print "Fixing makefiles...",

#
# Change SUBDIRS and INSTALL_SUBDIRS in top-level Makefile.
#
for makeFileName in [os.path.join("cpp", "Makefile.mak"), os.path.join("cpp", "Makefile")]:
    makeFile = open(makeFileName, "r")
    lines = makeFile.readlines()
    makeFile.close()
    for i in range(len(lines)):
        if lines[i].find("SUBDIRS") == 0:
            lines[i] = "SUBDIRS = src\n"
        if lines[i].find("INSTALL_SUBDIRS") == 0:
            lines[i] = "INSTALL_SUBDIRS = $(install_bindir) $(install_libdir)\n"
    makeFile = open(makeFileName, "w")
    makeFile.writelines(lines)
    makeFile.close()

#
# Disable install targets for libIceUtil, libSlice.
#
for makeFileName in [os.path.join("cpp", "src", "IceUtil", "Makefile.mak"), \
                     os.path.join("cpp", "src", "IceUtil", "Makefile"), \
                     os.path.join("cpp", "src", "Slice", "Makefile.mak"), \
                     os.path.join("cpp", "src", "Slice", "Makefile")]:
    makeFile = open(makeFileName, "r")
    lines = makeFile.readlines()
    makeFile.close()

    doComment = 0
    for i in range(len(lines)):
        if lines[i].find("install::") == 0:
            doComment = 1
        elif len(lines[i].strip()) == 0:
            doComment = 0
        elif doComment:
            lines[i] = "#" + lines[i]

    makeFile = open(makeFileName, "w")
    makeFile.writelines(lines)
    makeFile.close()

print "ok"

print "Walking through distribution to fix permissions, versions, etc...",
sys.stdout.flush()

bisonFiles = []
flexFiles = []
for root, dirnames, filesnames in os.walk('.'):

    for f in filesnames:
        filepath = os.path.join(root, f) 
        if f == ".gitignore":
            os.remove(filepath)
        else:

            # Fix version of README/INSTALL files and keep track of bison/flex files for later processing
            if fnmatch.fnmatch(f, "README*") or fnmatch.fnmatch(f, "INSTALL*"):
                substitute(filepath, [("@ver@", version)])
            elif fnmatch.fnmatch(f, "*.y"):
                bisonFiles.append(filepath)
            elif fnmatch.fnmatch(f, "*.l"):
                flexFiles.append(filepath)

            fixFilePermission(filepath)
    
    for d in dirnames:
        os.chmod(os.path.join(root, d), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) # rwxr-xr-x

for root, dirnames, filesnames in os.walk('cppe'):

    for f in filesnames:
        filepath = os.path.join(root, f)
        if f == "expect.py":
            move(filepath, os.path.join(distDir, demoscriptDir, filepath))

move("demoscript", os.path.join(demoscriptDir, "demoscript")) # Move the demoscript directory
copy("scripts", os.path.join(demoscriptDir, "scripts")) # Move the scripts directory
move(os.path.join("cppe", "allDemos.py"), os.path.join(demoscriptDir, "cppe", "allDemos.py"))

print "ok"

#
# Generate bison & flex files.
#
print "Generating bison and flex files...",
sys.stdout.flush()
for x in bisonFiles:
    generateBisonFile(x)
for x in flexFiles:
    generateFlexFile(x)
print "ok"

#
# Remove or move non-public files out of source distribution.
#
print "Removing non-Ice directories and files...",
sys.stdout.flush()
for x in genRemoveList(filesToKeep):
    remove(x, False)

for x in filesToRemove:
    remove(x)
print "ok"

#
# Copy IceE specific install files.
#
print "Copying icee install files...",
move(os.path.join("README.ICEE"), os.path.join("README"))
move(os.path.join("CHANGES.ICEE"), os.path.join("CHANGES"))
move(os.path.join("RELEASE_NOTES.ICEE"), os.path.join("RELEASE_NOTES"))

#
# Move *.icee to the correct names.
#
move(os.path.join("cpp", "config", "Make.rules.mak.icee"), os.path.join("cpp", "config", "Make.rules.mak"))
move(os.path.join("cpp", "config", "Make.rules.icee"), os.path.join("cpp", "config", "Make.rules"))
move(os.path.join("Makefile.mak.icee"), os.path.join("Makefile.mak"))
move(os.path.join("Makefile.icee"), os.path.join("Makefile"))

print "ok"

#
# Everything should be clean now, we can create the source distributions archives
# 
print "Archiving..."
sys.stdout.flush()
os.chdir(distDir)

for d in [srcDir]:
    zipArchive(srcDir, verbose)

for d in [srcDir]:
    tarArchive(srcDir, verbose)

for (dir, archiveDir) in [(demoscriptDir, "IceE-" + version + "-demos")]:
    tarArchive(dir, verbose, archiveDir)

#
# Write source distribution report in README file.
#
writeSrcDistReport("IceE", version, compareToDir, [srcDir])

#
# Done.
#
print "Cleaning up...",
sys.stdout.flush()
remove(srcDir)
remove(demoscriptDir)
print "ok"

os.chdir(cwd)
