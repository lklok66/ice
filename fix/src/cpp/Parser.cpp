// **********************************************************************
//
// Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
//
// This copy of Ice FIX is licensed to you under the terms described in the
// ICE_FIX_LICENSE file included in this distribution.
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
Parser::createParser(const CommunicatorPtr& communicator, const vector<pair<string, IceFIX::BridgeAdminPrx> >& admin)
{
    return new Parser(communicator, admin);
}

void
Parser::usage()
{
    cout <<
        "help                     Print this message.\n"
        "exit, quit               Exit this program.\n"
        "activate [list]          Activate the listed bridges, or all by default.\n"
        "deactivate [list]        Deactivate the listed bridges, or all by default.\n"
        "status [list]            Report the status for the listed bridges, or all by default.\n"
        "testclean timeout [list] Show the number of records older than timeout that would be erased for the listed bridges, or for all bridges by default.\n"
        "clean timeout [list]     Clean the database of all records older than timeout for the listed bridges, or all by default.\n"
        "dbstat [list]            Report the status of the databases for the listed bridges, or all by default.\n"
        "clients [list]           Show the registered clients for the listed bridges, or all by default.\n"
        "unregister [--force] [bridge] ID"
        "                        Unregister the given client from the listed bridge, or all by default.\n"
        ;
}

void
Parser::activate(const std::list<string>& args)
{
    for(vector<pair<string, IceFIX::BridgeAdminPrx> >::const_iterator p = _admin.begin(); p != _admin.end(); ++p)
    {
        if(args.size() == 0 || find(args.begin(), args.end(), p->first) != args.end())
        {
            cout << "activating " << p->first << endl;
            try
            {
                p->second->activate();
            }
            catch(const Ice::Exception& ex)
            {
                cerr << "activate: " << ex << endl;
            }
        }
    }
}

void
Parser::deactivate(const std::list<string>& args)
{
    for(vector<pair<string, IceFIX::BridgeAdminPrx> >::const_iterator p = _admin.begin(); p != _admin.end(); ++p)
    {
        if(args.size() == 0 || find(args.begin(), args.end(), p->first) != args.end())
        {
            cout << "deactivating " << p->first << endl;
            try
            {
                p->second->deactivate();
            }
            catch(const Ice::Exception& ex)
            {
                cerr << "deactivate: " << ex << endl;
            }
        }
    }
}

