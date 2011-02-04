# **********************************************************************
#
# Copyright (c) 2003-2010 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

import sys, os, re, getopt, time, StringIO, string, threading, atexit

# Global flags and their default values.
protocol = ""                   # If unset, default to TCP. Valid values are "tcp" or "ssl".
compress = False                # Set to True to enable bzip2 compression.
serialize = False               # Set to True to have tests use connection serialization
host = "127.0.0.1"              # Default to loopback.
debug = False                   # Set to True to enable test suite debugging.
mono = False                    # Set to True when not on Windows
keepGoing = False               # Set to True to have the tests continue on failure.
ipv6 = False                    # Default to use IPv4 only
iceHome = None                  # Binary distribution to use (None to use binaries from source distribution)
x64 = False                     # Binary distribution is 64-bit
javaCmd = "java"                # Default java loader
valgrind = False                # Set to True to use valgrind for C++ executables.
appverifier = False             # Set to True to use appverifier for C++ executables, This is windows only feature
tracefile = None
printenv = False
cross = []
watchDog = None
sqlType = None
sqlDbName = None
sqlHost = None
sqlPort = None
sqlUser = None
sqlPassword = None
serviceDir = None

def isCygwin():
    # The substring on sys.platform is required because some cygwin
    # versions return variations like "cygwin_nt-4.01".
    return sys.platform[:6] == "cygwin"

def isWin32():
    return sys.platform == "win32" or isCygwin()

def isVista():
    return isWin32() and sys.getwindowsversion()[0] == 6

def isWin9x():
    if isWin32():
        return not (os.environ.has_key("OS") and os.environ["OS"] == "Windows_NT")
    else:
        return 0

def isSolaris():
    return sys.platform == "sunos5"
       
def isSparc():
    p = os.popen("uname -m")
    l = p.readline().strip()
    p.close()
    if l == "sun4u":
        return True
    else:
        return False

def isAIX():
    return sys.platform in ['aix4', 'aix5']
  
def isDarwin():
    return sys.platform == "darwin"

def isLinux():
    return sys.platform.startswith("linux")

def getCppCompiler():
    compiler = ""
    if os.environ.get("CPP_COMPILER", "") != "":
        compiler = os.environ["CPP_COMPILER"]
    else:
        config = open(os.path.join(toplevel, "cpp", "config", "Make.rules.mak"), "r")
        compiler = re.search("CPP_COMPILER[\t\s]*= ([A-Z0-9]*)", config.read()).group(1)
    return compiler


def isBCC2010():
    if not isWin32():
        return False
    return getCppCompiler() == "BCC2010"

def isVC6():
    if not isWin32():
        return False
    return getCppCompiler() == "VC60"

def isVS2010():
    if not isWin32():
        return False
    compiler = getCppCompiler()
    return compiler == "VC100" or compiler == "VC100_EXPRESS"

#
# The PHP interpreter is called "php5" on some platforms (e.g., SLES).
#
phpCmd = "php"
for path in string.split(os.environ["PATH"], os.pathsep):
    #
    # Stop if we find "php" in the PATH first.
    #
    if os.path.exists(os.path.join(path, "php")):
        break
    elif os.path.exists(os.path.join(path, "php5")):
        phpCmd = "php5"
        break

#
# This is set by the choice of init method. If not set, before it is
# used, it indicates a bug and things should terminate.
#
defaultMapping = None

testErrors = []

toplevel = None 

path = [ ".", "..", "../..", "../../..", "../../../..", "../../../../.." ]
head = os.path.dirname(sys.argv[0])
if len(head) > 0:
    path = [os.path.join(head, p) for p in path]
path = [os.path.abspath(p) for p in path if os.path.exists(os.path.join(p, "scripts", "TestUtil.py")) ]
if len(path) == 0:
    raise "can't find toplevel directory!"
toplevel = path[0]

def sanitize(cp):
    np = ""
    for p in cp.split(os.pathsep):
        if os.path.normpath(p) == "classes":
            continue
        if len(np) > 0:
            np = np + os.pathsep
        np = np + p
    return np

def quoteArgument(arg):
    if arg is None:
        return None
    return '"%s"' % arg

def dumpenv(env, lang):
    if env is None:
        env = os.environ
    vars = ["PATH", "LD_LIBRARY_PATH", "DYLD_LIBRARY_PATH", "SHLIB_PATH", "LIBPATH", "LD_LIBRARY_PATH_64"]
    if lang is None:
        vars.extend(["CLASSPATH", "MONO_PATH", "DEVPATH", "PYTHONPATH", "RUBYLIB"])
    elif lang == "cpp":
        pass
    elif lang == "java":
        vars.append("CLASSPATH")
    elif lang == "cs":
        vars.extend(["MONO_PATH", "DEVPATH"])
    elif lang == "py":
        vars.append("PYTHONPATH")
    elif lang ==  "rb":
        vars.append("RUBYLIB")
    for i in vars:
        if i in env:
            print "%s=%s" % (i, env[i])

def configurePaths():
    if iceHome:
        print "*** using Ice installation from " + iceHome,
        if x64:
            print "(64bit)",
        print

    # First sanitize the environment.
    os.environ["CLASSPATH"] = sanitize(os.getenv("CLASSPATH", ""))

    #
    # If Ice is installed from RPMs, just set the CLASSPATH for Java.
    #
    if iceHome == "/usr":
        javaDir = os.path.join("/", "usr", "share", "java")
        addClasspath(os.path.join(javaDir, "Ice.jar"))
        addClasspath(os.path.join(javaDir, "Freeze.jar"))
        return # That's it, we're done!
    
    if isWin32():
        libDir = getCppBinDir()
        if iceHome and isBCC2010():
            addLdPath(libDir)
            libDir = os.path.join(libDir, "bcc10")
    else:
        libDir = os.path.join(getIceDir("cpp"), "lib")
        if iceHome and x64: 
	    if isSolaris():
		if isSparc():
		    libDir = os.path.join(libDir, "sparcv9")
		else:
		    libDir = os.path.join(libDir, "amd64")
	    else:
		libDir = libDir + "64"
    addLdPath(libDir)

    if getDefaultMapping() == "javae":
        javaDir = os.path.join(getIceDir("javae"), "jdk", "lib")
	addClasspath(os.path.join(javaDir, "IceE.jar"))
        os.environ["CLASSPATH"] = os.path.join(javaDir, "IceE.jar") + os.pathsep + os.getenv("CLASSPATH", "")
    else:
        # The Ice.jar and Freeze.jar comes from the installation
        # directory or the toplevel dir.
        javaDir = os.path.join(getIceDir("java"), "lib")
	addClasspath(os.path.join(javaDir, "Ice.jar"))
	addClasspath(os.path.join(javaDir, "Freeze.jar"))
    addClasspath(os.path.join(javaDir))
    
    # 
    # On Windows, C# assemblies are found thanks to the .exe.config files.
    #
    if isWin32():
        addPathToEnv("DEVPATH", os.path.join(getIceDir("cs"), "bin"))
    else:
        addPathToEnv("MONO_PATH", os.path.join(getIceDir("cs"), "bin"))
        
    #
    # On Windows x64, set PYTHONPATH to python/x64.
    #
    pythonDir = os.path.join(getIceDir("py"), "python")
    if isWin32() and x64:
        addPathToEnv("PYTHONPATH", os.path.join(pythonDir, "x64"))
    else:
        addPathToEnv("PYTHONPATH", pythonDir)

    addPathToEnv("RUBYLIB", os.path.join(getIceDir("rb"), "ruby"))

