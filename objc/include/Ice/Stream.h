// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Config.h>

#import <Foundation/NSData.h>

//
// Forward declarations
//
@class ICEObject;
@class ICEObjectPrx;
@protocol ICECommunicator;
@class ICEUserException;

@protocol ICEReadObjectCallback <NSObject>
-(void)invoke:(ICEObject*)obj;
@end

@protocol ICEInputStream <NSObject>

-(id<ICECommunicator>) communicator;

-(void) sliceObjects:(BOOL)b;

-(BOOL) readBool;
-(NSMutableData*) readBoolSeq;

-(ICEByte) readByte;
-(ICEByte) readByte:(ICEInt)limit;
-(NSMutableData*) readByteSeq;
-(NSData*) readByteSeqNoCopy;

-(ICEShort) readShort;
-(ICEShort) readShort:(ICEInt)limit;
-(NSMutableData*) readShortSeq;

-(ICEInt) readInt;
-(ICEInt) readInt:(ICEInt)limit;
-(NSMutableData*) readIntSeq;

-(ICELong) readLong;
-(NSMutableData*) readLongSeq;

-(ICEFloat) readFloat;
-(NSMutableData*) readFloatSeq;

-(ICEDouble) readDouble;
-(NSMutableData*) readDoubleSeq;

-(NSMutableString*) readString;
-(NSMutableArray*) readStringSeq;

-(ICEInt) readSize;

-(ICEObjectPrx*) readProxy;

-(void) readObject:(id<ICEReadObjectCallback>)callback;

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

@protocol ICEOutputStream <NSObject>

-(id<ICECommunicator>) communicator;

-(void) writeBool:(BOOL)v;
-(void) writeBoolSeq:(NSData*)v;

-(void) writeByte:(ICEByte)v;
-(void) writeByte:(ICEByte)v limit:(ICEInt)limit;
-(void) writeByteSeq:(NSData*)v;

-(void) writeShort:(ICEShort)v;
-(void) writeShort:(ICEShort)v limit:(ICEInt)limit;
-(void) writeShortSeq:(NSData*)v;

-(void) writeInt:(ICEInt)v;
-(void) writeInt:(ICEInt)v limit:(ICEInt)limit;
-(void) writeIntSeq:(NSData*)v;

-(void) writeLong:(ICELong)v;
-(void) writeLongSeq:(NSData*)v;

-(void) writeFloat:(ICEFloat)v;
-(void) writeFloatSeq:(NSData*)v;

-(void) writeDouble:(ICEDouble)v;
-(void) writeDoubleSeq:(NSData*)v;

-(void) writeString:(NSString*)v;
-(void) writeStringSeq:(NSArray*)v;

-(void) writeSize:(ICEInt)v;

-(void) writeProxy:(ICEObjectPrx*)v;

-(void) writeObject:(ICEObject*)v;

-(void) writeTypeId:(const char*)v;

-(void) writeException:(ICEUserException*)v;

-(void) startSlice;
-(void) endSlice;

-(void) startEncapsulation;
-(void) endEncapsulation;

-(void) writePendingObjects;

-(NSData*) finished;

@end
