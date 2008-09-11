// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Config.h>
#import <Ice/Stream.h>

//
// Forward declarations
//
@protocol ICEObjectAdapter;
@protocol ICEConnection;
@class ICEIdentity;

typedef NSDictionary ICEContext;
typedef NSMutableDictionary ICEMutableContext;

typedef enum
{
    ICENormal,
    ICENonmutating,
    ICEIdempotent
} ICEOperationMode;

@interface ICECurrent : NSObject <NSCopying>
{
    @private
        id<ICEObjectAdapter> adapter;
        id<ICEConnection> con;
        ICEIdentity *id_;
        NSString *facet;
        NSString *operation;
        ICEOperationMode mode;
        NSDictionary *ctx;
        ICEInt requestId;
}

@property(nonatomic, retain) id<ICEObjectAdapter> adapter;
@property(nonatomic, retain) id<ICEConnection> con;
@property(nonatomic, retain) ICEIdentity *id_;
@property(nonatomic, retain) NSString *facet;
@property(nonatomic, retain) NSString *operation;
@property(nonatomic, assign) ICEOperationMode mode;
@property(nonatomic, retain) NSDictionary *ctx;
@property(nonatomic, assign) ICEInt requestId;

-(id) init:(id<ICEObjectAdapter>)adapter con:(id<ICEConnection>)con_ id_:(ICEIdentity *)id_ facet:(NSString *)facet operation:(NSString *)operation mode:(ICEOperationMode)mode ctx:(NSDictionary *)ctx requestId:(ICEInt)requestId;
+(id) current:(id<ICEObjectAdapter>)adapter con:(id<ICEConnection>)con_ id_:(ICEIdentity *)id_ facet:(NSString *)facet operation:(NSString *)operation mode:(ICEOperationMode)mode ctx:(NSDictionary *)ctx requestId:(ICEInt)requestId;
+(id) current;
-(id) copyWithZone:(NSZone *)zone;
-(NSUInteger) hash;
-(BOOL) isEqual:(id)anObject;
-(void) dealloc;
@end

@interface ICEContextHelper : ICEDictionaryHelper
+(ICEKeyValueTypeHelper) getContained;
@end

@interface ICEOperationModeHelper : ICEEnumHelper
+(ICEInt) getLimit;
@end