def addLdPath(libpath, env = None):
    if env is None:
        env = os.environ
    if isWin32():
        addPathToEnv("PATH", libpath, env)
    elif isDarwin():
        addPathToEnv("DYLD_LIBRARY_PATH", libpath, env)
    elif isAIX():
        addPathToEnv("LIBPATH", libpath, env)
    else:
        addPathToEnv("LD_LIBRARY_PATH", libpath, env)
        addPathToEnv("LD_LIBRARY_PATH_64", libpath, env)
    return env

def addClasspath(path, env = None):
    return addPathToEnv("CLASSPATH", path, env)

def addPathToEnv(variable, path, env = None):
    if env is None:
        env = os.environ
    if not env.has_key(variable):
        env[variable] = path
    else:
        env[variable] = path + os.pathsep + env.get(variable)
    return env

# List of supported cross languages test.
crossTests = [ "Ice/adapterDeactivation",
               #"Ice/background",
               "Ice/binding",
               "Ice/checksum",
               #"Ice/custom",
               "Ice/exceptions",
               "Ice/facets",
               "Ice/hold",
               "Ice/inheritance",
               "Ice/location",
               "Ice/objects",
               "Ice/operations",
               "Ice/proxy",
               "Ice/retry",
               #"Ice/servantLocator",
               "Ice/timeout",
               "Ice/slicing/exceptions",
               "Ice/slicing/objects",
               ]
    
def run(tests, root = False):
    def usage():
        print "usage: " + sys.argv[0] + """
          --all                   Run all sensible permutations of the tests.
          --all-cross             Run all sensible permutations of cross language tests.
          --start=index           Start running the tests at the given index.
          --loop                  Run the tests in a loop.
          --filter=<regex>        Run all the tests that match the given regex.
          --rfilter=<regex>       Run all the tests that do not match the given regex.
          --debug                 Display debugging information on each test.
          --protocol=tcp|ssl      Run with the given protocol.
          --compress              Run the tests with protocol compression.
          --host=host             Set --Ice.Default.Host=<host>.
          --valgrind              Run the test with valgrind.
          --appverifier           Run the test with appverifier under Windows.
          --serialize             Run with connection serialization.
          --continue              Keep running when a test fails
          --ipv6                  Use IPv6 addresses.
          --no-ipv6               Don't use IPv6 addresses.
          --ice-home=<path>       Use the binary distribution from the given path.
          --x64                   Binary distribution is 64-bit.
          --cross=lang            Run cross language test.
          --script                Generate a script to run the tests.
          --env                   Print important environment variables.
          --sql-type=<driver>     Run IceStorm/IceGrid tests using QtSql with specified driver.
          --sql-db=<db>           Set SQL database name.
          --sql-host=<host>       Set SQL host name.
          --sql-port=<port>       Set SQL server port.
          --sql-user=<user>       Set SQL user name.
          --sql-passwd=<passwd>   Set SQL password.
          --service-dir=<dir>     Where to locate services for builds without service support.
        """
        sys.exit(2)

    try:
        opts, args = getopt.getopt(sys.argv[1:], "lr:R:",
                                   ["start=", "start-after=", "filter=", "rfilter=", "all", "all-cross", "loop",
                                    "debug", "protocol=", "compress", "valgrind", "host=", "serialize", "continue",
                                    "ipv6", "no-ipv6", "ice-home=", "cross=", "x64", "script", "env", "sql-type=",
                                    "sql-db=", "sql-host=", "sql-port=", "sql-user=", "sql-passwd=", "service-dir=",
                                    "appverifier"])
    except getopt.GetoptError:
        usage()

    if args:
        usage()

    start = 0
    loop = False
    all = False
    allCross = False
    arg = ""
    script = False
    noipv6 = False

    filters = []
    for o, a in opts:
        if o == "--continue":
            global keepGoing
            keepGoing = True
        elif o in ("-l", "--loop"):
            loop = True
        elif o in ("-r", "-R", "--filter", '--rfilter'):
            testFilter = re.compile(a)
            if o in ("--rfilter", "-R"):
                filters.append((testFilter, True))
            else:
                filters.append((testFilter, False))
        elif o == "--cross":
            global cross
            if not a in ["cpp", "java", "cs", "py", "rb" ]:
                print "cross must be one of cpp, java, cs, py or rb"
                sys.exit(1)
            cross.append(a)
        elif o == "--all" :
            all = True
        elif o == "--no-ipv6" :
            noipv6 = True
        elif o == "--all-cross" :
            allCross = True
        elif o in '--start':
            start = int(a)
        elif o == "--script":
            script = True
        elif o == "--protocol":
            if a not in ( "ssl", "tcp"):
                usage()
            if mono and getDefaultMapping() == "cs" and a == "ssl":
                print "SSL is not supported with mono"
                sys.exit(1)

        if o in ( "--cross", "--protocol", "--host", "--debug", "--compress", "--valgrind", "--serialize", "--ipv6", \
                  "--ice-home", "--x64", "--env", "--sql-type", "--sql-db", "--sql-host", "--sql-port", "--sql-user", \
                  "--sql-passwd", "--service-dir", "--appverifier"):
            arg += " " + o
            if len(a) > 0:
                arg += " " + a

    if not root:
        tests = [ (os.path.join(getDefaultMapping(), "test", x), y) for x, y in tests ]

    # Expand all the tests and argument combinations.
    expanded = []
    if all:
        expanded.append([(test, arg, config) for test,config in tests if "once" in config ])

        a = '--protocol=tcp %s'  % arg
        expanded.append([ (test, a, config) for test,config in tests if "core" in config])

        a = '--protocol=ssl %s'  % arg
        expanded.append([ (test, a, config) for test,config in tests if "core" in config])

        a = '--protocol=tcp --compress %s'  % arg
        expanded.append([ (test, a, config) for test,config in tests if "core" in config])

        if not noipv6:
            a = "--ipv6 --protocol=tcp %s" % arg
            expanded.append([ (test, a, config) for test,config in tests if "core" in config])

            a = "--ipv6 --protocol=ssl %s" % arg
            expanded.append([ (test, a, config) for test,config in tests if "core" in config])

        a = "--protocol=tcp %s" % arg
        expanded.append([ (test, a, config) for test,config in tests if "service" in config])

        if not noipv6:
            a = "--protocol=ssl --ipv6 %s" % arg
            expanded.append([ (test, a, config) for test,config in tests if "service" in config])

        a = "--protocol=tcp --serialize %s" % arg
        expanded.append([ (test, a, config) for test,config in tests if "stress" in config])
    elif not allCross:
        expanded.append([ (test, arg, config) for test,config in tests])

    if allCross:
        if len(cross) == 0:
            cross = ["cpp", "java", "cs" ]
	if root:
	    allLang = ["cpp", "java", "cs" ]
	else:
	    allLang = [ getDefaultMapping() ]
        for lang in allLang:
            # This is all other languages than the current mapping.
            crossLang = [ l for l in cross if lang != l ]
            # This is all eligible cross tests for the current mapping.
            # Now expand out the tests. We run only tcp for most cross tests.
            for c in crossLang:
                a = "--cross=%s --protocol=tcp" % c
                expanded.append([ ( "%s/test/%s" % (lang, test), a, []) for test in crossTests if not (test == "Ice/background" and (lang == "cs" or c == "cs"))])
                
                # Add ssl & compress for the operations test.
                if mono and c == "cs": # Don't add the ssl tests for mono.
                    continue
                a = "--cross=%s --protocol=ssl --compress" % c
                expanded.append([("%s/test/Ice/operations" % lang, a, [])])

    # Apply filters after expanding.
    if len(filters) > 0:
        for testFilter, removeFilter in filters:
            nexpanded = []
            for tests in expanded:
                if removeFilter:
                    nexpanded.append([ (x, a, y) for x,a,y in tests if not testFilter.search(x) ])
                else:
                    nexpanded.append([ (x, a, y) for x,a,y in tests if testFilter.search(x) ])
            expanded = nexpanded

    if loop:
        num = 1
        while 1:
            runTests(start, expanded, num, script = script)
            num += 1
    else:
        runTests(start, expanded, script = script)

    global testErrors
    if len(testErrors) > 0:
        print "The following errors occurred:"
        for x in testErrors:
            print x