void
Parser::status(const std::list<string>& args)
{
    for(vector<pair<string, IceFIX::BridgeAdminPrx> >::const_iterator p = _admin.begin(); p != _admin.end(); ++p)
    {
        try
        {
            if(args.size() == 0 || find(args.begin(), args.end(), p->first) != args.end())
            {
                cout << p->first << endl;
                IceFIX::BridgeStatus status = p->second->getStatus();
                cout << "  status: ";
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
        }
        catch(const Ice::Exception& ex)
        {
            cerr << "  status: " << ex << endl;
        }
    }
}

static long
parseTimeout(const string& timeout)
{
    int days = 0;
    int hours = 0;
    int minutes = 0;
    int seconds = 0;

    int state = 0;
    string::size_type pos = 0;
    while(pos < timeout.size() && state < 4)
    {
        string num;
        while(pos < timeout.size() && isdigit(timeout[pos]))
        {
            num += timeout[pos];
            ++pos;
        }
        int n = atoi(num.c_str());

        char qual = 0;
        if(pos < timeout.size())
        {
            qual = timeout[pos];
            ++pos;
            while(pos < timeout.size() && isspace(timeout[pos]))
            {
                ++pos;
            }
        }

        switch(state)
        {
        case 0: // d
            if(qual == 'd')
            {
                days = n;
                break;
            }
            else
            {
                state = 1;
                // fall through
            }

        case 1:
            hours = n;
            break;

        case 2:
            minutes = n;
            break;

        case 3:
            seconds = n;
            break;
        }

        if((qual == 'd' && state == 0) || qual == ':')
        {
            ++state;
        }
        else if(qual == 0)
        {
            break;
        }
        else
        {
            return 0; // Invalid
        }
    }

    return seconds + (minutes * 60) + (hours * 60 * 60) + (days * 24 * 60 * 60);
}

void
Parser::clean(bool commit, const std::list<string>& _args)
{
    std::list<string> args = _args;
    if(args.empty())
    {
        error("`clean' requires at least one argument (type `help' for more info)");
        return;
    }

    int timeout = parseTimeout(args.front());
    args.pop_front();
    if(timeout == 0)
    {
        error("`timeout' is either zero, or invalid. timeout format is [nd]hh[:mm[:ss]]");
        return;
    }

    for(vector<pair<string, IceFIX::BridgeAdminPrx> >::const_iterator p = _admin.begin(); p != _admin.end(); ++p)
    {
        try
        {
            if(args.size() == 0 || find(args.begin(), args.end(), p->first) != args.end())
            {
                cout << p->first << endl;
                Ice::Long erased = p->second->clean(timeout, commit);
                if(commit)
                {
                    cout << erased << " records were erased." << endl;
                }
                else
                {
                    cout << erased << " records would be erased." << endl;
                }
            }
        }
        catch(const Ice::Exception& ex)
        {
            cerr << "  clean: " << ex << endl;
        }
    }
}

void
Parser::dbstat(const std::list<string>& args)
{
    for(vector<pair<string, IceFIX::BridgeAdminPrx> >::const_iterator p = _admin.begin(); p != _admin.end(); ++p)
    {
        try
        {
            if(args.size() == 0 || find(args.begin(), args.end(), p->first) != args.end())
            {
                cout << p->first << endl;
                cout << p->second->dbstat();
            }
        }
        catch(const Ice::Exception& ex)
        {
            cerr << "  dbstat: " << ex << endl;
        }
    }
}

void
Parser::clients(const std::list<string>& args)
{
    for(vector<pair<string, IceFIX::BridgeAdminPrx> >::const_iterator p = _admin.begin(); p != _admin.end(); ++p)
    {
        if(args.size() == 0 || find(args.begin(), args.end(), p->first) != args.end())
        {
            cout << p->first << endl;
            try
            {
                IceFIX::ClientInfoSeq clients = p->second->getClients();
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
    }
}

void
Parser::unregister(const std::list<string>& _args)
{
    std::list<string> args = _args;
    if(args.empty() || args.size() > 3)
    {
        error("`unregister' requires at least one argument (type `help' for more info)");
        return;
    }

    bool force = false;
    if(args.front() == "--force")
    {
        force = true;
        args.pop_front();
    }
    
    string bridge;
    string id;
    
    if(args.size() == 1)
    {
        id = args.front();
    }
    else
    {
        bridge = args.front();
        args.pop_front();
        id = args.front();
    }
    for(vector<pair<string, IceFIX::BridgeAdminPrx> >::const_iterator p = _admin.begin(); p != _admin.end(); ++p)
    {
        if(bridge.empty() || bridge == p->first)
        {
            cout << p->first << endl;
            try
            {
                p->second->unregister(id, force);
            }
            catch(const IceFIX::RegistrationException& ex)
            {
                cerr << "  unregister: " << ex << ": " << ex.reason << endl;
            }
            catch(const Ice::Exception& ex)
            {
                cerr << "  unregister: " << ex << endl;
            }
        }
    }
}


void
Parser::showBanner()
{
    cout << "Ice " << ICE_STRING_VERSION << "  Copyright 2003-2011 ZeroC, Inc." << endl;
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
Parser::parse(const string& commands, bool debug)
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

Parser::Parser(const CommunicatorPtr& communicator, const vector<pair<string, IceFIX::BridgeAdminPrx> >& admin) :
    _communicator(communicator),
    _admin(admin)
{
}
