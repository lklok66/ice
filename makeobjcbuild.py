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

def writeBinDistReport(product, version, compareToDir, distributions):

    cwd = os.getcwd()
    os.chdir(cwd)

    print "Writing report in README...",
    readme = open("README", "w")
    print >>readme, "This directory contains the binary distributions of " + product + " " + version + ".\n"
    print >>readme, "Creation time: " + time.strftime("%a %b %d %Y, %I:%M:%S %p (%Z)")
    (sysname, nodename, release, ver, machine) = os.uname();
    print >>readme, "Host: " + nodename
    print >>readme, "Platform: " + sysname + " " + release
    print >>readme, "User: " + os.environ["USER"]
    print >>readme, ""

    if compareToDir:
        print
        print >>readme, "Comparison with", compareToDir
        modifications = ([], [], [])
        for dist in distributions:
            dist = os.path.basename(dist)
            print "   comparing " + dist + " ...",
            sys.stdout.flush()
            if untarArchive(os.path.join(compareToDir, dist) + ".tar.gz", False, dist + "-orig"):
                n = compareDirs(dist + "-orig", dist)
                modifications = [ modifications[i] + n[i]  for i in range(len(modifications))]
                if n != ([], [], []):
                    os.system("diff -r -N " + dist + "-orig " + dist + " > patch-" + dist)
                remove(dist + "-orig")
            print "ok"

        (added, updated, removed) = modifications
        for (desc, list) in [("Added", added), ("Removed", removed), ("Updated", updated)]:
            if len(list) > 0:
                list.sort()
                print >>readme
                print >>readme, desc, "files:"
                print >>readme, string.join(["=" for c in range(len(desc + " files:"))], '')
                for f in list:
                    print >>readme, f

    else:
        print "ok"
        
    readme.close()

    os.chdir(cwd)

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
    print "-X      Temporary no build option"

#
# Check arguments
#
verbose = 0
tag = "HEAD"
compareToDir = None
build = True

try:
    opts, args = getopt.getopt(sys.argv[1:], "Xhvc:k:")
except getopt.GetoptError:
    usage()
    sys.exit(1)

for o, a in opts:
    if o == "-h":
        usage()
        sys.exit(0)
    elif o == "-X":
        build = False
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
# Get the distDir
#
distDir = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "disticetouch-" + tag.replace('/', '-')))
baseDir = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "buildicetouch-" + tag.replace('/', '-')))
if build: # TODO:
    if os.path.exists(baseDir):
        remove(baseDir)
    os.mkdir(baseDir)
buildDir = os.path.join(baseDir, "build")
buildLog = os.path.join(baseDir, "build.log")

print "Building " + version + " distributions in " + baseDir

os.chdir(os.path.join(baseDir))

if build: # TODO:
    print "Extracting archive...",
    baseName = os.path.join(distDir, "IceTouch-" + version)
    os.system("tar xfz " + os.path.join(distDir, baseName + ".tar.gz"))
    print "ok"

    os.rename("IceTouch-" + version, "build")

os.chdir(buildDir)

if build:
    print "Building distributions...",

    os.system("OPTIMIZE_SPEED=yes make > %s 2>&1" % buildLog)
    os.system("OPTIMIZE_SPEED=yes COMPILE_FOR_COCOA=yes make >> %s 2>&1" % buildLog)
    os.system("OPTIMIZE_SIZE=yes COMPILE_FOR_IPHONE=yes make >> %s 2>&1" % buildLog)
    os.system("OPTIMIZE_SIZE=yes COMPILE_FOR_IPHONE_SIMULATOR=yes make >> %s 2>&1" % buildLog)

    os.chdir(os.path.join(buildDir, "Xcode", "Slice2ObjcPlugin"))

    os.system("xcodebuild -configuration Release >> %s 2>&1")

    print "ok"

print "Creating distributions",

os.chdir(buildDir)
os.system("prefix=%s make install >> %s 2>&1" % (os.path.join(baseDir, "IceTouch-" + version), buildLog))

copy(os.path.join(buildDir, "objc", "SDK", "IceTouch"), os.path.join(baseDir, "IceTouch-" + version + "-SDK"))

copy(os.path.join(buildDir, "Xcode", "Slice2ObjcPlugin", "build", "Release", "slice2objcplugin.pbplugin"),
     os.path.join(baseDir, "slice2objcplugin.pbplugin"))

copy(os.path.join(distDir, "IceTouch-" + version + "-demos.tar.gz"), 
     os.path.join(baseDir, "IceTouch-" + version + "-demos.tar.gz"))

#
# Everything should be clean now, we can create the source distributions archives
# 
print "Archiving..."
sys.stdout.flush()
os.chdir(baseDir)

dirs = [ os.path.join(baseDir, "IceTouch-" + version),
         os.path.join(baseDir, "IceTouch-" + version + "-SDK"),
         os.path.join(baseDir, "slice2objcplugin.pbplugin")
         ]

for d in dirs:
    tarArchive(d, verbose)

#
# Write source distribution report in README file.
#
writeBinDistReport("IceTouch", version, compareToDir, dirs)

#
# Done.
#
print "Cleaning up...",
sys.stdout.flush()
for d in dirs:
    remove(d)
#remove(buildDir)
print "ok"

os.chdir(cwd)
