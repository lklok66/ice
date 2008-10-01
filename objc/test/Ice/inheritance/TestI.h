// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICETOUCH_LICENSE file included in this distribution.
//
// **********************************************************************


#import <Test.h>

@interface InitialI : TestInitial<TestInitial>
{
    id<ICEObjectAdapter> adapter_;
    id<TestMACAPrx> ca_;
    id<TestMBCBPrx> cb_;
    id<TestMACCPrx> cc_;
    id<TestMACDPrx> cd_;
    id<TestMAIAPrx> ia_;
    id<TestMBIB1Prx> ib1_;
    id<TestMBIB2Prx> ib2_;
    id<TestMAICPrx> ic_;
}
@end

@interface CAI : TestMACA<TestMACA>
@end

@interface CBI : TestMBCB<TestMBCB>
@end

@interface CCI : TestMACC<TestMACC>
@end

@interface IAI : TestMAIA<TestMAIA>
@end

@interface IB1I : TestMBIB1<TestMBIB1>
@end

@interface IB2I : TestMBIB2<TestMBIB2>
@end

@interface ICI : TestMAIC<TestMAIC>
@end

@interface CDI : TestMACD<TestMACD>
@end

