// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Ice.h>
#import <Library.h>
#import <Foundation/NSArray.h>
#import <Foundation/NSAutoreleasePool.h>

//
// Stuff for flex and bison
//

#define YYSTYPE NSMutableArray*
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
#define YYMAXDEPTH  10000 // 10000 should suffice. Bison default is 10000 as maximum.
#define YYINITDEPTH YYMAXDEPTH // Initial depth is set to max depth, for the reasons described above.

//
// Newer bison versions allow to disable stack resizing by defining
// yyoverflow.
//
#define yyoverflow(a, b, c, d, e, f) yyerror(a)


@interface Parser : NSObject
{
@private

DemoBookQueryResultPrx* query_;
DemoBookDescription* current_;
id<DemoLibraryPrx> library_;

NSAutoreleasePool* pool_;
BOOL continue_;
int errors_;
}

-(id)initWithLibrary: (id<DemoLibraryPrx>) library;
+(id)parserWithLibrary: (id<DemoLibraryPrx>) library;

-(void) usage;

-(void) addBook: (NSArray*) data;
-(void) findIsbn: (NSArray*)data;
-(void) findAuthors:(NSArray*)data;
-(void) nextFoundBook;
-(void) printCurrent;
-(void) rentCurrent:(NSArray*)data;
-(void) returnCurrent;
-(void) removeCurrent;

-(int) getInput: (char*)buf max: (int)max;
-(void) continueLine;
-(const char*) getPrompt;

-(void) error: (const char*) s;
-(void) errorWithString: (NSString*) s;

-(void) warning: (const char*) s;
-(void) warningWithString: (NSString*) s;

-(int)parse;
-(int)dealloc;

@property(retain, nonatomic) DemoBookQueryResultPrx* query_;
@property(retain, nonatomic) DemoBookDescription* current_;
@property(retain, nonatomic) id<DemoLibraryPrx> library_;

@end

extern Parser* parser; // The current parser for bison/flex
