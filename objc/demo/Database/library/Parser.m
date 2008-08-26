// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Parser.h>
#import <assert.h>
#import <string.h>
#import <stdio.h>

extern FILE* yyin;
Parser* parser;

@implementation Parser

@synthesize query_;
@synthesize current_;
@synthesize library_;

-(id)initWithLibrary: (id<DemoLibraryPrx>) library
{
    if(![super init])
    {
        return nil;
    }

    [self setLibrary_:library];
    [self setQuery_:nil];
    [self setCurrent_:nil];
    pool_ = nil;

    return self;
}

+(id)parserWithLibrary: (id<DemoLibraryPrx>) library
{
    return [[[Parser alloc] initWithLibrary:library] autorelease];
}

-(void) usage
{
    printf("help                    Print this message.\n"
           "exit, quit              Exit this program.\n"
           "add isbn title authors  Create new book.\n"
           "isbn NUMBER             Find the book with given ISBN number.\n"
           "authors NAME            Find all books by the given authors.\n"
           "next                    Set the current book to the next one that was found.\n"
           "current                 Display the current book.\n"
           "rent NAME               Rent the current book for customer NAME.\n"
           "return                  Return the currently rented book.\n"
           "remove                  Permanently remove the current book from the library.\n"
           "size SIZE               Set the evictor size for books to SIZE.\n"
           "shutdown                Shut the library server down.\n");
}

-(void) addBook:(NSArray*) args
{
    if([args count] != 3)
    {
        [self errorWithString:@"`add' requires exactly three arguments (type `help' for more info)"];
        return;
    }

    @try
    {
        NSString* isbn = [args objectAtIndex:0];
        NSString* title = [args objectAtIndex:1];

        NSArray* authors = [[args objectAtIndex:2] componentsSeparatedByString:@","];
        id<DemoBookPrx> book = [library_ createBook:isbn title:title authors:authors];
        printf("added new book with isbn %s\n", [isbn UTF8String]);
    }
    @catch(DemoBookExistsException* ex)
    {
        [self errorWithString:@"the book already exists."];
    }
    @catch(ICELocalException* ex)
    {
        [self errorWithString:[ex description]];
    }
}
-(void) findIsbn:(NSArray*)args
{
    if([args count] != 1)
    {
        [self errorWithString:@"`isbn' requires exactly one argument (type `help' for more info)"];
        return;
    }

    @try
    {
        if(query_ != nil)
        {
            @try
            {
                [query_ destroy];
            }
            @catch(ICEException* e)
            {
                // Ignore
            }
        }
        [self setQuery_:nil];
        [self setCurrent_:nil];

        NSString* isbn = [args objectAtIndex:0];
        
        DemoBookQueryResultPrx* query;
        DemoBookDescription* current;
        [[self library_] queryByIsbn:[args objectAtIndex:0] first:&current result:&query];

        [self setCurrent_:current];
        [self setQuery_:query];
        
        [self printCurrent];
    }
    @catch(DemoNoResultsException* ex)
    {
        [self errorWithString:[ex description]];
    }
    @catch(ICELocalException* ex)
    {
        [self errorWithString:[ex description]];
    }
}
-(void) findAuthors:(NSArray*)args
{
    if([args count] != 1)
    {
        [self errorWithString:@"`authors' requires exactly one argument (type `help' for more info)"];
        return;
    }

    @try
    {
        if(query_ != nil)
        {
            @try
            {
                [query_ destroy];
            }
            @catch(ICEException* e)
            {
                // Ignore
            }
        }
        [self setQuery_:nil];
        [self setCurrent_:nil];

        DemoBookQueryResultPrx* query;
        DemoBookDescription* current;
        [library_ queryByAuthor:[args objectAtIndex:0] first:&current result:&query];

        [self setCurrent_:current];
        [self setQuery_:query];
        [self printCurrent];
    }
    @catch(DemoNoResultsException* ex)
    {
        [self errorWithString:[ex description]];
    }
    @catch(ICELocalException* ex)
    {
        [self errorWithString:[ex description]];
    }
}

-(void) nextFoundBook
{
    if(query_ != nil)
    {
        BOOL destroyed;
        NSArray* next = [query_ next:1 destroyed:&destroyed];
        if(destroyed)
        {
            [self setQuery_:nil];
            [self setCurrent_:nil];
        }
        else
        {
            [self setCurrent_:[next objectAtIndex:0]];
        }
    }
    else
    {
        [self setCurrent_:nil];
    }
    [self printCurrent];
}

