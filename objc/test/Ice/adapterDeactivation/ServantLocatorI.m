// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <ServantLocatorI.h>
#import <TestCommon.h>
#import <TestI.h>


-(XXX) ServantLocatorI :
    _deactivated(NO)
{
}

ServantLocatorI~ServantLocatorI()
{
    test(_deactivated);
}

ICEObjectPtr
-(XXX) locate:(ICECurrent* current, ICELocalObjectPtr& cookie)
{
    test(!_deactivated);

    test([current id].category == @"");
    test([current id].name == @"test");

    cookie = [[CookieI alloc] init];

    return [[TestI alloc] init];
}

void
ServantLocatorIfinished(ICECurrent* current, id<ICEObject> servant,
                          id<ICELocalObject> cookie)
{
    test(!_deactivated);

    id<Cookie> co = CookiePtrdynamicCast(cookie);
    test(co);
    test([co message]:isEqualToString:@"blahblah");
}

void
-(XXX) deactivate:(string*)
{
    test(!_deactivated);

    _deactivated = YES;
}
