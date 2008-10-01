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
# This is an explicit list of some IceTouch specific files to remove. The
# other files are all removed by reversing the below list.
#
filesToRemove = [
    "./cpp/src/Ice/Makefile",
    "./cpp/src/Ice/Makefile.mak",
]

# List of files & subdirectories to keep, all others are removed.
filesToKeep = [
    "./LICENSE",
    "./ICE_LICENSE",
    "./Makefile.objc",
    "./config/Make.common.rules",
    "./config/TestUtil.py",
    "./cpp/Makefile",
    "./cpp/config/Make.rules.objc",
    "./cpp/config/Make.rules.Darwin",
    "./cpp/include/IceUtil",
    "./cpp/include/Ice",
    "./cpp/include/Slice",
    "./cpp/src/Makefile",
    "./cpp/src/IceUtil",
    "./cpp/src/Ice",
    "./cpp/src/Slice",
    "./cpp/src/slice2cpp",
    "./cpp/src/slice2objc",
    "./cpp/bin",
    "./cpp/lib",
    "./objc",
    "./slice"
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
    print "-k key  Specify Ice public key token."

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
# Get IceTouch version.
#
config = open(os.path.join("cpp", "config", "Make.rules.objc"), "r")
version = re.search("VERSION[\s]*= ([0-9\.]*)", config.read()).group(1)
versionMinor = re.search("([0-9\.]*).([0-9\.]*)", version).group(2)
versionMajor = re.search("([0-9\.]*).([0-9\.]*)", version).group(1)

#
# Remove any existing "disticee-" directory and create a new one
# and sub-directories for the each source distribution.
#
distDir = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "disticetouch-" + tag.replace('/', '-')))
if os.path.exists(distDir):
    remove(distDir)
os.mkdir(distDir)

print "Creating " + version + " source distributions in " + distDir

srcDir = os.path.join(distDir, "IceTouch-" + version)

#
# Extract the sources with git archive using the given tag.
#
print "Creating git archive using " + tag + "...",
sys.stdout.flush()
os.system("git archive --prefix=IceTouch-" + version + "/ " + tag + " | ( cd " + distDir + " && tar xfm - )")
print "ok"

os.chdir(os.path.join(srcDir))

print "Fixing makefiles...",

#
# Change SUBDIRS and INSTALL_SUBDIRS in top-level Makefile.
#
substitute(os.path.join("Makefile"), [(r'^SUBDIRS([\s]*)=.*', r'SUBDIRS\1= cpp objc'), 
                                      (r'^CLEAN_SUBDIRS([\s]*)=.*', r'INSTALL_SUBDIRS\1= $(install_bindir)'),
                                      (r'^DEPEND_SUBDIRS([\s]*)=.*', r'INSTALL_SUBDIRS\1= $(install_bindir)'),
                                      (r'^INSTALL_SUBDIRS([\s]*)=.*', r'INSTALL_SUBDIRS\1= $(install_bindir)')])

substitute(os.path.join("cpp", "Makefile"), [(r'^SUBDIRS([\s]*)=.*', r'SUBDIRS\1= src'), 
                                             (r'^INSTALL_SUBDIRS([\s]*)=.*', r'INSTALL_SUBDIRS\1= $(install_bindir)')])

#
# Comment out dependency on SLICEPARSERLIB since the translators are built with 
# static libraries and make sure the defaut targets the simulator
#
substitute(os.path.join("objc", "config", "Make.rules"),
           [(r'^([\s]*)SLICEPARSERLIB.*=.*', r'\1SLICEPARSERLIB = '),
            (r'^[\s#]*OPTIMIZE_SPEED([\s]*)=.*', r'OPTIMIZE_SPEED\1= yes'),
            (r'^[\s#]*COMPILE_FOR_IPHONE([\s]*)=.*', r'#COMPILE_FOR_IPHONE\1= yes'),
            (r'^[\s#]*COMPILE_FOR_IPHONE_SIMULATOR([\s]*)=.*', r'COMPILE_FOR_IPHONE_SIMULATOR\1= yes')])

