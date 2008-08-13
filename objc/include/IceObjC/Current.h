// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <IceObjC/Config.h>

//
// Forward declarations
//
@protocol ICEObjectAdapter;
@class ICEIdentity;

typedef enum
{
    ICENormal,
    ICENonmutating,
    ICEIdempotent
} 
ICEOperationMode;

@interface ICECurrent : NSObject
{
    id<ICEObjectAdapter> adapter;
    ICEIdentity* id_;
    NSString* facet;
    NSString* operation;
    ICEOperationMode mode;
    NSDictionary* ctx;
    ICEInt requestId;
}
@property(retain, nonatomic) id<ICEObjectAdapter> adapter;
@property(retain, nonatomic) ICEIdentity* id_;
@property(retain, nonatomic) NSString* facet;
@property(retain, nonatomic) NSString* operation;
@property(assign, nonatomic) ICEOperationMode mode;
@property(retain, nonatomic) NSDictionary* ctx;
@property(assign, nonatomic) ICEInt requestId;
@end