if not isWin32():
    mono = True

def getIceDir(subdir = None):
    #
    # If ICE_HOME is set we're running the test against a binary distribution. Otherwise,
    # we're running the test against a source distribution.
    # 
    global iceHome
    if iceHome:
        return iceHome
    elif subdir:
        return os.path.join(toplevel, subdir)
    else: 
        return toplevel

def phpCleanup():
    if os.path.exists("tmp.ini"):
        os.remove("tmp.ini")

def phpSetup():
    extDir = None
    ext = None
    incDir = None

    #
    # TODO
    #
    # When we no longer support PHP 5.1.x, we can use the following PHP
    # command-line options:
    #
    # -d extension_dir=...
    # -d extension=[php_ice.dll|IcePHP.so]
    #
    if isWin32():
        ext = "php_ice.dll"
        if not iceHome:
            extDir = os.path.abspath(os.path.join(getIceDir("php"), "lib"))
            incDir = extDir
        else:
            extDir = os.path.join(iceHome, "bin")
            incDir = os.path.join(iceHome, "php")
    else:
        ext = "IcePHP.so"
        if not iceHome:
            extDir = os.path.abspath(os.path.join(toplevel, "php", "lib"))
            incDir = extDir
        else:
            #
            # If ICE_HOME points to the installation directory of a source build, the
            # PHP extension will be located in $ICE_HOME/php. For an RPM installation,
            # PHP is already configured to load the extension. (We could also execute
            # "php -m" and check if the output includes "ice".)
            #
            extDir = os.path.join(iceHome, "php")
            incDir = extDir

            if not os.path.exists(os.path.join(extDir, ext)):
                if iceHome == "/usr":
                    extDir = None # Assume PHP is already configured to load the extension.
                    #
                    # On SLES11, the PHP run time files are installed in /usr/share/php5, but
                    # this directory is not in the default include path when you specify an
                    # alternate configuration file, so we have to configure it explicitly.
                    #
                    if os.path.exists("/usr/share/php5/Ice.php"):
                        incDir = "/usr/share/php5"
                    else:
                        incDir = None
                else:
                    print "unable to find IcePHP extension!"
                    sys.exit(1)

    atexit.register(phpCleanup)
    tmpini = open("tmp.ini", "w")
    tmpini.write("; Automatically generated by Ice test driver.\n")
    if extDir:
        tmpini.write("extension_dir=\"%s\"\n" % extDir)
        tmpini.write("extension=%s\n" % ext)
    if incDir:
        tmpini.write("include_path=\"%s\"\n" % incDir)
    tmpini.close()

def getIceVersion():
    config = open(os.path.join(toplevel, "config", "Make.common.rules"), "r")
    return re.search("VERSION[\t\s]*= ([0-9]+\.[0-9]+(\.[0-9]+|b[0-9]*))", config.read()).group(1)

def getIceSoVersion():
    config = open(os.path.join(toplevel, "cpp", "include", "IceUtil", "Config.h"), "r")
    intVersion = int(re.search("ICE_INT_VERSION ([0-9]*)", config.read()).group(1))
    majorVersion = intVersion / 10000
    minorVersion = intVersion / 100 - 100 * majorVersion    
    patchVersion = intVersion % 100
    if patchVersion > 50:
        if patchVersion >= 52:
            return '%db%d' % (majorVersion * 10 + minorVersion, patchVersion - 50)
        else:
            return '%db' % (majorVersion * 10 + minorVersion)
    else:
        return '%d' % (majorVersion * 10 + minorVersion)

def getIceSSLVersion():
    javaPipeIn, javaPipeOut = os.popen4("java IceSSL.Util")
    if not javaPipeIn or not javaPipeOut:
        print "unable to get IceSSL version!"
        sys.exit(1)
    version = javaPipeOut.readline()
    if not version:
        print "unable to get IceSSL version!"
        sys.exit(1)
    javaPipeIn.close()
    javaPipeOut.close()
    return version.strip()

def getJdkVersion():
    javaPipeIn, javaPipeOut = os.popen4("java -version")
    if not javaPipeIn or not javaPipeOut:
        print "unable to get Java version!"
        sys.exit(1)
    version = javaPipeOut.readline()
    if not version:
        print "unable to get Java version!"
        sys.exit(1)
    javaPipeIn.close()
    javaPipeOut.close()
    return version

