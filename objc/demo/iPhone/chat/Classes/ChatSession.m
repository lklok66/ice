// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICETOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

// Ice version 3.3.0
// Generated from file `ChatSession.ice'

#import <Ice/LocalException.h>
#import <Ice/Stream.h>
#import <ChatSession.h>

#import <objc/message.h>

@implementation ChatChatRoomCallback

-(id) init
{
    if(![super init])
    {
        return nil;
    }
    return self;
}

-(void) write__:(id<ICEOutputStream>)os_
{
    [os_ writeTypeId:@"::Chat::ChatRoomCallback"];
    [os_ startSlice];
    [os_ endSlice];
    [super write__:os_];
}

-(void) read__:(id<ICEInputStream>)is_ readTypeId:(BOOL)rid_
{
    if(rid_)
    {
        [[is_ readTypeId] release];
    }
    [is_ startSlice];
    [is_ endSlice];
    [super read__:is_ readTypeId:YES];
}

static NSString *ChatChatRoomCallback_ids__[] = 
{
    @"::Chat::ChatRoomCallback",
    @"::Ice::Object"
};

+(BOOL)init___:(ICEObject<ChatChatRoomCallback> *)servant current:(ICECurrent *)current is:(id<ICEInputStream>)is_ os:(id<ICEOutputStream>)os_
{
    [servant checkModeAndSelector__:ICENormal selector:@selector(init:current:) current:current];
    ICEMutableStringSeq *users = nil;
    @try
    {
        users = [is_ readStringSeq];
        [servant init:users current:current];
    }
    @finally
    {
        [(id<NSObject>)users release];
    }
    return YES;
}

+(BOOL)send___:(ICEObject<ChatChatRoomCallback> *)servant current:(ICECurrent *)current is:(id<ICEInputStream>)is_ os:(id<ICEOutputStream>)os_
{
    [servant checkModeAndSelector__:ICENormal selector:@selector(send:name:message:current:) current:current];
    ICELong timestamp;
    NSMutableString *name = nil;
    NSMutableString *message = nil;
    @try
    {
        timestamp = [is_ readLong];
        name = [is_ readString];
        message = [is_ readString];
        [servant send:timestamp name:name message:message current:current];
    }
    @finally
    {
        [(id<NSObject>)name release];
        [(id<NSObject>)message release];
    }
    return YES;
}

+(BOOL)join___:(ICEObject<ChatChatRoomCallback> *)servant current:(ICECurrent *)current is:(id<ICEInputStream>)is_ os:(id<ICEOutputStream>)os_
{
    [servant checkModeAndSelector__:ICENormal selector:@selector(join:name:current:) current:current];
    ICELong timestamp;
    NSMutableString *name = nil;
    @try
    {
        timestamp = [is_ readLong];
        name = [is_ readString];
        [servant join:timestamp name:name current:current];
    }
    @finally
    {
        [(id<NSObject>)name release];
    }
    return YES;
}

+(BOOL)leave___:(ICEObject<ChatChatRoomCallback> *)servant current:(ICECurrent *)current is:(id<ICEInputStream>)is_ os:(id<ICEOutputStream>)os_
{
    [servant checkModeAndSelector__:ICENormal selector:@selector(leave:name:current:) current:current];
    ICELong timestamp;
    NSMutableString *name = nil;
    @try
    {
        timestamp = [is_ readLong];
        name = [is_ readString];
        [servant leave:timestamp name:name current:current];
    }
    @finally
    {
        [(id<NSObject>)name release];
    }
    return YES;
}

static NSString *ChatChatRoomCallback_all__[] =
{
    @"ice_id",
    @"ice_ids",
    @"ice_isA",
    @"ice_ping",
    @"init",
    @"join",
    @"leave",
    @"send"
};

