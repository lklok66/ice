// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************


#import <Test.h>

@interface AI : TestA<TestA>
@end

@interface BI : TestB<TestB>
@end

@interface CI : TestC<TestC>
@end

@interface DI : TestD<TestD>
@end

@interface EI : TestE<TestE>
@end

@interface FI : TestF<TestF>
@end

@interface GI : TestG<TestG>
{
    id<ICECommunicator> communicator_;
}
-(void) shutdown:(ICECurrent*)current;
-(NSString*) callG:(ICECurrent*)current;
@end

@interface HI : TestH<TestH>
-(NSString*) callH:(ICECurrent*)current;
@end
