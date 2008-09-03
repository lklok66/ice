// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/StreamI.h>
#import <Ice/CommunicatorI.h>
#import <Ice/ProxyI.h>
#import <Ice/Util.h>
#import <Ice/Object.h>
#import <Ice/LocalException.h>

#include <IceCpp/Stream.h>
#include <IceCpp/ObjectFactory.h>

#import <objc/runtime.h>

namespace IceObjC
{

class ObjectWriter : public Ice::ObjectWriter
{
public:
    
    ObjectWriter(ICEObject* obj, ICEOutputStream* stream) : _obj(obj), _stream(stream)
    {
    }
    
    virtual void
    write(const Ice::OutputStreamPtr& stream) const
    {
        @try
        {
            [_obj write__:_stream];
        }
        @catch(NSException* ex)
        {
            rethrowCxxException(ex);
        }
    }

private:

    ICEObject* _obj;
    ICEOutputStream* _stream;
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
            [_obj read__:[ICEInputStream getWrapperWithCxxObjectNoAutoRelease:stream.get()] readTypeId:rid];
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
    
    UserExceptionWriter(ICEUserException* ex, ICEOutputStream* stream, const Ice::CommunicatorPtr& communicator) : 
        Ice::UserExceptionWriter(communicator),
        _ex(ex),
        _stream(stream)
    {
    }
    
