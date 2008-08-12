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
#import <IceObjC/Object.h>
#import <IceObjC/LocalException.h>

#include <Ice/Stream.h>
#include <Ice/ObjectFactory.h>

#import <objc/runtime.h>

namespace IceObjC
{

class ObjectWriter : public Ice::ObjectWriter
{
public:
    
    ObjectWriter(ICEObject* obj) : _obj(obj)
    {
    }
    
    virtual void
    write(const Ice::OutputStreamPtr& stream) const
    {
        @try
        {
            [_obj write__:(id<ICEOutputStream>)stream->getClosure()];
        }
        @catch(NSException* ex)
        {
            rethrowCxxException(ex);
        }
    }

private:

    ICEObject* _obj;
};

class ObjectReader : public Ice::ObjectReader
{
public:
    
    ObjectReader(ICEObject* obj) : _obj(obj)
    {
    }
    
    virtual void
    read(const Ice::InputStreamPtr& stream, bool rid)
    {
        @try
        {
            [_obj read__:(id<ICEInputStream>)stream->getClosure() readTypeId:rid];
        }
        @catch(NSException* ex)
        {
            rethrowCxxException(ex);
        }
    }

    ICEObject*
    getObject()
    {
        return _obj;
    }

private:

    ICEObject* _obj;
};
typedef IceUtil::Handle<ObjectReader> ObjectReaderPtr;

class UserExceptionWriter : public Ice::UserExceptionWriter
{
public:
    
    UserExceptionWriter(ICEUserException* ex, const Ice::CommunicatorPtr& communicator) : 
        Ice::UserExceptionWriter(communicator),
        _ex(ex)
    {
    }
    
    virtual void
    write(const Ice::OutputStreamPtr& stream) const
    {
        @try
        {
            [_ex write__:(id<ICEOutputStream>)stream->getClosure()];
        }
        @catch(NSException* ex)
        {
            rethrowCxxException(ex);
        }
    }

    virtual bool 
    usesClasses() const
    {
        return [_ex usesClasses__];
    }

    virtual std::string 
    ice_name() const
    {
        return fromNSString([_ex ice_name]);
    }

    virtual Ice::Exception*
    ice_clone() const
    {
        return new UserExceptionWriter(*this);
    }

    virtual void 
    ice_throw() const
    {
        throw *this;
    }

private:

    ICEUserException* _ex;
};

class ReadObjectCallbackI : public Ice::ReadObjectCallback
{
public:
    
    ReadObjectCallbackI(id<ICEReadObjectCallback> cb) : _cb(cb)
    {
    }

    virtual void
    invoke(const Ice::ObjectPtr& obj)
    {
        @try
        {
            [_cb invoke:ObjectReaderPtr::dynamicCast(obj)->getObject()];
        }
        @catch(NSException* ex)
        {
            rethrowCxxException(ex);
        }
    }

private:

    id<ICEReadObjectCallback> _cb;
};

class ObjectReaderFactory : public Ice::ObjectFactory
{
public:

    virtual Ice::ObjectPtr
    create(const std::string& type)
    {
        std::string tId = toObjCSliceId(type);
        Class c = objc_lookUpClass(tId.c_str());
        if(c == nil)
        {
            return 0; // No object factory.
        }

        return new ObjectReader([[c alloc] init]);
    }

    virtual void
    destroy()
    {
    }
};

}

@implementation ICEInputStream

+(void)installObjectFactory:(const Ice::CommunicatorPtr&)communicator
{
    communicator->addObjectFactory(new IceObjC::ObjectReaderFactory(), "");
}
-(ICEInputStream*) initWithInputStream:(const Ice::InputStreamPtr&)arg
{
    if(![super init])
    {
        return nil;
    }
    is__ = arg.get();
    is__->__incRef();
    is__->setClosure(self);
    return self;
}
-(Ice::InputStream*) is__
{
    return is__;
}
-(void) dealloc
{
    is__->__decRef();
    is__ = 0;
    [super dealloc];
}

// @protocol ICEInputStream methods

-(id<ICECommunicator>) communicator
{
    try
    {
        return [ICECommunicator communicatorWithCommunicator:is__->communicator()];
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
        is__->sliceObjects(b);
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
        return is__->readBool();
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return NO; // Keep the compiler happy.
    }
}

-(NSArray*) readBoolSeq
{
    try
    {
        return [toNSArray(is__->readBoolSeq()) autorelease];
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
        return is__->readByte();
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return 0; // Keep the compiler happy.
    }
}

