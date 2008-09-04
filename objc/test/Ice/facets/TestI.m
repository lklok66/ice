// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Ice.h>
#import <TestI.h>

@implementation AI
-(NSString*) callA:(ICECurrent*)current
{
    return @"A";
}
@end

@implementation BI
-(NSString*) callA:(ICECurrent*)current
{
    return @"A";
}
-(NSString*) callB:(ICECurrent*)current
{
    return @"B";
}
@end

@implementation CI
-(NSString*) callA:(ICECurrent*)current
{
    return @"A";
}
-(NSString*) callC:(ICECurrent*)current
{
    return @"C";
}
@end

@implementation DI
-(NSString*) callA:(ICECurrent*)current
{
    return @"A";
}
-(NSString*) callB:(ICECurrent*)current
{
    return @"B";
}
-(NSString*) callC:(ICECurrent*)current
{
    return @"C";
}
-(NSString*) callD:(ICECurrent*)current
{
    return @"D";
}
@end

@implementation EI
-(NSString*) callE:(ICECurrent*)current
{
    return @"E";
}
@end

@implementation FI
-(NSString*) callE:(ICECurrent*)current
{
    return @"E";
}
-(NSString*) callF:(ICECurrent*)current
{
    return @"F";
}
@end

@implementation GI
-(void) shutdown:(ICECurrent*)current
{
    [[[current adapter] getCommunicator] shutdown];
}
-(NSString*) callG:(ICECurrent*)current
{
    return @"G";
}
@end

@implementation HI
-(void) shutdown:(ICECurrent*)current
{
    [[[current adapter] getCommunicator] shutdown];
}
-(NSString*) callG:(ICECurrent*)current
{
    return @"G";
}
-(NSString*) callH:(ICECurrent*)current
{
    return @"H";
}
@end
