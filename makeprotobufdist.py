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
    "./cpp/config//Make.rules.mak", \
    "./cpp/config//Make.rules.bcc", \
    "./cpp/config//Make.rules.msvc", \
    "./protobuf/config//Make.rules.mak", \
    "./protobuf/demo/cpp/Makefile.mak", \
]

# List of files & subdirectories to keep, all others are removed.
filesToKeep = [
    "./LICENSE", \
    "./config/Make.common.rules", \
    "./cpp/config", \
    "./java/config", \
    "./protobuf/config", \
    "./protobuf/demo", \
    "./protobuf/README", \
    "./protobuf/ICE_PROTOBUF_LICENSE", \
]

#
# Files from the top-level, cpp and java config directories to include in the
# source distribution config directory.
#
configFiles = [ \
    "Make.*", \
    "common.xml", \
    "build.properties", \
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
    print "Usage: " + sys.argv[0] + " [options] version [tag]"
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

if  len(args) < 1 or len(args) > 2:
    usage()
    sys.exit(1)

if len(args) == 1:
    version = args[0]

if len(args) == 2:
    tag = args[1]

cwd = os.getcwd()
os.chdir(os.path.dirname(__file__))

#
# Remove any existing "disticee-" directory and create a new one
# and sub-directories for the each source distribution.
#
distDir = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "distprotobuf-" + tag.replace('/', '-')))
if os.path.exists(distDir):
    remove(distDir)
os.mkdir(distDir)

print "Creating " + version + " source distributions in " + distDir

srcDir = os.path.join(distDir, "IceProtobuf-" + version)

#
# Extract the sources with git archive using the given tag.
#
print "Creating git archive using " + tag + "...",
sys.stdout.flush()
os.system("git archive --prefix=IceProtobuf-" + version + "/ " + tag + " | ( cd " + distDir + " && tar xfm - )")
print "ok"

os.chdir(os.path.join(srcDir))

print "Walking through distribution to fix permissions, versions, etc...",
sys.stdout.flush()

for root, dirnames, filesnames in os.walk('.'):

    for f in filesnames:
        filepath = os.path.join(root, f) 
        if f == ".gitignore" or f == "expect.py":
            os.remove(filepath)
        else:

            # Fix version of README/INSTALL files and keep track of bison/flex files for later processing
            if fnmatch.fnmatch(f, "README*") or fnmatch.fnmatch(f, "INSTALL*"):
                fixVersion(filepath, version)

            fixFilePermission(filepath)
    
    for d in dirnames:
        os.chmod(os.path.join(root, d), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) # rwxr-xr-x

print "ok"

#
# Remove or move non-public files out of source distribution.
#
print "Removing non-IceProtobuf directories and files...",
sys.stdout.flush()
for x in genRemoveList(filesToKeep):
    remove(x, False)

for x in filesToRemove:
    remove(x)
print "ok"

#
# Copy IceE specific install files.
#
print "Copying protobuf install files...",
#move(os.path.join("distribution", "src", "protobuf", "README"), os.path.join("README"))

#
# Move demo directories to the correct places.
#
move(os.path.join("protobuf", "demo"), os.path.join("demo"))
move(os.path.join("protobuf", "README"), os.path.join("README"))
move(os.path.join("protobuf", "ICE_PROTOBUF_LICENSE"), os.path.join("ICE_PROTOBUF_LICENSE"))

for d in ["cpp", "java"]:
    copyMatchingFiles(os.path.join(d, "config"), os.path.join("config"), configFiles)

remove("cpp")
remove("java")
remove("protobuf")

print "ok"

#
# Everything should be clean now, we can create the source distributions archives
# 
print "Archiving..."
sys.stdout.flush()
os.chdir(distDir)

tarArchive(srcDir, verbose)
zipArchive(srcDir, verbose)

#
# Write source distribution report in README file.
#
writeSrcDistReport("IceProtobuf-", version, compareToDir, [srcDir])

#
# Done.
#
print "Cleaning up...",
sys.stdout.flush()
remove(srcDir)
print "ok"

os.chdir(cwd)