-(void) printCurrent
{
    @try
    {
        if(current_ != nil)
        {
            printf("current book is:\n");
            printf("isbn: %s\n", [[current_ isbn] UTF8String]);
            printf("title: %s\n", [[current_ title] UTF8String]);
            NSMutableString* auth = [[NSMutableString alloc] init];
            for(NSString* s in [current_ authors])
            {
                if(s != [[current_ authors] objectAtIndex:0])
                {
                    [auth appendString:@", "];
                }
                [auth appendString:s];
            }
            printf("authors: %s\n",  [auth UTF8String]);
            [auth release];
            if([[current_ rentedBy] length] > 0)
            {
                printf("rented: %s\n", [[current_ rentedBy] UTF8String]);
            }
        }
        else
        {
            printf("no current book\n");
        }
    }
    @catch(ICEObjectNotExistException* ex)
    {
        printf("current book no longer exists\n");
    }
    @catch(ICELocalException* ex)
    {
        [self errorWithString:[ex description]];
    }
}

-(void) rentCurrent:(NSArray*)args
{
    if([args count] != 1)
    {
        [self errorWithString:@"`rent' requires exactly one argument (type `help' for more info)"];
        return;
    }

    @try
    {
        if(current_ != nil)
        {
            [[current_ proxy] rentBook:[args objectAtIndex:0]];
            printf("the book is now rented by `%s'\n", [[args objectAtIndex:0] UTF8String]);
        }
        else
        {
            printf("no current book\n");
        }
    }
    @catch(DemoBookRentedException* ex)
    {
        printf("the book has already been rented.\n");
    }
    @catch(ICEObjectNotExistException* ex)
    {
        printf("current book no longer exists\n");
    }
    @catch(ICELocalException* ex)
    {
        [self errorWithString:[ex description]];
    }
}
-(void) returnCurrent
{
    @try
    {
        if(current_ != nil)
        {
            [[current_ proxy] returnBook];
            printf( "the book has been returned.\n");
        }
        else
        {
            printf("no current book\n");
        }
    }
    @catch(DemoBookNotRentedException* ex)
    {
        printf("the book is not currently rented.\n");
    }
    @catch(ICEObjectNotExistException* ex)
    {
        printf("current book no longer exists\n");
    }
    @catch(ICELocalException* ex)
    {
        [self errorWithString:[ex description]];
    }
}
-(void) removeCurrent
{
    @try
    {
        if(current_ != nil)
        {
            [[current_ proxy] destroy];
            [self setCurrent_: nil];
            printf("removed current book\n");
        }
        else
        {
            printf("no current book\n");
        }
    }
    @catch(ICEObjectNotExistException* ex)
    {
        printf("current book no longer exists\n");
    }
    @catch(ICELocalException* ex)
    {
        [self errorWithString:[ex description]];
    }
}

-(int) getInput:(char*)buf max:(int)max
{
    assert(pool_ != nil);
    [pool_ drain];

    printf("%s", [self getPrompt]);
    fflush(stdout);

    NSMutableData* line = [ [ NSMutableData alloc] init];
    while(true)
    {
        char c = (char)(getc(yyin));
        if(c == EOF)
        {
            if([line length])
            {
                c = '\n';
                [line appendBytes:&c length:1];
            }
            break;
        }

        [line appendBytes:&c length:1];

        if(c == '\n')
        {
            break;
        }
    }
    NSString* ss = [ [NSString alloc] initWithData:line encoding:NSUTF8StringEncoding ];
    @try
    {
        const char* utf8 = [ss UTF8String];

        int len = strlen(utf8);
        if(len > max)
        {
            [self errorWithString:@"input line too long"];
            buf[0] = EOF;
            return 1;
        }
        else
        {
            strcpy(buf, utf8);
        }
        return len;
    }
    @finally
    {
        [ss release];
        [line release];
    }
}

-(void) continueLine
{
    continue_ = YES;
}

-(const char*) getPrompt
{
    if(continue_)
    {
        continue_ = NO;
        return "(cont) ";
    }
    else
    {
        return ">>> ";
    }
}

-(void) error:(const char*) s
{
    fprintf(stderr, "error: %s", s);
}

-(void) errorWithString:(NSString*) s
{
    fprintf(stderr, "error: %s", [s UTF8String]);
}

-(void) warning:(const char*) s
{
    fprintf(stderr, "warning: %s", s);
}

-(void) warningWithString:(NSString*) s
{
    fprintf(stderr, "warning: %s", [s UTF8String]);
}

-(int)parse
{
    //extern int yydebug;
    //yydebug = debug ? 1 : 0;

    assert(pool_ == nil);

    pool_ = [[NSAutoreleasePool alloc] init];

    assert(!parser);
    parser = self;

    errors_ = 0;
    yyin = stdin;
    assert(yyin);

    continue_ = false;

    query_ = nil;
    current_ = nil;

    int status = yyparse();
    if(errors_)
    {
        status = EXIT_FAILURE;
    }

    [pool_ release];
    pool_ = nil;

    parser = nil;
    return status;
}

-(int)dealloc
{
    [query_ release];
    [current_ release];
    [library_ release];
    assert(pool_ == nil);
    [super dealloc];
}
@end
