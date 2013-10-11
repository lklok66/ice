#!/usr/bin/env python
# -*- coding: utf-8 -*-
# **********************************************************************
#
# Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

import os, sys, fnmatch, re, getopt, atexit, shutil, subprocess, zipfile, time, stat

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "lib")))


from BuildUtils import *
from DistUtils import *

def runCommand(cmd, verbose):
    if len(cmd) > 0:
        if verbose:
            print(cmd)
        if os.system(cmd) != 0:
            sys.exit(1)


def _handle_error(fn, path, excinfo):  
    print("error removing %s" % path)
    os.chmod(path, stat.S_IWRITE)
    fn(path)

def setMakefileOption(filename, optionName, value):
    optre = re.compile("^\#?\s*?%s\s*?=.*" % optionName)
    if os.path.exists(filename + ".tmp"):
        os.remove(filename + ".tmp")
    new = open(filename + ".tmp", "w")
    old = open(filename, "r")
    for line in old:
        if optre.search(line):
            new.write("%s = %s\n" % (optionName, value))
        else:
            new.write(line)
    old.close()
    new.close()
    shutil.move(filename + ".tmp", filename)

def executeCommand(command, env):
    print(command)
    p = subprocess.Popen(command, shell = True, stdin = subprocess.PIPE, stdout = subprocess.PIPE, \
                         stderr = subprocess.STDOUT, bufsize = 0, env = env)

    if p:
        while(True):
            c = p.stdout.read(1)
            
            if not c:
                if p.poll() is not None:
                    break
                time.sleep(0.1)
                continue

            if type(c) != str:
                c = c.decode()
            
            sys.stdout.write(c)
        
        if p.poll() != 0:
            #
            # Command failed
            #
            print("Command failed exit status %s" % p.poll())
            sys.exit(1)

def relPath(sourceDir, targetDir, f):
    sourceDir = os.path.normpath(sourceDir)
    targetDir = os.path.normpath(targetDir)
    f = os.path.normpath(f)
    if f.find(sourceDir) == 0:
        f =  os.path.join(targetDir, f[len(sourceDir) + 1:])
    return f
#
# Program usage.
#
def usage():
    print("")
    print(r"Options:")
    print("")
    print(r"  --help                      Show this message.")
    print("")
    print(r"  --verbose                   Be verbose.")
    print("")
    print(r"  --skip-build                Skip build and go directly to installer creation,")
    print(r"                              existing build will be used")
    print("")
    print(r"  --skip-installer            Skip the installer creation, just do the build")
    print("")
    print(r"  --filter-languages=<name>   Just build and run the given languages")
    print("")
    print(r"  --filter-compilers=<name>   Just build the given compilers")
    print("")
    print(r"  --filter-archs=<name>       Just build the given architectures")
    print("")
    print(r"  --filter-confs=<name>       Just build the given configurations")
    print("")
    print(r"  --filter-profiles=<name>    Just build the given profiles")
    print("")
    print(r"  --cert-file=<path>          Certificate file used to sign the installer")
    print("")
    print(r"makemsi.py --verbose")
    print("")

version = "0.1.0"
verbose = False

args = None
opts = None

proguardHome = None
phpHome = None
phpBinHome = None
rubyHome = None
rubyDevKitHome = None
skipBuild = False
skipInstaller = False

filterLanguages = []
filterCompilers = []
filterArchs = []
filterConfs = []
filterProfiles = []

rFilterLanguages = []
rFilterCompilers = []
rFilterArchs = []
rFilterConfs = []
rFilterProfiles = []

certFile = None
keyFile = None

try:
    opts, args = getopt.getopt(sys.argv[1:], "", ["help", "verbose", "proguard-home=", "php-home=", "php-bin-home=", \
                                                  "ruby-home=", "ruby-devkit-home=", "skip-build", "skip-installer", \
                                                  "filter-languages=", "filter-compilers=", "filter-archs=", \
                                                  "filter-confs=", "filter-profiles=", "filter-languages=", \
                                                  "filter-compilers=", "filter-archs=", "filter-confs=", \
                                                  "filter-profiles=", "cert-file=", "key-file="])
