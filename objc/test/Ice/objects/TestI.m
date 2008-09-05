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


// -(XXX) BI :
//     _postUnmarshalInvoked(NO)
// {
// }

// BOOL
// -(XXX) postUnmarshalInvoked:(ICECurrent*)
// {
//     return _postUnmarshalInvoked;
// }

// void
// -(XXX) ice_preMarshal
// {
//     preMarshalInvoked = YES;
// }

// void
// -(XXX) ice_postUnmarshal
// {
//     _postUnmarshalInvoked = YES;
// }

// -(XXX) CI :
//     _postUnmarshalInvoked(NO)
// {
// }

// BOOL
// -(XXX) postUnmarshalInvoked:(ICECurrent*)
// {
//     return _postUnmarshalInvoked;
// }

// void
// -(XXX) ice_preMarshal
// {
//     preMarshalInvoked = YES;
// }

// void
// -(XXX) ice_postUnmarshal
// {
//     _postUnmarshalInvoked = YES;
// }

// -(XXX) DI :
//     _postUnmarshalInvoked(NO)
// {
// }

// BOOL
// -(XXX) postUnmarshalInvoked:(ICECurrent*)
// {
//     return _postUnmarshalInvoked;
// }

// void
// -(XXX) ice_preMarshal
// {
//     preMarshalInvoked = YES;
// }

// void
// -(XXX) ice_postUnmarshal
// {
//     _postUnmarshalInvoked = YES;
// }

// -(XXX) EI :
//     E(1, @"hello")
// {
// }

// BOOL
// -(XXX) checkValues:(ICECurrent*)
// {
//     return i == 1 && s == @"hello";
// }

// -(XXX) FI
// {
// }

// -(XXX) FI:(id<E> e) :
//     F(e, e)
// {
// }

// BOOL
// -(XXX) checkValues:(ICECurrent*)
// {
//     return e1 && e1 == e2;
// }

@implementation InitialI

-(id) init
{
    if(![super init])
    {
        return nil;
    }

    _b1 = [[TestB alloc] init];
    _b2 = [[TestB alloc] init];
    _c = [[TestC alloc] init];
    _d = [[TestD alloc] init];
    _e = [[TestE alloc] init];
    _f = [[TestF alloc] init:_e e2:_e];

    _b1.theA = _b2; // Cyclic reference to another B
    _b1.theB = _b1; // Self reference.
    _b1.theC = nil; // Null reference.

    _b2.theA = _b2; // Self reference, using base.
    _b2.theB = _b1; // Cyclic reference to another B
    _b2.theC = _c; // Cyclic reference to a C.

    _c.theB = _b2; // Cyclic reference to a B.

    _d.theA = _b1; // Reference to a B.
    _d.theB = _b2; // Reference to a B.
    _d.theC = nil; // Reference to a C.
    return self;
}
-(void) dealloc
{
    _b1.theA = nil; // Break cyclic reference.
    _b1.theB = nil; // Break cyclic reference.

    _b2.theA = nil; // Break cyclic reference.
    _b2.theB = nil; // Break cyclic reference.
    _b2.theC = nil; // Break cyclic reference.

    _c.theB = nil; // Break cyclic reference.

    _d.theA = nil; // Break cyclic reference.
    _d.theB = nil; // Break cyclic reference.
    _d.theC = nil; // Break cyclic reference.

    [_b1 release];
    [_b2 release];
    [_c release];
    [_d release];
    [_e release];
    [_f release];
    [super dealloc];
}

-(void) shutdown:(ICECurrent*)current
{
    [[current.adapter getCommunicator] shutdown];
}

-(TestB*) getB1:(ICECurrent*)current
{
    _b1.preMarshalInvoked = NO;
    _b2.preMarshalInvoked = NO;
    _c.preMarshalInvoked = NO;
    return _b1;
}

-(TestB*) getB2:(ICECurrent*)current
{
    _b1.preMarshalInvoked = NO;
    _b2.preMarshalInvoked = NO;
    _c.preMarshalInvoked = NO;
    return _b2;
}


-(TestC*) getC:(ICECurrent*)current
{
    _b1.preMarshalInvoked = NO;
    _b2.preMarshalInvoked = NO;
    _c.preMarshalInvoked = NO;
    return _c;
}


-(TestD*) getD:(ICECurrent*)current
{
    _b1.preMarshalInvoked = NO;
    _b2.preMarshalInvoked = NO;
    _c.preMarshalInvoked = NO;
    _d.preMarshalInvoked = NO;
    return _d;
}


-(TestE*) getE:(ICECurrent*)current
{
    return _e;
}


-(TestF*) getF:(ICECurrent*)current
{
    return _f;
}


-(void) getAll:(TestB **)b1 b2:(TestB **)b2 theC:(TestC **)theC theD:(TestD **)theD current:(ICECurrent *)current;
{
    _b1.preMarshalInvoked = NO;
    _b2.preMarshalInvoked = NO;
    _c.preMarshalInvoked = NO;
    _d.preMarshalInvoked = NO;
    *b1 = _b1;
    *b2 = _b2;
    *theC = _c;
    *theD = _d;
}

-(TestI*) getI:(ICECurrent*)current
{
    return [[[TestI alloc] init] autorelease];
}

-(void) setI:(TestI*)i current:(ICECurrent*)current
{
}

-(TestI*) getJ:(ICECurrent*)current
{
    return [[[TestJ alloc] init] autorelease];
}

-(TestI*) getH:(ICECurrent*)current
{
    return [[[TestH alloc] init] autorelease];
}
@end
// BOOL
// UnexpectedObjectExceptionTestIice_invoke(stdvector<ICEByte>*,
//                                            stdvector<ICEByte>& outParams,
//                                            ICECurrent* current)
// {
//     id<ICECommunicator> communicator = [[current adapter] getCommunicator];
//     id<ICEOutputStream> out = ICEcreateOutputStream(communicator);
//     id<AlsoEmpty> ae = [[AlsoEmpty alloc] init];
//     ice_writeAlsoEmpty(out, ae);
//     [out writePendingObjects];
//     [out finished:outParams];
//     return YES;
// }
