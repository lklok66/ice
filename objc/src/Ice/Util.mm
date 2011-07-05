// **********************************************************************
//
// Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Util.h>
#import <Ice/LocalException.h>
#import <Ice/ExceptionI.h>
#import <Ice/StreamI.h>
#import <Ice/ProxyI.h>

#include <IceCpp/LocalException.h>
#include <IceCpp/Initialize.h>

#include <Block.h>

#import <objc/runtime.h>

#import <Foundation/NSAutoreleasePool.h>

namespace 
{

class AsyncCallback : public IceUtil::Shared
{
public:

AsyncCallback(void (^completed)(const Ice::AsyncResultPtr&), void (^exception)(ICEException*), void (^sent)(BOOL)) :
    _completed(Block_copy(completed)), _exception(Block_copy(exception)), _sent(Block_copy(sent))
{
}

virtual ~AsyncCallback()
{
    Block_release(_completed);
    Block_release(_exception);
    Block_release(_sent);
}

void completed(const Ice::AsyncResultPtr& result)
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    NSException* exception = nil;
    @try
    {
        try
        {
            _completed(result);
        }    
        catch(const Ice::Exception& ex)
        {
            @try
            {
                NSException* nsex = toObjCException(ex);
                @throw nsex;
            }
            @catch(ICEException* e)
            {
                if(_exception)
                {
                    _exception(e);
                }
            }
        }
    }
    @catch(NSException* e)
    {
        exception = [e retain];
    }
    [pool release];
    
    if(exception != nil)
    {
        rethrowCxxException(exception, true); // True = release the exception.
    }
}

void sent(const Ice::AsyncResultPtr& result)
{
    if(!_sent)
    {
        return;
    }

    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    NSException* exception = nil;
    @try
    {
        _sent(result->sentSynchronously());
    }
    @catch(NSException* e)
    {
        exception = [e retain];
    }
    [pool release];
    
    if(exception != nil)
    {
        rethrowCxxException(exception, true); // True = release the exception.
    }
}

private:

void (^_completed)(const Ice::AsyncResultPtr&);
void (^_exception)(ICEException*);
void (^_sent)(BOOL);

};

};

void cppCall(void (^fn)())
{
    NSException* nsex = nil;
    try
    {
        fn();
        return;
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
}

void cppCall(void (^fn)(const Ice::Context&), ICEContext* context)
{
    NSException* nsex = nil;
    try
    {
        Ice::Context ctx;
        fromNSDictionary(context, ctx);
        fn(ctx);
        return;
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
}

ICEAsyncResult* beginCppCall(void (^fn)(Ice::AsyncResultPtr&), ICEObjectPrx* prx)
{
    NSException* nsex = nil;
    try
    {
        Ice::AsyncResultPtr r;
        fn(r);
        return [ICEAsyncResult asyncResultWithAsyncResult__:r operation:nil proxy:prx];
    }
    catch(const IceUtil::IllegalArgumentException& ex)
    {
        nsex = [NSException exceptionWithName:NSInvalidArgumentException reason:toNSString(ex.reason()) userInfo:nil];
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
}

ICEAsyncResult* beginCppCall(void (^fn)(Ice::AsyncResultPtr&, const Ice::CallbackPtr&), 
                             void (^completed)(const Ice::AsyncResultPtr&),
                             void (^exception)(ICEException*),
                             void (^sent)(BOOL), 
                             ICEObjectPrx* prx)
{
    NSException* nsex = nil;
    try
    {
        AsyncCallback* cb = new AsyncCallback(completed, exception, sent);
        Ice::AsyncResultPtr r;
        Ice::CallbackPtr callback = Ice::newCallback(cb, &AsyncCallback::completed, &AsyncCallback::sent);
        fn(r, callback);
        return [ICEAsyncResult asyncResultWithAsyncResult__:r operation:nil proxy:prx];
    }
    catch(const IceUtil::IllegalArgumentException& ex)
    {
        nsex = [NSException exceptionWithName:NSInvalidArgumentException reason:toNSString(ex.reason()) userInfo:nil];
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
}

ICEAsyncResult* beginCppCall(void (^fn)(Ice::AsyncResultPtr&, const Ice::Context&),
                             ICEContext* context, 
                             ICEObjectPrx* prx)
{
    NSException* nsex = nil;
    try
    {
        Ice::Context ctx;
        fromNSDictionary(context, ctx);
        Ice::AsyncResultPtr r;
        fn(r, ctx);
        return [ICEAsyncResult asyncResultWithAsyncResult__:r operation:nil proxy:prx];
    }
    catch(const IceUtil::IllegalArgumentException& ex)
    {
        nsex = [NSException exceptionWithName:NSInvalidArgumentException reason:toNSString(ex.reason()) userInfo:nil];
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
}

ICEAsyncResult* beginCppCall(void (^fn)(Ice::AsyncResultPtr&, const Ice::Context&, const Ice::CallbackPtr&), 
                             ICEContext* context,
                             void (^completed)(const Ice::AsyncResultPtr&),
                             void (^exception)(ICEException*),
                             void (^sent)(BOOL), 
                             ICEObjectPrx* prx)
{
    NSException* nsex = nil;
    try
    {
        Ice::Context ctx;
        fromNSDictionary(context, ctx);
        AsyncCallback* cb = new AsyncCallback(completed, exception, sent);
        Ice::AsyncResultPtr r;
        Ice::CallbackPtr callback = Ice::newCallback(cb, &AsyncCallback::completed, &AsyncCallback::sent);
        fn(r, ctx, callback);
        return [ICEAsyncResult asyncResultWithAsyncResult__:r operation:nil proxy:prx];
    }
    catch(const IceUtil::IllegalArgumentException& ex)
    {
        nsex = [NSException exceptionWithName:NSInvalidArgumentException reason:[toNSString(ex.reason()) autorelease]
                            userInfo:nil];
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
}

void endCppCall(void (^fn)(const Ice::AsyncResultPtr&), ICEAsyncResult* r)
{
    NSException* nsex = nil;
    try
    {
        fn([r asyncResult__]);
        return;
    }
    catch(const IceUtil::IllegalArgumentException& ex)
    {
        nsex = [NSException exceptionWithName:NSInvalidArgumentException reason:[toNSString(ex.reason()) autorelease]
                            userInfo:nil];
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
}

NSException*
toObjCException(const std::exception& ex)
{
    NSException *nsex;
    {
        const Ice::LocalException* lex = dynamic_cast<const Ice::LocalException*>(&ex);
        if(lex)
        {
            std::string typeId = std::string("ICE") + lex->ice_name().substr(5);
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
    return nsex;
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
toObjCSliceId(const std::string& sliceId, NSDictionary* prefixTable)
{
    std::string objcType = sliceId;

    if(objcType.find("::Ice::") == 0)
    {
        return objcType.replace(0, 7, "ICE"); 
    }

    std::string::size_type pos = objcType.rfind("::");
    if(pos != std::string::npos)
    {
        NSString* moduleName = toNSString(objcType.substr(0, pos));
        NSString* prefix = [prefixTable objectForKey:moduleName];
        [moduleName release];
        if(prefix)
        {
            return objcType.replace(0, pos + 2, fromNSString(prefix));
        }
    }

    while((pos = objcType.find("::")) != std::string::npos)
    {
        objcType = objcType.replace(pos, 2, "");
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
