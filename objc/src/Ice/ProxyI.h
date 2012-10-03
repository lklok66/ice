// **********************************************************************
//
// Copyright (c) 2003-2012 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Proxy.h>

#include <IceCpp/Proxy.h>

@interface ICEAsyncResult : NSObject<ICEAsyncResult>
{
@private
    void* asyncResult__;
    NSString* operation_;
    id<ICEObjectPrx> proxy_;
}
-(ICEAsyncResult*)initWithAsyncResult__:(const Ice::AsyncResultPtr&)arg operation:(NSString*)op proxy:(id<ICEObjectPrx>)p;
-(Ice::AsyncResult*) asyncResult__;
+(ICEAsyncResult*)asyncResultWithAsyncResult__:(const Ice::AsyncResultPtr&)arg;
+(ICEAsyncResult*)asyncResultWithAsyncResult__:(const Ice::AsyncResultPtr&)arg operation:(NSString*)op proxy:(id<ICEObjectPrx>)p;
-(NSString*)operation;
@end

@interface ICEObjectPrx (ICEInternal)
-(ICEObjectPrx*)initWithObjectPrx__:(const Ice::ObjectPrx&)arg;
-(IceProxy::Ice::Object*) objectPrx__;
+(ICEObjectPrx*)objectPrxWithObjectPrx__:(const Ice::ObjectPrx&)arg;
@end
