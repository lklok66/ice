// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************


#import <Test.h>

// @interface BI : TestB
// {
//     BOOL _postUnmarshalInvoked;
// }
// -(bool) postUnmarshalInvoked:(ICECurrent*)current;
// -(void) ice_preMarshal;
// -(void) ice_postUnmarshal;
// @end

// @interface CI : TestC
// {
//     BOOL _postUnmarshalInvoked;
// }
// -(bool) postUnmarshalInvoked:(ICECurrent*)current;
// -(void) ice_preMarshal;
// -(void) ice_postUnmarshal;
// @end

// @interface DI : TestD
// {
//     BOOL _postUnmarshalInvoked;
// }
// -(bool) postUnmarshalInvoked:(ICECurrent*)current;
// -(void) ice_preMarshal;
// -(void) ice_postUnmarshal;
// @end

// @interface EI : TestE
// -(bool) checkValues:(ICECurrent*)current;
// @end

// @interface FI : TestF
// -(bool) checkValues:(ICECurrent*)current;
// @end

// @interface II : TestI
// @end

// @interface JI : TestJ
// @end

// @interface HI : TestH
// @end

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

// @interface UnexpectedObjectExceptionTestI : ICEBlobject
// {
// public:

// -(bool) ice_invoke:(stdvector<ICEByte>*)XXX XXX:(stdvector<ICEByte>&)XXX XXX:(stdvector<ICEByte>&)XXX;
// };
// typedef IceUtilHandle<UnexpectedObjectExceptionTestI> UnexpectedObjectExceptionTestIPtr;