-(BOOL) dispatch__:(ICECurrent *)current is:(id<ICEInputStream>)is os:(id<ICEOutputStream>)os
{
    switch(ICEInternalLookupString(ChatChatRoomCallback_all__, sizeof(ChatChatRoomCallback_all__) / sizeof(NSString*), current.operation))
    {
        case 0:
            return [ICEObject ice_id___:(ICEObject<ICEObject> *)self current:current is:is os:os];
        case 1:
            return [ICEObject ice_ids___:(ICEObject<ICEObject> *)self current:current is:is os:os];
        case 2:
            return [ICEObject ice_isA___:(ICEObject<ICEObject> *)self current:current is:is os:os];
        case 3:
            return [ICEObject ice_ping___:(ICEObject<ICEObject> *)self current:current is:is os:os];
        case 4:
            return [ChatChatRoomCallback init___:(ICEObject<ChatChatRoomCallback> *)self current:current is:is os:os];
        case 5:
            return [ChatChatRoomCallback join___:(ICEObject<ChatChatRoomCallback> *)self current:current is:is os:os];
        case 6:
            return [ChatChatRoomCallback leave___:(ICEObject<ChatChatRoomCallback> *)self current:current is:is os:os];
        case 7:
            return [ChatChatRoomCallback send___:(ICEObject<ChatChatRoomCallback> *)self current:current is:is os:os];
        default:
            @throw [ICEOperationNotExistException operationNotExistException:__FILE__
                                                                             line:__LINE__
                                                                             id_:current.id_
                                                                             facet:current.facet
                                                                             operation:current.operation];
    }
}

+(NSString **) staticIds__:(int*)count idIndex:(int*)idx
{
    *count = sizeof(ChatChatRoomCallback_ids__) / sizeof(NSString *);
    *idx = 0;
    return ChatChatRoomCallback_ids__;
}
@end

@implementation ChatChatSession

-(id) init
{
    if(![super init])
    {
        return nil;
    }
    return self;
}

-(void) write__:(id<ICEOutputStream>)os_
{
    [os_ writeTypeId:@"::Chat::ChatSession"];
    [os_ startSlice];
    [os_ endSlice];
    [super write__:os_];
}

-(void) read__:(id<ICEInputStream>)is_ readTypeId:(BOOL)rid_
{
    if(rid_)
    {
        [[is_ readTypeId] release];
    }
    [is_ startSlice];
    [is_ endSlice];
    [super read__:is_ readTypeId:YES];
}

static NSString *ChatChatSession_ids__[] = 
{
    @"::Chat::ChatSession",
    @"::Glacier2::Session",
    @"::Ice::Object"
};

+(BOOL)setCallback___:(ICEObject<ChatChatSession> *)servant current:(ICECurrent *)current is:(id<ICEInputStream>)is_ os:(id<ICEOutputStream>)os_
{
    [servant checkModeAndSelector__:ICENormal selector:@selector(setCallback:current:) current:current];
    id<ChatChatRoomCallbackPrx> cb = nil;
    @try
    {
        cb = (id<ChatChatRoomCallbackPrx>)[is_ readProxy:objc_getClass("ChatChatRoomCallbackPrx")];
        [servant setCallback:cb current:current];
    }
    @finally
    {
        [(id<NSObject>)cb release];
    }
    return YES;
}

+(BOOL)send___:(ICEObject<ChatChatSession> *)servant current:(ICECurrent *)current is:(id<ICEInputStream>)is_ os:(id<ICEOutputStream>)os_
{
    [servant checkModeAndSelector__:ICENormal selector:@selector(send:current:) current:current];
    NSMutableString *message = nil;
    @try
    {
        message = [is_ readString];
        ICELong ret_ = [servant send:message current:current];
        [os_ writeLong:ret_];
    }
    @catch(ChatInvalidMessageException *ex)
    {
        [os_ writeException:ex];
        return NO;
    }
    @finally
    {
        [(id<NSObject>)message release];
    }
    return YES;
}

static NSString *ChatChatSession_all__[] =
{
    @"destroy",
    @"ice_id",
    @"ice_ids",
    @"ice_isA",
    @"ice_ping",
    @"send",
    @"setCallback"
};

-(BOOL) dispatch__:(ICECurrent *)current is:(id<ICEInputStream>)is os:(id<ICEOutputStream>)os
{
    switch(ICEInternalLookupString(ChatChatSession_all__, sizeof(ChatChatSession_all__) / sizeof(NSString*), current.operation))
    {
        case 0:
            return [Glacier2Session destroy___:(ICEObject<Glacier2Session> *)self current:current is:is os:os];
        case 1:
            return [ICEObject ice_id___:(ICEObject<ICEObject> *)self current:current is:is os:os];
        case 2:
            return [ICEObject ice_ids___:(ICEObject<ICEObject> *)self current:current is:is os:os];
        case 3:
            return [ICEObject ice_isA___:(ICEObject<ICEObject> *)self current:current is:is os:os];
        case 4:
            return [ICEObject ice_ping___:(ICEObject<ICEObject> *)self current:current is:is os:os];
        case 5:
            return [ChatChatSession send___:(ICEObject<ChatChatSession> *)self current:current is:is os:os];
        case 6:
            return [ChatChatSession setCallback___:(ICEObject<ChatChatSession> *)self current:current is:is os:os];
        default:
            @throw [ICEOperationNotExistException operationNotExistException:__FILE__
                                                                             line:__LINE__
                                                                             id_:current.id_
                                                                             facet:current.facet
                                                                             operation:current.operation];
    }
}

