// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <IceUtil/Options.h>
#include <Slice/Preprocessor.h>
#include <Slice/FileTracker.h>
#include <IceUtil/CtrlCHandler.h>
#include <IceUtil/StaticMutex.h>
#include <Slice/Util.h>
#include <Gen.h>

using namespace std;
using namespace Slice;

static IceUtil::StaticMutex _mutex = ICE_STATIC_MUTEX_INITIALIZER;
static bool _interrupted = false;

void
interruptedCallback(int signal)
{
    IceUtil::StaticMutex::Lock lock(_mutex);

    _interrupted = true;
}

void
usage(const char* n)
{
    cerr << "Usage: " << n << " [options] slice-files...\n";
    cerr <<        
        "Options:\n"
        "-h, --help              Show this message.\n"
        "-v, --version           Display the Ice version.\n"
        "-DNAME                  Define NAME as 1.\n"
        "-DNAME=DEF              Define NAME as DEF.\n"
        "-UNAME                  Remove any definition for NAME.\n"
        "-IDIR                   Put DIR in the include file search path.\n"
        "-E                      Print preprocessor output on stdout.\n"
	"--include-dir DIR       Use DIR as the header include directory in source files.\n"
        "--output-dir DIR        Create files in the directory DIR.\n"
        "--depend                Generate Makefile dependencies.\n"
        "--depend-xml            Generate dependencies in XML format.\n"
        "-d, --debug             Print debug messages.\n"
        "--ice                   Permit `Ice' prefix (for building Ice source code only)\n"
        ;
    // Note: --case-sensitive is intentionally not shown here!
}

int
main(int argc, char* argv[])
{
    IceUtilInternal::Options opts;
    opts.addOpt("h", "help");
    opts.addOpt("v", "version");
    opts.addOpt("D", "", IceUtilInternal::Options::NeedArg, "", IceUtilInternal::Options::Repeat);
    opts.addOpt("U", "", IceUtilInternal::Options::NeedArg, "", IceUtilInternal::Options::Repeat);
    opts.addOpt("I", "", IceUtilInternal::Options::NeedArg, "", IceUtilInternal::Options::Repeat);
    opts.addOpt("E");
    opts.addOpt("", "include-dir", IceUtilInternal::Options::NeedArg);
    opts.addOpt("", "output-dir", IceUtilInternal::Options::NeedArg);
    opts.addOpt("", "depend");
    opts.addOpt("", "depend-xml");
    opts.addOpt("d", "debug");
    opts.addOpt("", "ice");
    opts.addOpt("", "case-sensitive");

    vector<string> args;
    try
    {
        args = opts.parse(argc, (const char**)argv);
    }
    catch(const IceUtilInternal::BadOptException& e)
    {
        cerr << argv[0] << ": " << e.reason << endl;
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    if(opts.isSet("help"))
    {
        usage(argv[0]);
        return EXIT_SUCCESS;
    }

    if(opts.isSet("version"))
    {
        cout << ICE_STRING_VERSION << endl;
        return EXIT_SUCCESS;
    }

    vector<string> cppArgs;
    vector<string> optargs = opts.argVec("D");
    vector<string>::const_iterator i;
    for(i = optargs.begin(); i != optargs.end(); ++i)
    {
        cppArgs.push_back("-D" + *i);
    }

    optargs = opts.argVec("U");
    for(i = optargs.begin(); i != optargs.end(); ++i)
    {
        cppArgs.push_back("-U" + *i);
    }

    vector<string> includePaths = opts.argVec("I");
    for(i = includePaths.begin(); i != includePaths.end(); ++i)
    {
        cppArgs.push_back("-I" + Preprocessor::normalizeIncludePath(*i));
    }

    bool preprocess = opts.isSet("E");

    string include = opts.optArg("include-dir");

    string output = opts.optArg("output-dir");

    bool depend = opts.isSet("depend");
    bool dependxml = opts.isSet("depend-xml");

    bool debug = opts.isSet("debug");

    bool ice = opts.isSet("ice");

    bool caseSensitive = opts.isSet("case-sensitive");

    if(args.empty())
    {
        getErrorStream() << argv[0] << ": no input file" << endl;
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    int status = EXIT_SUCCESS;

    if(dependxml)
    {
        cout << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<dependencies>" << endl;
    }

    IceUtil::CtrlCHandler ctrlCHandler;
    ctrlCHandler.setCallback(interruptedCallback);

    for(i = args.begin(); i != args.end(); ++i)
    {
        if(depend || dependxml)
        {
            Preprocessor icecpp(argv[0], *i, cppArgs);
            if(!icecpp.printMakefileDependencies(depend ? Preprocessor::ObjC : Preprocessor::XML, includePaths))
            {
                return EXIT_FAILURE;
            }
        }
        else
        {
            Preprocessor icecpp(argv[0], *i, cppArgs);
            FILE* cppHandle = icecpp.preprocess(false);

            if(cppHandle == 0)
            {
                return EXIT_FAILURE;
            }
            if(preprocess)
            {
                char buf[4096];
                while(fgets(buf, static_cast<int>(sizeof(buf)), cppHandle) != NULL)
                {
                    if(fputs(buf, stdout) == EOF)
                    {
                        return EXIT_FAILURE;
                    }
                }
                if(!icecpp.close())
                {
                    return EXIT_FAILURE;
                }           
            }
            else
            {
                UnitPtr u = Unit::createUnit(false, false, ice, caseSensitive);
                int parseStatus = u->parse(*i, cppHandle, debug);

                if(!icecpp.close())
                {
                    u->destroy();
                    return EXIT_FAILURE;
                }           
                
                if(parseStatus == EXIT_FAILURE)
                {
                    status = EXIT_FAILURE;
                }
                else
                {
                    try
                    {
                        Gen gen(argv[0], icecpp.getBaseName(), include, includePaths, output);
                        if(!gen)
                        {
                            u->destroy();
                            return EXIT_FAILURE;
                        }
                        gen.generate(u);
                    }
                    catch(const Slice::FileException& ex)
                    {
                        // If a file could not be created, then
                        // cleanup any created files.
                        FileTracker::instance()->cleanup();
                        u->destroy();
                        getErrorStream() << argv[0] << ": error: " << ex.reason() << endl;
                        return EXIT_FAILURE;
                    }
                }

                u->destroy();
            }
        }

        {
            IceUtil::StaticMutex::Lock lock(_mutex);

            if(_interrupted)
            {
                //
                // If the translator was interrupted then cleanup any files we've already created.
                //
                FileTracker::instance()->cleanup();
                return EXIT_FAILURE;
            }
        }
    }

    if(dependxml)
    {
        cout << "</dependencies>\n";
    }

    return status;
}
