// **********************************************************************
//
// Copyright (c) 2003-2010 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <SliceCompilerSpecification.h>
#import <XCPBuildSystem.h>
#import <XCPDependencyGraph.h>
#import <XCPSupport.h>
#import <Foundation/NSException.h>

@interface SliceCompilerConfiguration : NSObject
{
@private
    NSString* translator;
    NSString* shlibpath;
    NSString* slicedir;
    BOOL cpp;
    NSString* error;
    BOOL sdk;
}

@property (readonly) NSString* translator;
@property (readonly) NSString* shlibpath;
@property (readonly) NSString* slicedir;
@property (readonly) BOOL cpp;
@property (readonly) BOOL sdk;
@property (readonly) NSString* error;

-(id)initWithContext:(PBXTargetBuildContext*)context;

@end

@implementation SliceCompilerConfiguration

@synthesize translator;
@synthesize shlibpath;
@synthesize slicedir;
@synthesize cpp;
@synthesize sdk;
@synthesize error;

-(id)initWithContext:(PBXTargetBuildContext*)context
{
    if(!(self = [super init]))
    {
        return nil;
    }

    cpp = [[context expandedValueForString:@"$(SLICE_CPP_FLAG)"] isEqualToString:@"YES"];

    NSString* translatorExe = (cpp ? @"slice2cpp" : @"slice2objc");

    NSFileManager* fileManager = [NSFileManager defaultManager];
    [fileManager changeCurrentDirectoryPath:context.baseDirectoryPath];
    NSString* sliceIceHome = [context expandedValueForString:@"$(SLICE_ICE_HOME)"];
    if(sliceIceHome.length > 0)
    {
        sdk = NO;
        BOOL dir = NO;
        if(![fileManager fileExistsAtPath:sliceIceHome isDirectory:&dir] || !dir)
        {
            error = [NSString stringWithFormat:@"Ice installation cannot be found: \"%@\"", sliceIceHome];
            return self;
        }

        NSString* homeCpp = [sliceIceHome stringByAppendingPathComponent:@"cpp"];
        
        // Is this a development tree, as opposed to an install? If so the bin and lib directories
        // are in cpp, not at the root.
        if([fileManager fileExistsAtPath:homeCpp isDirectory:&dir] && dir)
        {
            slicedir = [sliceIceHome stringByAppendingPathComponent:@"slice"];
            sliceIceHome = homeCpp;
        }
        else
        {
            slicedir = [sliceIceHome stringByAppendingPathComponent:@"slice"];
        }

        translator = [[sliceIceHome stringByAppendingPathComponent:@"bin"] stringByAppendingPathComponent:translatorExe];
        
        NSDictionary* env = [[NSProcessInfo processInfo] environment];
        NSString* libdir = [sliceIceHome stringByAppendingPathComponent:@"lib"];
        shlibpath = [env objectForKey:@"DYLD_LIBRARY_PATH"];
        if(shlibpath)
        {
            shlibpath = [shlibpath stringByAppendingPathComponent:libdir];
        }
        else
        {
            shlibpath = libdir;
        }
    }
    else
    {
        NSString* sdksRaw = [context expandedValueForString:@"$(ADDITIONAL_SDKS)"];
        NSArray* sdks = [sdksRaw componentsSeparatedByString:@" "];
        BOOL found = NO;
        for(NSString* sdkDir in sdks)
        {
            if([sdkDir rangeOfString:@"IceTouch"].location != NSNotFound)
            {
                sdk = YES;
                found = YES;
                sdkDir = [sdkDir stringByDeletingLastPathComponent];
                // The bin and slice directories exist at the root of the SDK.
                slicedir = [sdkDir stringByAppendingPathComponent:@"slice"];
                translator = [[sdkDir stringByAppendingPathComponent:@"bin"] stringByAppendingPathComponent:translatorExe];
                break;
            }
        }
        if(!found)
        {
            error = [NSString stringWithFormat:@"IceTouch SDK cannot be found: \"%@\"", sdksRaw];
            return self;
        }
    }

    if(![fileManager isExecutableFileAtPath:translator])
    {
        error = [NSString stringWithFormat:@"Slice translator is not executable: \"%@\"", translator];
        return self;
    }

    BOOL dir = NO;
    if(![fileManager fileExistsAtPath:slicedir isDirectory:&dir] || !dir)
    {
        error = [NSString stringWithFormat:@"Slice files cannot be found: \"%@\"", slicedir];
        return self;
    }
    
    return self;
}

