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

#import <Ice/Config.h>
#import <Ice/Proxy.h>
#import <Ice/Object.h>
#import <Ice/Current.h>
#import <Ice/Exception.h>
#import <Ice/Stream.h>
#import <Ice/BuiltinSequences.h>
#import <Glacier2/Session.h>
#import <Chat.h>

@class ChatChatRoomCallback;
@protocol ChatChatRoomCallback;

@class ChatChatSession;
@protocol ChatChatSession;

@class ChatChatRoomCallbackPrx;
@protocol ChatChatRoomCallbackPrx;

@class ChatChatSessionPrx;
@protocol ChatChatSessionPrx;

@protocol ChatChatRoomCallback <ICEObject>
-(void) init:(ICEMutableStringSeq *)users current:(ICECurrent *)current;
-(void) send:(ICELong)timestamp name:(NSMutableString *)name message:(NSMutableString *)message current:(ICECurrent *)current;
-(void) join:(ICELong)timestamp name:(NSMutableString *)name current:(ICECurrent *)current;
-(void) leave:(ICELong)timestamp name:(NSMutableString *)name current:(ICECurrent *)current;
@end

@interface ChatChatRoomCallback : ICEObject
-(id) init;
+(BOOL)init___:(ICEObject<ChatChatRoomCallback> *)servant current:(ICECurrent *)current is:(id<ICEInputStream>)is_ os:(id<ICEOutputStream>)os_;
+(BOOL)send___:(ICEObject<ChatChatRoomCallback> *)servant current:(ICECurrent *)current is:(id<ICEInputStream>)is_ os:(id<ICEOutputStream>)os_;
+(BOOL)join___:(ICEObject<ChatChatRoomCallback> *)servant current:(ICECurrent *)current is:(id<ICEInputStream>)is_ os:(id<ICEOutputStream>)os_;
+(BOOL)leave___:(ICEObject<ChatChatRoomCallback> *)servant current:(ICECurrent *)current is:(id<ICEInputStream>)is_ os:(id<ICEOutputStream>)os_;
-(void) write__:(id<ICEOutputStream>)stream;
-(void) read__:(id<ICEInputStream>)stream readTypeId:(BOOL)rid_;
@end

@protocol ChatChatSession <Glacier2Session>
-(void) setCallback:(id<ChatChatRoomCallbackPrx>)cb current:(ICECurrent *)current;
-(ICELong) send:(NSMutableString *)message current:(ICECurrent *)current;
@end

@interface ChatChatSession : ICEObject
-(id) init;
+(BOOL)setCallback___:(ICEObject<ChatChatSession> *)servant current:(ICECurrent *)current is:(id<ICEInputStream>)is_ os:(id<ICEOutputStream>)os_;
+(BOOL)send___:(ICEObject<ChatChatSession> *)servant current:(ICECurrent *)current is:(id<ICEInputStream>)is_ os:(id<ICEOutputStream>)os_;
-(void) write__:(id<ICEOutputStream>)stream;
-(void) read__:(id<ICEInputStream>)stream readTypeId:(BOOL)rid_;
@end

