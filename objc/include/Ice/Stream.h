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
@protocol ICEObjectPrx;
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

-(id<ICEObjectPrx>) readProxy:(Class)c;

-(void) readObject:(ICEObject**)object;
-(void) readObjectWithCallback:(id<ICEReadObjectCallback>)callback;
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

-(void) writeProxy:(id<ICEObjectPrx>)v;

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
+(id) ice_readWithStream:(id<ICEInputStream>)stream;
+(void) ice_writeWithStream:(id)obj stream:(id<ICEOutputStream>)stream;
@end

@interface ICEByteHelper : NSObject
+(id) ice_readWithStream:(id<ICEInputStream>)stream;
+(void) ice_writeWithStream:(id)obj stream:(id<ICEOutputStream>)stream;
@end

@interface ICEShortHelper : NSObject
+(id) ice_readWithStream:(id<ICEInputStream>)stream;
+(void) ice_writeWithStream:(id)obj stream:(id<ICEOutputStream>)stream;
@end

@interface ICEIntHelper : NSObject
+(id) ice_readWithStream:(id<ICEInputStream>)stream;
+(void) ice_writeWithStream:(id)obj stream:(id<ICEOutputStream>)stream;
@end

@interface ICELongHelper : NSObject
+(id) ice_readWithStream:(id<ICEInputStream>)stream;
+(void) ice_writeWithStream:(id)obj stream:(id<ICEOutputStream>)stream;
@end

@interface ICEFloatHelper : NSObject
+(id) ice_readWithStream:(id<ICEInputStream>)stream;
+(void) ice_writeWithStream:(id)obj stream:(id<ICEOutputStream>)stream;
@end

@interface ICEDoubleHelper : NSObject
+(id) ice_readWithStream:(id<ICEInputStream>)stream;
+(void) ice_writeWithStream:(id)obj stream:(id<ICEOutputStream>)stream;
@end

@interface ICEStringHelper : NSObject
+(id) ice_readWithStream:(id<ICEInputStream>)stream;
+(void) ice_writeWithStream:(id)obj stream:(id<ICEOutputStream>)stream;
@end

@interface ICEObjectHelper : NSObject
+(id) ice_readWithStream:(id<ICEInputStream>)stream;
+(void) ice_writeWithStream:(id)obj stream:(id<ICEOutputStream>)stream;
@end

//
// Helper for enums.
//
@interface ICEEnumHelper : NSObject
+(id) ice_readWithStream:(id<ICEInputStream>)stream;
+(void) ice_writeWithStream:(id)obj stream:(id<ICEOutputStream>)stream;
+(ICEInt) getLimit;
@end

//
// Helper for sequence of proxies
//
@interface ICEObjectPrxSequenceHelper : NSObject
+(id) ice_readWithStream:(id<ICEInputStream>)stream;
+(void) ice_writeWithStream:(id)obj stream:(id<ICEOutputStream>)stream;
@end

//
// Helper for sequence of objects
//
@interface ICEObjectSequenceHelper : NSObject
+(id) ice_readWithStream:(id<ICEInputStream>)stream;
+(void) ice_writeWithStream:(id)obj stream:(id<ICEOutputStream>)stream;
@end

//
// Helper for dictionary of objects
//
@interface ICEObjectDictionaryHelper : NSObject
+(id) ice_readWithStream:(id<ICEInputStream>)stream;
+(void) ice_writeWithStream:(id)obj stream:(id<ICEOutputStream>)stream;
+(Class) getKeyClass;
@end

//
// Helper for sequences of non-value types.
//
@interface ICESequenceHelper : NSObject
+(id) ice_readWithStream:(id<ICEInputStream>)stream;
+(void) ice_writeWithStream:(id)obj stream:(id<ICEOutputStream>)stream;
+(Class) getContained;
@end

//
// Helper for dictionaries.
//
@interface ICEDictionaryHelper : NSObject
+(id) ice_readWithStream:(id<ICEInputStream>)stream;
+(void) ice_writeWithStream:(id)obj stream:(id<ICEOutputStream>)stream;
+(ICEKeyValueHelper) getContained;
@end
