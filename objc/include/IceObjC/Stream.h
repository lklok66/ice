// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <IceObjC/Config.h>

#import <Foundation/NSData.h>

//
// Forward declarations
//
@class ICEObject;
@class ICEObjectPrx;
@class ICECommunicator;
@class ICEUserException;

@interface ICEInputStream : NSObject
{
    void* is__;
}

-(ICECommunicator*) communicator;

-(void) sliceObjects:(BOOL)b;

-(BOOL) readBool;
-(NSArray*) readBoolSeq;

-(ICEByte) readByte;
-(NSArray*) readByteSeq;

-(ICEShort) readShort;
-(NSArray*) readShortSeq;

-(ICEInt) readInt;
-(NSArray*) readIntSeq;

-(ICELong) readLong;
-(NSArray*) readLongSeq;

-(ICEFloat) readFloat;
-(NSArray*) readFloatSeq;

-(ICEDouble) readDouble;
-(NSArray*) readDoubleSeq;

-(NSString*) readString;
-(NSArray*) readStringSeq;

-(ICEInt) readSize;

-(ICEObjectPrx*) readProxy;

//-(void) readObject(const ICEReadObjectCallbackPtr&);

-(NSString*) readTypeId;

-(void) throwException;

-(void) startSlice;
-(void) endSlice;
-(void) skipSlice;

-(void) startEncapsulation;
-(void) endEncapsulation;
-(void) skipEncapsulation;

-(void) readPendingObjects;
@end

@interface ICEOutputStream : NSObject
{
    void* os__;
}

-(ICECommunicator*) communicator;

-(void) writeBool:(BOOL)v;
-(void) writeBoolSeq:(NSArray*)v;

-(void) writeByte:(ICEByte)v;
-(void) writeByteSeq:(NSArray*)v;

-(void) writeShort:(ICEShort)v;
-(void) writeShortSeq:(NSArray*)v;

-(void) writeInt:(ICEInt)v;
-(void) writeIntSeq:(NSArray*)v;

-(void) writeLong:(ICELong)v;
-(void) writeLongSeq:(NSArray*)v;

-(void) writeFloat:(ICEFloat)v;
-(void) writeFloatSeq:(NSArray*)v;

-(void) writeDouble:(ICEDouble)v;
-(void) writeDoubleSeq:(NSArray*)v;

-(void) writeString:(NSString*)v;
-(void) writeStringSeq:(NSArray*)v;

-(void) writeSize:(ICEInt)v;

-(void) writeProxy:(ICEObjectPrx*)v;

//-(void) writeObject:(ICEObject*)v;

-(void) writeTypeId:(NSString*)v;

-(void) writeException:(ICEUserException*)v;

-(void) startSlice;
-(void) endSlice;

-(void) startEncapsulation;
-(void) endEncapsulation;

-(void) writePendingObjects;

-(NSData*) finished;

@end