except getopt.GetoptError as e:
    print("Error %s " % e)
    usage()
    sys.exit(1)

if args:
    usage()
    sys.exit(1)

for o, a in opts:
    if o == "--help":
        usage()
        sys.exit(0)
    elif o == "--verbose":
        verbose = True
    elif o == "--skip-build":
        skipBuild = True
    elif o == "--skip-installer":
        skipInstaller = True
    elif o == "--filter-languages":
        filterLanguages.append(a)
    elif o == "--filter-compilers":
        filterCompilers.append(a)
    elif o == "--filter-archs":
        filterArchs.append(a)
    elif o == "--filter-confs":
        filterConfs.append(a)
    elif o == "--filter-profiles":
        filterProfiles.append(a)
    elif o == "--rfilter-languages":
        rFilterLanguages.append(a)
    elif o == "--rfilter-compilers":
        rFilterCompilers.append(a)
    elif o == "--rfilter-archs":
        rFilterArchs.append(a)
    elif o == "--rfilter-confs":
        rFilterConfs.append(a)
    elif o == "--rfilter-profiles":
        rFilterProfiles.append(a)
    elif o == "--cert-file":
        certFile = a
    elif o == "--key-file":
        keyFile = a

basePath = os.path.abspath(os.path.dirname(__file__))
iceBuildHome = os.path.abspath(os.path.join(basePath, "..", ".."))
sourceArchive = os.path.join(iceBuildHome, "IceWS-%s.zip" % version)
demoArchive = os.path.join(iceBuildHome, "IceWS-%s-demos.zip" % version)

distFiles = os.path.join(iceBuildHome, "distfiles-%s" % version)

iceInstallerFile = os.path.join(distFiles, "src", "windows" , "IceWS.aip")

thirdPartyHome = getThirdpartyHome("3.5.1")
if thirdPartyHome is None:
    print("Cannot detect Ice %s ThirdParty installation" % version)
    sys.exit(1)

if not certFile:
    if os.path.exists("c:\\release\\authenticode\\zeroc2013.pfx"):
        certFile = "c:\\release\\authenticode\\zeroc2013.pfx"
    elif os.path.exists(os.path.join(os.getcwd(), "..", "..", "release", "authenticode", "zeroc2013.pfx")):
        certFile = os.path.join(os.getcwd(), "..", "..", "release", "authenticode", "zeroc2013.pfx")
else:
    if not os.path.isabs(certFile):
        certFile = os.path.abspath(os.path.join(os.getcwd(), certFile))
        
if certFile is None:
    print("You need to specify the sign certificate using --cert-file option")
    sys.exit(1)

if not os.path.exists(certFile):
    print("Certificate `%s' not found")
    sys.exit(1)
    
    
if not keyFile:
    if os.path.exists("c:\\release\\strongname\\IceReleaseKey.snk"):
        keyFile = "c:\\release\\strongname\\IceReleaseKey.snk"
    elif os.path.exists(os.path.join(os.getcwd(), "..", "..", "release", "strongname", "IceReleaseKey.snk")):
        keyFile = os.path.join(os.getcwd(), "..", "..", "release", "strongname", "IceReleaseKey.snk")
else:
    if not os.path.isabs(keyFile):
        keyFile = os.path.abspath(os.path.join(os.getcwd(), keyFile))
        
if keyFile is None:
    print("You need to specify the key file to sign assemblies using --key-file option")
    sys.exit(1)

if not os.path.exists(keyFile):
    print("Key file `%s' not found")
    sys.exit(1)

if not os.path.exists(sourceArchive):
    print("Couldn't find %s in %s" % (os.path.basename(sourceArchive), os.path.dirname(sourceArchive)))
    sys.exit(1)

