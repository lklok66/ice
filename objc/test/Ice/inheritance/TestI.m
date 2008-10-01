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
-(id<TestMACAPrx>) caop:(id<TestMACAPrx>)p current:(ICECurrent*)current
{
    return p;
}
@end

@implementation CBI
-(id<TestMACAPrx>) caop:(id<TestMACAPrx>)p current:(ICECurrent*)current
{
    return p;
}
-(id<TestMBCBPrx>) cbop:(id<TestMBCBPrx>)p current:(ICECurrent*)current
{
    return p;
}
@end

@implementation CCI
-(id<TestMACAPrx>) caop:(id<TestMACAPrx>)p current:(ICECurrent*)current
{
    return p;
}
-(id<TestMBCBPrx>) cbop:(id<TestMBCBPrx>)p current:(ICECurrent*)current
{
    return p;
}
-(id<TestMACCPrx>) ccop:(id<TestMACCPrx>)p current:(ICECurrent*)current
{
    return p;
}
@end

@implementation IAI
-(id<TestMAIAPrx>) iaop:(id<TestMAIAPrx>)p current:(ICECurrent*)current
{
    return p;
}
@end

@implementation IB1I
-(id<TestMAIAPrx>) iaop:(id<TestMAIAPrx>)p current:(ICECurrent*)current
{
    return p;
}
-(id<TestMBIB1Prx>) ib1op:(id<TestMBIB1Prx>)p current:(ICECurrent*)current
{
    return p;
}
@end

@implementation IB2I
-(id<TestMAIAPrx>) iaop:(id<TestMAIAPrx>)p current:(ICECurrent*)current
{
    return p;
}
-(id<TestMBIB2Prx>) ib2op:(id<TestMBIB2Prx>)p current:(ICECurrent*)current
{
    return p;
}
@end

@implementation ICI
-(id<TestMAIAPrx>) iaop:(id<TestMAIAPrx>)p current:(ICECurrent*)current
{
    return p;
}
-(id<TestMBIB1Prx>) ib1op:(id<TestMBIB1Prx>)p current:(ICECurrent*)current
{
    return p;
}
-(id<TestMBIB2Prx>) ib2op:(id<TestMBIB2Prx>)p current:(ICECurrent*)current
{
    return p;
}
-(id<TestMAICPrx>) icop:(id<TestMAICPrx>)p current:(ICECurrent*)current
{
    return p;
}
@end

@implementation CDI
-(id<TestMACAPrx>) caop:(id<TestMACAPrx>)p current:(ICECurrent*)current
{
    return p;
}
-(id<TestMBCBPrx>) cbop:(id<TestMBCBPrx>)p current:(ICECurrent*)current
{
    return p;
}
-(id<TestMACCPrx>) ccop:(id<TestMACCPrx>)p current:(ICECurrent*)current
{
    return p;
}
-(id<TestMACDPrx>) cdop:(id<TestMACDPrx>)p current:(ICECurrent*)current
{
    return p;
}
-(id<TestMAIAPrx>) iaop:(id<TestMAIAPrx>)p current:(ICECurrent*)current
{
    return p;
}
-(id<TestMBIB1Prx>) ib1op:(id<TestMBIB1Prx>)p current:(ICECurrent*)current
{
    return p;
}
-(id<TestMBIB2Prx>) ib2op:(id<TestMBIB2Prx>)p current:(ICECurrent*)current
{
    return p;
}
@end

@implementation InitialI
-(id) init:(id<ICEObjectAdapter>)adapter
{
    if(![super init])
    {
        return nil;
    }
    ca_ = [TestMACAPrx uncheckedCast:[adapter addWithUUID:[[[CAI alloc] init] autorelease]]];
    cb_ = [TestMBCBPrx uncheckedCast:[adapter addWithUUID:[[[CBI alloc] init] autorelease]]];
    cc_ = [TestMACCPrx uncheckedCast:[adapter addWithUUID:[[[CCI alloc] init] autorelease]]];
    cd_ = [TestMACDPrx uncheckedCast:[adapter addWithUUID:[[[CDI alloc] init] autorelease]]];
    ia_ = [TestMAIAPrx uncheckedCast:[adapter addWithUUID:[[[IAI alloc] init] autorelease]]];
    ib1_ = [TestMBIB1Prx uncheckedCast:[adapter addWithUUID:[[[IB1I alloc] init] autorelease]]];
    ib2_ = [TestMBIB2Prx uncheckedCast:[adapter addWithUUID:[[[IB2I alloc] init] autorelease]]];
    ic_ = [TestMAICPrx uncheckedCast:[adapter addWithUUID:[[[ICI alloc] init] autorelease]]];
    return self;
}
-(void) shutdown:(ICECurrent*)current
{
    [[[current adapter] getCommunicator] shutdown];
}

-(id<TestMACAPrx>) caop:(ICECurrent*)current
{
    return ca_;
}

-(id<TestMBCBPrx>) cbop:(ICECurrent*)current
{
    return cb_;
}

-(id<TestMACCPrx>) ccop:(ICECurrent*)current
{
    return cc_;
}

-(id<TestMACDPrx>) cdop:(ICECurrent*)current
{
    return cd_;
}

-(id<TestMAIAPrx>) iaop:(ICECurrent*)current
{
    return ia_;
}

-(id<TestMBIB1Prx>) ib1op:(ICECurrent*)current
{
    return ib1_;
}

-(id<TestMBIB2Prx>) ib2op:(ICECurrent*)current
{
    return ib2_;
}

-(id<TestMAICPrx>) icop:(ICECurrent*)current
{
    return ic_;
}
@end