def getIceBox():
    #
    # Get and return the path of the IceBox executable
    #
    lang = getDefaultMapping()
    if lang == "cpp":
        iceBox = ""
        if isBCC2010():
            iceBox = os.path.join(getServiceDir(), "icebox.exe")
        elif isWin32():
            #
            # Read the build.txt file from the test directory to figure out 
            # how the IceBox service was built ("debug" vs. "release") and 
            # decide which icebox executable to use.
            # 
            build = open(os.path.join(os.getcwd(), "build.txt"), "r")
            type = build.read().strip()
            if type == "debug":
                iceBox = os.path.join(getCppBinDir(), "iceboxd.exe")
            elif type == "release":
                iceBox = os.path.join(getCppBinDir(), "icebox.exe")
        else:
            iceBox = os.path.join(getCppBinDir(), "icebox")

        if not os.path.exists(iceBox):
            print "couldn't find icebox executable to run the test"
            sys.exit(0)
    elif lang == "java":
        iceBox = "IceBox.Server"
    elif lang == "cs":
        iceBox = os.path.join(getIceDir("cs"), "bin", "iceboxnet")
                
    if iceBox == "":
        print "couldn't find icebox executable to run the test"
        sys.exit(0)
    
    return iceBox

def getIceBoxAdmin():
    return getIceExe("iceboxadmin")

def getIceGridAdmin():
    return getIceExe("icegridadmin")

def getIceStormAdmin():
    return getIceExe("icestormadmin")

def getIceGridNode():
    return getIceExe("icegridnode")

def getIceGridRegistry():
    return getIceExe("icegridregistry")

def getGlacier2Router():
    return getIceExe("glacier2router")

def getIceExe(name):
    if isBCC2010() or isVC6():
        return os.path.join(getServiceDir(), name)
    else:
        return os.path.join(getCppBinDir(), name)

class InvalidSelectorString(Exception):
    def __init__(self, value):
        self.value = value
    def __str__(self):
        return repr(self.value)

sslConfigTree = { 
        "cpp" : { 
            "plugin" : " --Ice.Plugin.IceSSL=IceSSL:createIceSSL --Ice.Default.Protocol=ssl " +
            "--IceSSL.DefaultDir=%(certsdir)s --IceSSL.CertAuthFile=cacert.pem",
            "client" : " --IceSSL.CertFile=c_rsa1024_pub.pem --IceSSL.KeyFile=c_rsa1024_priv.pem",
            "server" : " --IceSSL.CertFile=s_rsa1024_pub.pem --IceSSL.KeyFile=s_rsa1024_priv.pem",
            "colloc" : " --IceSSL.CertFile=c_rsa1024_pub.pem --IceSSL.KeyFile=c_rsa1024_priv.pem"
            },
        "java" : { 
            "plugin" : " --Ice.Plugin.IceSSL=IceSSL.PluginFactory --Ice.Default.Protocol=ssl " +
            "--IceSSL.DefaultDir=%(certsdir)s --IceSSL.Password=password",
            "client" : " --IceSSL.Keystore=client.jks",
            "server" : " --IceSSL.Keystore=server.jks",
            "colloc" : " --IceSSL.Keystore=client.jks"
            },
        "cs" : {
            "plugin" : " --Ice.Plugin.IceSSL=IceSSL:IceSSL.PluginFactory --Ice.Default.Protocol=ssl" +
            " --IceSSL.Password=password --IceSSL.DefaultDir=%(certsdir)s",
            "client" : " --IceSSL.CertFile=c_rsa1024.pfx --IceSSL.CheckCertName=0",
            "server" : " --IceSSL.CertFile=s_rsa1024.pfx --IceSSL.ImportCert.CurrentUser.Root=cacert.pem",
            "colloc" : " --IceSSL.CertFile=c_rsa1024.pfx --IceSSL.ImportCert.CurrentUser.Root=cacert.pem --IceSSL.CheckCertName=0"
            },
        }
sslConfigTree["py"] = sslConfigTree["cpp"]
sslConfigTree["rb"] = sslConfigTree["cpp"]
sslConfigTree["php"] = sslConfigTree["cpp"]

def getDefaultMapping():
    """Try and guess the language mapping out of the current path"""
    here = os.getcwd().split(os.sep)
    here.reverse()
    for i in range(0, len(here)):
        if here[i] in ["cpp", "cs", "java", "php", "py", "rb", "cppe", "javae", "tmp"]:
            return here[i]
    raise "cannot determine mapping"

class DriverConfig:
    lang = None
    protocol = None 
    compress = 0
    serialize = 0
    host = None 
    mono = False
    valgrind = False
    appverifier = False
    type = None
    overrides = None
    ipv6 = False
    x64 = False
    sqlType = None 
    sqlDbName = None
    sqlHost = None
    sqlPort = None
    sqlUser = None
    sqlPassword = None
    serviceDir = None

    def __init__(self, type = None):
        global protocol
        global compress
        global serialize
        global host 
        global mono
        global valgrind
        global appverifier
        global ipv6
        global x64
        global sqlType
        global sqlDbName
        global sqlHost
        global sqlPort
        global sqlUser
        global sqlPassword
        global serviceDir
        self.lang = getDefaultMapping()
        self.protocol = protocol
        self.compress = compress
        self.serialize = serialize
        self.host = host
        self.mono = mono
        self.valgrind = valgrind
        self.appverifier = appverifier
        self.type = type
        self.ipv6 = ipv6
        self.x64 = x64
        self.sqlType = sqlType 
        self.sqlDbName = sqlDbName
        self.sqlHost = sqlHost
        self.sqlPort = sqlPort
        self.sqlUser = sqlUser
        self.sqlPassword = sqlPassword
        self.serviceDir = serviceDir

def argsToDict(argumentString, results):
    """Converts an argument string to dictionary"""
    args = argumentString.strip()
    while len(args) > 0:
        end = args.find(" --")
        if end == -1:
            current = args.strip()
            args = ""
        else:
            current = args[:end].strip()
            args = args[end:].strip()

        value = current.find("=")
        if value != -1:
            results[current[:value]] = current[value+1:]
        else:
            results[current] = None
    return results

