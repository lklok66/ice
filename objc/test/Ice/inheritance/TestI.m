// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICETOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Ice.h>
#import <TestI.h>

@implementation CAI
-(id<TestInheritanceMACAPrx>) caop:(id<TestInheritanceMACAPrx>)p current:(ICECurrent*)current
{
    return p;
}
@end

@implementation CBI
-(id<TestInheritanceMACAPrx>) caop:(id<TestInheritanceMACAPrx>)p current:(ICECurrent*)current
{
    return p;
}
-(id<TestInheritanceMBCBPrx>) cbop:(id<TestInheritanceMBCBPrx>)p current:(ICECurrent*)current
{
    return p;
}
@end

@implementation CCI
-(id<TestInheritanceMACAPrx>) caop:(id<TestInheritanceMACAPrx>)p current:(ICECurrent*)current
{
    return p;
}
-(id<TestInheritanceMBCBPrx>) cbop:(id<TestInheritanceMBCBPrx>)p current:(ICECurrent*)current
{
    return p;
}
-(id<TestInheritanceMACCPrx>) ccop:(id<TestInheritanceMACCPrx>)p current:(ICECurrent*)current
{
    return p;
}
@end

@implementation IAI
-(id<TestInheritanceMAIAPrx>) iaop:(id<TestInheritanceMAIAPrx>)p current:(ICECurrent*)current
{
    return p;
}
@end

@implementation IB1I
-(id<TestInheritanceMAIAPrx>) iaop:(id<TestInheritanceMAIAPrx>)p current:(ICECurrent*)current
{
    return p;
}
-(id<TestInheritanceMBIB1Prx>) ib1op:(id<TestInheritanceMBIB1Prx>)p current:(ICECurrent*)current
{
    return p;
}
@end

@implementation IB2I
-(id<TestInheritanceMAIAPrx>) iaop:(id<TestInheritanceMAIAPrx>)p current:(ICECurrent*)current
{
    return p;
}
-(id<TestInheritanceMBIB2Prx>) ib2op:(id<TestInheritanceMBIB2Prx>)p current:(ICECurrent*)current
{
    return p;
}
@end

@implementation ICI
-(id<TestInheritanceMAIAPrx>) iaop:(id<TestInheritanceMAIAPrx>)p current:(ICECurrent*)current
{
    return p;
}
-(id<TestInheritanceMBIB1Prx>) ib1op:(id<TestInheritanceMBIB1Prx>)p current:(ICECurrent*)current
{
    return p;
}
-(id<TestInheritanceMBIB2Prx>) ib2op:(id<TestInheritanceMBIB2Prx>)p current:(ICECurrent*)current
{
    return p;
}
-(id<TestInheritanceMAICPrx>) icop:(id<TestInheritanceMAICPrx>)p current:(ICECurrent*)current
{
    return p;
}
@end

@implementation CDI
-(id<TestInheritanceMACAPrx>) caop:(id<TestInheritanceMACAPrx>)p current:(ICECurrent*)current
{
    return p;
}
-(id<TestInheritanceMBCBPrx>) cbop:(id<TestInheritanceMBCBPrx>)p current:(ICECurrent*)current
{
    return p;
}
-(id<TestInheritanceMACCPrx>) ccop:(id<TestInheritanceMACCPrx>)p current:(ICECurrent*)current
{
    return p;
}
-(id<TestInheritanceMACDPrx>) cdop:(id<TestInheritanceMACDPrx>)p current:(ICECurrent*)current
{
    return p;
}
-(id<TestInheritanceMAIAPrx>) iaop:(id<TestInheritanceMAIAPrx>)p current:(ICECurrent*)current
{
    return p;
}
-(id<TestInheritanceMBIB1Prx>) ib1op:(id<TestInheritanceMBIB1Prx>)p current:(ICECurrent*)current
{
    return p;
}
-(id<TestInheritanceMBIB2Prx>) ib2op:(id<TestInheritanceMBIB2Prx>)p current:(ICECurrent*)current
{
    return p;
}
@end

@implementation TestInheritanceInitialI
-(id) init:(id<ICEObjectAdapter>)adapter
{
    if(![super init])
    {
        return nil;
    }
    ca_ = [TestInheritanceMACAPrx uncheckedCast:[adapter addWithUUID:[[[CAI alloc] init] autorelease]]];
    cb_ = [TestInheritanceMBCBPrx uncheckedCast:[adapter addWithUUID:[[[CBI alloc] init] autorelease]]];
    cc_ = [TestInheritanceMACCPrx uncheckedCast:[adapter addWithUUID:[[[CCI alloc] init] autorelease]]];
    cd_ = [TestInheritanceMACDPrx uncheckedCast:[adapter addWithUUID:[[[CDI alloc] init] autorelease]]];
    ia_ = [TestInheritanceMAIAPrx uncheckedCast:[adapter addWithUUID:[[[IAI alloc] init] autorelease]]];
    ib1_ = [TestInheritanceMBIB1Prx uncheckedCast:[adapter addWithUUID:[[[IB1I alloc] init] autorelease]]];
    ib2_ = [TestInheritanceMBIB2Prx uncheckedCast:[adapter addWithUUID:[[[IB2I alloc] init] autorelease]]];
    ic_ = [TestInheritanceMAICPrx uncheckedCast:[adapter addWithUUID:[[[ICI alloc] init] autorelease]]];
    return self;
}
-(void) shutdown:(ICECurrent*)current
{
    [[current.adapter getCommunicator] shutdown];
}

-(id<TestInheritanceMACAPrx>) caop:(ICECurrent*)current
{
    return ca_;
}

-(id<TestInheritanceMBCBPrx>) cbop:(ICECurrent*)current
{
    return cb_;
}

-(id<TestInheritanceMACCPrx>) ccop:(ICECurrent*)current
{
    return cc_;
}

-(id<TestInheritanceMACDPrx>) cdop:(ICECurrent*)current
{
    return cd_;
}

-(id<TestInheritanceMAIAPrx>) iaop:(ICECurrent*)current
{
    return ia_;
}

-(id<TestInheritanceMBIB1Prx>) ib1op:(ICECurrent*)current
{
    return ib1_;
}

-(id<TestInheritanceMBIB2Prx>) ib2op:(ICECurrent*)current
{
    return ib2_;
}

-(id<TestInheritanceMAICPrx>) icop:(ICECurrent*)current
{
    return ic_;
}
@end
