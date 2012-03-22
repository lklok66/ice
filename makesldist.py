#!/usr/bin/env python

import os, sys, fnmatch, re, getopt

sys.path.append(os.path.join(os.path.dirname(__file__), "distribution", "lib"))
from DistUtils import *

#
# This is an explicit list of some SL specific files to remove. The
# other files are all removed by reversing the below list.
#
filesToRemove = [
    "./cs/config/Make.rules.cs"
    "./cs/config/Make.rules.mak.cs"
]

# List of files & subdirectories to keep, all others are removed.
filesToKeep = [
    "./config/Make.common.rules.mak",
    "./config/IceDevKey.snk",
    "./cs",
    "./vsaddin",
    "./distribution",
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
publickey = "cdd571ade22f2f16"
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
    elif o == "-k":
        publickey = a
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
# Get IceSL version.
#
config = open(os.path.join("cs", "src", "Ice", "AssemblyInfo.cs"), "r")
version = re.search("AssemblyVersion.*\"([0-9\.]*)\".*", config.read()).group(1)

#
# Remove any existing "distsl-" directory and create a new one
# and sub-directories for the each source distribution.
#
distDir = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "distsl-" + tag.replace('/', '-')))
if os.path.exists(distDir):
    remove(distDir)
os.mkdir(distDir)

print "Creating " + version + " source distributions in " + distDir

srcDir = os.path.join(distDir, "IceSL-" + version)

#
# Extract the sources with git archive using the given tag.
#
print "Creating git archive using " + tag + "...",
sys.stdout.flush()
os.system("git archive --prefix=IceSL-" + version + "/ " + tag + " | ( cd \"" + distDir + "\" && tar xfm - )")
print "ok"

os.chdir(os.path.join(srcDir))

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

for root, dirnames, filesnames in os.walk('.'):
    for f in filesnames:
        if f == "Makefile":
            os.remove(os.path.join(root, f))
print "ok"

#
# Change the public key token in web.config files
#
print "Changing the PublicKeyToken in web.config files to " + publickey + "...",
for root, dirnames, filesnames in os.walk('.'):
    for f in filesnames:
        if f == "Web.config" or f == "web.config":
            substitute(os.path.join(root, f), [('PublicKeyToken=1f998c50fec78381', 'PublicKeyToken=%s' % publickey)])
print "ok"

#
# Copy IceSL specific install files.
#
print "Copying icesl install files...",
move(os.path.join("distribution", "src", "sl", "ICESL_LICENSE.txt"), os.path.join("ICESL_LICENSE.txt"))
move(os.path.join("distribution", "src", "sl", "LICENSE.txt"), os.path.join("LICENSE.txt"))
move(os.path.join("distribution", "src", "sl", "README.txt"), os.path.join("README.txt"))
move(os.path.join("distribution", "src", "sl", "INSTALL.txt"), os.path.join("INSTALL.txt"))
move(os.path.join("distribution", "src", "sl", "CHANGES.txt"), os.path.join("CHANGES.txt"))

#
# Move *.icesl to the correct names.
#
move(os.path.join("distribution", "src", "sl", "Makefile.mak"), os.path.join("Makefile.mak"))
move(os.path.join("distribution", "src", "sl", "Make.rules.mak.cs"), os.path.join("cs", "config", "Make.rules.mak.cs"))
remove("distribution")

print "ok"

#
# Everything should be clean now, we can create the source distributions archives
# 
print "Archiving..."
sys.stdout.flush()
os.chdir(distDir)

for d in [srcDir]:
    zipArchive(srcDir, verbose)

#
# Write source distribution report in README file.
#
#writeSrcDistReport("IceSL", version, compareToDir, [srcDir])

#
# Done.
#
print "Cleaning up...",
sys.stdout.flush()
remove(srcDir)
print "ok"

os.chdir(cwd)
