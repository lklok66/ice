// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <IceObjC/StreamI.h>
#import <IceObjC/CommunicatorI.h>
#import <IceObjC/ProxyI.h>
#import <IceObjC/Util.h>

#include <Ice/Stream.h>

#define IS ((Ice::InputStream*)is__)
#define OS ((Ice::OutputStream*)os__)

@implementation ICEInputStream (Internal)

-(ICEInputStream*) initWithInputStream:(const Ice::InputStreamPtr&)arg
{
    if(![super init])
    {
        return nil;
    }
    is__ = arg.get();
    ((Ice::InputStream*)is__)->__incRef();
    return self;
}

-(Ice::InputStream*) is__
{
    return (Ice::InputStream*)is__;
}

-(void) dealloc
{
    ((Ice::InputStream*)is__)->__decRef();
    is__ = 0;
    [super dealloc];
}

@end

@implementation ICEInputStream

-(ICECommunicator*) communicator
{
    return [ICECommunicator communicatorWithCommunicator:IS->communicator()];
}

-(void) sliceObjects:(BOOL)b
{
    IS->sliceObjects(b);
}

-(BOOL)readBool
{
    return IS->readBool();
}

-(NSArray*) readBoolSeq
{
    return toNSArray(IS->readBoolSeq());
}

-(ICEByte) readByte
{
    return IS->readByte();
}

-(NSArray*) readByteSeq
{
    return toNSArray(IS->readByteSeq());
}

-(ICEShort) readShort
{
    return IS->readShort();
}

-(NSArray*) readShortSeq
{
    return toNSArray(IS->readShortSeq());
}

-(ICEInt) readInt
{
    return IS->readInt();
}

-(NSArray*) readIntSeq
{
    return toNSArray(IS->readIntSeq());
}

-(ICELong) readLong
{
    return IS->readLong();
}

-(NSArray*) readLongSeq
{
    return toNSArray(IS->readLongSeq());
}

-(ICEFloat) readFloat
{
    return IS->readFloat();
}

-(NSArray*) readFloatSeq
{
    return toNSArray(IS->readFloatSeq());
}

-(ICEDouble) readDouble
{
    return IS->readDouble();
}

-(NSArray*) readDoubleSeq
{
    return toNSArray(IS->readDoubleSeq());
}

-(NSString*)readString
{
    return [NSString stringWithUTF8String:IS->readString().c_str()];
}

-(NSArray*) readStringSeq
{
    return toNSArray(IS->readStringSeq());
}

-(ICEInt) readSize
{
    return IS->readSize();
}

-(ICEObjectPrx*) readProxy
{
    return [ICEObjectPrx objectPrxWithObjectPrx__:IS->readProxy()];
}

//-(void) readObject(const ICEReadObjectCallbackPtr&)
//{
//}

-(NSString*) readTypeId
{
    return [NSString stringWithUTF8String:IS->readTypeId().c_str()];
}


-(void) throwException
{
    try
    {
        IS->throwException();
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}

-(void) startSlice
{
    IS->startSlice();
}

-(void) endSlice
{
    IS->endSlice();
}

-(void) skipSlice
{
    IS->skipSlice();
}

-(void) startEncapsulation
{
    IS->startEncapsulation();
}

-(void) endEncapsulation
{
    IS->endEncapsulation();
}

-(void) skipEncapsulation
{
    IS->skipEncapsulation();
}

-(void) readPendingObjects
{
    IS->readPendingObjects();
}

@end

@implementation ICEOutputStream (Internal)

-(ICEOutputStream*) initWithOutputStream:(const Ice::OutputStreamPtr&)arg
{
    if(![super init])
    {
        return nil;
    }
    os__ = arg.get();
    ((Ice::OutputStream*)os__)->__incRef();
    return self;
}

-(Ice::OutputStream*) os__
{
    return (Ice::OutputStream*)os__;
}

-(void) dealloc
{
    ((Ice::OutputStream*)os__)->__decRef();
    os__ = 0;
    [super dealloc];
}

@end

@implementation ICEOutputStream

-(ICECommunicator*) communicator
{
    return [ICECommunicator communicatorWithCommunicator:OS->communicator()];
}

-(void)writeBool:(BOOL)v
{
    OS->writeBool(v);
}

-(void) writeBoolSeq:(NSArray*)v
{
    std::vector<bool> s;
    OS->writeBoolSeq(fromNSArray(v, s));
}

-(void) writeByte:(ICEByte)v
{
    OS->writeByte(v);
}

-(void) writeByteSeq:(NSArray*)v
{    
    std::vector<ICEByte> s;
    OS->writeByteSeq(fromNSArray(v, s));
}


-(void) writeShort:(ICEShort)v
{
    OS->writeShort(v);
}

-(void) writeShortSeq:(NSArray*)v
{
    std::vector<ICEShort> s;
    OS->writeShortSeq(fromNSArray(v, s));
}


-(void) writeInt:(ICEInt)v
{
    OS->writeInt(v);
}

-(void) writeIntSeq:(NSArray*)v
{
    std::vector<ICEInt> s;
    OS->writeIntSeq(fromNSArray(v, s));
}


-(void) writeLong:(ICELong)v
{
    OS->writeLong(v);
}

-(void) writeLongSeq:(NSArray*)v
{
    std::vector<ICELong> s;
    OS->writeLongSeq(fromNSArray(v, s));
}


-(void) writeFloat:(ICEFloat)v
{
    OS->writeFloat(v);
}

-(void) writeFloatSeq:(NSArray*)v
{
    std::vector<ICEFloat> s;
    OS->writeFloatSeq(fromNSArray(v, s));
}


-(void) writeDouble:(ICEDouble)v
{
    OS->writeDouble(v);
}

-(void) writeDoubleSeq:(NSArray*)v
{
    std::vector<ICEDouble> s;
    OS->writeDoubleSeq(fromNSArray(v, s));
}


-(void) writeString:(NSString*)v
{
    OS->writeString([v UTF8String]);
}

-(void) writeStringSeq:(NSArray*)v
{
    std::vector<std::string> s;
    OS->writeStringSeq(fromNSArray(v, s));
}

-(void) writeSize:(ICEInt)v
{
    OS->writeSize(v);
}


-(void) writeProxy:(ICEObjectPrx*)v
{
    OS->writeProxy([v objectPrx__]);
}

//-(void) writeObject:(ICEObject*)v
//{
    // TODO
    //OS->writeObject([v object__]);
//}

-(void) writeTypeId:(NSString*)v
{
    OS->writeTypeId([v UTF8String]);
}

-(void) writeException:(ICEUserException*)v
{
    // TODO
}

-(void) startSlice
{
    OS->startSlice();
}

-(void) endSlice
{
    OS->endSlice();
}

-(void) startEncapsulation
{
    OS->startEncapsulation();
}

-(void) endEncapsulation
{
    OS->endEncapsulation();
}

-(void) writePendingObjects
{
    OS->writePendingObjects();
}

-(NSData*) finished
{
    std::vector<Ice::Byte> buf;
    OS->finished(buf);
    return [NSData dataWithBytes:&buf[0] length:buf.size()];
}

@end
