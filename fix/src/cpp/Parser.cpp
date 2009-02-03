// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <IceUtil/DisableWarnings.h>
#include <Ice/Ice.h>
#include <Parser.h>
#include <IceFIX/IceFIX.h>
#include <algorithm>
#include <list>

#ifdef HAVE_READLINE
#   include <readline/readline.h>
#   include <readline/history.h>
#endif

extern FILE* yyin;
extern int yydebug;

using namespace std;
using namespace Ice;

Parser* parser;

ParserPtr
Parser::createParser(const CommunicatorPtr& communicator, const IceFIX::BridgeAdminPrx& admin)
{
    return new Parser(communicator, admin);
}

void
Parser::usage()
{
    cout <<
        "help                     Print this message.\n"
        "exit, quit               Exit this program.\n"
        "activate                 Activate the bridge.\n"
        "deactivate               Deactivate the bridge.\n"
        "status                   Report the bridge status.\n"
        "list                     List the connected clients.\n"
        "unregister ID            Unregister the given client.\n"
        ;
}

void
Parser::activate()
{
    try
    {
        _admin->activate();
    }
    catch(const Ice::Exception& ex)
    {
        cerr << "activate: " << ex << endl;
    }
}

void
Parser::deactivate()
{
    try
    {
        _admin->deactivate();
    }
    catch(const Ice::Exception& ex)
    {
        cerr << "deactivate: " << ex << endl;
    }
}

void
Parser::status()
{
    try
    {
        IceFIX::BridgeStatus status = _admin->getStatus();
        cout << "status: ";
        switch(status)
        {
        case IceFIX::BridgeStatusNotActive:
            cout << "not active";
            break;
        case IceFIX::BridgeStatusActive:
            cout << "active";
            break;
        case IceFIX::BridgeStatusLoggedOn:
            cout << "logged on";
            break;

        }
        cout << endl;
    }
    catch(const Ice::Exception& ex)
    {
        cerr << "status: " << ex << endl;
    }
}

void
Parser::list()
{
    try
    {
        IceFIX::ClientInfoSeq clients = _admin->getClients();
        cout << clients.size() << " registered clients" << endl;
        for(IceFIX::ClientInfoSeq::const_iterator p = clients.begin(); p != clients.end(); ++p)
        {
            cout << "  id: " << p->id << endl;
            cout << "  connected: " << (p->isConnected ? "TRUE" : "FALSE") << endl;
        }
    }
    catch(const Ice::Exception& ex)
    {
        cerr << "list: " << ex << endl;
    }
}

void
Parser::unregister(const std::list<std::string>& args)
{
    if(args.empty())
    {
        error("`unregister' requires at least one argument (type `help' for more info)");
        return;
    }

    for(std::list<std::string>::const_iterator p = args.begin(); p != args.end() ; ++p)
    {
        try
        {
            _admin->unregister(*p);
        }
        catch(const IceFIX::RegistrationException& ex)
        {
            cerr << "unregister: " << ex << ": " << ex.reason << endl;
        }
        catch(const Ice::Exception& ex)
        {
            cerr << "unregister: " << ex << endl;
        }
    }
}

void
Parser::showBanner()
{
    cout << "Ice " << ICE_STRING_VERSION << "  Copyright 2003-2009 ZeroC, Inc." << endl;
}

void
Parser::getInput(char* buf, int& result, int maxSize)
{
    if(!_commands.empty())
    {
        if(_commands == ";")
        {
            result = 0;
        }
        else
        {
#if defined(_MSC_VER) && _MSC_VER < 1500 && !defined(_STLP_MSVC)
            // COMPILERBUG: Visual C++ defines min and max as macros
            result = _MIN(maxSize, static_cast<int>(_commands.length()));
#else
            result = min(maxSize, static_cast<int>(_commands.length()));
#endif
            strncpy(buf, _commands.c_str(), result);
            _commands.erase(0, result);
            if(_commands.empty())
            {
                _commands = ";";
            }
        }
    }
    else
    {
#ifdef HAVE_READLINE

        const char* prompt = parser->getPrompt();
        char* line = readline(const_cast<char*>(prompt));
        if(!line)
        {
            result = 0;
        }
        else
        {
            if(*line)
            {
                add_history(line);
            }

            result = strlen(line) + 1;
            if(result > maxSize)
            {
                free(line);
                error("input line too long");
                result = 0;
            }
            else
            {
                strcpy(buf, line);
                strcat(buf, "\n");
                free(line);
            }
        }

#else

        cout << parser->getPrompt() << flush;

        string line;
        while(true)
        {
            char c = static_cast<char>(getc(yyin));
            if(c == EOF)
            {
                if(line.size())
                {
                    line += '\n';
                }
                break;
            }

            line += c;

            if(c == '\n')
            {
                break;
            }
        }
        
        result = (int) line.length();
        if(result > maxSize)
        {
            error("input line too long");
            buf[0] = EOF;
            result = 1;
        }
        else
        {
            strcpy(buf, line.c_str());
        }

#endif
    }
}

void
Parser::continueLine()
{
    _continue = true;
}

const char*
Parser::getPrompt()
{
    assert(_commands.empty());

    if(_continue)
    {
        _continue = false;
        return "(cont) ";
    }
    else
    {
        return ">>> ";
    }
}

void
Parser::error(const char* s)
{
    cerr << "error: " << s << endl;
    _errors++;
}

void
Parser::error(const string& s)
{
    error(s.c_str());
}

void
Parser::warning(const char* s)
{
    cerr << "warning: " << s << endl;
}

void
Parser::warning(const string& s)
{
    warning(s.c_str());
}

void
Parser::invalidCommand(const string& s)
{
    cerr << s << endl;
}

int
Parser::parse(FILE* file, bool debug)
{
    yydebug = debug ? 1 : 0;

    assert(!parser);
    parser = this;

    _errors = 0;
    _commands.empty();
    yyin = file;
    assert(yyin);

    _continue = false;

    int status = yyparse();
    if(_errors)
    {
        status = EXIT_FAILURE;
    }

    parser = 0;
    return status;
}

int
Parser::parse(const std::string& commands, bool debug)
{
    yydebug = debug ? 1 : 0;

    assert(!parser);
    parser = this;

    _errors = 0;
    _commands = commands;
    assert(!_commands.empty());
    yyin = 0;

    _continue = false;

    int status = yyparse();
    if(_errors)
    {
        status = EXIT_FAILURE;
    }

    parser = 0;
    return status;
}

Parser::Parser(const CommunicatorPtr& communicator, const IceFIX::BridgeAdminPrx& admin) :
    _communicator(communicator),
    _admin(admin)
{
}
