// **********************************************************************
//
// Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
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
} ICEKeyValueTypeHelper;

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

-(void) readObject:(ICEObject**)object typeId:(NSString*)type;
-(void) readObjectWithCallback:(id<ICEReadObjectCallback>)callback typeId:(NSString*)type;
-(NSMutableArray*) readObjectSeq:(NSString*)typeId;
-(NSMutableDictionary*) readObjectDict:(Class)keyType typeId:(NSString*)typeId;

-(NSMutableArray*) readSequence:(Class)type;
-(NSMutableDictionary*) readDictionary:(ICEKeyValueTypeHelper)type;

-(ICEInt) readSize;
-(ICEInt) readAndCheckSeqSize:(ICEInt)minSize;

-(NSString*) readTypeId;

-(void) throwException;

-(void) startSlice;
-(void) endSlice;
-(void) skipSlice;

-(void) startEncapsulation;
-(void) endEncapsulation;
-(void) skipEncapsulation;

-(void) readPendingObjects;

-(void) rewind;
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

-(void) writeObject:(ICEObject*)v typeId:(NSString*)typeId;
-(void) writeObjectSeq:(NSArray*)v typeId:(NSString*)typeId;
-(void) writeObjectDict:(NSDictionary*)v keyType:(Class)type typeId:(NSString*)typeId;

-(void) writeSequence:(NSArray*)arr type:(Class)type;
-(void) writeDictionary:(NSDictionary*)dictionary type:(ICEKeyValueTypeHelper)type;

-(void) writeSize:(ICEInt)v;

-(void) writeTypeId:(NSString*)v;

-(void) writeException:(ICEUserException*)v;

-(void) startSlice;
-(void) endSlice;

-(void) startEncapsulation;
-(void) endEncapsulation;

-(void) writePendingObjects;

-(NSMutableData*) finished;

-(void) reset:(BOOL) clearBuffer;
@end

//
// Helper classes for the Slice built-in types.
//
@protocol ICEStreamBaseHelper
+(id) ice_readWithStream:(id<ICEInputStream>)stream;
+(void) ice_writeWithStream:(id)obj stream:(id<ICEOutputStream>)stream;
+(ICEInt) minWireSize;
@end
 
@interface ICEBoolHelper : NSObject<ICEStreamBaseHelper>
@end

@interface ICEByteHelper : NSObject<ICEStreamBaseHelper>
@end

@interface ICEShortHelper : NSObject<ICEStreamBaseHelper>
@end

@interface ICEIntHelper : NSObject<ICEStreamBaseHelper>
@end

@interface ICELongHelper : NSObject<ICEStreamBaseHelper>
@end

@interface ICEFloatHelper : NSObject<ICEStreamBaseHelper>
@end

@interface ICEDoubleHelper : NSObject<ICEStreamBaseHelper>
@end

@interface ICEStringHelper : NSObject<ICEStreamBaseHelper>
@end

@interface ICEObjectHelper : NSObject<ICEStreamBaseHelper>
@end

//
// Helper for enums.
//
@interface ICEEnumHelper : NSObject<ICEStreamBaseHelper>
+(ICEInt) getLimit;
@end

//
// Helper for sequence of proxies
//
@interface ICEObjectPrxSequenceHelper : NSObject<ICEStreamBaseHelper>
@end

//
// Helper for sequence of objects
//
@interface ICEObjectSequenceHelper : NSObject<ICEStreamBaseHelper>
+(NSString*) getContained;
@end

//
// Helper for dictionary of objects
//
@interface ICEObjectDictionaryHelper : NSObject<ICEStreamBaseHelper>
+(Class) getContained:(NSString**)typeId;
@end

//
// Helper for sequences of non-value types.
//
@interface ICESequenceHelper : NSObject<ICEStreamBaseHelper>
+(Class) getContained;
@end

//
// Helper for dictionaries.
//
@interface ICEDictionaryHelper : NSObject<ICEStreamBaseHelper>
+(ICEKeyValueTypeHelper) getContained;
@end