+(NSString **) staticIds__:(int*)count idIndex:(int*)idx
{
    *count = sizeof(ChatChatSession_ids__) / sizeof(NSString *);
    *idx = 0;
    return ChatChatSession_ids__;
}
@end

@implementation ChatChatRoomCallbackPrx

-(void) init:(ICEStringSeq *)users
{
    [ChatChatRoomCallbackPrx init___:users prx:self context:nil];
}

-(void) init:(ICEStringSeq *)users context:(ICEContext *)ctx_
{
    [ChatChatRoomCallbackPrx init___:users prx:self context:ctx_];
}

-(BOOL) init_async:(id)target_ response:(SEL)response_ exception:(SEL)exception_ users:(ICEStringSeq *)users
{
    return [ChatChatRoomCallbackPrx init_async___:target_ response:response_ exception:exception_ sent:nil  users:users prx:self context:nil];
}

-(BOOL) init_async:(id)target_ response:(SEL)response_ exception:(SEL)exception_ users:(ICEStringSeq *)users context:(ICEContext *)ctx_
{
    return [ChatChatRoomCallbackPrx init_async___:target_ response:response_ exception:exception_ sent:nil  users:users prx:self context:ctx_];
}

-(BOOL) init_async:(id)target_ response:(SEL)response_ exception:(SEL)exception_ sent:(SEL)sent_ users:(ICEStringSeq *)users
{
    return [ChatChatRoomCallbackPrx init_async___:target_ response:response_ exception:exception_ sent:sent_  users:users prx:self context:nil];
}

-(BOOL) init_async:(id)target_ response:(SEL)response_ exception:(SEL)exception_ sent:(SEL)sent_ users:(ICEStringSeq *)users context:(ICEContext *)ctx_
{
    return [ChatChatRoomCallbackPrx init_async___:target_ response:response_ exception:exception_ sent:sent_  users:users prx:self context:ctx_];
}

-(void) join:(ICELong)timestamp name:(NSString *)name
{
    [ChatChatRoomCallbackPrx join___:timestamp name:name prx:self context:nil];
}

-(void) join:(ICELong)timestamp name:(NSString *)name context:(ICEContext *)ctx_
{
    [ChatChatRoomCallbackPrx join___:timestamp name:name prx:self context:ctx_];
}

-(BOOL) join_async:(id)target_ response:(SEL)response_ exception:(SEL)exception_ timestamp:(ICELong)timestamp name:(NSString *)name
{
    return [ChatChatRoomCallbackPrx join_async___:target_ response:response_ exception:exception_ sent:nil  timestamp:timestamp name:name prx:self context:nil];
}

-(BOOL) join_async:(id)target_ response:(SEL)response_ exception:(SEL)exception_ timestamp:(ICELong)timestamp name:(NSString *)name context:(ICEContext *)ctx_
{
    return [ChatChatRoomCallbackPrx join_async___:target_ response:response_ exception:exception_ sent:nil  timestamp:timestamp name:name prx:self context:ctx_];
}

-(BOOL) join_async:(id)target_ response:(SEL)response_ exception:(SEL)exception_ sent:(SEL)sent_ timestamp:(ICELong)timestamp name:(NSString *)name
{
    return [ChatChatRoomCallbackPrx join_async___:target_ response:response_ exception:exception_ sent:sent_  timestamp:timestamp name:name prx:self context:nil];
}

-(BOOL) join_async:(id)target_ response:(SEL)response_ exception:(SEL)exception_ sent:(SEL)sent_ timestamp:(ICELong)timestamp name:(NSString *)name context:(ICEContext *)ctx_
{
    return [ChatChatRoomCallbackPrx join_async___:target_ response:response_ exception:exception_ sent:sent_  timestamp:timestamp name:name prx:self context:ctx_];
}

-(void) leave:(ICELong)timestamp name:(NSString *)name
{
    [ChatChatRoomCallbackPrx leave___:timestamp name:name prx:self context:nil];
}

-(void) leave:(ICELong)timestamp name:(NSString *)name context:(ICEContext *)ctx_
{
    [ChatChatRoomCallbackPrx leave___:timestamp name:name prx:self context:ctx_];
}