def getCommandLineProperties(exe, config):
            
    #
    # Command lines are built up from the items in the components
    # sequence, which is initialized with command line options common to
    # all test drivers.
    #
    components = ["--Ice.NullHandleAbort=1"]
    if getDefaultMapping() != "javae":
        components += ["--Ice.Warn.Connections=1"]

    #
    # Turn on network tracing.
    #
    # components.append("--Ice.Trace.Network=3")

    #
    # Now we add additional components dependent on the desired
    # configuration.
    #
    if config.protocol == "ssl":
        sslenv = {}
        sslenv["certsdir"] = quoteArgument(os.path.abspath(os.path.join(toplevel, "certs")))
        components.append(sslConfigTree[config.lang]["plugin"] % sslenv)
        components.append(sslConfigTree[config.lang][config.type] % sslenv)

    if config.compress:
        components.append("--Ice.Override.Compress=1")

    if config.serialize:
        components.append("--Ice.ThreadPool.Server.Serialize=1")
        
    if config.type == "server" or config.type == "colloc" and config.lang == "py":
        components.append("--Ice.ThreadPool.Server.Size=1 --Ice.ThreadPool.Server.SizeMax=3 --Ice.ThreadPool.Server.SizeWarn=0")

    if config.type == "server":
        components.append("--Ice.PrintAdapterReady=1")

    if config.ipv6:
        components.append("--Ice.Default.Host=0:0:0:0:0:0:0:1 --Ice.IPv6=1")
    elif config.host != None and len(config.host) != 0:
        components.append("--Ice.Default.Host=%s" % config.host)

    #
    # Not very many tests actually require an option override, so not to worried
    # about optimal here.
    #
    if config.overrides != None and len(config.overrides) > 0:
        propTable = {}
        for c in components:
            argsToDict(c, propTable)
                    
        argsToDict(config.overrides, propTable)
        components = []
        for k, v in propTable.iteritems():
            if v != None:
                components.append("%s=%s" % (k, v))
            else:
                components.append("%s" % k)

    output = StringIO.StringIO()
    for c in components:
        print >>output, c,
    properties = output.getvalue()
    output.close()
    return properties

def getCommandLine(exe, config):
    
    output = StringIO.StringIO()
    if config.mono and config.lang == "cs":
        print >>output, "mono", "--debug '%s.exe'" % exe,
    elif config.lang == "rb" and config.type == "client":
        print >>output, "ruby '" + exe + "'",
    elif config.lang == "java" or config.lang == "javae":
        print >>output, "%s -ea" % javaCmd,
        if isSolaris() and config.x64:
            print >>output, "-d64",
        if not config.ipv6:
            print >>output, "-Djava.net.preferIPv4Stack=true",
        print >>output, exe,
    elif config.lang == "py":
        print >>output, sys.executable, '"%s"' % exe,
    elif config.lang == "php" and config.type == "client":
        print >>output, phpCmd, "-c tmp.ini -f \""+ exe +"\" -- ",
    elif config.lang == "cpp" and config.valgrind:
        # --child-silent-after-fork=yes is required for the IceGrid/activator test where the node
        # forks a process with execv failing (invalid exe name).
        print >>output, "valgrind -q --child-silent-after-fork=yes --leak-check=full ",
        print >>output, '--suppressions="' + os.path.join(toplevel, "config", "valgrind.sup") + '" "' + exe + '"',
    else:
        if exe.find(" ") != -1:
            print >>output, '"' + exe + '"',
        else:
            print >>output, exe,

    print >>output, getCommandLineProperties(exe, config),
    commandline = output.getvalue()
    output.close()

    return commandline

def directoryToPackage():
    """Determine the package name from the directory."""
    base = os.getcwd()
    after = []
    before = base
    lang = getDefaultMapping()
    while len(before) > 0:
	current = os.path.basename(before)
	before = os.path.dirname(before)
	if current == lang:
	    break
	after.insert(0, current)
    else:
        raise "cannot find language dir"
    return ".".join(after)

def getDefaultServerFile():
    lang = getDefaultMapping()
    if lang in ["rb", "php", "cpp", "cs", "cppe"]:
        return "server"
    if lang == "py":
        return "Server.py"
    if lang in ["java", "javae"]:
        return directoryToPackage() + ".Server"

def getDefaultClientFile(lang = None):
    if lang is None:
        lang = getDefaultMapping()
    if lang == "rb":
        return "Client.rb"
    if lang == "php":
        return "Client.php"
    if lang in ["cpp", "cs", "cppe"]:
        return "client"
    if lang == "py":
        return "Client.py"
    if lang in ["java", "javae"]:
        return directoryToPackage() + ".Client"

def getDefaultCollocatedFile():
    lang = getDefaultMapping()
    if lang == "rb":
        return "Collocated.rb"
    if lang == "php":
        return "Collocated.php"
    if lang in ["cpp", "cs", "cppe"]:
        return "collocated"
    if lang == "py":
        return "Collocated.py"
    if lang in ["java", "javae"]:
        return directoryToPackage() + ".Collocated"

def isDebug():
    return debug

def getQtSqlOptions(prefix, dataDir = None):
    if sqlType is None:
        return '';

    options = '--Ice.Plugin.DB=' + prefix + 'SqlDB:createSqlDB';
    options += ' --' + prefix+ '.SQL.DatabaseType=' + sqlType

    options += ' --' + prefix+ '.SQL.DatabaseName='
    if sqlDbName is None:
        if sqlType == "QSQLITE":
            if dataDir != None:
                options += dataDir + '/SQL.db'
        elif sqlType == "QODBC":
            options += 'testdsn'
        else:
            options += 'test'
    else:
        options += sqlDbName

    options += ' --' + prefix+ '.SQL.HostName='
    if sqlHost is None:
        if sqlType == "QODBC":
            options += '.\SQLExpress'
        else:
            options += 'localhost'
    else:
        options += sqlHost

    options += ' --' + prefix+ '.SQL.Port='
    if sqlPassword != None:
        options += sqlPort

    options += ' --' + prefix+ '.SQL.UserName='
    if sqlUser is None:
        options += 'test'
    else:
        options += sqlUser

    options += ' --' + prefix+ '.SQL.Password='
    if sqlPassword != None:
        options += sqlPassword

    return options

import Expect
def spawn(cmd, env=None, cwd=None, startReader=True, lang=None):
    # Start/Reset the watch dog thread
    global watchDog
    if watchDog is None:
        watchDog = WatchDog()
    else:
        watchDog.reset()

    if debug:
        print "(%s)" % cmd,
    if printenv:
        dumpenv(env, lang)
    return Expect.Expect(cmd, startReader=startReader, env=env, logfile=tracefile, cwd=cwd)

def spawnClient(cmd, env=None, cwd=None, echo=True, startReader=True, lang=None):
    client = spawn(cmd, env, quoteArgument(cwd), startReader=startReader, lang=lang)
    if echo:
        client.trace()
    return client

def spawnServer(cmd, env=None, cwd=None, count=1, adapter=None, echo=True, lang=None):
    server = spawn(cmd, env, quoteArgument(cwd), lang=lang)
    if adapter:
        server.expect("%s ready\n" % adapter)
    else:
        while count > 0:
            server.expect("[^\n]+ ready\n")
            count = count -1
    if echo:
        server.trace([re.compile("[^\n]+ ready")])
    return server

