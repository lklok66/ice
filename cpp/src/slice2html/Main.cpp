// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <IceUtil/Options.h>
#include <IceUtil/CtrlCHandler.h>
#include <IceUtil/StaticMutex.h>
#include <Slice/Preprocessor.h>
#include <Gen.h>
#include <stdlib.h>

using namespace std;
using namespace Slice;
using namespace IceUtil;

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
        "-h, --help           Show this message.\n"
        "-v, --version        Display the Ice version.\n"
        "-DNAME               Define NAME as 1.\n"
        "-DNAME=DEF           Define NAME as DEF.\n"
        "-UNAME               Remove any definition for NAME.\n"
        "-IDIR                Put DIR in the include file search path.\n"
        "-E                   Print preprocessor output on stdout.\n"
        "--output-dir DIR     Create files in the directory DIR.\n"
        "--hdr FILE           Use the contents of FILE as the header.\n"
        "--ftr FILe           Use the contents of FILE as the footer.\n"
        "--indexhdr FILE      Use the contents of FILE as the header of the index/toc page (default=--hdr).\n"
        "--indexftr FILE      Use the contents of FILE as the footer of the index/toc page (default=--ftr).\n"
        "--image-dir DIR      Directory containing images for style sheets.\n"
        "--logo-url URL       Link to URL from logo image (requires --image-dir).\n"
        "--search ACTION      Generate search box with specified ACTION.\n"
        "--index NUM          Generate subindex if it has at least NUM entries (0 for no index, default=1).\n"
        "--summary NUM        Print a warning if a summary sentence exceeds NUM characters.\n"
        "-d, --debug          Print debug messages.\n"
        "--ice                Permit `Ice' prefix (for building Ice source code only).\n"
        ;
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
    opts.addOpt("", "output-dir", IceUtilInternal::Options::NeedArg, ".");
    opts.addOpt("", "hdr", IceUtilInternal::Options::NeedArg);
    opts.addOpt("", "ftr", IceUtilInternal::Options::NeedArg);
    opts.addOpt("", "indexhdr", IceUtilInternal::Options::NeedArg);
    opts.addOpt("", "indexftr", IceUtilInternal::Options::NeedArg);
    opts.addOpt("", "index", IceUtilInternal::Options::NeedArg, "1");
    opts.addOpt("", "image-dir", IceUtilInternal::Options::NeedArg);
    opts.addOpt("", "logo-url", IceUtilInternal::Options::NeedArg);
    opts.addOpt("", "search", IceUtilInternal::Options::NeedArg);
    opts.addOpt("", "summary", IceUtilInternal::Options::NeedArg, "0");
    opts.addOpt("d", "debug");
    opts.addOpt("", "ice");

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

    optargs = opts.argVec("I");
    for(i = optargs.begin(); i != optargs.end(); ++i)
    {
        cppArgs.push_back("-I" + Preprocessor::normalizeIncludePath(*i));
    }

    bool preprocess = opts.isSet("E");

    string output = opts.optArg("output-dir");

    string header = opts.optArg("hdr");

    string footer = opts.optArg("ftr");

    string indexHeader = opts.optArg("indexhdr");

    string indexFooter = opts.optArg("indexftr");

    string ind = opts.optArg("index");
    unsigned indexCount = 0;
    if(!ind.empty())
    {
        istringstream s(ind);
        s >>  indexCount;
        if(!s)
        {
            cerr << argv[0] << ": the --index operation requires a positive integer argument" << endl;
            usage(argv[0]);
            return EXIT_FAILURE;
        }
    }

    string imageDir = opts.optArg("image-dir");

    string logoURL = opts.optArg("logo-url");

    string searchAction = opts.optArg("search");

    string warnSummary = opts.optArg("summary");
    unsigned summaryCount = 0;
    if(!warnSummary.empty())
    {
        istringstream s(warnSummary);
        s >>  summaryCount;
        if(!s)
        {
            cerr << argv[0] << ": the --summary operation requires a positive integer argument" << endl;
            usage(argv[0]);
            return EXIT_FAILURE;
        }
    }

    bool debug = opts.isSet("debug");

    bool ice = opts.isSet("ice");

    if(args.empty())
    {
        cerr << argv[0] << ": no input file" << endl;
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    UnitPtr p = Unit::createUnit(true, false, ice, false);

    int status = EXIT_SUCCESS;

    IceUtil::CtrlCHandler ctrlCHandler;
    ctrlCHandler.setCallback(interruptedCallback);

    for(vector<string>::size_type idx = 0; idx < args.size(); ++idx)
    {
        Preprocessor icecpp(argv[0], args[idx], cppArgs);
        FILE* cppHandle = icecpp.preprocess(true);

        if(cppHandle == 0)
        {
            p->destroy();
            return EXIT_FAILURE;
        }
        if(preprocess)
        {
            char buf[4096];
            while(fgets(buf, static_cast<int>(sizeof(buf)), cppHandle) != NULL)
            {
                if(fputs(buf, stdout) == EOF)
                {
                    p->destroy();
                    return EXIT_FAILURE;
                }
            }
        }
        else
        {
            status = p->parse(args[idx], cppHandle, debug);
        }

        if(!icecpp.close())
        {
            p->destroy();
            return EXIT_FAILURE;
        }

        {
            IceUtil::StaticMutex::Lock lock(_mutex);

            if(_interrupted)
            {
                return EXIT_FAILURE;
            }
        }
    }

    if(status == EXIT_SUCCESS && !preprocess)
    {
        try
        {
            Slice::generate(p, output, header, footer, indexHeader, indexFooter, imageDir, logoURL,
                            searchAction, indexCount, summaryCount);
        }
        catch(const string& err)
        {
            cerr << argv[0] << ": " << err << endl;
            status = EXIT_FAILURE;
        }
        catch(const char* err)
        {
            cerr << argv[0] << ": " << err << endl;
            status = EXIT_FAILURE;
        }
    }

    p->destroy();

    {
        IceUtil::StaticMutex::Lock lock(_mutex);

        if(_interrupted)
        {
            return EXIT_FAILURE;
        }
    }

    return status;
}
