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

enum ICEOperationMode
{
    ICENormal,
    ICENonmutating,
    ICEIdempotent
};

@interface ICECurrent : NSObject
{
    id<ICEObjectAdapter> adapter;
    ICEIdentity* id_objc_;
    NSString* facet;
    NSString* operation;
    enum ICEOperationMode mode;
    NSDictionary* ctx;
    ICEInt requestId;
}
@property(readonly, nonatomic) id<ICEObjectAdapter> adapter;
@property(readonly, nonatomic) ICEIdentity* id_objc_;
@property(readonly, nonatomic) NSString* facet;
@property(readonly, nonatomic) NSString* operation;
@property(readonly, nonatomic) enum ICEOperationMode mode;
@property(readonly, nonatomic) NSDictionary* ctx;
@property(readonly, nonatomic) ICEInt requestId;
@end