#
# Fix the versions in Make.rules.common
#
substitute(os.path.join("config", "Make.common.rules"),
           [(r'VERSION_MINOR([\s]*)=.*', r'VERSION_MINOR\1= ' + versionMinor),
            (r'VERSION_MAJOR([\s]*)=.*', r'VERSION_MAJOR\1= ' + versionMajor),
            (r'VERSION([\s]*)=.*', r'VERSION\1= ' + version),
            (r'SHORT_VERSION([\s]*)=.*', r'<remove>'),
            (r'SOVERSION([\s]*)=.*', r'<remove>'),
            (r'ICE_LICENSE', r'ICETOUCH_LICENSE')])

for makeFileName in [os.path.join("cpp", "src", "Makefile")]:
    makeFile = open(makeFileName, "r")
    lines = makeFile.readlines()
    makeFile.close()

    doRemove = 0
    newlines = []
    for i in range(len(lines)):
        if lines[i].find("SUBDIRS") == 0:
            newlines.append("SUBDIRS = IceUtil Slice slice2cpp slice2objc\n\n")
            doRemove = 1
        elif len(lines[i].strip()) == 0 and doRemove:
            doRemove = 0
        elif not doRemove:
            newlines.append(lines[i])

    makeFile = open(makeFileName, "w")
    makeFile.writelines(newlines)
    makeFile.close()


#
# Disable install targets for libIceUtil, libSlice.
#
for makeFileName in [os.path.join("cpp", "src", "IceUtil", "Makefile"), \
                     os.path.join("cpp", "src", "Ice", "Makefile"), \
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

print "Removing non-Ice directories and files...",
sys.stdout.flush()
for x in genRemoveList(filesToKeep):
    remove(x, False)

for x in filesToRemove:
    remove(x)
print "ok"

#
# Copy IceTouch specific install files.
#
print "Copying icetouch install files...",
move(os.path.join("objc", "README"), os.path.join("README"))
move(os.path.join("objc", "RELEASE_NOTES"), os.path.join("RELEASE_NOTES"))
move(os.path.join("objc", "INSTALL"), os.path.join("INSTALL"))
move(os.path.join("objc", "ICETOUCH_LICENSE"), os.path.join("ICETOUCH_LICENSE"))
#move(os.path.join("objc", "CHANGES"), os.path.join("CHANGES"))

#
# Move *.iceobjc to the correct names.
#
move(os.path.join("cpp", "config", "Make.rules.objc"), os.path.join("cpp", "config", "Make.rules"))
move(os.path.join("Makefile.objc"), os.path.join("Makefile"))
print "ok"

#
# Remove or move non-public files out of source distribution.
#
print "Walking through distribution to fix permissions, versions, etc...",
sys.stdout.flush()

bisonFiles = []
flexFiles = []
for root, dirnames, filesnames in os.walk('.'):

    for f in filesnames:
        filepath = os.path.join(root, f) 
        if f == ".gitignore":
            os.remove(filepath)
        elif f.endswith(".mak") or f.endswith(".rc"):
            os.remove(filepath)
        else:

            # Fix version of README/INSTALL/RELEASE_NOTES files and
            # keep track of bison/flex files for later processing
            if fnmatch.fnmatch(f, "README*") or fnmatch.fnmatch(f, "INSTALL*") or fnmatch.fnmatch(f, "RELEASE_NOTES*"):
                substitute(filepath, [("@ver@", version)])
            elif fnmatch.fnmatch(f, "*.y"):
                bisonFiles.append(filepath)
            elif fnmatch.fnmatch(f, "*.l"):
                flexFiles.append(filepath)

            fixFilePermission(filepath)
    
    for d in dirnames:
        os.chmod(os.path.join(root, d), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) # rwxr-xr-x

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
# Everything should be clean now, we can create the source distributions archives
# 
print "Archiving..."
sys.stdout.flush()
os.chdir(distDir)

for d in [srcDir]:
    zipArchive(srcDir, verbose)
for d in [srcDir]:
    tarArchive(srcDir, verbose)

#
# Write source distribution report in README file.
#
writeSrcDistReport("IceTouch", version, compareToDir, [srcDir])

#
# Done.
#
print "Cleaning up...",
sys.stdout.flush()
remove(srcDir)
print "ok"

os.chdir(cwd)