-(BOOL) leave_async:(id)target_ response:(SEL)response_ exception:(SEL)exception_ timestamp:(ICELong)timestamp name:(NSString *)name
{
    return [ChatChatRoomCallbackPrx leave_async___:target_ response:response_ exception:exception_ sent:nil  timestamp:timestamp name:name prx:self context:nil];
}

-(BOOL) leave_async:(id)target_ response:(SEL)response_ exception:(SEL)exception_ timestamp:(ICELong)timestamp name:(NSString *)name context:(ICEContext *)ctx_
{
    return [ChatChatRoomCallbackPrx leave_async___:target_ response:response_ exception:exception_ sent:nil  timestamp:timestamp name:name prx:self context:ctx_];
}

-(BOOL) leave_async:(id)target_ response:(SEL)response_ exception:(SEL)exception_ sent:(SEL)sent_ timestamp:(ICELong)timestamp name:(NSString *)name
{
    return [ChatChatRoomCallbackPrx leave_async___:target_ response:response_ exception:exception_ sent:sent_  timestamp:timestamp name:name prx:self context:nil];
}

-(BOOL) leave_async:(id)target_ response:(SEL)response_ exception:(SEL)exception_ sent:(SEL)sent_ timestamp:(ICELong)timestamp name:(NSString *)name context:(ICEContext *)ctx_
{
    return [ChatChatRoomCallbackPrx leave_async___:target_ response:response_ exception:exception_ sent:sent_  timestamp:timestamp name:name prx:self context:ctx_];
}

-(void) send:(ICELong)timestamp name:(NSString *)name message:(NSString *)message
{
    [ChatChatRoomCallbackPrx send___:timestamp name:name message:message prx:self context:nil];
}

-(void) send:(ICELong)timestamp name:(NSString *)name message:(NSString *)message context:(ICEContext *)ctx_
{
    [ChatChatRoomCallbackPrx send___:timestamp name:name message:message prx:self context:ctx_];
}

-(BOOL) send_async:(id)target_ response:(SEL)response_ exception:(SEL)exception_ timestamp:(ICELong)timestamp name:(NSString *)name message:(NSString *)message
{
    return [ChatChatRoomCallbackPrx send_async___:target_ response:response_ exception:exception_ sent:nil  timestamp:timestamp name:name message:message prx:self context:nil];
}

-(BOOL) send_async:(id)target_ response:(SEL)response_ exception:(SEL)exception_ timestamp:(ICELong)timestamp name:(NSString *)name message:(NSString *)message context:(ICEContext *)ctx_
{
    return [ChatChatRoomCallbackPrx send_async___:target_ response:response_ exception:exception_ sent:nil  timestamp:timestamp name:name message:message prx:self context:ctx_];
}

-(BOOL) send_async:(id)target_ response:(SEL)response_ exception:(SEL)exception_ sent:(SEL)sent_ timestamp:(ICELong)timestamp name:(NSString *)name message:(NSString *)message
{
    return [ChatChatRoomCallbackPrx send_async___:target_ response:response_ exception:exception_ sent:sent_  timestamp:timestamp name:name message:message prx:self context:nil];
}

-(BOOL) send_async:(id)target_ response:(SEL)response_ exception:(SEL)exception_ sent:(SEL)sent_ timestamp:(ICELong)timestamp name:(NSString *)name message:(NSString *)message context:(ICEContext *)ctx_
{
    return [ChatChatRoomCallbackPrx send_async___:target_ response:response_ exception:exception_ sent:sent_  timestamp:timestamp name:name message:message prx:self context:ctx_];
}

+(NSString *) ice_staticId
{
    return @"::Chat::ChatRoomCallback";
}

+(void) init___:(ICEStringSeq *)users prx:(ICEObjectPrx <ChatChatRoomCallbackPrx> *)prx_ context:(ICEContext *)ctx_
{
    id<ICEOutputStream> os_ = [prx_ createOutputStream__];
    id<ICEInputStream> is_ = nil;
    @try
    {
        [os_ writeStringSeq:users];
        [prx_ invoke__:@"init" mode:ICENormal os:os_ is:&is_ context:ctx_];
    }
    @catch(ICEUserException *ex_)
    {
        @throw [ICEUnknownUserException unknownUserException:__FILE__ line:__LINE__ unknown:[ex_ ice_name]];
    }
    @finally
    {
        [os_ release];
        [is_ release];
    }
}

