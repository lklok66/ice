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
    try
    {
        return [ICECommunicator communicatorWithCommunicator:IS->communicator()];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

-(void) sliceObjects:(BOOL)b
{
    try
    {
        IS->sliceObjects(b);
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}

-(BOOL)readBool
{
    try
    {
        return IS->readBool();
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return FALSE; // Keep the compiler happy.
    }
}

-(NSArray*) readBoolSeq
{
    try
    {
        return toNSArray(IS->readBoolSeq());
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

-(ICEByte) readByte
{
    try
    {
        return IS->readByte();
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return 0; // Keep the compiler happy.
    }
}

-(NSArray*) readByteSeq
{
    try
    {
        return toNSArray(IS->readByteSeq());
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

-(ICEShort) readShort
{
    try
    {
        return IS->readShort();
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return 0; // Keep the compiler happy.
    }
}

-(NSArray*) readShortSeq
{
    try
    {
        return toNSArray(IS->readShortSeq());
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

-(ICEInt) readInt
{
    try
    {
        return IS->readInt();
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return 0;
    }
}

-(NSArray*) readIntSeq
{
    try
    {
        return toNSArray(IS->readIntSeq());
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

-(ICELong) readLong
{
    try
    {
        return IS->readLong();
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

-(NSArray*) readLongSeq
{
    try
    {
        return toNSArray(IS->readLongSeq());
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

-(ICEFloat) readFloat
{
    try
    {
        return IS->readFloat();
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return 0; // Keep the compiler happy.
    }
}

-(NSArray*) readFloatSeq
{
    try
    {
        return toNSArray(IS->readFloatSeq());
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

-(ICEDouble) readDouble
{
    try
    {
        return IS->readDouble();
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return 0; // Keep the compiler happy.
    }
}

-(NSArray*) readDoubleSeq
{
    try
    {
        return toNSArray(IS->readDoubleSeq());
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

-(NSString*)readString
{
    try
    {
        return [toNSString(IS->readString()) autorelease];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

-(NSArray*) readStringSeq
{
    try
    {
        return toNSArray(IS->readStringSeq());
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

-(ICEInt) readSize
{
    try
    {
        return IS->readSize();
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return 0; // Keep the compiler happy.
    }
}

-(ICEObjectPrx*) readProxy
{
    try
    {
        return [ICEObjectPrx objectPrxWithObjectPrx__:IS->readProxy()];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

//-(void) readObject(const ICEReadObjectCallbackPtr&)
//{
//}

-(NSString*) readTypeId
{
    try
    {
        return [toNSString(IS->readTypeId()) autorelease];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

-(void) throwException
{
    try
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
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}

-(void) startSlice
{
    try
    {
        IS->startSlice();
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}

-(void) endSlice
{
    try
    {
        IS->endSlice();
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}

-(void) skipSlice
{
    try
    {
        IS->skipSlice();
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}

-(void) startEncapsulation
{
    try
    {
        IS->startEncapsulation();
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}

-(void) endEncapsulation
{
    try
    {
        IS->endEncapsulation();
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}

-(void) skipEncapsulation
{
    try
    {
        IS->skipEncapsulation();
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}

-(void) readPendingObjects
{
    try
    {
        IS->readPendingObjects();
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
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
    try
    {
        return [ICECommunicator communicatorWithCommunicator:OS->communicator()];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

-(void)writeBool:(BOOL)v
{
    try
    {
        OS->writeBool(v);
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}

-(void) writeBoolSeq:(NSArray*)v
{
    try
    {
        std::vector<bool> s;
        OS->writeBoolSeq(fromNSArray(v, s));
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}

-(void) writeByte:(ICEByte)v
{
    try
    {
        OS->writeByte(v);
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}

-(void) writeByteSeq:(NSArray*)v
{ 
    try
    {
        std::vector<ICEByte> s;
        OS->writeByteSeq(fromNSArray(v, s));
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}


-(void) writeShort:(ICEShort)v
{
    try
    {
        OS->writeShort(v);
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}

-(void) writeShortSeq:(NSArray*)v
{
    try
    {
        std::vector<ICEShort> s;
        OS->writeShortSeq(fromNSArray(v, s));
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}


-(void) writeInt:(ICEInt)v
{
    try
    {
        OS->writeInt(v);
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}

-(void) writeIntSeq:(NSArray*)v
{
    try
    {
        std::vector<ICEInt> s;
        OS->writeIntSeq(fromNSArray(v, s));
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}


-(void) writeLong:(ICELong)v
{
    try
    {
        OS->writeLong(v);
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}

-(void) writeLongSeq:(NSArray*)v
{
    try
    {
        std::vector<ICELong> s;
        OS->writeLongSeq(fromNSArray(v, s));
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}


-(void) writeFloat:(ICEFloat)v
{
    try
    {
        OS->writeFloat(v);
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}

-(void) writeFloatSeq:(NSArray*)v
{
    try
    {
        std::vector<ICEFloat> s;
        OS->writeFloatSeq(fromNSArray(v, s));
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}


-(void) writeDouble:(ICEDouble)v
{
    try
    {
        OS->writeDouble(v);
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}

-(void) writeDoubleSeq:(NSArray*)v
{
    try
    {
        std::vector<ICEDouble> s;
        OS->writeDoubleSeq(fromNSArray(v, s));
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}


-(void) writeString:(NSString*)v
{
    try
    {
        OS->writeString(fromNSString(v));
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}

-(void) writeStringSeq:(NSArray*)v
{
    try
    {
        std::vector<std::string> s;
        OS->writeStringSeq(fromNSArray(v, s));
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}

-(void) writeSize:(ICEInt)v
{
    try
    {
        OS->writeSize(v);
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}


-(void) writeProxy:(ICEObjectPrx*)v
{
    try
    {
        OS->writeProxy([v objectPrx__]);
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}

//-(void) writeObject:(ICEObject*)v
//{
    // TODO
    //OS->writeObject([v object__]);
//}

-(void) writeTypeId:(NSString*)v
{
    try
    {
        OS->writeTypeId(fromNSString(v));
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}

-(void) writeException:(ICEUserException*)v
{
    try
    {
        // TODO
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}

-(void) startSlice
{
    try
    {
        OS->startSlice();
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}

-(void) endSlice
{
    try
    {
        OS->endSlice();
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}

-(void) startEncapsulation
{
    try
    {
        OS->startEncapsulation();
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}

-(void) endEncapsulation
{
    try
    {
        OS->endEncapsulation();
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}

-(void) writePendingObjects
{
    try
    {
        OS->writePendingObjects();
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}

-(NSData*) finished
{
    try
    {
        std::vector<Ice::Byte> buf;
        OS->finished(buf);
        return [NSData dataWithBytes:&buf[0] length:buf.size()];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

@end