import subprocess
def runCommand(command):
    #
    # popen3 has problems dealing with white spaces in command line.
    #
    if isWin32():
        CREATE_NEW_PROCESS_GROUP = 512
        p = subprocess.Popen(command, shell=False, bufsize=0, stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                            stderr=subprocess.PIPE, creationflags = 512)
    else:
        p = subprocess.Popen(command, shell=True, bufsize=1024, stdin=subprocess.PIPE, stdout=subprocess.PIPE, \
            stderr=subprocess.PIPE, close_fds=True)

    return p;

def matchAppVerifierSuccess():
    return re.escape("\nApplication Verifier ") +  ".*\n" + \
           re.escape("Copyright (c) Microsoft Corporation. All rights reserved.") + \
           ".*\n\n$" #After all match to newlines at end.

def setAppVerifierSettings(targets, cwd=os.getcwd()):
    for exe in targets:
        if exe.endswith(".exe") == False:
            exe += ".exe"
        
        #First enable all appverifier tests
        cmd = "appverif -enable * -for " + exe
        verifier = spawn(cmd, cwd=cwd)
        verifier.expect(matchAppVerifierSuccess(), -1)
        
        #Now disable all tests we are not intested in
        cmd = "appverif -disable LuaPriv PrintDriver PrintApi -for " + exe
        verifier = spawn(cmd, cwd=cwd)
        verifier.expectall(["Application Verifier 4.0","",""], -1)

def appVerifierAfterTestEnd(targets, cwd=os.getcwd()):
    for exe in targets:
        if exe.endswith(".exe") == False:
            exe += ".exe"

        # Export appverifier logs to a xml file in cwd
        logName = cwd
        if logName is None:
            logName = os.path.dirname(exe)
        logName += "/" + os.path.basename(exe) + "_appverifier_log.xml"
        cmd = "appverif -export log -for " + exe + " -with To=" + logName
        verifier = spawn(cmd, cwd=cwd)
        verifier.expect(matchAppVerifierSuccess(), -1)

        # Delete appverifier logs from registry
        cmd = "appverif -delete logs -for " + exe
        verifier = spawn(cmd, cwd=cwd)
        verifier.expect(matchAppVerifierSuccess(), -1)

        # Delete appverifier settings
        cmd = "appverif -delete settings -for " + exe
        verifier = spawn(cmd, cwd=cwd)
        verifier.expect(matchAppVerifierSuccess(), -1)


def getMirrorDir(base, mapping):
    """Get the mirror directory for the current test in the given mapping."""
    lang = getDefaultMapping()
    after = []
    before = base
    while len(before) > 0:
	current = os.path.basename(before)
	before = os.path.dirname(before)
	if current == lang:
	    break
	after.insert(0, current)
    else:
        raise "cannot find language dir"
    return os.path.join(before, mapping, *after)

def clientServerTest(additionalServerOptions = "", additionalClientOptions = "",
                     server = None, client = None, serverenv = None, clientenv = None):
    if server is None:
        server = getDefaultServerFile()
    if client is None:
        client = getDefaultClientFile()
    serverDesc = server
    clientDesc = client

    lang = getDefaultMapping()
    testdir = os.getcwd()

    # Setup the server.
    if lang in ["rb", "php"]:
        serverdir = getMirrorDir(testdir, "cpp")
    else:
        serverdir = testdir
    if lang != "java":
        server = os.path.join(serverdir, server)

    if serverenv is None:
        serverenv = getTestEnv(lang, serverdir)

    global cross
    if len(cross) == 0:
        cross.append(lang)

    for clientLang in cross:
        clientCfg = DriverConfig("client")
        if clientLang != lang:
            if clientDesc != getDefaultClientFile():
                print "** skipping cross test"
                return

            clientCfg.lang = clientLang
            client = getDefaultClientFile(clientLang)
            clientdir = getMirrorDir(testdir, clientLang)
	    print clientdir
            if not os.path.exists(clientdir):
                print "** no matching test for %s" % clientLang
                return
        else:
            clientdir = testdir

        if clientLang != "java":
            client = os.path.join(clientdir, client)

        if clientenv is None:
            clientenv = getTestEnv(clientLang, clientdir)

        if lang == "php":
            phpSetup()
        
        clientExe = client
        serverExe = server
        
        if appverifier:
          setAppVerifierSettings([clientExe, serverExe])

        print "starting " + serverDesc + "...",
        serverCfg = DriverConfig("server")
        if lang in ["rb", "php"]:
            serverCfg.lang = "cpp"
        server = getCommandLine(server, serverCfg) + " " + additionalServerOptions
        serverProc = spawnServer(server, env = serverenv, lang=serverCfg.lang)
        print "ok"

        if clientLang == lang:
            print "starting %s..." % clientDesc,
        else:
            print "starting %s %s ..." % (clientLang, clientDesc),
        client = getCommandLine(client, clientCfg) + " " + additionalClientOptions
        clientProc = spawnClient(client, env = clientenv, startReader = False, lang=clientCfg.lang)
        print "ok"
        clientProc.startReader()

        clientProc.waitTestSuccess()
        serverProc.waitTestSuccess()
        

        if appverifier:
            appVerifierAfterTestEnd([clientExe, serverExe])

def collocatedTest(additionalOptions = ""):
    lang = getDefaultMapping()
    if len(cross) > 1 or cross[0] != lang:
        print "** skipping cross test"
        return
    testdir = os.getcwd()

    collocated = getDefaultCollocatedFile()
    if lang != "java" and lang != "javae":
        collocated = os.path.join(testdir, collocated) 

    exe = collocated
    if appverifier:
        setAppVerifierSettings([exe])
        
    env = getTestEnv(lang, testdir)

    print "starting collocated...",
    collocated = getCommandLine(collocated, DriverConfig("colloc")) + ' ' + additionalOptions 
    collocatedProc = spawnClient(collocated, env = env, startReader = False, lang=lang)
    print "ok"
    collocatedProc.startReader()
    collocatedProc.waitTestSuccess()
    if appverifier:
        appVerifierAfterTestEnd([exe])

def cleanDbDir(path):
    if os.path.exists(os.path.join(path, "__Freeze", "lock")):
        os.remove(os.path.join(path, "__Freeze", "lock"))
    if os.path.exists(os.path.join(path, "__Freeze")):
        os.rmdir(os.path.join(path, "__Freeze"))
    for filename in [ os.path.join(path, f) for f in os.listdir(path) if f != ".gitignore" and f != "DB_CONFIG" ]:
        os.remove(filename)