+(BOOL) init_async___:(id)target_ response:(SEL)response_ exception:(SEL)exception_ sent:(SEL)sent_ users:(ICEStringSeq *)users prx:(ICEObjectPrx <ChatChatRoomCallbackPrx> *)prx_ context:(ICEContext *)ctx_
{
    SEL finished_ = @selector(init_async_finished___:response:exception:ok:is:);
    id<ICEOutputStream> os_ = [prx_ createOutputStream__];
    @try
    {
        [os_ writeStringSeq:users];
        return [prx_ invoke_async__:target_ response:response_ exception:exception_ sent:sent_ finishedClass:self finished:finished_ operation:@"init" mode:ICENormal os:os_ context:ctx_];
    }
    @finally
    {
        [os_ release];
    }
    return FALSE; // Keep the compiler happy.
}

+(void) init_async_finished___:(id)target_ response:(SEL)response_ exception:(SEL)exception_ ok:(BOOL)ok_ is:(id<ICEInputStream>)is_
{
    @try
    {
        if(!ok_)
        {
            [is_ throwException];
        }
        objc_msgSend(target_, response_);
    }
    @catch(ICEUserException *ex_)
    {
        ICEUnknownUserException* uuex_;
        uuex_ = [ICEUnknownUserException unknownUserException:__FILE__ line:__LINE__ unknown:[ex_ ice_name]];
        objc_msgSend(target_, exception_, uuex_);
    }
}

+(void) send___:(ICELong)timestamp name:(NSString *)name message:(NSString *)message prx:(ICEObjectPrx <ChatChatRoomCallbackPrx> *)prx_ context:(ICEContext *)ctx_
{
    id<ICEOutputStream> os_ = [prx_ createOutputStream__];
    id<ICEInputStream> is_ = nil;
    @try
    {
        [os_ writeLong:timestamp];
        [os_ writeString:name];
        [os_ writeString:message];
        [prx_ invoke__:@"send" mode:ICENormal os:os_ is:&is_ context:ctx_];
    }
    @catch(ICEUserException *ex_)
    {
        @throw [ICEUnknownUserException unknownUserException:__FILE__ line:__LINE__ unknown:[ex_ ice_name]];
    }
    @finally
    {
        [os_ release];
        [is_ release];
    }
}

+(BOOL) send_async___:(id)target_ response:(SEL)response_ exception:(SEL)exception_ sent:(SEL)sent_ timestamp:(ICELong)timestamp name:(NSString *)name message:(NSString *)message prx:(ICEObjectPrx <ChatChatRoomCallbackPrx> *)prx_ context:(ICEContext *)ctx_
{
    SEL finished_ = @selector(send_async_finished___:response:exception:ok:is:);
    id<ICEOutputStream> os_ = [prx_ createOutputStream__];
    @try
    {
        [os_ writeLong:timestamp];
        [os_ writeString:name];
        [os_ writeString:message];
        return [prx_ invoke_async__:target_ response:response_ exception:exception_ sent:sent_ finishedClass:self finished:finished_ operation:@"send" mode:ICENormal os:os_ context:ctx_];
    }
    @finally
    {
        [os_ release];
    }
    return FALSE; // Keep the compiler happy.
}

+(void) send_async_finished___:(id)target_ response:(SEL)response_ exception:(SEL)exception_ ok:(BOOL)ok_ is:(id<ICEInputStream>)is_
{
    @try
    {
        if(!ok_)
        {
            [is_ throwException];
        }
        objc_msgSend(target_, response_);
    }
    @catch(ICEUserException *ex_)
    {
        ICEUnknownUserException* uuex_;
        uuex_ = [ICEUnknownUserException unknownUserException:__FILE__ line:__LINE__ unknown:[ex_ ice_name]];
        objc_msgSend(target_, exception_, uuex_);
    }
}

+(void) join___:(ICELong)timestamp name:(NSString *)name prx:(ICEObjectPrx <ChatChatRoomCallbackPrx> *)prx_ context:(ICEContext *)ctx_
{
    id<ICEOutputStream> os_ = [prx_ createOutputStream__];
    id<ICEInputStream> is_ = nil;
    @try
    {
        [os_ writeLong:timestamp];
        [os_ writeString:name];
        [prx_ invoke__:@"join" mode:ICENormal os:os_ is:&is_ context:ctx_];
    }
    @catch(ICEUserException *ex_)
    {
        @throw [ICEUnknownUserException unknownUserException:__FILE__ line:__LINE__ unknown:[ex_ ice_name]];
    }
    @finally
    {
        [os_ release];
        [is_ release];
    }
}

