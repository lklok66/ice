// **********************************************************************
//
// Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Foundation/Foundation.h>

@class ICEException;

enum LogEntryType
{
    LogEntryTypeTrace,
    LogEntryTypeWarning,
    LogEntryTypeError,
    LogEntryTypePrint,
};
typedef enum LogEntryType LogEntryType;

@interface LogEntry : NSObject
{
    LogEntryType type;
    NSDate* timestamp;
    NSString* category;
    NSString* message;
}

@property (nonatomic, readonly) LogEntryType type;
@property (nonatomic, readonly) NSDate* timestamp;
@property (nonatomic, readonly) NSString* category;
@property (nonatomic, readonly) NSString* message;

+(id)logEntryPrint:(NSString*)message;
+(id)logEntryTrace:(NSString*)message category:(NSString*)category;
+(id)logEntryWarning:(NSString*)message;
+(id)logEntryError:(NSString*)message;

@end

@protocol LoggingDelegate<NSObject>

-(void)log:(LogEntry*)s;

@end
