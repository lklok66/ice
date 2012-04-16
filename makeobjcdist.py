#!/usr/bin/env python
# **********************************************************************
#
# Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
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
    "./ICE_TOUCH_LICENSE",
    "./Makefile.objc",
    "./config/makedepend.py",
    "./config/Make.common.rules.objc",
    "./config/TestUtil.py",
    "./cpp/Makefile",
    "./cpp/config/Make.rules.objc",
    "./cpp/config/Make.rules.Darwin",
    "./cpp/include/IceUtil",
    "./cpp/include/Ice",
    "./cpp/include/IceSSL",
    "./cpp/include/Glacier2",
    "./cpp/include/Slice",
    "./cpp/src/Makefile",
    "./cpp/src/IceUtil",
    "./cpp/src/Ice",
    "./cpp/src/IceSSL",
    "./cpp/src/Glacier2Lib",
    "./cpp/src/Slice",
    "./cpp/src/slice2cpp",
    "./cpp/src/slice2objc",
    "./cpp/bin",
    "./cpp/lib",
    "./objc",
    "./Xcode",
    "./slice",
    "./certs",
    "./scripts",
    "./distribution/src/mac/IceTouch"
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
mmversion = re.search("([0-9]+\.[0-9b]+)[\.0-9]*", version).group(1)
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
demoDir = os.path.join(distDir, "IceTouch-" + version + "-demos")
os.mkdir(demoDir)

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
substitute(os.path.join("cpp", "Makefile"), [(r'^SUBDIRS([\s]*)=.*', r'SUBDIRS\1= src'), 
                                             (r'^INSTALL_SUBDIRS([\s]*)=.*', r'INSTALL_SUBDIRS\1= $(install_bindir)')])

#
# Comment out dependency on SLICEPARSERLIB since the translators are built with 
# static libraries and make sure the defaut targets the simulator
#
substitute(os.path.join("objc", "config", "Make.rules"),
           [(r'^([\s]*)SLICEPARSERLIB.*=.*', r'\1SLICEPARSERLIB = '),
            (r'^[#]*OPTIMIZE_SPEED([\s]*)=.*', r'#OPTIMIZE_SPEED\1= yes'),
            (r'^[#]*OPTIMIZE_SIZE([\s]*)=.*', r'#OPTIMIZE_SIZE\1= yes'),
            (r'^[#]*COMPILE_FOR_IPHONE([\s]*)=.*', r'#COMPILE_FOR_IPHONE\1= yes'),
            (r'^[#]*COMPILE_FOR_IPHONE_SIMULATOR([\s]*)=.*', r'#COMPILE_FOR_IPHONE_SIMULATOR\1= yes')])


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
move(os.path.join("objc", "CHANGES"), os.path.join("CHANGES"))

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
        elif f == "expect.py":
            os.remove(filepath)
        elif f.endswith(".mak") or f.endswith(".rc"):
            os.remove(filepath)
        else:

            # Fix version of README/INSTALL/RELEASE_NOTES files and
            # keep track of bison/flex files for later processing
            if fnmatch.fnmatch(f, "README*") or fnmatch.fnmatch(f, "INSTALL*") or fnmatch.fnmatch(f, "RELEASE_NOTES*"):
                substitute(filepath, [("@ver@", version), ("@mmver@", mmversion)])
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
# Create the demo distribution.
#
print "Creating demo distribution...",
sys.stdout.flush()

#
# Files from the top-level, cpp, java and cs config directories to include in the demo 
# source distribution config directory.
#
configFiles = [ \
    "Make.*", \
]

#
# Files from the top-level certs directory to include in the demo distribution certs
# directory.
#
certsFiles = [ \
    "*.pem", \
]


# Demo distribution
copy("ICE_LICENSE", demoDir)
copy("ICE_TOUCH_LICENSE", demoDir)
copy(os.path.join("distribution", "src", "mac", "IceTouch", "README.DEMOS"), os.path.join(demoDir, "README"))

copyMatchingFiles(os.path.join("certs"), os.path.join(demoDir, "certs"), certsFiles)
for d in ["", "objc"]:
    copyMatchingFiles(os.path.join(d, "config"), os.path.join(demoDir, "config"), configFiles)

# Consolidate demoscript and demo distribution with files from each language mapping
baseDemoDir = os.path.join('objc', 'demo')
for d in os.listdir(baseDemoDir):
    if os.path.isdir(os.path.join(baseDemoDir, d)):
        copy(os.path.join(baseDemoDir, d), os.path.join(demoDir, d))
copy(os.path.join(baseDemoDir, "Makefile"), os.path.join(demoDir, "Makefile"))

configSubstituteExprs = [(re.compile(regexpEscape("../../../certs")), "../certs")]
makeSubstituteExprs = [ (re.compile("= \.\.$"), " = ."),
                        (re.compile(regexpEscape("../..")), ".."),
                        (re.compile(regexpEscape("../../..")), "../..")]
xcodeCppSubstituteExprs = [ (re.compile("ADDITIONAL_SDKS = .*;"), "ADDITIONAL_SDKS = \"/Library/Developer/IceTouch-%s/SDKs/Cpp/$(PLATFORM_NAME).sdk\";" % mmversion) ]
xcodeSubstituteExprs = [ (re.compile("ADDITIONAL_SDKS = .*;"), "ADDITIONAL_SDKS = \"/Library/Developer/IceTouch-%s/SDKs/ObjC/$(PLATFORM_NAME).sdk\";" % mmversion) ]

for root, dirnames, filesnames in os.walk(demoDir):
    for f in filesnames:
        if fnmatch.fnmatch(f, "config*"):
            substitute(os.path.join(root, f), configSubstituteExprs)
        elif fnmatch.fnmatch(f, "Makefile"):
            substitute(os.path.join(root, f), makeSubstituteExprs)
        elif fnmatch.fnmatch(f, "project.pbxproj"):
            if fnmatch.fnmatch(root, "*/iPhone/cpp/*"):
                substitute(os.path.join(root, f), xcodeCppSubstituteExprs)
            else:
                substitute(os.path.join(root, f), xcodeSubstituteExprs)

print "ok"

#
# Build the translators
#
shutil.copytree("cpp", "cpp-translators", True)
os.chdir("cpp-translators")
if os.system("make") != 0:
    print sys.argv[0] + ": failed to the build the translators"
    sys.exit(1)
os.chdir("..")
move(os.path.join("cpp-translators", "bin", "slice2objc"), os.path.join("cpp", "bin", "slice2objc"))
move(os.path.join("cpp-translators", "bin", "slice2cpp"), os.path.join("cpp", "bin", "slice2cpp"))
remove("cpp-translators")

remove("distribution")

#
# Everything should be clean now, we can create the source distributions archives
# 
print "Archiving..."
sys.stdout.flush()
os.chdir(distDir)

for d in [srcDir, demoDir]:
    tarArchive(d, verbose)

for d in [srcDir]:
    zipArchive(d, verbose)

#
# Write source distribution report in README file.
#
writeSrcDistReport("IceTouch", version, compareToDir, [srcDir, demoDir])

#
# Done.
#
print "Cleaning up...",
sys.stdout.flush()
remove(srcDir)
remove(demoDir)
print "ok"

os.chdir(cwd)