-(NSString*)description
{
    return [NSString stringWithFormat:@"translator=%@ shlibpath=%@ slicedir=%@ cpp=%d", 
            translator, shlibpath, slicedir, cpp];
}
@end


typedef struct Configuration Configuration;

@interface XMLSliceParserDelegate : NSObject<NSXMLParserDelegate>
{
@private
    
    NSMutableArray* depends;
}

- (void)parser:(NSXMLParser *)parser didStartElement:(NSString *)elementName namespaceURI:(NSString *)namespaceURI
    qualifiedName:(NSString *)qualifiedName attributes:(NSDictionary *)attributeDict;

@property(nonatomic, readonly) NSArray* depends;

@end

@implementation XMLSliceParserDelegate

@synthesize depends;

-(id)init
{
    if(![super init])
    {
        return nil;
    }
    depends = [[NSMutableArray alloc] init];
    return self;
}

-(void)parser:(NSXMLParser *)parser didStartElement:(NSString *)elementName namespaceURI:(NSString *)namespaceURI
    qualifiedName:(NSString *)qualifiedName attributes:(NSDictionary *)attributeDict
{
    if([elementName isEqualToString:@"dependsOn"])
    {
        NSString* name = [attributeDict objectForKey:@"name"];
        if(name != 0)
        {
            [depends addObject:name];
        }
    }
    
}

@end

@implementation SliceCompilerSpecification
+ (void) initialize
{
	PBXFileType* type = (PBXFileType*)[PBXFileType specificationForIdentifier:@"sourcecode.slice"];
	XCCompilerSpecification* spec = (XCCompilerSpecification*)
    [XCCompilerSpecification specificationForIdentifier:@"com.zeroc.compilers.slice"];
	[PBXTargetBuildContext activateImportedFileType:type withCompiler:spec];
}


// Run the slice compiler with --depend-xml to determine the dependencies for the given slice file.
-(NSArray*)dependenciesForSliceFile:(NSString*)path context:(PBXTargetBuildContext*)context
{
    NSMutableDictionary* env = [[[NSProcessInfo processInfo] environment] mutableCopy];
    SliceCompilerConfiguration* conf = [[SliceCompilerConfiguration alloc] initWithContext:context];
    if(conf.error)
    {
        [context addDependencyAnalysisErrorMessageFormat:@"%@", conf.error];
        return [NSArray array];
    }
    
    if(conf.shlibpath)
    {
        [env setObject:conf.shlibpath forKey:@"DYLD_LIBRARY_PATH"];
    }
        
    NSTask *dependTask = [[NSTask alloc] init];
    NSMutableArray *args = [NSMutableArray array];

    [dependTask setLaunchPath:conf.translator];
    [dependTask setEnvironment:env];
    [dependTask setCurrentDirectoryPath:[context baseDirectoryPath]];
    
    NSPipe* newPipe = [NSPipe pipe];
    NSFileHandle* readHandle = [newPipe fileHandleForReading];
    NSData* inData = nil;
    
    // write handle is closed to this process
    [dependTask setStandardOutput:newPipe];
    // Stderr goes no-where.
    //[dependTask setStandardError:newPipe];
    
    /* set arguments */
    [args addObjectsFromArray:[self commandLineForAutogeneratedOptionsInTargetBuildContext:context]];
    [args addObjectsFromArray:[[context expandedValueForString:@"$(build_file_compiler_flags)"]
                               arrayByParsingAsStringList]];
    [args addObject:[NSString stringWithFormat:@"-I%@", conf.slicedir]];

    // Use old style dependency parsing?
    if(conf.cpp)
    {
        [args addObject:@"--depend"];
    }
    else
    {
        [args addObject:@"--depend-xml"];
    }
    [args addObject:path];
    
    //NSLog(@"args: %@", args);
    
    [dependTask setArguments:args];
    
    @try
    {
        [dependTask launch];
    }
    @catch(NSException* ex)
    {
        NSLog(@"translator not accessible");
        return [NSArray array];
    }
    
    NSMutableData* output = [[NSMutableData alloc] init];
    while((inData = [readHandle availableData]) && [inData length])
    {
        [output appendData:inData];
    }

    [dependTask waitUntilExit];
    if([dependTask terminationStatus] != 0)
    {
        NSLog(@"translator exited with non-zero status %d", [dependTask terminationStatus]);
        return [NSArray array];
    }
    
    if(conf.cpp)
    {
        NSMutableArray* dep = [NSMutableArray array];
        NSString* soutput = [[NSString alloc]initWithData:output encoding:NSUTF8StringEncoding];
        // Parse C++ style dependencies.
        NSArray* lines = [soutput componentsSeparatedByString:@"\n"];
        // Ignore the first two lines.
        if(lines.count > 2)
        {
            NSEnumerator *e = [lines objectEnumerator];
            [e nextObject]; // Foo.cpp
            [e nextObject]; // Foo.ice
            // The remaning lines are the slice dependencies themselves.
            NSString* line;
            while((line = [e nextObject]))
            {
                // Strip the trailing \ if any.
                if([line characterAtIndex:line.length-1] == '\\')
                {
                    line = [line substringToIndex:line.length-1];
                }
                // Strip leading and trailing whitespace
                line = [line stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceCharacterSet]];
                // Don't add empty dependencies.
                if(line.length > 0)
                {
                    [dep addObject:line];
                }
            }
        }
        return dep;
    }
    else
    {
        // Parse XML style dependencies.
        XMLSliceParserDelegate* del = [[XMLSliceParserDelegate alloc] init];
        NSXMLParser* parser = [[NSXMLParser alloc] initWithData:output];
        [parser setDelegate:del];
        BOOL success = [parser parse];
        if(!success)
        {
            NSLog(@"XML parse of dependencies failed\n%@", output);
            return [NSArray array];
        }
        return [del depends];
    }
}