-(NSData*) readByteSeq
{
    try
    {
        std::pair<const Ice::Byte*, const Ice::Byte*> seq;
        is__->readByteSeq(seq);
        return [NSData dataWithBytes:seq.first length:(seq.second - seq.first)];
    }    
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

-(NSData*) readByteSeqNoCopy
{
    try
    {
        std::pair<const Ice::Byte*, const Ice::Byte*> seq;
        is__->readByteSeq(seq);
        return [NSData dataWithBytesNoCopy:const_cast<Ice::Byte*>(seq.first) 
                       length:(seq.second - seq.first) freeWhenDone:NO];
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
        return is__->readShort();
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
        return [toNSArray(is__->readShortSeq()) autorelease];
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
        return is__->readInt();
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
        return [toNSArray(is__->readIntSeq()) autorelease];
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
        return is__->readLong();
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
        return [toNSArray(is__->readLongSeq()) autorelease];
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
        return is__->readFloat();
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
        return [toNSArray(is__->readFloatSeq()) autorelease];
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
        return is__->readDouble();
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
        return [toNSArray(is__->readDoubleSeq()) autorelease];
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
        return [toNSString(is__->readString()) autorelease];
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
        return [toNSArray(is__->readStringSeq()) autorelease];
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
        return is__->readSize();
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
        return [ICEObjectPrx objectPrxWithObjectPrx__:is__->readProxy()];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

-(void) readObject:(id<ICEReadObjectCallback>)callback
{
    try
    {
        is__->readObject(new IceObjC::ReadObjectCallbackI(callback));
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}

-(NSString*) readTypeId
{
    try
    {
        return [toNSString(is__->readTypeId()) autorelease];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

-(void) throwException
{
    ICEUserException* ex = nil;
    bool usesClasses = false;
    try
    {
        usesClasses = is__->readBool();

        std::string typeId = is__->readString(false);
        for(;;)
        {
            typeId = toObjCSliceId(typeId);
            Class c = objc_lookUpClass(typeId.c_str());
            if(c != nil)
            {
                ex = [[c alloc] init];
                break;
            }
            else
            {
                is__->skipSlice(); // Slice off what we don't understand.
                typeId = is__->readString(false); // Read type id for next slice.
            }
        }

        //
        // The only way out of the loop above is to find an exception for
        // which the receiver has a factory. If this does not happen,
        // sender and receiver disagree about the Slice definitions they
        // use. In that case, the receiver will eventually fail to read
        // another type ID and throw a MarshalException.
        //
        NSAssert(ex, @"error while unmarshalling exception");
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }

    if(ex)
    {
        [ex read__:self readTypeId:false];
        if(usesClasses)
        {
            [self readPendingObjects];
        }
        @throw ex;
    }
}

-(void) startSlice
{
    try
    {
        is__->startSlice();
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
        is__->endSlice();
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
        is__->skipSlice();
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
        is__->startEncapsulation();
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
        is__->endEncapsulation();
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
        is__->skipEncapsulation();
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
        is__->readPendingObjects();
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}

@end

@implementation ICEOutputStream

-(ICEOutputStream*) initWithOutputStream:(const Ice::OutputStreamPtr&)arg
{
    if(![super init])
    {
        return nil;
    }
    os__ = arg.get();
    os__->__incRef();
    os__->setClosure(self);
    return self;
}

-(Ice::OutputStream*) os__
{
    return os__;
}

-(void) dealloc
{
    os__->__decRef();
    os__ = 0;
    [super dealloc];
}

// @protocol ICEOutputStream methods

-(id<ICECommunicator>) communicator
{
    try
    {
        return [ICECommunicator communicatorWithCommunicator:os__->communicator()];
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
        os__->writeBool(v);
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
        os__->writeBoolSeq(fromNSArray(v, s));
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
        os__->writeByte(v);
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}

-(void) writeByteSeq:(NSData*)v
{ 
    try
    {
        os__->writeByteSeq((ICEByte*)[v bytes], (ICEByte*)[v bytes] + [v length]);
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
        os__->writeShort(v);
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
        os__->writeShortSeq(fromNSArray(v, s));
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
        os__->writeInt(v);
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
        os__->writeIntSeq(fromNSArray(v, s));
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
        os__->writeLong(v);
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
        os__->writeLongSeq(fromNSArray(v, s));
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
        os__->writeFloat(v);
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
        os__->writeFloatSeq(fromNSArray(v, s));
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
        os__->writeDouble(v);
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
        os__->writeDoubleSeq(fromNSArray(v, s));
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
        os__->writeString(fromNSString(v));
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
        os__->writeStringSeq(fromNSArray(v, s));
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
        os__->writeSize(v);
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
        os__->writeProxy([v objectPrx__]);
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}

-(void) writeObject:(ICEObject*)v
{
    try
    {
        os__->writeObject(new IceObjC::ObjectWriter(v));
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}

-(void) writeTypeId:(const char*)v
{
    try
    {
        os__->writeTypeId(v);
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
        os__->writeException(IceObjC::UserExceptionWriter(v, os__->communicator()));
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
        os__->startSlice();
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
        os__->endSlice();
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
        os__->startEncapsulation();
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
        os__->endEncapsulation();
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
        os__->writePendingObjects();
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
        os__->finished(buf);
        return [NSData dataWithBytes:&buf[0] length:buf.size()];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

@end