+(BOOL) join_async___:(id)target_ response:(SEL)response_ exception:(SEL)exception_ sent:(SEL)sent_ timestamp:(ICELong)timestamp name:(NSString *)name prx:(ICEObjectPrx <ChatChatRoomCallbackPrx> *)prx_ context:(ICEContext *)ctx_
{
    SEL finished_ = @selector(join_async_finished___:response:exception:ok:is:);
    id<ICEOutputStream> os_ = [prx_ createOutputStream__];
    @try
    {
        [os_ writeLong:timestamp];
        [os_ writeString:name];
        return [prx_ invoke_async__:target_ response:response_ exception:exception_ sent:sent_ finishedClass:self finished:finished_ operation:@"join" mode:ICENormal os:os_ context:ctx_];
    }
    @finally
    {
        [os_ release];
    }
    return FALSE; // Keep the compiler happy.
}

+(void) join_async_finished___:(id)target_ response:(SEL)response_ exception:(SEL)exception_ ok:(BOOL)ok_ is:(id<ICEInputStream>)is_
{
    @try
    {
        if(!ok_)
        {
            [is_ throwException];
        }
        objc_msgSend(target_, response_);
    }
    @catch(ICEUserException *ex_)
    {
        ICEUnknownUserException* uuex_;
        uuex_ = [ICEUnknownUserException unknownUserException:__FILE__ line:__LINE__ unknown:[ex_ ice_name]];
        objc_msgSend(target_, exception_, uuex_);
    }
}

+(void) leave___:(ICELong)timestamp name:(NSString *)name prx:(ICEObjectPrx <ChatChatRoomCallbackPrx> *)prx_ context:(ICEContext *)ctx_
{
    id<ICEOutputStream> os_ = [prx_ createOutputStream__];
    id<ICEInputStream> is_ = nil;
    @try
    {
        [os_ writeLong:timestamp];
        [os_ writeString:name];
        [prx_ invoke__:@"leave" mode:ICENormal os:os_ is:&is_ context:ctx_];
    }
    @catch(ICEUserException *ex_)
    {
        @throw [ICEUnknownUserException unknownUserException:__FILE__ line:__LINE__ unknown:[ex_ ice_name]];
    }
    @finally
    {
        [os_ release];
        [is_ release];
    }
}

+(BOOL) leave_async___:(id)target_ response:(SEL)response_ exception:(SEL)exception_ sent:(SEL)sent_ timestamp:(ICELong)timestamp name:(NSString *)name prx:(ICEObjectPrx <ChatChatRoomCallbackPrx> *)prx_ context:(ICEContext *)ctx_
{
    SEL finished_ = @selector(leave_async_finished___:response:exception:ok:is:);
    id<ICEOutputStream> os_ = [prx_ createOutputStream__];
    @try
    {
        [os_ writeLong:timestamp];
        [os_ writeString:name];
        return [prx_ invoke_async__:target_ response:response_ exception:exception_ sent:sent_ finishedClass:self finished:finished_ operation:@"leave" mode:ICENormal os:os_ context:ctx_];
    }
    @finally
    {
        [os_ release];
    }
    return FALSE; // Keep the compiler happy.
}

+(void) leave_async_finished___:(id)target_ response:(SEL)response_ exception:(SEL)exception_ ok:(BOOL)ok_ is:(id<ICEInputStream>)is_
{
    @try
    {
        if(!ok_)
        {
            [is_ throwException];
        }
        objc_msgSend(target_, response_);
    }
    @catch(ICEUserException *ex_)
    {
        ICEUnknownUserException* uuex_;
        uuex_ = [ICEUnknownUserException unknownUserException:__FILE__ line:__LINE__ unknown:[ex_ ice_name]];
        objc_msgSend(target_, exception_, uuex_);
    }
}
@end

@implementation ChatChatSessionPrx

-(ICELong) send:(NSString *)message
{
    return [ChatChatSessionPrx send___:message prx:self context:nil];
}

-(ICELong) send:(NSString *)message context:(ICEContext *)ctx_
{
    return [ChatChatSessionPrx send___:message prx:self context:ctx_];
}

-(BOOL) send_async:(id)target_ response:(SEL)response_ exception:(SEL)exception_ message:(NSString *)message
{
    return [ChatChatSessionPrx send_async___:target_ response:response_ exception:exception_ sent:nil  message:message prx:self context:nil];
}

-(BOOL) send_async:(id)target_ response:(SEL)response_ exception:(SEL)exception_ message:(NSString *)message context:(ICEContext *)ctx_
{
    return [ChatChatSessionPrx send_async___:target_ response:response_ exception:exception_ sent:nil  message:message prx:self context:ctx_];
}

