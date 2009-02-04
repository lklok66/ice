// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <IceUtil/DisableWarnings.h>
#include <IceUtil/Options.h>
#include <Ice/Application.h>
#include <Ice/SliceChecksums.h>
#include <Parser.h>

#include <fstream>

using namespace std;
using namespace Ice;

class Client : public Application
{
public:

    void usage();
    virtual int run(int, char*[]);
};

int
main(int argc, char* argv[])
{
    Client app;
    int rc = app.main(argc, argv);
    return rc;
}

void
Client::usage()
{
    cerr << "Usage: " << appName() << " [options]\n";
    cerr <<     
        "Options:\n"
        "-h, --help           Show this message.\n"
        "-v, --version        Display the Ice version.\n"
        "-DNAME               Define NAME as 1.\n"
        "-DNAME=DEF           Define NAME as DEF.\n"
        "-UNAME               Remove any definition for NAME.\n"
        "-IDIR                Put DIR in the include file search path.\n"
        "-e COMMANDS          Execute COMMANDS.\n"
        "-d, --debug          Print debug messages.\n"
        ;
}

int
Client::run(int argc, char* argv[])
{
    string commands;
    bool debug;

    IceUtilInternal::Options opts;
    opts.addOpt("h", "help");
    opts.addOpt("v", "version");
    opts.addOpt("e", "", IceUtilInternal::Options::NeedArg, "", IceUtilInternal::Options::Repeat);
    opts.addOpt("d", "debug");

    vector<string> args;
    try
    {
        args = opts.parse(argc, (const char**)argv);
    }
    catch(const IceUtilInternal::BadOptException& e)
    {
        cerr << e.reason << endl;
        usage();
        return EXIT_FAILURE;
    }
    if(!args.empty())
    {
        cerr << argv[0] << ": too many arguments" << endl;
        usage();
        return EXIT_FAILURE;
    }

    if(opts.isSet("help"))
    {
        usage();
        return EXIT_SUCCESS;
    }
    if(opts.isSet("version"))
    {
        cout << ICE_STRING_VERSION << endl;
        return EXIT_SUCCESS;
    }
    if(opts.isSet("e"))
    {
        vector<string> optargs = opts.argVec("e");
        for(vector<string>::const_iterator i = optargs.begin(); i != optargs.end(); ++i)
        {
            commands += *i + ";";
        }
    }
    debug = opts.isSet("debug");

    IceFIX::BridgeAdminPrx admin =
        IceFIX::BridgeAdminPrx::uncheckedCast(communicator()->propertyToProxy("IceFIXAdmin.BridgeAdmin"));
    if(!admin)
    {
        cerr << appName() << ": no bridge admin proxy configured" << endl;
        return EXIT_FAILURE;
    }

    ParserPtr p = Parser::createParser(communicator(), admin);
    int status = EXIT_SUCCESS;

    if(!commands.empty()) // Commands were given
    {
        int parseStatus = p->parse(commands, debug);
        if(parseStatus == EXIT_FAILURE)
        {
            status = EXIT_FAILURE;
        }
    }
    else // No commands, let's use standard input
    {
        p->showBanner();

        int parseStatus = p->parse(stdin, debug);
        if(parseStatus == EXIT_FAILURE)
        {
            status = EXIT_FAILURE;
        }
    }

    return status;
}