if not os.path.exists(demoArchive):
    print("Couldn't find %s in %s" % (os.path.basename(demoArchive), os.path.dirname(demoArchive)))
    sys.exit(1)

    
#
# Windows build configurations by Compiler Arch 
#
builds = {
    "VC110": {
        "x86": {
            "release": ["cpp"], 
            "debug": ["cpp"]},
        "amd64": {
            "release": ["cpp"], 
            "debug": ["cpp"]}
        }
    }
            
if not skipBuild:
    
    for compiler in ["VC110"]:
    
        for arch in ["x86", "amd64"]:
            
            if not arch in builds[compiler]:
                continue
            
            if filterArchs and arch not in filterArchs:
                continue

            if rFilterArchs and arch in rFilterArchs:
                continue

            for conf in ["release", "debug"]:
                
                if not conf in builds[compiler][arch]:
                    continue
        
                if filterConfs and conf not in filterConfs:
                    continue

                if rFilterConfs and conf in rFilterConfs:
                    continue

                buildDir = os.path.join(iceBuildHome, "build-%s-%s-%s" % (arch, compiler, conf))

                if not os.path.exists(buildDir):
                    os.makedirs(buildDir)

                os.chdir(buildDir)

                sourceDir = os.path.join(buildDir, "IceWS-%s-src" % version)
                installDir = os.path.join(buildDir, "IceWS-%s" % version)
                if not os.path.exists(sourceDir):
                    sys.stdout.write("extracting %s to %s... " % (os.path.basename(sourceArchive), sourceDir))
                    sys.stdout.flush()
                    zipfile.ZipFile(sourceArchive).extractall()
                    if os.path.exists(sourceDir):
                        shutil.rmtree(sourceDir, onerror = _handle_error)
                    shutil.move(installDir, sourceDir)
                    print("ok")

                print ("Build: (%s/%s/%s)" % (compiler,arch,conf))
                for lang in builds[compiler][arch][conf]:

                    if filterLanguages and lang not in filterLanguages:
                        continue

                    if rFilterLanguages and lang in rFilterLanguages:
                        continue

                    env = os.environ.copy()

                    env["THIRDPARTY_HOME"] = thirdPartyHome
                    env["RELEASEPDBS"] = "yes"
                    if conf == "release":
                        env["OPTIMIZE"] = "yes"

                    os.chdir(os.path.join(sourceDir, lang))

                    command = None
                    rules = "Make.rules.mak"

                    setMakefileOption(os.path.join(sourceDir, lang, "config", rules), "prefix", installDir)

                    vcvars = getVcVarsAll(compiler)

                    if lang == "cpp" and compiler == "VC110":
                        command = "\"%s\" %s  && nmake /f Makefile.mak install" % (vcvars, arch)
                        executeCommand(command, env)
#
# Filter files, list of files that must not be included.
#
filterFiles = []

if not os.path.exists(os.path.join(iceBuildHome, "installer")):
    os.makedirs(os.path.join(iceBuildHome, "installer"))

os.chdir(os.path.join(iceBuildHome, "installer"))

installerDir = os.path.join(iceBuildHome, "installer", "IceWS-%s" % version)
installerdSrcDir = os.path.join(iceBuildHome, "installer", "IceWS-%s-src" % version)
installerDemoDir = os.path.join(iceBuildHome, "installer", "IceWS-%s-demos" % version)

if not os.path.exists(installerdSrcDir):
    sys.stdout.write("extracting %s to %s... " % (os.path.basename(sourceArchive), installerdSrcDir))
    sys.stdout.flush()
    zipfile.ZipFile(sourceArchive).extractall()
    shutil.move(installerDir, installerdSrcDir)
    print("ok")