    virtual void
    write(const Ice::OutputStreamPtr& stream) const
    {
        @try
        {
            [_ex write__:_stream];
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
    ICEOutputStream* _stream;
};

class ReadObjectCallbackI : public Ice::ReadObjectCallback
{
public:
    
    ReadObjectCallbackI(id<ICEReadObjectCallback> cb) : _cb(cb)
    {
        [_cb retain];
    }

    virtual ~ReadObjectCallbackI()
    {
        [_cb release];
    }
    
    virtual void
    invoke(const Ice::ObjectPtr& obj)
    {
        @try
        {
            if(obj)
            {
                [_cb invoke:ObjectReaderPtr::dynamicCast(obj)->getObject()];
            }
            else
            {
                [_cb invoke:nil];
            }
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
-(id) initWithCxxObject:(IceUtil::Shared*)cxxObject
{
    if(![super initWithCxxObject:cxxObject])
    {
        return nil;
    }
    is_ = dynamic_cast<Ice::InputStream*>(cxxObject);
    return self;
}
+(void)installObjectFactory:(const Ice::CommunicatorPtr&)communicator
{
    communicator->addObjectFactory(new IceObjC::ObjectReaderFactory(), "");
}
-(Ice::InputStream*) is
{
    return is_;
}

// @protocol ICEInputStream methods

-(id<ICECommunicator>) communicator
{
    return [ICECommunicator wrapperWithCxxObject:is_->communicator().get()];
}

-(void) sliceObjects:(BOOL)b
{
    try
    {
        is_->sliceObjects(b);
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
        return is_->readBool();
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return NO; // Keep the compiler happy.
    }
}

-(NSMutableData*) readBoolSeq
{
    try
    {
        std::pair<const bool*, const bool*> seq;
	// TODO: size check. This is awkward because, to do that, we need to read the size
	// from the stream first, but that also happens inside readBoolSeq().
        is_->readBoolSeq(seq);
        return [[NSMutableData alloc] initWithBytes:seq.first length:(seq.second - seq.first) * sizeof(BOOL)];
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
        return is_->readByte();
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return 0; // Keep the compiler happy.
    }
}

-(NSMutableData*) readByteSeq
{
    try
    {
        std::pair<const Ice::Byte*, const Ice::Byte*> seq;
	// TODO: size check
        is_->readByteSeq(seq);
        return [[NSMutableData alloc] initWithBytes:seq.first length:(seq.second - seq.first)];
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
	// TODO: size check
        is_->readByteSeq(seq);
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
        return is_->readShort();
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return 0; // Keep the compiler happy.
    }
}

-(NSMutableData*) readShortSeq
{
    try
    {
        std::pair<const Ice::Short*, const Ice::Short*> seq;
	// TODO: size check
        is_->readShortSeq(seq);
        return [[NSMutableData alloc] initWithBytes:seq.first length:(seq.second - seq.first) * sizeof(ICEShort)];
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
        return is_->readInt();
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return 0;
    }
}

-(NSMutableData*) readIntSeq
{
    try
    {
        std::pair<const Ice::Int*, const Ice::Int*> seq;
	// TODO: size check
        is_->readIntSeq(seq);
        return [[NSMutableData alloc] initWithBytes:seq.first length:(seq.second - seq.first) * sizeof(ICEInt)];
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
        return is_->readLong();
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

-(NSMutableData*) readLongSeq
{
    try
    {
        std::pair<const Ice::Long*, const Ice::Long*> seq;
	// TODO: size check
        is_->readLongSeq(seq);
        return [[NSMutableData alloc] initWithBytes:seq.first length:(seq.second - seq.first) * sizeof(ICELong)];
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
        return is_->readFloat();
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return 0; // Keep the compiler happy.
    }
}

-(NSMutableData*) readFloatSeq
{
    try
    {
        std::pair<const Ice::Float*, const Ice::Float*> seq;
	// TODO: size check
        is_->readFloatSeq(seq);
        return [[NSMutableData alloc] initWithBytes:seq.first length:(seq.second - seq.first) * sizeof(ICEFloat)];
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
        return is_->readDouble();
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return 0; // Keep the compiler happy.
    }
}

-(NSMutableData*) readDoubleSeq
{
    try
    {
        std::pair<const Ice::Double*, const Ice::Double*> seq;
	// TODO: size check
        is_->readDoubleSeq(seq);
        return [[NSMutableData alloc] initWithBytes:seq.first length:(seq.second - seq.first) * sizeof(ICEDouble)];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

-(NSMutableString*)readString
{
    try
    {
        return toNSMutableString(is_->readString());
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

-(NSMutableArray*) readStringSeq
{
    try
    {
	// TODO: size check
        return toNSArray(is_->readStringSeq());
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

-(ICEInt) readEnumerator:(ICEInt)limit
{
    ICEInt val;
    try
    {
	if(limit <= 0x7f)
	{
	    val = [self readByte];
	}
	else if(limit <= 0x7fff)
	{
	    val = [self readShort];
	}
	else
	{
	    val = [self readInt];
	}
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
    if(val >= limit)
    {
	@throw [ICEMarshalException marshalException:__FILE__ line:__LINE__ reason_:@"enumerator out of range"];
    }
    return val;
}

//
// Size of an enum (which can vary with processor and compiler). We don't know what it is absolutely.
// The definition below assumes that all enums have the same size, regardless of the number of
// enumerators.
//
typedef enum { dummy } Dummy_Enum;
#define ENUM_SIZE (sizeof(Dummy_Enum))

-(NSMutableData*) readEnumSeq:(ICEInt)limit
{
    int minWireSize;
    if(limit <= 0x7f)
    {
	minWireSize = 1;
    }
    else if(limit <= 0x7fff)
    {
	minWireSize = 2;
    }
    else
    {
	minWireSize = 4;
    }

    NSMutableData* ret;
    try
    {
	int count = is_->readSize();
	[self checkFixedSeq:count elemSize:minWireSize];
	if((ret = [[NSMutableData alloc] initWithLength:(count * ENUM_SIZE)]) == 0)
	{
	    return ret;
	}

	Dummy_Enum *v = (Dummy_Enum *)[ret bytes];
	if(limit <= 0x7f)
	{
	    while(count-- > 0)
	    {
		*v = (Dummy_Enum)[self readByte];
		if(*v >= limit)
		{
		    @throw [ICEMarshalException marshalException:__FILE__ line:__LINE__
		                                reason_:@"enumerator out of range"];
		}
		++v;
	    }
	}
	else if (limit <= 0x7fff)
	{
	    while(count-- > 0)
	    {
		*v = (Dummy_Enum)[self readShort];
		if(*v < 0 || *v >= limit)
		{
		    @throw [ICEMarshalException marshalException:__FILE__ line:__LINE__
		                                reason_:@"enumerator out of range"];
		}
		++v;
	    }
	}
	else
	{
	    while(count-- > 0)
	    {
		*v = (Dummy_Enum)[self readInt];
		if(*v < 0 || *v >= limit)
		{
		    @throw [ICEMarshalException marshalException:__FILE__ line:__LINE__
		                                reason_:@"enumerator out of range"];
		}
		++v;
	    }
	}
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
    return ret;
}

-(ICEInt) readSize
{
    try
    {
        return is_->readSize();
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return 0; // Keep the compiler happy.
    }
}

-(id<ICEObjectPrx>) readProxy:(Class)c
{
    try
    {
	Ice::ObjectPrx p = is_->readProxy();
	if(!p)
	{
	     return nil;
	}
        else
        {
            return [[c alloc] initWithObjectPrx__:p];
        }
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
        is_->readObject(new IceObjC::ReadObjectCallbackI(callback));
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}

-(NSMutableArray*) readObjectSeq
{
    // TODO
    return nil;
}

-(NSMutableDictionary*) readObjectDict:(Class)cl
{
    // TODO
    return nil;
}

-(NSMutableArray*) readSequence:(Class)cl
{
    ICEInt sz = [self readSize];
    // TODO sequence size check
    NSMutableArray* arr = [[NSMutableArray alloc] initWithCapacity:sz];
    id obj = nil;
    @try
    {
	while(sz-- > 0)
	{
	    obj = [cl readWithStream:self];
	    [arr addObject:obj];
	    [obj release];
	    obj = nil;
	}
    }
    @catch(NSException *ex)
    {
        [arr release];
	[obj release];
	return nil;
    }
    return arr;
}

-(NSMutableDictionary*) readDictionary:(ICEKeyValueHelper)c
{
    ICEInt sz = [self readSize];
    // TODO size check
    NSMutableDictionary* dictionary = [[NSMutableDictionary alloc] initWithCapacity:sz];
    Class key = nil;
    Class value = nil;
    @try
    {
	while(sz-- > 0)
	{
	    key = [c.key readWithStream:self];
	    value = [c.value readWithStream:self];
	    [dictionary setObject:value forKey:key];
	    [key release];
	    key = nil;
	    [value release];
	    value = nil;
	}
    }
    @catch(NSException *ex)
    {
	[dictionary release];
	[key release];
	[value release];
	return nil;
    }
    return dictionary;
}

-(NSString*) readTypeId
{
    try
    {
        return toNSString(is_->readTypeId());
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
        usesClasses = is_->readBool();

        std::string typeId = is_->readString(false);
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
                is_->skipSlice(); // Slice off what we don't understand.
                typeId = is_->readString(false); // Read type id for next slice.
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
        @throw ex; // TODO: Should we auto release the exception here or should it be the responsability of the caller?
    }
}

-(void) startSeq:(ICEInt)numElements minSize:(ICEInt)minSize
{
}

-(void) checkSeq
{
}

-(void) checkSeq:(ICEInt)bytesLeft
{
}

-(void) checkFixedSeq:(ICEInt)numElements elemSize:(ICEInt)elemSize
{
}

-(void) endElement
{
}

-(void) endSeq:(ICEInt)size
{
}

-(void) startSlice
{
    try
    {
        is_->startSlice();
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
        is_->endSlice();
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
        is_->skipSlice();
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
        is_->startEncapsulation();
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
        is_->endEncapsulation();
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
        is_->skipEncapsulation();
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
        is_->readPendingObjects();
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}

@end

@implementation ICEOutputStream
-(id) initWithCxxObject:(IceUtil::Shared*)cxxObject
{
    if(![super initWithCxxObject:cxxObject])
    {
        return nil;
    }
    os_ = dynamic_cast<Ice::OutputStream*>(cxxObject);
    return self;
}
-(Ice::OutputStream*) os
{
    return os_;
}

// @protocol ICEOutputStream methods

-(id<ICECommunicator>) communicator
{
    return [ICECommunicator wrapperWithCxxObject:os_->communicator().get()];
}

-(void)writeBool:(BOOL)v
{
    try
    {
        os_->writeBool(v);
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}

-(void) writeBoolSeq:(NSData*)v
{
    try
    {
        v == nil ? os_->writeSize(0)
	         : os_->writeBoolSeq((bool*)[v bytes], (bool*)[v bytes] + [v length] / sizeof(BOOL));
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
        os_->writeByte(v);
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
        v == nil ? os_->writeSize(0)
                 : os_->writeByteSeq((ICEByte*)[v bytes], (ICEByte*)[v bytes] + [v length]);
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
        os_->writeShort(v);
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}

-(void) writeShortSeq:(NSData*)v
{
    try
    {
        v == nil ? os_->writeSize(0)
                 : os_->writeShortSeq((ICEShort*)[v bytes], (ICEShort*)[v bytes] + [v length] / sizeof(ICEShort));
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
        os_->writeInt(v);
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}

-(void) writeIntSeq:(NSData*)v
{
    try
    {
        v == nil ? os_->writeSize(0)
                 : os_->writeIntSeq((ICEInt*)[v bytes], (ICEInt*)[v bytes] + [v length] / sizeof(ICEInt));
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
        os_->writeLong(v);
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}

-(void) writeLongSeq:(NSData*)v
{
    try
    {
        v == nil ? os_->writeSize(0)
                 : os_->writeLongSeq((ICELong*)[v bytes], (ICELong*)[v bytes] + [v length] / sizeof(ICELong));
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
        os_->writeFloat(v);
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}

-(void) writeFloatSeq:(NSData*)v
{
    try
    {
        v == nil ? os_->writeSize(0)
                 : os_->writeFloatSeq((ICEFloat*)[v bytes], (ICEFloat*)[v bytes] + [v length] / sizeof(ICEFloat));
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
        os_->writeDouble(v);
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}

-(void) writeDoubleSeq:(NSData*)v
{
    try
    {
        v == nil ? os_->writeSize(0)
                 : os_->writeDoubleSeq((ICEDouble*)[v bytes],
		                           (ICEDouble*)[v bytes] + [v length] / sizeof(ICEDouble));
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
	 os_->writeString(fromNSString(v));
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
	os_->writeStringSeq(fromNSArray(v, s));
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}

-(void)writeSequence:(NSArray*)arr c:(Class)c
{
    if(arr == nil)
    {
        [self writeSize:0];
        return;
    }

    [self writeSize:[arr count]];
    for(id i in arr)
    {
	    [c writeWithStream:i stream:self];
    }
}

-(void) writeDictionary:(NSDictionary*)dictionary c:(ICEKeyValueHelper)c
{
    if(dictionary == nil)
    {
        [self writeSize:0];
	return;
    }

    [self writeSize:[dictionary count]];
    NSEnumerator* e = [dictionary keyEnumerator];
    id key;
    while((key = [e nextObject]))
    {
	[c.key writeWithStream:key stream:self];
	[c.value writeWithStream:[dictionary objectForKey:key] stream:self];
    }
}

-(void) writeEnumerator:(ICEInt)v limit:(int)limit
{
    if(v >= limit)
    {
	@throw [ICEMarshalException marshalException:__FILE__ line:__LINE__ reason_:@"enumerator out of range"];
    }
    try
    {
        if(limit <= 0x7f)
	{
	    os_->writeByte(v);
	}
	else if(limit <= 0x7fff)
	{
	    os_->writeShort(v);
	}
	else
	{
	    os_->writeInt(v);
	}
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}

//
// The C standard does not fix the size of an enum. The compiler is free
// to choose an enum size that depends on the number of enumerators, and
// the choice may vary depending on the processor. This means that we don't
// know what the size of an enum is until run time, so the marshaling
// has to be generic and copy with enums that could be 8, 16, or 32 bits wide.
//
-(void) writeEnumSeq:(NSData*)v limit:(ICEInt)limit
{
    try
    {
	int count = v == nil ? 0 : [v length] / ENUM_SIZE;
	[self writeSize:count];
	if(count == 0)
	{
	    return;
	}

	const Dummy_Enum* p = (const Dummy_Enum*)[v bytes];
	if(limit <= 0x7f)
	{
	    while(count-- > 0)
	    {
		if(*p >= limit)
		{
		    @throw [ICEMarshalException marshalException:__FILE__ line:__LINE__
		                                reason_:@"enumerator out of range"];
		}
		[self writeByte:*p++];
	    }
	}
	else if(limit <= 0x7fff)
	{
	    while(count-- > 0)
	    {
		if(*p < 0 || *p >= limit)
		{
		    @throw [ICEMarshalException marshalException:__FILE__ line:__LINE__
		                                reason_:@"enumerator out of range"];
		}
		[self writeShort:*p++];
	    }
	}
	else
	{
	    while(count-- > 0)
	    {
		if(*p < 0 || *p >= limit)
		{
		    @throw [ICEMarshalException marshalException:__FILE__ line:__LINE__
		                                reason_:@"enumerator out of range"];
		}
		[self writeInt:*p++];
	    }
	}
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
        os_->writeSize(v);
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}


-(void) writeProxy:(id<ICEObjectPrx>)v
{
    try
    {
        os_->writeProxy([(ICEObjectPrx*)v objectPrx__]);
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
        os_->writeObject(new IceObjC::ObjectWriter(v, self));
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}

-(void) writeObjectSeq:(NSArray*)v
{
    // TODO
}

-(void) writeObjectDict:(NSDictionary*)v c:(Class)c
{
    // TODO
}

-(void) writeTypeId:(const char*)v
{
    try
    {
        os_->writeTypeId(v);
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
        os_->writeException(IceObjC::UserExceptionWriter(v, self, os_->communicator()));
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
        os_->startSlice();
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
        os_->endSlice();
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
        os_->startEncapsulation();
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
        os_->endEncapsulation();
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
        os_->writePendingObjects();
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
        os_->finished(buf);
        return [NSData dataWithBytes:&buf[0] length:buf.size()];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

@end

@implementation ICEBoolHelper
+(id) readWithStream:(id<ICEInputStream>)stream
{
    return [[NSNumber alloc] initWithBool:[stream readBool]];
}

+(void) writeWithStream:(id)obj stream:(id<ICEOutputStream>)stream
{
    [stream writeBool:[obj boolValue]];
}
@end

@implementation ICEByteHelper
+(id) readWithStream:(id<ICEInputStream>)stream
{
    return [[NSNumber alloc] initWithUnsignedChar:[stream readByte]];
}

+(void) writeWithStream:(id)obj stream:(id<ICEOutputStream>)stream
{
    [stream writeByte:[obj unsignedCharValue]];
}
@end

@implementation ICEShortHelper
+(id) readWithStream:(id<ICEInputStream>)stream
{
    return [[NSNumber alloc] initWithShort:[stream readShort]];
}

+(void) writeWithStream:(id)obj stream:(id<ICEOutputStream>)stream
{
    [stream writeShort:[obj shortValue]];
}
@end

@implementation ICEIntHelper
+(id) readWithStream:(id<ICEInputStream>)stream
{
    return [[NSNumber alloc] initWithInt:[stream readInt]];
}

+(void) writeWithStream:(id)obj stream:(id<ICEOutputStream>)stream
{
    [stream writeInt:[obj intValue]];
}
@end

@implementation ICELongHelper
+(id) readWithStream:(id<ICEInputStream>)stream
{
    return [[NSNumber alloc] initWithLong:[stream readLong]];
}

+(void) writeWithStream:(id)obj stream:(id<ICEOutputStream>)stream
{
    [stream writeLong:[obj longValue]];
}
@end

@implementation ICEFloatHelper
+(id) readWithStream:(id<ICEInputStream>)stream
{
    return [[NSNumber alloc] initWithFloat:[stream readFloat]];
}

+(void) writeWithStream:(id)obj stream:(id<ICEOutputStream>)stream
{
    [stream writeFloat:[obj floatValue]];
}
@end

@implementation ICEDoubleHelper
+(id) readWithStream:(id<ICEInputStream>)stream
{
    return [[NSNumber alloc] initWithDouble:[stream readDouble]];
}

+(void) writeWithStream:(id)obj stream:(id<ICEOutputStream>)stream
{
    [stream writeDouble:[obj doubleValue]];
}
@end

@implementation ICEStringHelper
+(id) readWithStream:(id<ICEInputStream>)stream
{
    return [stream readString];
}

+(void) writeWithStream:(id)obj stream:(id<ICEOutputStream>)stream
{
    [stream writeString:obj];
}
@end

@implementation ICEEnumHelper
+(id) readWithStream:(id<ICEInputStream>)stream
{
    return [[NSNumber alloc] initWithInt:[stream readEnumerator:[self getLimit]]];
}

+(void) writeWithStream:(id)obj stream:(id<ICEOutputStream>)stream
{
    [stream writeEnumerator:[obj intValue] limit:[self getLimit]];
}

+(ICEInt) getLimit
{
    NSAssert(false, @"ICEEnumHelper getLimit requires override");
    return nil;
}
@end

@implementation ICESeqHelper
+(id) readWithStream:(id<ICEInputStream>)stream
{
    return [stream readSequence:[self getContained]];
}

+(void) writeWithStream:(id)obj stream:(id<ICEOutputStream>)stream
{
    return [stream writeSequence:obj c:[self getContained]];
}

+(Class) getContained
{
    NSAssert(false, @"ICESeqHelper getContained requires override");
    return nil;
}
@end

@implementation ICEDictHelper
+(id) readWithStream:(id<ICEInputStream>)stream
{
    return [stream readDictionary:[self getContained]];
}

+(void) writeWithStream:(id)obj stream:(id<ICEOutputStream>)stream
{
    [stream writeDictionary:obj c:[self getContained]];
}

+(ICEKeyValueHelper) getContained
{
    NSAssert(false, @"ICEDictHelper getContained requires override");
    ICEKeyValueHelper dummy;
    return dummy; // Keep compiler quiet
}
@end
