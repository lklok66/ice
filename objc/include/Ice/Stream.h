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
-(NSMutableData*) readByteSeq;
-(NSData*) readByteSeqNoCopy;

-(ICEShort) readShort;
-(NSMutableData*) readShortSeq;

-(ICEInt) readInt;
-(NSMutableData*) readIntSeq;

-(ICELong) readLong;
-(NSMutableData*) readLongSeq;

-(ICEFloat) readFloat;
-(NSMutableData*) readFloatSeq;

-(ICEDouble) readDouble;
-(NSMutableData*) readDoubleSeq;

-(NSMutableString*) readString;
-(NSMutableArray*) readStringSeq;

-(NSMutableArray*) readSequence: (Class)c;

-(ICEInt) readEnumerator:(ICEInt)limit;

-(ICEInt) readSize;

-(ICEObjectPrx*) readProxy:(Class)c;

-(void) readObject:(id<ICEReadObjectCallback>)callback;

-(NSString*) readTypeId;

-(void) throwException;

-(void) startSeq:(ICEInt)numElements minSize:(ICEInt)minSize;
-(void) checkSeq;
-(void) checkSeq:(ICEInt)bytesLeft;
-(void) checkFixedSeq:(ICEInt)numElements elemSize:(ICEInt)elemSize;
-(void) endElement;
-(void) endSeq:(ICEInt)size;

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
-(void) writeByteSeq:(NSData*)v;

-(void) writeShort:(ICEShort)v;
-(void) writeShortSeq:(NSData*)v;

-(void) writeInt:(ICEInt)v;
-(void) writeIntSeq:(NSData*)v;

-(void) writeLong:(ICELong)v;
-(void) writeLongSeq:(NSData*)v;

-(void) writeFloat:(ICEFloat)v;
-(void) writeFloatSeq:(NSData*)v;

-(void) writeDouble:(ICEDouble)v;
-(void) writeDoubleSeq:(NSData*)v;

-(void) writeString:(NSString*)v;
-(void) writeStringSeq:(NSArray*)v;

-(void) writeSequence:(NSArray*)arr class:(Class)cl;

-(void) writeEnumerator:(ICEInt)v limit:(ICEInt)limit;

-(void) writeSize:(ICEInt)v;

-(void) writeProxy:(ICEObjectPrx*)v;

-(void) writeObject:(ICEObject*)v;

-(void) writeTypeId:(const char*)v;

-(void) writeException:(ICEUserException*)v;

// -(void) startSeq:(ICEInt)numElements minSize:(ICEInt)minSize;
// -(void) checkSeq;
// -(void) checkSeq:(ICEInt)bytesLeft;
// -(void) checkFixedSeq:(ICEInt)numElements elemSize:(ICEInt)elemSize;
// -(void) endEleemnt;
// -(void) endSeq;

-(void) startSlice;
-(void) endSlice;

-(void) startEncapsulation;
-(void) endEncapsulation;

-(void) writePendingObjects;

-(NSData*) finished;

@end
