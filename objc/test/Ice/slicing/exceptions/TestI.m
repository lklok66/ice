// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICETOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <TestI.h>
#import <Ice/Ice.h>

@implementation TestI
-(void) baseAsBase:(ICECurrent*)current
{
    @throw [TestBase base:@"Base.b"];
}

-(void) unknownDerivedAsBase:(ICECurrent*)current
{
    @throw [TestUnknownDerived unknownDerived:@"UnknownDerived.b" ud:@"UnknownDerived.ud"];
}

-(void) knownDerivedAsBase:(ICECurrent*)current
{
    @throw [TestKnownDerived knownDerived:@"KnownDerived.b" kd:@"KnownDerived.kd"];
}

-(void) knownDerivedAsKnownDerived:(ICECurrent*)current
{
    @throw [TestKnownDerived knownDerived:@"KnownDerived.b" kd:@"KnownDerived.kd"];
}

-(void) unknownIntermediateAsBase:(ICECurrent*)current
{
    @throw [TestUnknownIntermediate unknownIntermediate:@"UnknownIntermediate.b" ui:@"UnknownIntermediate.ui"];
}

-(void) knownIntermediateAsBase:(ICECurrent*)current
{
    @throw [TestKnownIntermediate knownIntermediate:@"KnownIntermediate.b" ki:@"KnownIntermediate.ki"];
}

-(void) knownMostDerivedAsBase:(ICECurrent*)current
{
    @throw [TestKnownMostDerived knownMostDerived:@"KnownMostDerived.b" ki:@"KnownMostDerived.ki" kmd:@"KnownMostDerived.kmd"];
}

-(void) knownIntermediateAsKnownIntermediate:(ICECurrent*)current
{
    @throw [TestKnownIntermediate knownIntermediate:@"KnownIntermediate.b" ki:@"KnownIntermediate.ki"];
}

-(void) knownMostDerivedAsKnownIntermediate:(ICECurrent*)current
{
    @throw [TestKnownMostDerived knownMostDerived:@"KnownMostDerived.b" ki:@"KnownMostDerived.ki" kmd:@"KnownMostDerived.kmd"];
}

-(void) knownMostDerivedAsKnownMostDerived:(ICECurrent*)current
{
    @throw [TestKnownMostDerived knownMostDerived:@"KnownMostDerived.b" ki:@"KnownMostDerived.ki" kmd:@"KnownMostDerived.kmd"];
}

-(void) unknownMostDerived1AsBase:(ICECurrent*)current
{
    @throw [TestUnknownMostDerived1 unknownMostDerived1:@"UnknownMostDerived1.b" ki:@"UnknownMostDerived1.ki" umd1:@"UnknownMostDerived1.umd1"];
}

-(void) unknownMostDerived1AsKnownIntermediate:(ICECurrent*)current
{
    @throw [TestUnknownMostDerived1 unknownMostDerived1:@"UnknownMostDerived1.b" ki:@"UnknownMostDerived1.ki" umd1:@"UnknownMostDerived1.umd1"];
}

-(void) unknownMostDerived2AsBase:(ICECurrent*)current
{
    @throw [TestUnknownMostDerived2 unknownMostDerived2:@"UnknownMostDerived2.b" ui:@"UnknownMostDerived2.ui" umd2:@"UnknownMostDerived2.umd2"];
}

-(void) shutdown:(ICECurrent*)current
{
    [[current.adapter getCommunicator] shutdown];
}
@end
