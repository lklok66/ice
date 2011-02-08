#!/usr/bin/env python
# **********************************************************************
#
# Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

import os, sys, fnmatch, re, getopt

sys.path.append(os.path.join(os.path.dirname(__file__), "distribution", "lib"))
from DistUtils import *

#
# This is an explicit list of files to be removed. The
# other files are all removed by reversing the below list.
#
filesToRemove = [
]

# List of files & subdirectories to keep, all others are removed.
filesToKeep = [
    "./LICENSE",
    "./ICE_LICENSE",
    "./android"
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
    print "-c DIR  Compare distribution to the one from DIR"

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
# Ice for Android version.
#
version = "0.2"

#
# Ice tag.
#
iceTag = "R3_4_1"

#
# Patch file.
#
patchFile = "android/patch.txt"

distDir = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "disticeandroid-" + tag.replace('/', '-')))
print "Creating IceAndroid " + version + " distributions in " + distDir

#
# Remove any existing "disticeandroid-" directory and create a new one
#
if os.path.exists(distDir):
    remove(distDir)
os.mkdir(distDir)

srcDir = os.path.join(distDir, "IceAndroid-" + version)

#
# Extract the Ice for Android sources.
#
print "Extracting Ice for Android sources using tag " + tag + "...",
sys.stdout.flush()
os.system("git archive --prefix=IceAndroid-" + version + "/ " + tag + " | ( cd " + distDir + " && tar xfm - )")
print "ok"

#
# Extract the Ice sources.
#
print "Extracting Ice sources using tag " + iceTag + "...",
sys.stdout.flush()
os.system("git archive --prefix=Ice-" + iceTag + "/ " + iceTag + " | ( cd " + distDir + " && tar xfm - )")
print "ok"

iceDir = os.path.join(distDir, "Ice-" + iceTag)

print "Patching Ice sources..."
os.chdir(iceDir)
os.system("patch -p1 < " + os.path.join(srcDir, patchFile))

print "Building Ice for Android JAR file..."
os.chdir(os.path.join(iceDir, "cpp", "src", "IceUtil"))
os.system("make")
os.chdir(os.path.join(iceDir, "cpp", "src", "Slice"))
os.system("make")
os.chdir(os.path.join(iceDir, "cpp", "src", "slice2java"))
os.system("make")
os.chdir(os.path.join(iceDir, "cpp", "src", "slice2freezej"))
os.system("make")
os.chdir(os.path.join(iceDir, "java"))
os.system("ant ice-jar")

os.chdir(srcDir)

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
# Move install files.
#
print "Copying install files...",
move(os.path.join("android", "CHANGES"), "CHANGES")
move(os.path.join("android", "README"), "README")
move(os.path.join("android", "RELEASE_NOTES"), "RELEASE_NOTES")
move(os.path.join("android", "INSTALL"), "INSTALL")

#
# Copy CHANGES, README and RELEASE_NOTES.
#
copy(os.path.join(srcDir, "CHANGES"), os.path.join(distDir, "IceAndroid-" + version + "-CHANGES"))
copy(os.path.join(srcDir, "RELEASE_NOTES"), os.path.join(distDir, "IceAndroid-" + version + "-RELEASE_NOTES"))
copy(os.path.join(srcDir, "README"), os.path.join(distDir, "IceAndroid-" + version + "-README"))

print "ok"

#
# Move the JAR file.
#
os.mkdir(os.path.join("android", "lib"))
move(os.path.join(iceDir, "java", "lib", "Ice.jar"), os.path.join("android", "lib", "Ice.jar"))

#
# Everything should be clean now, we can create the distribution archives.
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
remove(iceDir)
print "ok"

os.chdir(cwd)
