// **********************************************************************
//
// Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <slicing/exceptions/TestI.h>
#import <Ice/Ice.h>

@implementation TestSlicingExceptionsServerI
-(void) baseAsBase:(ICECurrent*)current
{
    @throw [TestSlicingExceptionsServerBase base:@"Base.b"];
}

-(void) unknownDerivedAsBase:(ICECurrent*)current
{
    @throw [TestSlicingExceptionsServerUnknownDerived unknownDerived:@"UnknownDerived.b" ud:@"UnknownDerived.ud"];
}

-(void) knownDerivedAsBase:(ICECurrent*)current
{
    @throw [TestSlicingExceptionsServerKnownDerived knownDerived:@"KnownDerived.b" kd:@"KnownDerived.kd"];
}

-(void) knownDerivedAsKnownDerived:(ICECurrent*)current
{
    @throw [TestSlicingExceptionsServerKnownDerived knownDerived:@"KnownDerived.b" kd:@"KnownDerived.kd"];
}

-(void) unknownIntermediateAsBase:(ICECurrent*)current
{
    @throw [TestSlicingExceptionsServerUnknownIntermediate unknownIntermediate:@"UnknownIntermediate.b" ui:@"UnknownIntermediate.ui"];
}

-(void) knownIntermediateAsBase:(ICECurrent*)current
{
    @throw [TestSlicingExceptionsServerKnownIntermediate knownIntermediate:@"KnownIntermediate.b" ki:@"KnownIntermediate.ki"];
}

-(void) knownMostDerivedAsBase:(ICECurrent*)current
{
    @throw [TestSlicingExceptionsServerKnownMostDerived knownMostDerived:@"KnownMostDerived.b" ki:@"KnownMostDerived.ki" kmd:@"KnownMostDerived.kmd"];
}

-(void) knownIntermediateAsKnownIntermediate:(ICECurrent*)current
{
    @throw [TestSlicingExceptionsServerKnownIntermediate knownIntermediate:@"KnownIntermediate.b" ki:@"KnownIntermediate.ki"];
}

-(void) knownMostDerivedAsKnownIntermediate:(ICECurrent*)current
{
    @throw [TestSlicingExceptionsServerKnownMostDerived knownMostDerived:@"KnownMostDerived.b" ki:@"KnownMostDerived.ki" kmd:@"KnownMostDerived.kmd"];
}

-(void) knownMostDerivedAsKnownMostDerived:(ICECurrent*)current
{
    @throw [TestSlicingExceptionsServerKnownMostDerived knownMostDerived:@"KnownMostDerived.b" ki:@"KnownMostDerived.ki" kmd:@"KnownMostDerived.kmd"];
}

-(void) unknownMostDerived1AsBase:(ICECurrent*)current
{
    @throw [TestSlicingExceptionsServerUnknownMostDerived1 unknownMostDerived1:@"UnknownMostDerived1.b" ki:@"UnknownMostDerived1.ki" umd1:@"UnknownMostDerived1.umd1"];
}

-(void) unknownMostDerived1AsKnownIntermediate:(ICECurrent*)current
{
    @throw [TestSlicingExceptionsServerUnknownMostDerived1 unknownMostDerived1:@"UnknownMostDerived1.b" ki:@"UnknownMostDerived1.ki" umd1:@"UnknownMostDerived1.umd1"];
}

-(void) unknownMostDerived2AsBase:(ICECurrent*)current
{
    @throw [TestSlicingExceptionsServerUnknownMostDerived2 unknownMostDerived2:@"UnknownMostDerived2.b" ui:@"UnknownMostDerived2.ui" umd2:@"UnknownMostDerived2.umd2"];
}

-(void) shutdown:(ICECurrent*)current
{
    [[current.adapter getCommunicator] shutdown];
}
@end
