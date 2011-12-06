// **********************************************************************
//
// Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
//
// This copy of Ice FIX is licensed to you under the terms described in the
// ICE_FIX_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef ICE_STORM_PARSER_H
#define ICE_STORM_PARSER_H

#include <IceUtil/Handle.h>
#include <IceFIX/IceFIX.h>
#include <list>
#include <stdio.h>

//
// Stuff for flex and bison
//

#define YYSTYPE std::list<std::string>
#define YY_DECL int yylex(YYSTYPE* yylvalp)
YY_DECL;
int yyparse();

//
// I must set the initial stack depth to the maximum stack depth to
// disable bison stack resizing. The bison stack resizing routines use
// simple malloc/alloc/memcpy calls, which do not work for the
// YYSTYPE, since YYSTYPE is a C++ type, with constructor, destructor,
// assignment operator, etc.
//
#define YYMAXDEPTH  10000
#define YYINITDEPTH YYMAXDEPTH // Initial depth is set to max depth, for the reasons described above.

//
// Newer bison versions allow to disable stack resizing by defining
// yyoverflow.
//
#define yyoverflow(a, b, c, d, e, f) yyerror(a)

//
// Forward declaration.
//
class Parser;
typedef ::IceUtil::Handle<Parser> ParserPtr;

class Parser : public ::IceUtil::SimpleShared
{
public:

    static ParserPtr createParser(const Ice::CommunicatorPtr&, const std::vector<std::pair<std::string, IceFIX::BridgeAdminPrx> >&);

    void usage();

    void activate(const std::list<std::string>&);
    void deactivate(const std::list<std::string>&);
    void clients(const std::list<std::string>&);
    void status(const std::list<std::string>&);
    void clean(bool, const std::list<std::string>&);
    void dbstat(const std::list<std::string>&);
    void unregister(const std::list<std::string>&);

    void showBanner();

    void getInput(char*, int&, int);
    void continueLine();
    const char* getPrompt();

    void error(const char*);
    void error(const std::string&);

    void warning(const char*);
    void warning(const std::string&);

    void invalidCommand(const std::string&);

    int parse(FILE*, bool);
    int parse(const std::string&, bool);

private:

    Parser(const Ice::CommunicatorPtr&, const std::vector<std::pair<std::string, IceFIX::BridgeAdminPrx> >&);
    
    void exception(const Ice::Exception&, bool = false);

    const Ice::CommunicatorPtr _communicator;
    std::vector<std::pair<std::string, IceFIX::BridgeAdminPrx> > _admin;
    std::string _commands;
    bool _continue;
    int _errors;
};

extern Parser* parser; // The current parser for bison/flex

#endif