def startClient(exe, args = "", config=None, env=None, echo = True, startReader = True):
    if config is None:
        config = DriverConfig("client")
    if env is None:
        env = getTestEnv(getDefaultMapping(), os.getcwd())
    cmd = getCommandLine(exe, config) + ' ' + args
    if config.lang == "php":
        phpSetup()
    return spawnClient(cmd, env = env, echo = echo, startReader = startReader, lang=config.lang)

def startServer(exe, args = "", config=None, env=None, adapter = None, count = 1, echo = True):
    if config is None:
        config = DriverConfig("server")
    if env is None:
        env = getTestEnv(getDefaultMapping(), os.getcwd())
    cmd = getCommandLine(exe, config) + ' ' + args
    return spawnServer(cmd, env = env, adapter = adapter, count = count, echo = echo,lang=config.lang)

def startColloc(exe, args, config=None, env=None):
    exe = quoteArgument(exe)
    if config is None:
        config = DriverConfig("colloc")
    if env is None:
        env = getTestEnv(lang, testdir)
    cmd = getCommandLine(exe, config) + ' ' + args
    return spawnClient(cmd, env = env, lang=config.lang)

def simpleTest(exe, options = ""):
    exe = quoteArgument(exe)
    if appverifier:
        setAppVerifierSettings([exe])
    print "starting client...",
    command = exe + ' ' + options
    client = spawnClient(command, startReader = False, lang=getDefaultMapping())
    print "ok"
    client.startReader()
    client.waitTestSuccess()
    
    if appverifier:
        appVerifierAfterTestEnd([exe])
        
def getCppBinDir():
    binDir = os.path.join(getIceDir("cpp"), "bin")
    if iceHome:
        if isVS2010():
            binDir = os.path.join(binDir, "vc100")
        if x64:
            if isSolaris():
                if isSparc():
                    binDir = os.path.join(binDir, "sparcv9")
                else:
                    binDir = os.path.join(binDir, "amd64")
            elif isWin32():
                binDir = os.path.join(binDir, "x64")
    return binDir

def getServiceDir():
    global serviceDir
    if serviceDir is None:
        if iceHome:
            serviceDir = os.path.join(iceHome, "bin")
        else:
            serviceDir = "C:\\Program Files\ZeroC\Ice-" + str(getIceVersion()) + "\\bin"
    return serviceDir

def getTestEnv(lang, testdir):
    env = os.environ.copy()
    if lang == "cpp":
        addLdPath(os.path.join(testdir), env)
    elif lang == "java":
	addClasspath(os.path.join(toplevel, "java", "lib", "IceTest.jar"), env)
    return env;

def getTestName():
    lang = getDefaultMapping()
    here = os.getcwd().split(os.sep)
    here.reverse()
    for i in range(0, len(here)):
        if here[i] == lang:
            break
    else:
        raise "cannot find language dir"
    here = here[:i-1]
    here.reverse()
    # The crossTests list is in UNIX format.
    return os.path.join(*here).replace(os.sep, '/')

def joindog(dog):
    dog.stop()
    dog.join()
    
class WatchDog(threading.Thread):
    def __init__(self):
        self._reset = False
        self._stop = False
        self._cv = threading.Condition()
        threading.Thread.__init__(self)

	# The thread is marked as a daemon thread. The atexit handler
        # joins with the thread on exit. If the thread is not daemon,
        # the atexit handler will not be called.
	self.setDaemon(True)
        self.start()
        atexit.register(joindog, self)

    def run(self):
        try:
            self._cv.acquire()
            while True:
                self._cv.wait(180)
		if self._stop:
		    break
                if self._reset:
                    self._reset = False
                else:
                    print "\a*** %s Warning: Test has been inactive for 3 minutes and may be hung", \
                        time.strftime("%x %X")
            self._cv.release()
        except:
            #
            # Exceptions can be raised if the thread is still alive when the interpreter is
            # in the process of shutting down. For example, Python resets module objects to None,
            # so code such as "time.strftime()" can fail with AttributeError.
            #
            pass

    def reset(self):
        self._cv.acquire()
        self._reset = True
        self._cv.notify()
        self._cv.release()

    def stop(self):
        self._cv.acquire()
        self._stop = True
        self._cv.notify()
        self._cv.release()

def processCmdLine():
    def usage():
        print "usage: " + sys.argv[0] + """
          --debug                 Display debugging information on each test.
          --trace=<file>          Display tracing.
          --protocol=tcp|ssl      Run with the given protocol.
          --compress              Run the tests with protocol compression.
          --valgrind              Run the tests with valgrind.
          --appverifier           Run the tests with appverifier.
          --host=host             Set --Ice.Default.Host=<host>.
          --serialize             Run with connection serialization.
          --ipv6                  Use IPv6 addresses.
          --ice-home=<path>       Use the binary distribution from the given path.
          --x64                   Binary distribution is 64-bit.
          --env                   Print important environment variables.
          --cross=lang            Run cross language test.
          --sql-type=<driver>     Run IceStorm/IceGrid tests using QtSql with specified driver.
          --sql-db=<db>           Set SQL database name.
          --sql-host=<host>       Set SQL host name.
          --sql-port=<port>       Set SQL server port.
          --sql-user=<user>       Set SQL user name.
          --sql-passwd=<passwd>   Set SQL password.
          --service-dir=<dir>     Where to locate services for builds without service support.
        """
        sys.exit(2)

    try:
        opts, args = getopt.getopt(
            sys.argv[1:], "", ["debug", "trace=", "protocol=", "compress", "valgrind", "host=", "serialize", "ipv6", \
                              "ice-home=", "x64", "cross=", "env", "sql-type=", "sql-db=", "sql-host=", "sql-port=", \
                              "sql-user=", "sql-passwd=", "service-dir=", "appverifier"])
    except getopt.GetoptError:
        usage()

    if args:
        usage()

    for o, a in opts:
        if o == "--ice-home":
            global iceHome
            iceHome = a
        elif o == "--cross":
            global cross
            #testName = getTestName()
            #if testName == "Ice/custom":
            #if getTestName() not in crossTests:
            cross.append(a)
            if not a in ["cpp", "java", "cs", "py", "rb" ]:
                print "cross must be one of cpp, java, cs, py or rb"
                sys.exit(1)
            if getTestName() not in crossTests:
                print "*** This test does not support cross language testing"
                sys.exit(0)
	    # Temporary.
	    lang = getDefaultMapping()
            if getTestName() == "Ice/background" and (lang == "cs" or cross == "cs"):
                print "*** This test does not support cross language testing"
                sys.exit(0)
            
        elif o == "--x64":
            global x64
            x64 = True
        elif o == "--compress":
            global compress
            compress = True
        elif o == "--serialize":
            global serialize
            serialize = True
        elif o == "--host":
            global host
            host = a
        elif o == "--valgrind":
            global valgrind
            valgrind = True
        elif o == "--appverifier":
            if not isWin32() or getDefaultMapping() != "cpp":
                print "--appverifier option is only supported for Win32 c++ tests."
                sys.exit(1)
            global appverifier
            appverifier = True
        elif o == "--ipv6":
            global ipv6
            ipv6 = True
	if o == "--trace":
            global tracefile
	    if a == "stdout":
		tracefile = sys.stdout
	    else:
		tracefile = open(a, "w")
        elif o == "--debug":
            global debug
            debug = True
        elif o == "--env":
            global printenv
            printenv = True
        elif o == "--protocol":
            if a not in ( "ssl", "tcp"):
                usage()
            # ssl protocol isn't directly supported with mono.
            if mono and getDefaultMapping() == "cs" and a == "ssl":
                print "SSL is not supported with mono"
                sys.exit(1)
            global protocol
            protocol = a
        elif o == "--sql-type":
            global sqlType
            sqlType = a
        elif o == "--sql-db":
            global sqlDbName
            sqlDbName = a
        elif o == "--sql-host":
            global sqlHost
            sqlHost = a
        elif o == "--sql-port":
            global sqlPort
            sqlPort = a
        elif o == "--sql-user":
            global sqlUser
            sqlUser = a
        elif o == "--sql-passwd":
            global sqlPassword
            sqlPassword = a
        elif o == "--service-dir":
            global serviceDir
            serviceDir = a

    if len(args) > 0:
        usage()

    # Only use binary distribution from ICE_HOME environment variable if USE_BIN_DIST=yes
    if not iceHome and os.environ.get("USE_BIN_DIST", "no") == "yes":
        if os.environ.get("ICE_HOME", "") != "":
            iceHome = os.environ["ICE_HOME"]
        elif isLinux():
            iceHome = "/usr"
            
    if not x64:
        x64 = isWin32() and os.environ.get("XTARGET") == "x64" or os.environ.get("LP64") == "yes"
    
    configurePaths()

