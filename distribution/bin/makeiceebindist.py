#!/usr/bin/env python
# **********************************************************************
#
# Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

import os, sys, shutil, glob, fnmatch, string, re
from stat import *

#
# NOTE: See lib/DistUtils.py for default third-party locations and 
# languages to be built on each platform.
#

version = "@ver@"
distDir = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
sys.path.append(os.path.join(distDir, "lib"))
import DistUtils
from DistUtils import copy, move

#
# Program usage.
#
def usage():
    print "Usage: " + sys.argv[0] + " [options] tag"
    print
    print "Options:"
    print "-c    Clean previous build"
    print "-n    Don't build any language mapping"
    print "-h    Show this message."
    print "-v    Be verbose."

#
# Instantiate the gobal platform object with the given third-parties
#
thirdParties = [
    "Mcpp", \
]

platform = DistUtils.getPlatform(thirdParties)

#
# Check arguments
#
verbose = 0
forceclean = 0
nobuild = 0
for x in sys.argv[1:]:
    if x == "-h":
        usage()
        sys.exit(0)
    elif x == "-v":
        verbose = 1
    elif x == "-c":
        forceclean = 1
    elif x == "-n":
        nobuild = 1
    elif x.startswith("-"):
        print sys.argv[0] + ": unknown option `" + x + "'"
        print
        usage()
        sys.exit(1)

if verbose:
    quiet = "v"
else:
    quiet = ""

#
# Ensure the script is being run from the dist-@ver@ directory.
#
cwd = os.getcwd()
if not os.path.exists(os.path.join(distDir, "lib", "DistUtils.py")):
    print sys.argv[0] + ": you must run makebindist.py from the dist-" + version + " directory created by makedist.py"
    sys.exit(1)
    
packageName = "IceE-" + version + "-" + platform.pkgPlatform

print "Building Ice-E " + version + " binary distribution (" + packageName + ".tar.gz)"
print "Using the following third party libraries:"
if not platform.checkAndPrintThirdParties():
    print "error: some required third party dependencies were not found"
    sys.exit(1)
    
#
# Ensure that the source archive or directory exists and create the build directory.
#
buildRootDir = os.path.join(cwd, os.path.join("build-" + platform.pkgPlatform + "-" + version))
srcDir = os.path.join(buildRootDir, "IceE-" + version + "-src")
buildDir = os.path.join(buildRootDir, "IceE-" + version)
if forceclean or not os.path.exists(buildDir) or not os.path.exists(srcDir):
    if os.path.exists(buildRootDir):
        print "Removing previous build from " + os.path.join("build-" + platform.pkgPlatform + "-" + version) + "...",
        sys.stdout.flush()
        shutil.rmtree(buildRootDir)
        print "ok"
    os.mkdir(buildRootDir)

    #
    # If we can't find the source archive in the current directory, ask its location
    #
    srcDistDir = cwd
    if not os.path.exists(os.path.join(srcDistDir, "IceE-" + version + ".tar.gz")):
        srcDistDir = os.path.join(cwd, "..")
        if not os.path.exists(os.path.join(srcDistDir, "IceE-" + version + ".tar.gz")):
            print "Couldn't find IceE-" + version + ".tar.gz in current or parent directory"
            sys.exit(1)
        
    print "Unpacking " + os.path.join(srcDistDir, "IceE-" + version + ".tar.gz ..."),
    sys.stdout.flush()
    os.chdir(buildRootDir)
    if os.system("gzip -d -c " + os.path.join(srcDistDir, "IceE-" + version + ".tar.gz") + " | tar x" + quiet + "f -"):
        print sys.argv[0] + ": failed to unpack ./IceE-" + version + ".tar.gz"
        sys.exit(1)

    os.rename("IceE-" + version, srcDir)

    os.chdir(cwd)
    print "ok"

    copy(srcDir, buildDir)

#
# Build and install each language mappings supported by this platform in the build directory.
#
if not nobuild:
    print
    print "============= Building translator ============="
    print
    
    os.chdir(os.path.join(srcDir, "cpp"))

    if os.system(platform.getMake() + " " + platform.getMakeEnvs(version, "cpp")) != 0:
        print sys.argv[0] + ": failed to the build the translator"
        sys.exit(1)

    os.chdir(os.path.join(cwd))

    if str(platform) != "Windows":
        copy(os.path.join(srcDir, "cpp", "bin", "slice2cppe"), os.path.join(buildDir, "cpp", "bin"), False)
    else:
        copy(os.path.join(srcDir, "cpp", "bin", "slice2cppe.exe"), os.path.join(buildDir, "cpp", "bin"), False)
        
    print
    print "============= Finished buiding translator ============="
    print

#
# Everything should be clean now, we can create the binary distribution archive
#
if str(platform) != "Windows":
    print "Archiving " + packageName + ".tar.gz ...",
    sys.stdout.flush()
    os.chdir(buildRootDir)
    tarfile = os.path.join(cwd, packageName) + ".tar.gz"
    os.system("tar c" + quiet + "f - IceE-" + version + " | gzip -9 - > " + tarfile)
    os.chdir(cwd)
    print "ok"
else:
    print "Archiving " + packageName + ".zip ...",
    sys.stdout.flush()
    os.chdir(buildRootDir)
    zipfile = os.path.join(cwd, packageName) + ".zip"
    if verbose:
        os.system("zip -9r " + zipfile + " IceE-" + version)
    else:
        os.system("zip -9rq " + zipfile + " IceE-" + version)
    os.chdir(cwd)
    print "ok"
   
#
# Done.
#
if forceclean:
    print "Cleaning up...",
    sys.stdout.flush()
    shutil.rmtree(buildRootDir)
    print "ok"

os.chdir(cwd)
