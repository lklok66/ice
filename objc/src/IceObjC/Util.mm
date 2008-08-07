// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <IceObjC/Util.h>
#import <IceObjC/LocalException.h>
#import <IceObjC/ExceptionI.h>

#include <Ice/LocalException.h>

#import <Foundation/NSInvocation.h>

#import <objc/runtime.h>

void
rethrowObjCException(const std::exception& ex)
{
    NSException *nsex;
    {
        const Ice::LocalException* lex = dynamic_cast<const Ice::LocalException*>(&ex);
        if(lex)
        {
            std::string n = lex->ice_name();
            if(n.find("Ice::") == 0)
            {
                n = n.replace(0, 5, "ICE");
            }
            Class c = objc_lookUpClass(n.c_str());
            if(c != nil)
            {
                nsex = [c localExceptionWithLocalException:*lex];
            }
            else
            {
                Ice::UnknownLocalException ulex(__FILE__, __LINE__, ex.what());
                nsex = [ICEUnknownLocalException localExceptionWithLocalException:ulex];
            }
        }
        else
        {
            const Ice::UserException* uex = dynamic_cast<const Ice::UserException*>(&ex);
            if(uex)
            {
                Ice::UnknownUserException uuex(__FILE__, __LINE__, ex.what());
                nsex = [ICEUnknownUserException localExceptionWithLocalException:uuex];
            }    
            else
            {
                Ice::UnknownException e(__FILE__, __LINE__, ex.what());
                nsex = [ICEUnknownException localExceptionWithLocalException:e];
            }
        }
    }
    @throw nsex;
}

void
rethrowCxxException(NSException* ex)
{
    @try
    {
        @throw ex;
    }
    @catch(ICEUserException* ex)
    {
        throw Ice::UnknownUserException(__FILE__, __LINE__, fromNSString([ex description]));
    }
    @catch(ICELocalException* ex)
    {
        [ex rethrowCxx__];
    }
    @catch(NSException* ex)
    {
        throw Ice::UnknownException(__FILE__, __LINE__, fromNSString([ex description]));
    }
}

@implementation ExceptionHandlerForwarder
-(id)init:(id)fwdTo
{
    forwardTo = [fwdTo retain];
    return self;
}
-(void) forwardInvocation:(NSInvocation *)invocation
{
    try
    {
        if([forwardTo respondsToSelector:[invocation selector]])
        {
            [invocation invokeWithTarget:forwardTo];
        }
        else
        {
            [super forwardInvocation:invocation];
        }
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}
- (NSMethodSignature *)methodSignatureForSelector:(SEL)sel
{
    return [forwardTo methodSignatureForSelector:sel];
}
-(void)dealloc
{
    [forwardTo release];
    [super dealloc];
}
@end
