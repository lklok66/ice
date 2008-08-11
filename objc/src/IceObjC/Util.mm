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

#import <objc/runtime.h>

void
rethrowObjCException(const std::exception& ex)
{
    NSException *nsex;
    {
        const Ice::LocalException* lex = dynamic_cast<const Ice::LocalException*>(&ex);
        if(lex)
        {
            std::string typeId = toObjCSliceId("::" + lex->ice_name());
            Class c = objc_lookUpClass(typeId.c_str());
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

std::string 
toObjCSliceId(const std::string& sliceId)
{
    std::string objcType = sliceId;
    if(objcType.find("::Ice::") == 0)
    {
        objcType = objcType.replace(0, 7, "ICE");
    }
    else
    {
        std::string::size_type pos;
        while((pos = objcType.find("::")) != std::string::npos)
        {
            objcType = objcType.replace(pos, 2, "");
        }
    }
    return objcType;
}
