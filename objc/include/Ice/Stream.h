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

typedef struct
{
    Class key;
    Class value;
} ICEKeyValueHelper;

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

-(ICEInt) readEnumerator:(ICEInt)limit;
-(NSMutableData*) readEnumSeq:(ICEInt)limit;

-(ICEObjectPrx*) readProxy:(Class)c;

-(void) readObject:(id<ICEReadObjectCallback>)callback;
-(NSMutableArray*) readObjectSeq;
-(NSMutableDictionary*) readObjectDict:(Class)c;

-(NSMutableArray*) readSequence:(Class)c;

-(NSMutableDictionary*) readDictionary:(ICEKeyValueHelper)c;

-(ICEInt) readSize;

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

-(void) writeEnumerator:(ICEInt)v limit:(ICEInt)limit;
-(void) writeEnumSeq:(NSData*)v limit:(ICEInt)limit;

-(void) writeProxy:(ICEObjectPrx*)v;

-(void) writeObject:(ICEObject*)v;
-(void) writeObjectSeq:(NSArray*)v;
-(void) writeObjectDict:(NSDictionary*)v c:(Class)c;

-(void) writeSequence:(NSArray*)arr c:(Class)c;

-(void) writeDictionary:(NSDictionary*)dictionary c:(ICEKeyValueHelper)c;

-(void) writeSize:(ICEInt)v;

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

//
// Helper classes for the Slice built-in types.
//

@interface ICEBoolHelper : NSObject
+(id) readWithStream:(id<ICEInputStream>)stream;
+(void) writeWithStream:(id)obj stream:(id<ICEOutputStream>)stream;
@end

@interface ICEByteHelper : NSObject
+(id) readWithStream:(id<ICEInputStream>)stream;
+(void) writeWithStream:(id)obj stream:(id<ICEOutputStream>)stream;
@end

@interface ICEShortHelper : NSObject
+(id) readWithStream:(id<ICEInputStream>)stream;
+(void) writeWithStream:(id)obj stream:(id<ICEOutputStream>)stream;
@end

@interface ICEIntHelper : NSObject
+(id) readWithStream:(id<ICEInputStream>)stream;
+(void) writeWithStream:(id)obj stream:(id<ICEOutputStream>)stream;
@end

@interface ICELongHelper : NSObject
+(id) readWithStream:(id<ICEInputStream>)stream;
+(void) writeWithStream:(id)obj stream:(id<ICEOutputStream>)stream;
@end

@interface ICEFloatHelper : NSObject
+(id) readWithStream:(id<ICEInputStream>)stream;
+(void) writeWithStream:(id)obj stream:(id<ICEOutputStream>)stream;
@end

@interface ICEDoubleHelper : NSObject
+(id) readWithStream:(id<ICEInputStream>)stream;
+(void) writeWithStream:(id)obj stream:(id<ICEOutputStream>)stream;
@end

@interface ICEStringHelper : NSObject
+(id) readWithStream:(id<ICEInputStream>)stream;
+(void) writeWithStream:(id)obj stream:(id<ICEOutputStream>)stream;
@end

@interface ICEObjectHelper : NSObject
+(id) readWithStream:(id<ICEInputStream>)stream;
+(void) writeWithStream:(id)obj stream:(id<ICEOutputStream>)stream;
@end

//
// Helper for enums.
//
@interface ICEEnumHelper : NSObject
+(id) readWithStream:(id<ICEInputStream>)stream;
+(void) writeWithStream:(id)obj stream:(id<ICEOutputStream>)stream;
+(ICEInt) getLimit;
@end

//
// Helper for sequences of non-value types.
//
@interface ICESeqHelper : NSObject
+(id) readWithStream:(id<ICEInputStream>)stream;
+(void) writeWithStream:(id)obj stream:(id<ICEOutputStream>)stream;
+(Class) getContained;
@end

//
// Helper for dictionaries.
//
@interface ICEDictHelper : NSObject
+(id) readWithStream:(id<ICEInputStream>)stream;
+(void) writeWithStream:(id)obj stream:(id<ICEOutputStream>)stream;
+(ICEKeyValueHelper) getContained;
@end
