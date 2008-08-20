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
#import <TestCommon.h>

@implementation MyDerivedClassI
-(void) opVoid:(ICECurrent*)current
{
}

-(void) opDerived:(ICECurrent*)current
{
}

-(ICEByte) opByte:(ICEByte)p1 p2:(ICEByte)p2 p3:(ICEByte *)p3 current:(ICECurrent *)current
{
    *p3 = p1 ^ p2;
    return p1;
}

-(BOOL) opBool:(BOOL)p1 p2:(BOOL)p2 p3:(BOOL *)p3 current:(ICECurrent*)current
{
    *p3 = p1;
    return p2;
}

-(ICELong) opShortIntLong:(ICEShort)p1 p2:(ICEInt)p2 p3:(ICELong)p3
                       p4:(ICEShort *)p4 p5:(ICEInt *)p5 p6:(ICELong *)p6
		       current:(ICECurrent *)current
{
    *p4 = p1;
    *p5 = p2;
    *p6 = p3;
    return p3;
}

-(ICEDouble) opFloatDouble:(ICEFloat)p1 p2:(ICEDouble)p2 p3:(ICEFloat *)p3 p4:(ICEDouble *)p4
                   current:(ICECurrent *)current
{
    *p3 = p1;
    *p4 = p2;
    return p2;
}

-(NSString *) opString:(NSMutableString *)p1 p2:(NSMutableString *)p2 p3:(NSString **)p3 current:(ICECurrent *)current
{
    NSMutableString *sout = [NSMutableString stringWithCapacity:([p2 length] + 1 + [p1 length])];
    [sout appendString:p2];
    [sout appendString:@" "];
    [sout appendString:p1];
    *p3 = sout;

    NSMutableString *ret = [NSMutableString stringWithCapacity:([p1 length] + 1 + [p2 length])];
    [ret appendString:p1];
    [ret appendString:@" "];
    [ret appendString:p2];
    return ret;
}

-(TestMyEnum) opMyEnum:(TestMyEnum)p1 p2:(TestMyEnum *)p2 current:(ICECurrent *)current
{
    *p2 = p1;
    return Testenum3;
}

-(void) shutdown:(ICECurrent*)current
{
    [[[current adapter] getCommunicator] shutdown];
}
@end
