%{

// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Ice/Ice.h>
#include <Parser.h>

#ifdef _MSC_VER
// I get these warnings from some bison versions:
// warning C4102: 'yyoverflowlab' : unreferenced label
#   pragma warning( disable : 4102 )
// warning C4065: switch statement contains 'default' but no 'case' labels
#   pragma warning( disable : 4065 )
#endif

using namespace std;
using namespace Ice;

void
yyerror(const char* s)
{
    parser->error(s);
}

%}

%pure_parser

//
// All keyword tokens. Make sure to modify the "keyword" rule in this
// file if the list of keywords is changed. Also make sure to add the
// keyword to the keyword table in Scanner.l.
//
%token ICEFIX_HELP
%token ICEFIX_EXIT
%token ICEFIX_ACTIVATE
%token ICEFIX_DEACTIVATE
%token ICEFIX_LIST
%token ICEFIX_STATUS
%token ICEFIX_UNREGISTER
%token ICEFIX_STRING

%%

// ----------------------------------------------------------------------
start
// ----------------------------------------------------------------------
: commands
{
}
|
{
}
;

// ----------------------------------------------------------------------
commands
// ----------------------------------------------------------------------
: commands command
{
}
| command
{
}
;

// ----------------------------------------------------------------------
command
// ----------------------------------------------------------------------
: ICEFIX_HELP ';'
{
    parser->usage();
}
| ICEFIX_EXIT ';'
{
    return 0;
}
| ICEFIX_ACTIVATE ';'
{
    parser->activate();
}
| ICEFIX_DEACTIVATE ';'
{
    parser->deactivate();
}
| ICEFIX_LIST ';'
{
    parser->list();
}
| ICEFIX_STATUS ';'
{
    parser->status();
}
| ICEFIX_UNREGISTER strings ';'
{
    parser->unregister($2);
}
| ICEFIX_STRING error ';'
{
    parser->invalidCommand("unknown command `" + $1.front() + "' (type `help' for more info)");
}
| error ';'
{
    yyerrok;
}
| ';'
{
}
;

// ----------------------------------------------------------------------
strings
// ----------------------------------------------------------------------
: ICEFIX_STRING strings
{
    $$ = $2;
    $$.push_front($1.front());
}
| keyword strings
{
    $$ = $2;
    $$.push_front($1.front());
}
|
{
    $$ = YYSTYPE();
}
;

// ----------------------------------------------------------------------
keyword
// ----------------------------------------------------------------------
: ICEFIX_HELP
{
}
| ICEFIX_EXIT
{
}
| ICEFIX_ACTIVATE
{
}
| ICEFIX_DEACTIVATE
{
}
| ICEFIX_LIST
{
}
| ICEFIX_STATUS
{
}
| ICEFIX_UNREGISTER
{
}

%%
