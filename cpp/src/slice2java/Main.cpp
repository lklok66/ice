// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <IceUtil/Options.h>
#include <IceUtil/CtrlCHandler.h>
#include <IceUtil/StaticMutex.h>
#include <Slice/Preprocessor.h>
#include <Slice/FileTracker.h>
#include <Slice/Util.h>
#include <Gen.h>

#ifdef __BCPLUSPLUS__
#  include <iterator>
#endif

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
        "--output-dir DIR        Create files in the directory DIR.\n"
        "--tie                   Generate TIE classes.\n"
        "--impl                  Generate sample implementations.\n"
        "--impl-tie              Generate sample TIE implementations.\n"
        "--depend                Generate Makefile dependencies.\n"
        "--depend-xml            Generate dependencies in XML format.\n"
        "--list-generated        Emit list of generated files in XML format.\n"
        "-d, --debug             Print debug messages.\n"
        "--ice                   Permit `Ice' prefix (for building Ice source code only)\n"
        "--checksum CLASS        Generate checksums for Slice definitions into CLASS.\n"
        "--stream                Generate marshaling support for public stream API.\n"
        "--meta META             Define global metadata directive META.\n"
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
    opts.addOpt("", "tie");
    opts.addOpt("", "impl");
    opts.addOpt("", "impl-tie");
    opts.addOpt("", "depend");
    opts.addOpt("", "depend-xml");
    opts.addOpt("", "list-generated");
    opts.addOpt("d", "debug");
    opts.addOpt("", "ice");
    opts.addOpt("", "checksum", IceUtilInternal::Options::NeedArg);
    opts.addOpt("", "stream");
    opts.addOpt("", "meta", IceUtilInternal::Options::NeedArg, "", IceUtilInternal::Options::Repeat);
    opts.addOpt("", "case-sensitive");

    vector<string>args;
    try
    {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600)
        IceUtil::DummyBCC dummy;
#endif
        args = opts.parse(argc, (const char**)argv);
    }
    catch(const IceUtilInternal::BadOptException& e)
    {
        cerr << argv[0] << ": error: " << e.reason << endl;
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
        cerr << ICE_STRING_VERSION << endl;
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

    bool tie = opts.isSet("tie");

    bool impl = opts.isSet("impl");

    bool implTie = opts.isSet("impl-tie");

    bool depend = opts.isSet("depend");
    bool dependxml = opts.isSet("depend-xml");

    bool debug = opts.isSet("debug");

    bool ice = opts.isSet("ice");

    string checksumClass = opts.optArg("checksum");

    bool stream = opts.isSet("stream");

    bool listGenerated = opts.isSet("list-generated");

    StringList globalMetadata;
    vector<string> v = opts.argVec("meta");
    copy(v.begin(), v.end(), back_inserter(globalMetadata));

    //
    // Look for the Java2 metadata.
    //
    bool java2 = false;
    for(StringList::iterator p = globalMetadata.begin(); p != globalMetadata.end(); ++p)
    {
        if((*p) == "java:java2")
        {
            java2 = true;
            break;
        }
    }

    if(java2 && !listGenerated && !depend && !dependxml)
    {
        getErrorStream() << argv[0] << ": warning: The Java2 mapping is deprecated." << endl;
    }

    bool caseSensitive = opts.isSet("case-sensitive");

    if(args.empty())
    {
        getErrorStream() << argv[0] << ": error: no input file" << endl;
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    if(impl && implTie)
    {
        getErrorStream() << argv[0] << ": error: cannot specify both --impl and --impl-tie" << endl;
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    int status = EXIT_SUCCESS;

    ChecksumMap checksums;

    IceUtil::CtrlCHandler ctrlCHandler;
    ctrlCHandler.setCallback(interruptedCallback);

    if(dependxml)
    {
        cout << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<dependencies>" << endl;
    }

    for(i = args.begin(); i != args.end(); ++i)
    {
        if(depend || dependxml)
        {
            Preprocessor icecpp(argv[0], *i, cppArgs);
            if(!icecpp.printMakefileDependencies(depend ? Preprocessor::Java : Preprocessor::JavaXML, includePaths))
            {
                return EXIT_FAILURE;
            }
        }
        else
        {
            ostringstream os;
            if(listGenerated)
            {
                Slice::setErrorStream(os);
            }

            FileTracker::instance()->setSource(*i);

            Preprocessor icecpp(argv[0], *i, cppArgs);
            FILE* cppHandle = icecpp.preprocess(false);

            if(cppHandle == 0)
            {
                FileTracker::instance()->setOutput(os.str(), true);
                status = EXIT_FAILURE;
                break;
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
                UnitPtr p = Unit::createUnit(false, false, ice, caseSensitive, globalMetadata);
                int parseStatus = p->parse(*i, cppHandle, debug, Ice);

                if(!icecpp.close())
                {
                    p->destroy();
                    return EXIT_FAILURE;
                }           

                if(parseStatus == EXIT_FAILURE)
                {
                    p->destroy();
                    FileTracker::instance()->setOutput(os.str(), true);
                    status = EXIT_FAILURE;
                }
                else
                {
                    try
                    {
                        Gen gen(argv[0], icecpp.getBaseName(), includePaths, output);
                        gen.generate(p, stream);
                        if(tie)
                        {
                            gen.generateTie(p);
                        }
                        if(impl)
                        {
                            gen.generateImpl(p);
                        }
                        if(implTie)
                        {
                            gen.generateImplTie(p);
                        }
                        if(!checksumClass.empty())
                        {
                            //
                            // Calculate checksums for the Slice definitions in the unit.
                            //
                            ChecksumMap m = createChecksums(p);
                            copy(m.begin(), m.end(), inserter(checksums, checksums.begin()));
                        }
                        FileTracker::instance()->setOutput(os.str(), false);
                    }
                    catch(const Slice::FileException& ex)
                    {
                        //
                        // If a file could not be created then cleanup any files we've already created.
                        //
                        FileTracker::instance()->cleanup();
                        p->destroy();
                        os << argv[0] << ": error: " << ex.reason() << endl;
                        FileTracker::instance()->setOutput(os.str(), true);
                        status = EXIT_FAILURE;
                        break;
                    }
                }
                p->destroy();
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

    if(status == EXIT_SUCCESS && !checksumClass.empty())
    {
        try
        {
            Gen::writeChecksumClass(checksumClass, output, checksums, java2);
        }
        catch(const Slice::FileException& ex)
        {
            // If a file could not be created, then
            // cleanup any created files.
            FileTracker::instance()->cleanup();
            getErrorStream() << argv[0] << ": error: " << ex.reason() << endl;
            return EXIT_FAILURE;
        }
    }

    if(listGenerated)
    {
        FileTracker::instance()->dumpxml();
    }

    return status;
}