def runTests(start, expanded, num = 0, script = False):
    total = 0
    for tests in expanded:
        for i, args, config in tests:
            total = total + 1
    #
    # The configs argument is a list containing one or more test configurations.
    #
    index = 0
    for tests in expanded:
        for i, args, config in tests:
            index = index + 1
            if index < start:
                continue
            i = os.path.normpath(i)
            dir = os.path.join(toplevel, i)

            print
            if num > 0:
                print "[" + str(num) + "]",
            if script:
                prefix = "echo \""
                suffix = "\""
            else:
                prefix = ""
                suffix = ""

            print "%s*** running tests %d/%d in %s%s" % (prefix, index, total, dir, suffix)
            print "%s*** configuration:" % prefix,
            if len(args.strip()) == 0:
                print "Default",
            else:
                print args.strip(),
            print suffix

            if args.find("cross") != -1:
                test = os.path.join(*i.split(os.sep)[2:])
                # The crossTests list is in UNIX format.
                test = test.replace(os.sep, '/')
                if not test in crossTests:
                    print "%s*** test does not support cross testing%s" % (prefix, suffix)
                    continue

            #
            # Skip tests not supported with IPv6 if necessary
            #
            if args.find("ipv6") != -1 and "noipv6" in config:
                print "%s*** test not supported with IPv6%s" % (prefix, suffix)
                continue

            if args.find("compress") != -1 and "nocompress" in config:
                print "%s*** test not supported with compression%s" % (prefix, suffix)
                continue

            if isVista() and "novista" in config:
                print "%s*** test not supported under Vista%s" % (prefix, suffix)
                continue
            
            if isDarwin() and "nodarwin" in config:
                print "%s*** test not supported under Darwin%s" % (prefix, suffix)
                continue
            
            if not isWin32() and "win32only" in config:
                print "%s*** test only supported under Win32%s" % (prefix, suffix)
                continue

            if isBCC2010() and "nobcc" in config:
                print "%s*** test not supported with C++Builder%s" % (prefix, suffix)
                continue

            if isVC6() and "novc6" in config:
                print "%s*** test not supported with VC++ 6.0%s" % (prefix, suffix)
                continue

            # If this is mono and we're running ssl protocol tests
            # then skip. This occurs when using --all.
            if mono and ("nomono" in config or (i.find(os.path.join("cs","test")) != -1 and args.find("ssl") != -1)):
                print "%s*** test not supported with mono%s" % (prefix, suffix)
                continue

            # If this is java and we're running ipv6 under windows then skip.
            if isWin32() and i.find(os.path.join("java","test")) != -1 and args.find("ipv6") != -1:
                print "%s*** test not supported under windows%s" % (prefix, suffix)
                continue

            # Skip tests not supported by valgrind
            if args.find("valgrind") != -1 and ("novalgrind" in config or args.find("ssl") != -1):
                print "%s*** test not supported with valgrind%s" % (prefix, suffix)
                continue
            
            # Skip tests not supported by appverifier
            if args.find("appverifier") != -1 and ("noappverifier" in config or args.find("ssl") != -1):
                print "%s*** test not supported with appverifier%s" % (prefix, suffix)
                continue
            
            if script:
                print "echo \"*** test started: `date`\""
                print "cd %s" % dir
            else:
                print "*** test started:", time.strftime("%x %X")
                sys.stdout.flush()
                os.chdir(dir)

            global keepGoing
            if script:
                print "if ! %s %s %s; then" % (sys.executable, os.path.join(dir, "run.py"), args)
                print "  echo 'test in %s failed'" % os.path.abspath(dir)
                if not keepGoing:
                    print "  exit 1"
                print "fi"
            else:
                status = os.system(sys.executable + " " +  quoteArgument(os.path.join(dir, "run.py")) + " " + args)

                if status:
                    if(num > 0):
                        print "[" + str(num) + "]",
                    message = "test in " + os.path.abspath(dir) + " failed with exit status", status,
                    print message
                    if not keepGoing:
                        sys.exit(status)
                    else:
                        print " ** Error logged and will be displayed again when suite is completed **"
                        global testErrors
                        testErrors.append(message)

if os.environ.has_key("ICE_CONFIG"):
    os.unsetenv("ICE_CONFIG")

import inspect
frame = inspect.currentframe()
# Move to the top-most frame in the callback.
while frame.f_back is not None:
    frame = frame.f_back
if os.path.split(frame.f_code.co_filename)[1] == "run.py":
    # If we're not in the test directory, chdir to the correct
    # location.
    if not os.path.isabs(sys.argv[0]):
        d = os.path.join(os.getcwd(), sys.argv[0])
    else:
        d = sys.argv[0]
    d = os.path.split(d)[0]
    if os.path.normpath(d) != os.getcwd():
        os.chdir(d)
    processCmdLine()