@protocol ChatChatRoomCallbackPrx <ICEObjectPrx>
-(void) init:(ICEStringSeq *)users;
-(void) init:(ICEStringSeq *)users context:(ICEContext *)context;
-(BOOL) init_async:(id)target_ response:(SEL)response_ exception:(SEL)exception_ users:(ICEStringSeq *)users;
-(BOOL) init_async:(id)target_ response:(SEL)response_ exception:(SEL)exception_ users:(ICEStringSeq *)users context:(ICEContext *)context;
-(BOOL) init_async:(id)target_ response:(SEL)response_ exception:(SEL)exception_ sent:(SEL)sent_ users:(ICEStringSeq *)users;
-(BOOL) init_async:(id)target_ response:(SEL)response_ exception:(SEL)exception_ sent:(SEL)sent_ users:(ICEStringSeq *)users context:(ICEContext *)context;
-(void) send:(ICELong)timestamp name:(NSString *)name message:(NSString *)message;
-(void) send:(ICELong)timestamp name:(NSString *)name message:(NSString *)message context:(ICEContext *)context;
-(BOOL) send_async:(id)target_ response:(SEL)response_ exception:(SEL)exception_ timestamp:(ICELong)timestamp name:(NSString *)name message:(NSString *)message;
-(BOOL) send_async:(id)target_ response:(SEL)response_ exception:(SEL)exception_ timestamp:(ICELong)timestamp name:(NSString *)name message:(NSString *)message context:(ICEContext *)context;
-(BOOL) send_async:(id)target_ response:(SEL)response_ exception:(SEL)exception_ sent:(SEL)sent_ timestamp:(ICELong)timestamp name:(NSString *)name message:(NSString *)message;
-(BOOL) send_async:(id)target_ response:(SEL)response_ exception:(SEL)exception_ sent:(SEL)sent_ timestamp:(ICELong)timestamp name:(NSString *)name message:(NSString *)message context:(ICEContext *)context;
-(void) join:(ICELong)timestamp name:(NSString *)name;
-(void) join:(ICELong)timestamp name:(NSString *)name context:(ICEContext *)context;
-(BOOL) join_async:(id)target_ response:(SEL)response_ exception:(SEL)exception_ timestamp:(ICELong)timestamp name:(NSString *)name;
-(BOOL) join_async:(id)target_ response:(SEL)response_ exception:(SEL)exception_ timestamp:(ICELong)timestamp name:(NSString *)name context:(ICEContext *)context;
-(BOOL) join_async:(id)target_ response:(SEL)response_ exception:(SEL)exception_ sent:(SEL)sent_ timestamp:(ICELong)timestamp name:(NSString *)name;
-(BOOL) join_async:(id)target_ response:(SEL)response_ exception:(SEL)exception_ sent:(SEL)sent_ timestamp:(ICELong)timestamp name:(NSString *)name context:(ICEContext *)context;
-(void) leave:(ICELong)timestamp name:(NSString *)name;
-(void) leave:(ICELong)timestamp name:(NSString *)name context:(ICEContext *)context;
-(BOOL) leave_async:(id)target_ response:(SEL)response_ exception:(SEL)exception_ timestamp:(ICELong)timestamp name:(NSString *)name;
-(BOOL) leave_async:(id)target_ response:(SEL)response_ exception:(SEL)exception_ timestamp:(ICELong)timestamp name:(NSString *)name context:(ICEContext *)context;
-(BOOL) leave_async:(id)target_ response:(SEL)response_ exception:(SEL)exception_ sent:(SEL)sent_ timestamp:(ICELong)timestamp name:(NSString *)name;
-(BOOL) leave_async:(id)target_ response:(SEL)response_ exception:(SEL)exception_ sent:(SEL)sent_ timestamp:(ICELong)timestamp name:(NSString *)name context:(ICEContext *)context;
@end

@protocol ChatChatSessionPrx <Glacier2SessionPrx>
-(void) setCallback:(id<ChatChatRoomCallbackPrx>)cb;
-(void) setCallback:(id<ChatChatRoomCallbackPrx>)cb context:(ICEContext *)context;
-(BOOL) setCallback_async:(id)target_ response:(SEL)response_ exception:(SEL)exception_ cb:(id<ChatChatRoomCallbackPrx>)cb;
-(BOOL) setCallback_async:(id)target_ response:(SEL)response_ exception:(SEL)exception_ cb:(id<ChatChatRoomCallbackPrx>)cb context:(ICEContext *)context;
-(BOOL) setCallback_async:(id)target_ response:(SEL)response_ exception:(SEL)exception_ sent:(SEL)sent_ cb:(id<ChatChatRoomCallbackPrx>)cb;
-(BOOL) setCallback_async:(id)target_ response:(SEL)response_ exception:(SEL)exception_ sent:(SEL)sent_ cb:(id<ChatChatRoomCallbackPrx>)cb context:(ICEContext *)context;
-(ICELong) send:(NSString *)message;
-(ICELong) send:(NSString *)message context:(ICEContext *)context;
-(BOOL) send_async:(id)target_ response:(SEL)response_ exception:(SEL)exception_ message:(NSString *)message;
-(BOOL) send_async:(id)target_ response:(SEL)response_ exception:(SEL)exception_ message:(NSString *)message context:(ICEContext *)context;
-(BOOL) send_async:(id)target_ response:(SEL)response_ exception:(SEL)exception_ sent:(SEL)sent_ message:(NSString *)message;
-(BOOL) send_async:(id)target_ response:(SEL)response_ exception:(SEL)exception_ sent:(SEL)sent_ message:(NSString *)message context:(ICEContext *)context;
@end

