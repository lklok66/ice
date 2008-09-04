// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************


#import <Ice/Ice.h>

@interface ServantLocatorI : ICEServantLocator
{
public:

    ServantLocatorI();
    ~ServantLocatorI();

    id<ICEObject> locate(ICECurrent*, ICELocalObjectPtr&);
-(void) finished:(ICECurrent*)XXX XXX:(id<ICEObject>)XXX XXX:(id<ICEObject>)XXX;
-(void) deactivate:(const NSString*&)XXX;

public:

    BOOL _deactivated;
};

