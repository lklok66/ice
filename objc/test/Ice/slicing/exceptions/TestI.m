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

@implementation TestSlicingExceptionsI
-(void) baseAsBase:(ICECurrent*)current
{
    @throw [TestSlicingExceptionsBase base:@"Base.b"];
}

-(void) unknownDerivedAsBase:(ICECurrent*)current
{
    @throw [TestSlicingExceptionsUnknownDerived unknownDerived:@"UnknownDerived.b" ud:@"UnknownDerived.ud"];
}

-(void) knownDerivedAsBase:(ICECurrent*)current
{
    @throw [TestSlicingExceptionsKnownDerived knownDerived:@"KnownDerived.b" kd:@"KnownDerived.kd"];
}

-(void) knownDerivedAsKnownDerived:(ICECurrent*)current
{
    @throw [TestSlicingExceptionsKnownDerived knownDerived:@"KnownDerived.b" kd:@"KnownDerived.kd"];
}

-(void) unknownIntermediateAsBase:(ICECurrent*)current
{
    @throw [TestSlicingExceptionsUnknownIntermediate unknownIntermediate:@"UnknownIntermediate.b" ui:@"UnknownIntermediate.ui"];
}

-(void) knownIntermediateAsBase:(ICECurrent*)current
{
    @throw [TestSlicingExceptionsKnownIntermediate knownIntermediate:@"KnownIntermediate.b" ki:@"KnownIntermediate.ki"];
}

-(void) knownMostDerivedAsBase:(ICECurrent*)current
{
    @throw [TestSlicingExceptionsKnownMostDerived knownMostDerived:@"KnownMostDerived.b" ki:@"KnownMostDerived.ki" kmd:@"KnownMostDerived.kmd"];
}

-(void) knownIntermediateAsKnownIntermediate:(ICECurrent*)current
{
    @throw [TestSlicingExceptionsKnownIntermediate knownIntermediate:@"KnownIntermediate.b" ki:@"KnownIntermediate.ki"];
}

-(void) knownMostDerivedAsKnownIntermediate:(ICECurrent*)current
{
    @throw [TestSlicingExceptionsKnownMostDerived knownMostDerived:@"KnownMostDerived.b" ki:@"KnownMostDerived.ki" kmd:@"KnownMostDerived.kmd"];
}

-(void) knownMostDerivedAsKnownMostDerived:(ICECurrent*)current
{
    @throw [TestSlicingExceptionsKnownMostDerived knownMostDerived:@"KnownMostDerived.b" ki:@"KnownMostDerived.ki" kmd:@"KnownMostDerived.kmd"];
}

-(void) unknownMostDerived1AsBase:(ICECurrent*)current
{
    @throw [TestSlicingExceptionsUnknownMostDerived1 unknownMostDerived1:@"UnknownMostDerived1.b" ki:@"UnknownMostDerived1.ki" umd1:@"UnknownMostDerived1.umd1"];
}

-(void) unknownMostDerived1AsKnownIntermediate:(ICECurrent*)current
{
    @throw [TestSlicingExceptionsUnknownMostDerived1 unknownMostDerived1:@"UnknownMostDerived1.b" ki:@"UnknownMostDerived1.ki" umd1:@"UnknownMostDerived1.umd1"];
}

-(void) unknownMostDerived2AsBase:(ICECurrent*)current
{
    @throw [TestSlicingExceptionsUnknownMostDerived2 unknownMostDerived2:@"UnknownMostDerived2.b" ui:@"UnknownMostDerived2.ui" umd2:@"UnknownMostDerived2.umd2"];
}

-(void) shutdown:(ICECurrent*)current
{
    [[current.adapter getCommunicator] shutdown];
}
@end