@interface ChatChatRoomCallbackPrx : ICEObjectPrx <ChatChatRoomCallbackPrx>
+(NSString *) ice_staticId;
+(void) init___:(ICEStringSeq *)users prx:(ICEObjectPrx <ChatChatRoomCallbackPrx> *)prx context:(ICEContext *)ctx;
+(BOOL) init_async___:(id)target_ response:(SEL)response_ exception:(SEL)exception_ sent:(SEL)sent_ users:(ICEStringSeq *)users prx:(ICEObjectPrx <ChatChatRoomCallbackPrx> *)prx context:(ICEContext *)ctx;
+(void) init_async_finished___:(id)target response:(SEL)response exception:(SEL)exception ok:(BOOL)ok is:(id<ICEInputStream>)is;
+(void) send___:(ICELong)timestamp name:(NSString *)name message:(NSString *)message prx:(ICEObjectPrx <ChatChatRoomCallbackPrx> *)prx context:(ICEContext *)ctx;
+(BOOL) send_async___:(id)target_ response:(SEL)response_ exception:(SEL)exception_ sent:(SEL)sent_ timestamp:(ICELong)timestamp name:(NSString *)name message:(NSString *)message prx:(ICEObjectPrx <ChatChatRoomCallbackPrx> *)prx context:(ICEContext *)ctx;
+(void) send_async_finished___:(id)target response:(SEL)response exception:(SEL)exception ok:(BOOL)ok is:(id<ICEInputStream>)is;
+(void) join___:(ICELong)timestamp name:(NSString *)name prx:(ICEObjectPrx <ChatChatRoomCallbackPrx> *)prx context:(ICEContext *)ctx;
+(BOOL) join_async___:(id)target_ response:(SEL)response_ exception:(SEL)exception_ sent:(SEL)sent_ timestamp:(ICELong)timestamp name:(NSString *)name prx:(ICEObjectPrx <ChatChatRoomCallbackPrx> *)prx context:(ICEContext *)ctx;
+(void) join_async_finished___:(id)target response:(SEL)response exception:(SEL)exception ok:(BOOL)ok is:(id<ICEInputStream>)is;
+(void) leave___:(ICELong)timestamp name:(NSString *)name prx:(ICEObjectPrx <ChatChatRoomCallbackPrx> *)prx context:(ICEContext *)ctx;
+(BOOL) leave_async___:(id)target_ response:(SEL)response_ exception:(SEL)exception_ sent:(SEL)sent_ timestamp:(ICELong)timestamp name:(NSString *)name prx:(ICEObjectPrx <ChatChatRoomCallbackPrx> *)prx context:(ICEContext *)ctx;
+(void) leave_async_finished___:(id)target response:(SEL)response exception:(SEL)exception ok:(BOOL)ok is:(id<ICEInputStream>)is;
@end

@interface ChatChatSessionPrx : ICEObjectPrx <ChatChatSessionPrx>
+(NSString *) ice_staticId;
+(void) setCallback___:(id<ChatChatRoomCallbackPrx>)cb prx:(ICEObjectPrx <ChatChatSessionPrx> *)prx context:(ICEContext *)ctx;
+(BOOL) setCallback_async___:(id)target_ response:(SEL)response_ exception:(SEL)exception_ sent:(SEL)sent_ cb:(id<ChatChatRoomCallbackPrx>)cb prx:(ICEObjectPrx <ChatChatSessionPrx> *)prx context:(ICEContext *)ctx;
+(void) setCallback_async_finished___:(id)target response:(SEL)response exception:(SEL)exception ok:(BOOL)ok is:(id<ICEInputStream>)is;
+(ICELong) send___:(NSString *)message prx:(ICEObjectPrx <ChatChatSessionPrx> *)prx context:(ICEContext *)ctx;
+(BOOL) send_async___:(id)target_ response:(SEL)response_ exception:(SEL)exception_ sent:(SEL)sent_ message:(NSString *)message prx:(ICEObjectPrx <ChatChatSessionPrx> *)prx context:(ICEContext *)ctx;
+(void) send_async_finished___:(id)target response:(SEL)response exception:(SEL)exception ok:(BOOL)ok is:(id<ICEInputStream>)is;
@end