- (NSArray*) computeDependenciesForFilePath:(NSString*)input
                                     ofType:(PBXFileType*)type
                            outputDirectory:(NSString*)outputDir
                       inTargetBuildContext:(PBXTargetBuildContext*)context
{
    // compute input path (for variable substitution)
    input = [context expandedValueForString:input];

    SliceCompilerConfiguration* conf = [[SliceCompilerConfiguration alloc] initWithContext:context];
    if(conf.error)
    {
        [context addDependencyAnalysisErrorMessageFormat:@"%@", conf.error];
        return [NSArray array];
    }
    
    // The output file goes in the derived files dir.
    NSString* generatedOutputDir = [context expandedValueForString:@"$(DERIVED_FILE_DIR)"];
    NSString* outputBase = [generatedOutputDir stringByAppendingPathComponent:[[input lastPathComponent]
                                                                               stringByDeletingPathExtension]];
    NSString* sourceExtension = (conf.cpp) ? @"cpp" : @"m";
    NSString* sourceOutput = [outputBase stringByAppendingPathExtension:sourceExtension];
    NSString* headerOutput = [outputBase stringByAppendingPathExtension:@"h"];
    
    // create dependency nodes
    XCDependencyNode* outputSourceNode = [context dependencyNodeForName:sourceOutput createIfNeeded:YES];
    XCDependencyNode* outputHeaderNode = [context dependencyNodeForName:headerOutput createIfNeeded:YES];

    XCDependencyNode* inputNode = [context dependencyNodeForName:input createIfNeeded:YES];

    // create slice2objc command
    XCDependencyCommand* dep =
    [context createCommandWithRuleInfo:[NSArray arrayWithObjects:(conf.cpp ? @"slice2cpp" : @"slice2objc"),
                                        [context naturalPathForPath:input],nil]
                           commandPath:conf.translator
                             arguments:nil
                               forNode:outputHeaderNode];
    [dep addOutputNode:outputSourceNode];
    [dep setToolSpecification:self]; // So Xcode knows how to parse the output, etc.
    [dep addArgumentsFromArray:[self commandLineForAutogeneratedOptionsInTargetBuildContext:context]];
    [dep addArgumentsFromArray:[[context expandedValueForString:@"$(build_file_compiler_flags)"]
                                arrayByParsingAsStringList]];
    [dep addArgument:[NSString stringWithFormat:@"--output-dir=%@", generatedOutputDir]];
    [dep addArgument:[NSString stringWithFormat:@"-I%@", conf.slicedir]];
    [dep addArgument:input];
    [dep setPhaseNumber:3]; // This is the phase that the yacc plugin uses.
    if(conf.shlibpath)
    {
        [dep addEnvironmentValue:conf.shlibpath forKey:@"DYLD_LIBRARY_PATH"];
    }
    
    // Create dependency rules. The source and the header depend on the
    // input file.
    [outputSourceNode addDependedNode:inputNode];
    [outputHeaderNode addDependedNode:inputNode];
    
    // Add the source & headder output to the generated source files.
    [context addPath:sourceOutput toFilePathListWithIdentifier:@"GeneratedSourceFiles"];
    [context addPath:headerOutput toFilePathListWithIdentifier:@"GeneratedSourceFiles"];

    // The yacc plugin does this, not sure why.
    [context setStringValue:input forDynamicSetting:@"source_file_path"];
    [context setStringValue:[[input lastPathComponent] stringByDeletingPathExtension]
          forDynamicSetting:@"output_file_base"];
    [context setStringValue:sourceOutput forDynamicSetting:@"intermediate_file_path"];
    [context setStringValue:sourceExtension forDynamicSetting:@"output_file_extension"];
    [context setStringValue:sourceOutput forDynamicSetting:@"output_file_path"];
    [context setStringValue:headerOutput forDynamicSetting:@"output_header_path"];

    // This causes importedFilesForPath to be called (dependency management
    // between slice files).
	[inputNode setScansFileContentsForIncludes:YES];
    
    if(conf.sdk)
    {
        NSArray* options;
        if([[context expandedValueForString:@"$(PLATFORM_NAME)"] isEqualToString:@"macosx"])
        {
            options = [NSArray arrayWithObjects:@"-ObjC", @"-lstdc++", @"-liconv", @"-lbz2", @"-lcrypto", @"-lssl", @"-lIceObjC", nil];
        }
        else
        {
            options = [NSArray arrayWithObjects:@"-all_load", @"-ObjC", @"-lstdc++", @"-lIceObjC", nil];
        }
        NSArray* current = [[context expandedValueForString:@"$(OTHER_LDFLAGS)"] arrayByParsingAsStringList];
        NSMutableArray* copy = [current mutableCopy];
        for(NSString* o in options)
        {
            if(![current containsObject:o])
            {
                [copy addObject:o];
            }
        }
        if(copy.count != current.count)
        {
            [context setStringValue:[copy componentsJoinedByString:@" "] forDynamicSetting:@"OTHER_LDFLAGS"];
        }
    }

    // The output of the plugin is a single source node.
    return [NSArray arrayWithObject:outputSourceNode];
}

- (NSArray*)importedFilesForPath:(NSString*)path ensureFilesExist:(BOOL)ensure
            inTargetBuildContext:(PBXTargetBuildContext*)context
{
    XCDependencyNode* inputNode = [context dependencyNodeForName:path createIfNeeded:YES];
    NSMutableArray* imported = [NSMutableArray arrayWithCapacity:10];
    NSEnumerator *e = [[self dependenciesForSliceFile:path context:context] objectEnumerator];
    NSString *filename;
    while((filename = [e nextObject]))
    {
        NSString *filepath = [context absolutePathForPath:filename];
        XCDependencyNode *node = [context dependencyNodeForName:filepath createIfNeeded:YES];
        [node setDontCareIfExists:YES];
        [inputNode addIncludedNode:node];
        [imported addObject:filename];
    }
    
    return imported;
}

/*
- (XCPropertyDomainSpecification*) specificationToShowInUserInterface
{
	XCPropertyDomainSpecification* spec = [[XCPropertyDomainSpecification specificationRegistry] objectForKey:@"com.zeroc.compilers.slice"];
	return (XCPropertyDomainSpecification*)[spec loadedSpecification];
}
 */

@end