-(BOOL) send_async:(id)target_ response:(SEL)response_ exception:(SEL)exception_ sent:(SEL)sent_ message:(NSString *)message
{
    return [ChatChatSessionPrx send_async___:target_ response:response_ exception:exception_ sent:sent_  message:message prx:self context:nil];
}

-(BOOL) send_async:(id)target_ response:(SEL)response_ exception:(SEL)exception_ sent:(SEL)sent_ message:(NSString *)message context:(ICEContext *)ctx_
{
    return [ChatChatSessionPrx send_async___:target_ response:response_ exception:exception_ sent:sent_  message:message prx:self context:ctx_];
}

-(void) setCallback:(id<ChatChatRoomCallbackPrx>)cb
{
    [ChatChatSessionPrx setCallback___:cb prx:self context:nil];
}

-(void) setCallback:(id<ChatChatRoomCallbackPrx>)cb context:(ICEContext *)ctx_
{
    [ChatChatSessionPrx setCallback___:cb prx:self context:ctx_];
}

-(BOOL) setCallback_async:(id)target_ response:(SEL)response_ exception:(SEL)exception_ cb:(id<ChatChatRoomCallbackPrx>)cb
{
    return [ChatChatSessionPrx setCallback_async___:target_ response:response_ exception:exception_ sent:nil  cb:cb prx:self context:nil];
}

-(BOOL) setCallback_async:(id)target_ response:(SEL)response_ exception:(SEL)exception_ cb:(id<ChatChatRoomCallbackPrx>)cb context:(ICEContext *)ctx_
{
    return [ChatChatSessionPrx setCallback_async___:target_ response:response_ exception:exception_ sent:nil  cb:cb prx:self context:ctx_];
}

-(BOOL) setCallback_async:(id)target_ response:(SEL)response_ exception:(SEL)exception_ sent:(SEL)sent_ cb:(id<ChatChatRoomCallbackPrx>)cb
{
    return [ChatChatSessionPrx setCallback_async___:target_ response:response_ exception:exception_ sent:sent_  cb:cb prx:self context:nil];
}

-(BOOL) setCallback_async:(id)target_ response:(SEL)response_ exception:(SEL)exception_ sent:(SEL)sent_ cb:(id<ChatChatRoomCallbackPrx>)cb context:(ICEContext *)ctx_
{
    return [ChatChatSessionPrx setCallback_async___:target_ response:response_ exception:exception_ sent:sent_  cb:cb prx:self context:ctx_];
}

-(void) destroy
{
    [Glacier2SessionPrx destroy___:self context:nil];
}

-(void) destroy:(ICEContext *)ctx_
{
    [Glacier2SessionPrx destroy___:self context:ctx_];
}

-(BOOL) destroy_async:(id)target_ response:(SEL)response_ exception:(SEL)exception_
{
    return [Glacier2SessionPrx destroy_async___:target_ response:response_ exception:exception_ sent:nil  prx:self context:nil];
}

-(BOOL) destroy_async:(id)target_ response:(SEL)response_ exception:(SEL)exception_ context:(ICEContext *)ctx_
{
    return [Glacier2SessionPrx destroy_async___:target_ response:response_ exception:exception_ sent:nil  prx:self context:ctx_];
}

-(BOOL) destroy_async:(id)target_ response:(SEL)response_ exception:(SEL)exception_ sent:(SEL)sent_
{
    return [Glacier2SessionPrx destroy_async___:target_ response:response_ exception:exception_ sent:sent_  prx:self context:nil];
}

-(BOOL) destroy_async:(id)target_ response:(SEL)response_ exception:(SEL)exception_ sent:(SEL)sent_ context:(ICEContext *)ctx_
{
    return [Glacier2SessionPrx destroy_async___:target_ response:response_ exception:exception_ sent:sent_  prx:self context:ctx_];
}

+(NSString *) ice_staticId
{
    return @"::Chat::ChatSession";
}

+(void) setCallback___:(id<ChatChatRoomCallbackPrx>)cb prx:(ICEObjectPrx <ChatChatSessionPrx> *)prx_ context:(ICEContext *)ctx_
{
    id<ICEOutputStream> os_ = [prx_ createOutputStream__];
    id<ICEInputStream> is_ = nil;
    @try
    {
        [os_ writeProxy:(id<ICEObjectPrx>)cb];
        [prx_ invoke__:@"setCallback" mode:ICENormal os:os_ is:&is_ context:ctx_];
    }
    @catch(ICEUserException *ex_)
    {
        @throw [ICEUnknownUserException unknownUserException:__FILE__ line:__LINE__ unknown:[ex_ ice_name]];
    }
    @finally
    {
        [os_ release];
        [is_ release];
    }
}

