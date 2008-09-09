// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Util.h>
#import <Ice/LocalException.h>
#import <Ice/ExceptionI.h>

#include <IceCpp/LocalException.h>

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
            Class c = objc_getClass(typeId.c_str());
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
                //
                // std::exception from the Ice runtime are translated to NSException.
                //
                //Ice::UnknownException e(__FILE__, __LINE__, ex.what());
                //nsex = [ICEUnknownException localExceptionWithLocalException:e];
                nsex = [NSException exceptionWithName:@"std::exception" 
                                               reason:[NSString stringWithUTF8String:ex.what()]
                                             userInfo:nil];
            }
        }
    }
    @throw nsex;
}

void
rethrowCxxException(NSException* e, bool release)
{
    @try
    {
        @throw e;
    }
    @catch(ICEUserException* ex)
    {
        Ice::UnknownUserException uuex(__FILE__, __LINE__, fromNSString([ex description]));
        if(release)
        {
            [ex release];
        }
        throw uuex;
    }
    @catch(ICELocalException* ex)
    {
        if(release)
        {
            try
            {
                [ex rethrowCxx];
            }
            catch(const std::exception&)
            {
                [ex release];
                throw;
            }
        }
        else
        {
            [ex rethrowCxx];
        }
    }
    @catch(NSException* ex)
    {
        //
        // NSException from the runtime are translated to C++ IceObjc::Exception
        //
        //throw Ice::UnknownException(__FILE__, __LINE__, fromNSString([ex description]));
        IceObjC::Exception exo(__FILE__, __LINE__, fromNSString([ex description]));
        if(release)
        {
            [ex release];
        }
        throw exo;
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

IceObjC::Exception::Exception(const char* file, int line, const std::string& str)
{
    if(file && line > 0)
    {
        std::ostringstream os;
        os << file << ':' << line << ": " << str;
        _str = os.str();
    }
    else
    {
        _str = str;
    }
}

IceObjC::Exception::~Exception() throw()
{
}

const char*
IceObjC::Exception::what() const throw()
{
    return _str.c_str();
}
