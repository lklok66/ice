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
# This is an explicit list of some Ice Android specific files to remove. The
# other files are all removed by reversing the below list.
#
filesToRemove = [
    "./java/src/ant",
    "./java/resources",
    "./java/CHANGES",
    "./java/INSTALL",
    "./java/README",
    "./java/THIRD_PARTY_LICENSE",
    "./java/THIRD_PARTY_SOURCES",
    "./scripts/IceStormUtil.py",
    "./scripts/IceGridAdmin.py",
    "./java/allDemos.py"
]

# List of files & subdirectories to keep, all others are removed.
filesToKeep = [
    "./LICENSE",
    "./ICE_LICENSE",
    "./Makefile.android",
    "./Makefile.mak.android",
    "./java",
    "./android",
    "./scripts",
    "./certs"
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
# Get version.
#
config = open(os.path.join("java", "config", "build.properties"), "r")
version = re.search("iceandroid.version = ([0-9\.]*)", config.read()).group(1)

#
# Remove any existing "disticeandroid-" directory and create a new one
#
distDir = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "disticeandroid-" + tag.replace('/', '-')))
if os.path.exists(distDir):
    remove(distDir)
os.mkdir(distDir)

print "Creating IceAndroid " + version + " source distributions in " + distDir

srcDir = os.path.join(distDir, "IceAndroid-" + version)

#
# Extract the sources with git archive using the given tag.
#
print "Creating git archive using " + tag + "...",
sys.stdout.flush()
os.system("git archive --prefix=IceAndroid-" + version + "/ " + tag + " | ( cd " + distDir + " && tar xfm - )")
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

#fixVersion(os.path.join("distribution", "bin", "makeiceandroidbindist.py"), version)

bisonFiles = []
flexFiles = []
for root, dirnames, filesnames in os.walk('.'):

    for f in filesnames:
        filepath = os.path.join(root, f) 
        if f == ".gitignore" or f == "expect.py":
            os.remove(filepath)
        else:
            # Fix version of README/INSTALL files
            if fnmatch.fnmatch(f, "README*") or fnmatch.fnmatch(f, "INSTALL*"):
                fixVersion(filepath, version)
            fixFilePermission(filepath)
    
    for d in dirnames:
        os.chmod(os.path.join(root, d), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) # rwxr-xr-x

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
# Copy IceAndroid specific install files.
#
print "Copying install files...",
move(os.path.join("android", "CHANGES"), os.path.join("CHANGES"))
move(os.path.join("android", "README"), os.path.join("README"))
move(os.path.join("android", "RELEASE_NOTES"), os.path.join("RELEASE_NOTES"))
move(os.path.join("android", "INSTALL"), os.path.join("INSTALL"))

move(os.path.join("Makefile.mak.android"), os.path.join("Makefile.mak"))
move(os.path.join("Makefile.android"), os.path.join("Makefile"))

#
# Copy CHANGES and RELEASE_NOTES
#
copy(os.path.join(srcDir, "CHANGES"), os.path.join(distDir, "IceAndroid-" + version + "-CHANGES"))
copy(os.path.join(srcDir, "RELEASE_NOTES"), os.path.join(distDir, "IceAndroid-" + version + "-RELEASE_NOTES"))
copy(os.path.join(srcDir, "README"), os.path.join(distDir, "IceAndroid-" + version + "-README"))

print "ok"

#
# Build the .jar files.
#
copy("java", "java-build")
os.chdir("java-build")
if os.system("ant jar") != 0:
    print sys.argv[0] + ": failed to the build the Ice Android jar files"
    sys.exit(1)
os.chdir("..")
os.mkdir(os.path.join("java", "lib"))
move(os.path.join("java-build", "lib", "IceAndroid.jar"), os.path.join("java", "lib", "IceAndroid.jar"))
move(os.path.join("java-build", "lib", "IceTest.jar"), os.path.join("java", "lib", "IceTest.jar"))
remove("java-build")

#
# Everything should be clean now, we can create the source distributions archives
# 
print "Archiving..."
sys.stdout.flush()
os.chdir(distDir)

for d in [srcDir]:
    tarArchive(d, verbose)

for d in [srcDir]:
    zipArchive(d, verbose)

#
# Write source distribution report in README file.
#
writeSrcDistReport("IceAndroid", version, compareToDir, [srcDir])

#
# Done.
#
print "Cleaning up...",
sys.stdout.flush()
remove(srcDir)
print "ok"

os.chdir(cwd)
