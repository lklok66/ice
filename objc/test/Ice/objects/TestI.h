// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************


#import <Test.h>

@interface BI : TestB<TestB>
{
    BOOL _postUnmarshalInvoked;
}
@end

@interface CI : TestC<TestC>
{
    BOOL _postUnmarshalInvoked;
}
@end

@interface DI : TestD<TestD>
{
    BOOL _postUnmarshalInvoked;
}
@end

@interface EI : TestE<TestE>
@end

@interface FI : TestF<TestF>
@end

@interface II : TestI<TestI>
@end

@interface JI : TestJ<TestJ>
@end

@interface HI : TestH<TestH>
@end

@interface InitialI : TestInitial
{
    TestB* _b1;
    TestB* _b2;
    TestC* _c;
    TestD* _d;
    TestE* _e;
    TestF* _f;
}
-(id) init;
-(void) shutdown:(ICECurrent*)current;
-(TestB *) getB1:(ICECurrent *)current;
-(TestB *) getB2:(ICECurrent *)current;
-(TestC *) getC:(ICECurrent *)current;
-(TestD *) getD:(ICECurrent *)current;
-(TestE *) getE:(ICECurrent *)current;
-(TestF *) getF:(ICECurrent *)current;
-(void) getAll:(TestB **)b1 b2:(TestB **)b2 theC:(TestC **)theC theD:(TestD **)theD current:(ICECurrent *)current;
-(TestI *) getI:(ICECurrent *)current;
-(TestI *) getJ:(ICECurrent *)current;
-(TestI *) getH:(ICECurrent *)current;
-(void) setI:(TestI *)theI current:(ICECurrent *)current;
@end

@interface UnexpectedObjectExceptionTestI : ICEBlobject<ICEBlobject>
@end

