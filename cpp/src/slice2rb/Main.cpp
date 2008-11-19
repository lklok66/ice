// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <IceUtil/DisableWarnings.h>
#include <IceUtil/Options.h>
#include <IceUtil/CtrlCHandler.h>
#include <Slice/Preprocessor.h>
#include <Slice/RubyUtil.h>
#include <Slice/SignalHandler.h>

#include <fstream>

#include <sys/types.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <direct.h>
#endif

#ifndef _WIN32
#include <unistd.h>
#endif

using namespace std;
using namespace Slice;
using namespace Slice::Ruby;

//
// Callback for Crtl-C signal handling
//
static IceUtilInternal::Output _out;

static void closeCallback()
{
    _out.close();
}

void
usage(const char* n)
{
    cerr << "Usage: " << n << " [options] slice-files...\n";
    cerr <<        
        "Options:\n"
        "-h, --help           Show this message.\n"
        "-v, --version        Display the Ice version.\n"
        "-DNAME               Define NAME as 1.\n"
        "-DNAME=DEF           Define NAME as DEF.\n"
        "-UNAME               Remove any definition for NAME.\n"
        "-IDIR                Put DIR in the include file search path.\n"
        "-E                   Print preprocessor output on stdout.\n"
        "--output-dir DIR     Create files in the directory DIR.\n"
        "-d, --debug          Print debug messages.\n"
        "--ice                Permit `Ice' prefix (for building Ice source code only)\n"
        "--all                Generate code for Slice definitions in included files.\n"
        "--checksum           Generate checksums for Slice definitions.\n"
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
    opts.addOpt("", "output-dir", IceUtilInternal::Options::NeedArg);
    opts.addOpt("d", "debug");
    opts.addOpt("", "ice");
    opts.addOpt("", "all");
    opts.addOpt("", "checksum");
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

    string output = opts.optArg("output-dir");

    bool debug = opts.isSet("debug");

    bool ice = opts.isSet("ice");

    bool all = opts.isSet("all");

    bool checksum = opts.isSet("checksum");

    bool caseSensitive = opts.isSet("case-sensitive");

    if(args.empty())
    {
        cerr << argv[0] << ": no input file" << endl;
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    int status = EXIT_SUCCESS;

    for(i = args.begin(); i != args.end(); ++i)
    {
        SignalHandler::clearCleanupFileList();
        IceUtil::CtrlCHandler ctrlCHandler;
        ctrlCHandler.setCallback(SignalHandler::removeFilesOnInterrupt);

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
            UnitPtr u = Unit::createUnit(false, all, ice, caseSensitive);
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
                string base = icecpp.getBaseName();
                string::size_type pos = base.find_last_of("/\\");
                if(pos != string::npos)
                {
                    base.erase(0, pos + 1);
                }

                string file = base + ".rb";
                if(!output.empty())
                {
                    file = output + '/' + file;
                }

                SignalHandler::setCloseCallback(closeCallback);

                SignalHandler::addFileForCleanup(file);
                _out.open(file.c_str());
                if(!_out)
                {
                    cerr << argv[0] << ": can't open `" << file << "' for writing" << endl;
                    u->destroy();
                    return EXIT_FAILURE;
                }

                printHeader(_out);
                _out << "\n# Generated from file `" << base << ".ice'\n";

                //
                // Generate the Ruby mapping.
                //
                generate(u, all, checksum, includePaths, _out);

                _out.close();
                SignalHandler::setCloseCallback(0);
            }

            u->destroy();
        }
    }

    return status;
}