+(BOOL) setCallback_async___:(id)target_ response:(SEL)response_ exception:(SEL)exception_ sent:(SEL)sent_ cb:(id<ChatChatRoomCallbackPrx>)cb prx:(ICEObjectPrx <ChatChatSessionPrx> *)prx_ context:(ICEContext *)ctx_
{
    SEL finished_ = @selector(setCallback_async_finished___:response:exception:ok:is:);
    id<ICEOutputStream> os_ = [prx_ createOutputStream__];
    @try
    {
        [os_ writeProxy:(id<ICEObjectPrx>)cb];
        return [prx_ invoke_async__:target_ response:response_ exception:exception_ sent:sent_ finishedClass:self finished:finished_ operation:@"setCallback" mode:ICENormal os:os_ context:ctx_];
    }
    @finally
    {
        [os_ release];
    }
    return FALSE; // Keep the compiler happy.
}

+(void) setCallback_async_finished___:(id)target_ response:(SEL)response_ exception:(SEL)exception_ ok:(BOOL)ok_ is:(id<ICEInputStream>)is_
{
    @try
    {
        if(!ok_)
        {
            [is_ throwException];
        }
        objc_msgSend(target_, response_);
    }
    @catch(ICEUserException *ex_)
    {
        ICEUnknownUserException* uuex_;
        uuex_ = [ICEUnknownUserException unknownUserException:__FILE__ line:__LINE__ unknown:[ex_ ice_name]];
        objc_msgSend(target_, exception_, uuex_);
    }
}

+(ICELong) send___:(NSString *)message prx:(ICEObjectPrx <ChatChatSessionPrx> *)prx_ context:(ICEContext *)ctx_
{
    [prx_ checkTwowayOnly__:@"send"];
    id<ICEOutputStream> os_ = [prx_ createOutputStream__];
    id<ICEInputStream> is_ = nil;
    ICELong ret_;
    @try
    {
        [os_ writeString:message];
        [prx_ invoke__:@"send" mode:ICENormal os:os_ is:&is_ context:ctx_];
        ret_ = [is_ readLong];
    }
    @catch(ChatInvalidMessageException *ex_)
    {
        @throw;
    }
    @catch(ICEUserException *ex_)
    {
        @throw [ICEUnknownUserException unknownUserException:__FILE__ line:__LINE__ unknown:[ex_ ice_name]];
    }
    @finally
    {
        [os_ release];
        [is_ release];
    }
    return ret_;
}

+(BOOL) send_async___:(id)target_ response:(SEL)response_ exception:(SEL)exception_ sent:(SEL)sent_ message:(NSString *)message prx:(ICEObjectPrx <ChatChatSessionPrx> *)prx_ context:(ICEContext *)ctx_
{
    [prx_ checkTwowayOnly__:@"send"];
    SEL finished_ = @selector(send_async_finished___:response:exception:ok:is:);
    id<ICEOutputStream> os_ = [prx_ createOutputStream__];
    @try
    {
        [os_ writeString:message];
        return [prx_ invoke_async__:target_ response:response_ exception:exception_ sent:sent_ finishedClass:self finished:finished_ operation:@"send" mode:ICENormal os:os_ context:ctx_];
    }
    @finally
    {
        [os_ release];
    }
    return FALSE; // Keep the compiler happy.
}

+(void) send_async_finished___:(id)target_ response:(SEL)response_ exception:(SEL)exception_ ok:(BOOL)ok_ is:(id<ICEInputStream>)is_
{
    ICELong ret_;
    @try
    {
        if(!ok_)
        {
            [is_ throwException];
        }
        ret_ = [is_ readLong];
        ((void(*)(id, SEL, ICELong))objc_msgSend)(target_, response_, ret_);
    }
    @catch(ChatInvalidMessageException *ex_)
    {
        objc_msgSend(target_, exception_, ex_);
    }
    @catch(ICEUserException *ex_)
    {
        ICEUnknownUserException* uuex_;
        uuex_ = [ICEUnknownUserException unknownUserException:__FILE__ line:__LINE__ unknown:[ex_ ice_name]];
        objc_msgSend(target_, exception_, uuex_);
    }
}
@end
