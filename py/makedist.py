#!/usr/bin/env python
# **********************************************************************
#
# Copyright (c) 2003-2007 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

import os, sys, shutil, fnmatch, re, glob

#
# Show usage information.
#
def usage():
    print "Usage: " + sys.argv[0] + " [options] [tag]"
    print
    print "Options:"
    print "-h    Show this message."
    print "-v    Be verbose."
    print
    print "If no tag is specified, HEAD is used."

#
# Find files matching a pattern.
#
def find(path, patt):
    result = [ ]
    files = os.listdir(path)
    for x in files:
        fullpath = os.path.join(path, x);
        if fnmatch.fnmatch(x, patt):
            result.append(fullpath)
        if os.path.isdir(fullpath) and not os.path.islink(fullpath):
            result.extend(find(fullpath, patt))
    return result

#
# Fix version in README, INSTALL files
#
def fixVersion(files, version):

    for file in files:
        origfile = file + ".orig"
        os.rename(file, origfile)
        oldFile = open(origfile, "r")
        newFile = open(file, "w")
        newFile.write(re.sub("@ver@", version, oldFile.read()))
        newFile.close()
        oldFile.close()
        os.remove(origfile)

#
# Are we on Windows?
#
win32 = sys.platform.startswith("win") or sys.platform.startswith("cygwin")

#
# Check arguments
#
tag = "HEAD"
verbose = 0
for x in sys.argv[1:]:
    if x == "-h":
        usage()
        sys.exit(0)
    elif x == "-v":
        verbose = 1
    elif x.startswith("-"):
        print sys.argv[0] + ": unknown option `" + x + "'"
        print
        usage()
        sys.exit(1)
    else:
        tag = x

#
# Remove any existing "dist" directory and create a new one.
#
distdir = "dist"
if os.path.exists(distdir):
    shutil.rmtree(distdir)
os.mkdir(distdir)
os.mkdir(os.path.join(distdir, "icepy"))
os.mkdir(os.path.join(distdir, "ice"))

#
# Export Python and C++ sources from git.
#
# NOTE: Assumes that the C++ and Python trees will use the same tag.
#
print "Checking out sources " + tag + "..."
if verbose:
    quiet = "-v"
else:
    quiet = ""
os.system("git archive " + tag + " . | (cd dist/icepy && tar xf -)")
os.chdir(os.path.join("..", "cpp"))
os.system("git archive " + tag + " . | (cd ../py/dist/ice && tar xf -)")
os.chdir(os.path.join("..", "py"))

os.chdir(distdir)

print "Copying Make.rules.* files from ice..."
for x in glob.glob(os.path.join("ice", "config", "Make.rules.*")):
    if not os.path.exists(os.path.join("icepy", "config", os.path.basename(x))):
        shutil.copyfile(x, os.path.join("icepy", "config", os.path.basename(x)))

#
# Remove files.
#
print "Removing unnecessary files..."
filesToRemove = [ \
    os.path.join("icepy", "makedist.py"), \
    os.path.join("icepy", "makebindist.py"), \
    ]
filesToRemove.extend(find("icepy", ".dummy"))
for x in filesToRemove:
    os.remove(x)

#
# Taken from ice/config/TestUtil.py
#
# If having this duplicated is really a problem we should split these
# methods out into their own module.
#
def isHpUx():

   if sys.platform == "hp-ux11":
        return 1
   else:
        return 0

def isDarwin():

   if sys.platform == "darwin":
        return 1
   else:
        return 0

def isAIX():
   if sys.platform in ['aix4', 'aix5']:
        return 1
   else:
        return 0


#
# Get Ice version.
#
config = open(os.path.join("icepy", "config", "Make.rules"), "r")
versionMajor = ""
versionMinor = ""
version = ""
for l in config.readlines():
    if l.startswith("VERSION_MAJOR"):
        n, v = l.split('=')
        versionMajor = v.strip()
    elif l.startswith("VERSION_MINOR"):
        n, v = l.split('=')
        versionMinor = v.strip()
    elif l.startswith("VERSION"):
        n, v = l.split('=')
        version = v.strip()

config.close()

print "Fixing version in README and INSTALL files..."
fixVersion(find("icepy", "README*"), version)
fixVersion(find("icepy", "INSTALL*"), version)

#
# Create source archives.
#
print "Creating distribution archives..."
icever = "IcePy-" + version
os.rename("icepy", icever)
if verbose:
    quiet = "v"
else:
    quiet = ""
os.system("chmod -R u+rw,go+r-w . " + icever)
os.system("find " + icever + " \\( -name \"*.ice\" -or -name \"*.xml\" \\) -exec chmod a-x {} \\;")
os.system("find " + icever + " \\( -name \"README*\" -or -name \"INSTALL*\" \\) -exec chmod a-x {} \\;")
os.system("find " + icever + " -type d -exec chmod a+x {} \\;")
os.system("find " + icever + " -perm +111 -exec chmod a+x {} \\;")
os.system("tar c" + quiet + "f " + icever + ".tar " + icever)
os.system("gzip -9 " + icever + ".tar")
if verbose:
    quiet = ""
else:
    quiet = "-q"
os.system("zip -9 -r " + quiet + " " + icever + ".zip " + icever)

#
# Copy files (README, etc.).
#
shutil.copyfile(os.path.join(icever, "CHANGES"), "IcePy-" + version + "-CHANGES")

#
# Done.
#
print "Cleaning up..."
shutil.rmtree(icever)
shutil.rmtree("ice")
print "Done."
