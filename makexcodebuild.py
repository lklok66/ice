#!/usr/bin/env python
# **********************************************************************
#
# Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

import os, sys, fnmatch, re, getopt, stat

rootDir = os.path.abspath(os.path.dirname(__file__))

sys.path.append(os.path.join(rootDir, "distribution", "lib"))
from DistUtils import *

def writeBinDistReport(product, version):

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

#
# Check arguments
#
verbose = 0
tag = "HEAD"

try:
    opts, args = getopt.getopt(sys.argv[1:], "hv")
except getopt.GetoptError:
    usage()
    sys.exit(1)

for o, a in opts:
    if o == "-h":
        usage()
        sys.exit(0)
    elif o == "-v":
        verbose = 1

if len(args) > 1:
    usage()
    sys.exit(1)

if len(args) == 1:
    tag = args[0]

cwd = os.getcwd()
os.chdir(rootDir)

#
# Get IceTouch version.
#
config = open(os.path.join("cpp", "config", "Make.rules.objc"), "r")
version = re.search("VERSION[\s]*= ([0-9\.]*)", config.read()).group(1)
mmversion = re.search("([0-9]+\.[0-9b]+)[\.0-9]*", version).group(1)
versionMinor = re.search("([0-9\.]*).([0-9\.]*)", version).group(2)
versionMajor = re.search("([0-9\.]*).([0-9\.]*)", version).group(1)

volname = "Ice Xcode Plugin " + mmversion
# First check to see whether the disk image is already accidently mounted.
if not os.system("mount | grep \"%s (\" 2>&1 > /dev/null" % volname):
    print "\"/Volumes/Ice Xcode %s\": already mounted. Unmount and restart." % mmversion
    sys.exit(1)

#
# Get the distDir
#
distDir = os.path.abspath(os.path.join(rootDir, "..", "disticetouch-" + tag.replace('/', '-')))
baseDir = os.path.abspath(os.path.join(rootDir, "..", "buildxcode-" + tag.replace('/', '-')))
if not os.path.exists(baseDir):
    os.mkdir(baseDir)

# Create a symlink for use with Iceberg.
latestBuildDir = os.path.abspath(os.path.join(rootDir, "..", "buildxcode-LATEST"))
try:
    os.remove(latestBuildDir)
except OSError:
    pass
os.system("ln -s %s %s" % (baseDir, latestBuildDir))

buildDir = os.path.join(baseDir, "build")

print "Building " + version + " distributions in " + baseDir

os.chdir(os.path.join(baseDir))

if not os.path.exists(buildDir):
    print "Extracting archive...",
    sys.stdout.flush()
    baseName = os.path.join(distDir, "IceTouch-" + version)
    os.system("tar xfz " + os.path.join(distDir, baseName + ".tar.gz"))
    os.rename("IceTouch-" + version, "build")
    # Change SUBDIRS in top-level Makefile.
    substitute(os.path.join("build", "objc", "Makefile"), [(r'^SUBDIRS([\s]*)=.*', r'SUBDIRS\1= src include')])
    print "ok"
else:
    print "Using existing build directory"
    s1 = os.stat(buildDir)
    s2 = os.stat(os.path.join(distDir, "IceTouch-" + version + ".tar.gz"))
    if s2.st_mtime > s1.st_mtime:
        print "WARNING: distribution is more recent than the existing build"

os.chdir(buildDir)

print "Building distributions...",
sys.stdout.flush()

os.chdir(os.path.join(buildDir, "Xcode", "Slice2ObjcPlugin"))

os.system("xcodebuild -configuration Release >> %s 2>&1")

print "ok"

installerDir = os.path.join(baseDir, "installer")

for d in ["installer", "sdk", "examples", "doc", "opt", "slice2objcplugin.pbplugin"]:
    d = os.path.join(baseDir, d)
    if os.path.exists(d):
        remove(d)

print "Preparing installer...",
sys.stdout.flush()

if not os.path.exists(installerDir):
    os.mkdir(installerDir)
copy(os.path.join(rootDir, "distribution", "src", "mac", "Xcode", "installer-readme.rtf"),
     os.path.join(installerDir, "readme.rtf"))
copy(os.path.join(rootDir, "distribution", "src", "mac", "Xcode", "uninstall.sh"),
     os.path.join(installerDir, "uninstall.sh"))

copy(os.path.join(buildDir, "Xcode", "Slice2ObjcPlugin", "build", "Release", "slice2objcplugin.pbplugin"),
     os.path.join(baseDir, "slice2objcplugin.pbplugin"))

print "ok"

print "Creating installer...",
sys.stdout.flush()

os.chdir(baseDir)
freezeProj = os.path.join(rootDir, "distribution", "src", "mac", "Xcode", "IceXcodePlugin-1.0.packproj")
os.system("freeze %s" % freezeProj)

print "ok"

os.chdir(baseDir)

print "Building disk image...",
sys.stdout.flush()

if os.path.exists("scratch.dmg.sparseimage"):
    os.remove("scratch.dmg.sparseimage")
os.system("hdiutil create -quiet scratch.dmg -volname \"%s\" -type SPARSE -fs HFS+" % volname)
os.system("hdid -quiet scratch.dmg.sparseimage")
os.system("ditto -rsrc installer \"/Volumes/%s\"" % volname)
os.system("hdiutil detach -quiet \"/Volumes/%s\"" % volname)
resultImg = 'IceXcode-Plugin-%s.dmg' % mmversion
if os.path.exists(resultImg):
    os.remove(resultImg);
os.system("hdiutil convert -quiet  scratch.dmg.sparseimage -format UDZO -o %s -imagekey zlib-devel=9" % resultImg)
os.remove("scratch.dmg.sparseimage")
print "ok"

#
# Write source distribution report in README file.
#
writeBinDistReport("Ice Xcode Plugin", version)

os.chdir(cwd)