if not os.path.exists(installerDemoDir):
    sys.stdout.write("extracting %s to %s... " % (os.path.basename(demoArchive), installerDemoDir))
    sys.stdout.flush()
    zipfile.ZipFile(demoArchive).extractall()
    print("ok")


if os.path.exists(installerDir):
    shutil.rmtree(installerDir, onerror = _handle_error)
os.makedirs(installerDir)

for arch in ["x86", "amd64"]:
    for compiler in ["VC110"]:
        for conf in ["release", "debug"]:

            buildDir = os.path.join(iceBuildHome, "build-%s-%s-%s" % (arch, compiler, conf))
            sourceDir = os.path.join(buildDir, "IceWS-%s-src" % version)
            installDir = os.path.join(buildDir, "IceWS-%s" % version)

            if compiler == "VC110":
                for d in ["bin", "lib", "include", "slice"]:
                    for root, dirnames, filenames in os.walk(os.path.join(installDir, d)):
                        for f in filenames:
                            if f in filterFiles:
                                continue
                            targetFile = relPath(installDir, installerDir, os.path.join(root, f))
                            if not os.path.exists(targetFile):
                                copy(os.path.join(root, f), targetFile, verbose = verbose)

#
# docs dir
#
docsDir = os.path.join(distFiles, "src", "windows", "docs", "main")
for f in ["README.txt", "SOURCES.txt", "THIRD_PARTY_LICENSE.txt"]:
    copy(os.path.join(docsDir, f), os.path.join(installerDir, f), verbose = verbose)

#
# Copy VC110 thirdpary files
#
files = ["bzip2.dll", "bzip2d.dll", "ssleay32.dll", "ssleay32.pdb", "libeay32.dll", "libeay32.pdb", "openssl.exe"]

for root, dirnames, filenames in os.walk(os.path.join(thirdPartyHome, "bin", "vc110")):
    for f in filenames:
        if f not in files:
            continue
        targetFile = relPath(thirdPartyHome, installerDir, os.path.join(root, f))
        targetFile = targetFile.replace("vc110", "")
        if not os.path.exists(targetFile):
            copy(os.path.join(root, f), targetFile, verbose = verbose)



if not skipInstaller:
    #
    # Build installers with Advanced installer.
    #

    #
    # XML with path variables definitions
    #
    pathVariables = """<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<PathVariables Application="Advanced Installer" Version="10.3">
  <Var Name="ICE_BUILD_HOME" Path="%ICE_BUILD_HOME%" Type="2" ContentType="2"/>
</PathVariables>"""

    advancedInstallerHome = getAdvancedInstallerHome()
    if advancedInstallerHome is None:
        print("Advanced Installer installation not found")
        sys.exit(1)

    advancedInstaller = os.path.join(advancedInstallerHome, "bin", "x86", "AdvancedInstaller.com")

    if not os.path.exists(advancedInstaller):
        print("Advanced Installer executable not found in %s" % advancedInstaller)
        sys.exit(1)

    env = os.environ.copy()    
    env["ICE_BUILD_HOME"] = iceBuildHome

    paths = os.path.join(iceBuildHome, "installer", "paths.xml")
    f = open(os.path.join(iceBuildHome, "installer", "paths.xml"), "w")
    f.write(pathVariables)
    f.close()
    
    tmpCertFile = os.path.join(os.path.dirname(iceInstallerFile), os.path.basename(certFile))
    copy(certFile, tmpCertFile)

    #
    # Load path vars
    #
    command = "\"%s\" /loadpathvars %s" % (advancedInstaller, paths)
    executeCommand(command, env)

    #
    # Build the Ice main installer.
    #    
    command = "\"%s\" /rebuild %s" % (advancedInstaller, iceInstallerFile)
    executeCommand(command, env)

    shutil.move(os.path.join(os.path.dirname(iceInstallerFile), "IceWS.msi"), \
                             os.path.join(iceBuildHome, "IceWS-%s.msi" % version))

    remove(tmpCertFile)
