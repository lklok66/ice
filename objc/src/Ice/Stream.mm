// **********************************************************************
//
// Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/StreamI.h>
#import <Ice/CommunicatorI.h>
#import <Ice/ProxyI.h>
#import <Ice/Util.h>
#import <Ice/ObjectI.h>
#import <Ice/LocalException.h>

#include <IceCpp/Stream.h>

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

    virtual void ice_preMarshal()
    {
        [_obj ice_preMarshal];
    }

private:

    ICEObject* _obj;
    ICEOutputStream* _stream;
};

class ObjectReader : public Ice::ObjectReader
{
public:
    
    // We must explicitely CFRetain/CFRelease so that the garbage
    // collector does not trash the _obj.
    ObjectReader(ICEObject* obj) : _obj(obj)
    {
        CFRetain(_obj);
    }

    virtual ~ObjectReader()
    {
        CFRelease(_obj);
    }

    virtual void
    read(const Ice::InputStreamPtr& stream, bool rid)
    {
        @try
        {
            
            //
            // TODO: explain why calling getWrapperWithCxxObjectNoAutoRelease is safe here
            //
            ICEInputStream* is = [ICEInputStream getWrapperWithCxxObjectNoAutoRelease:stream.get()];
            assert(is != 0);
            [_obj read__:is readTypeId:rid];
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

    virtual void ice_postUnmarshal()
    {
        [_obj ice_postUnmarshal];
    }

private:

    ICEObject* _obj;
};
typedef IceUtil::Handle<ObjectReader> ObjectReaderPtr;

class ReadObjectBase : public Ice::ReadObjectCallback
{
public:
    
    ReadObjectBase(NSString* expectedType) : _expectedType(expectedType)
    {
    }
    
    void checkType(ICEObject*);

private:

    NSString* _expectedType;
};

void
ReadObjectBase::checkType(ICEObject* o)
{
    if(![o ice_isA:_expectedType])
    {
        NSString* actualType = [[o class] ice_staticId];
        NSString* expectedType = _expectedType;
        NSString* reason = [NSString stringWithFormat:@"expected element of type `%@' but received `%@'",
                                     expectedType, actualType];
        
        @throw [ICEUnexpectedObjectException unexpectedObjectException:__FILE__
                                             line:__LINE__
                                             reason_:reason
                                             type:actualType
                                             expectedType:expectedType];
    }
}

class ReadObject : public ReadObjectBase
{
public:
    
    ReadObject(ICEObject** addr, NSString* expectedType) : ReadObjectBase(expectedType), _addr(addr)
    {
    }

    virtual void
    invoke(const Ice::ObjectPtr& obj)
    {
        @try
        {
            if(obj)
            {
                ICEObject* o = ObjectReaderPtr::dynamicCast(obj)->getObject();
                checkType(o);
                *_addr = [o retain];
            }
            else
            {
                *_addr = nil;
            }
        }
        @catch(NSException* ex)
        {
            rethrowCxxException(ex);
        }
    }

private:

    ICEObject** _addr;
};

class ReadObjectAtIndex : public ReadObjectBase
{
public:
    
    ReadObjectAtIndex(NSMutableArray* array, ICEInt index, NSString* expectedType) : 
        ReadObjectBase(expectedType), _array(array), _index(index)
    {
    }

    virtual void
    invoke(const Ice::ObjectPtr& obj)
    {
        @try
        {
            if(obj)
            {
                ICEObject* o = ObjectReaderPtr::dynamicCast(obj)->getObject();
                checkType(o);
                [_array replaceObjectAtIndex:_index withObject:o];
            }
        }
        @catch(NSException* ex)
        {
            rethrowCxxException(ex);
        }
    }

private:

    NSMutableArray* _array;
    ICEInt _index;
};

class ReadObjectForKey : public ReadObjectBase
{
public:
    
    // We must explicitely CFRetain/CFRelease so that the garbage
    // collector does not trash the _key.
    ReadObjectForKey(NSMutableDictionary* dict, id key, NSString* expectedType) : 
        ReadObjectBase(expectedType), _dict(dict), _key(key)
    {
        CFRetain(_key);
    }

    virtual ~ReadObjectForKey()
    {
        CFRelease(_key);
    }
    
    virtual void
    invoke(const Ice::ObjectPtr& obj)
    {
        @try
        {
            if(obj)
            {
                ICEObject* o = ObjectReaderPtr::dynamicCast(obj)->getObject();
                checkType(o);
                [_dict setObject:o forKey:_key];
            }
            else
            {
                [_dict setObject:[NSNull null] forKey:_key];
            }
        }
        @catch(NSException* ex)
        {
            rethrowCxxException(ex);
        }
    }

private:

    NSMutableDictionary* _dict;
    id _key;
};

class ReadObjectCallback : public ReadObjectBase
{
public:
    
    // We must explicitely CFRetain/CFRelease so that the garbage
    // collector does not trash the _cb.
    ReadObjectCallback(id<ICEReadObjectCallback> cb, NSString* expectedType) : 
        ReadObjectBase(expectedType), _cb(cb)
    {
        CFRetain(_cb);
    }

    virtual ~ReadObjectCallback()
    {
        CFRelease(_cb);
    }
    
    virtual void
    invoke(const Ice::ObjectPtr& obj)
    {
        @try
        {
            if(obj)
            {
                ICEObject* o = ObjectReaderPtr::dynamicCast(obj)->getObject();
                checkType(o);
                [_cb invoke:o];
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

+(Ice::Object*)createObjectReader:(ICEObject*)obj
{
    return new IceObjC::ObjectReader(obj);
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
    NSException* nsex = nil;
    try
    {
        is_->sliceObjects(b);
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    if(nsex != nil)
    {
        @throw nsex;
    }
}

-(BOOL)readBool
{
    NSException* nsex = nil;
    try
    {
        return is_->readBool();
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
    return NO; // Keep the compiler happy.
}

-(NSMutableData*) readBoolSeq
{
    NSException* nsex = nil;
    try
    {
        std::pair<const bool*, const bool*> seq;
        is_->readBoolSeq(seq);
        return [[NSMutableData alloc] initWithBytes:seq.first length:(seq.second - seq.first) * sizeof(BOOL)];
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
    return nil; // Keep the compiler happy.
}

-(ICEByte) readByte
{
    NSException* nsex = nil;
    try
    {
        return is_->readByte();
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
    return nil; // Keep the compiler happy.
}

-(NSMutableData*) readByteSeq
{
    NSException* nsex = nil;
    try
    {
        std::pair<const Ice::Byte*, const Ice::Byte*> seq;
        is_->readByteSeq(seq);
        return [[NSMutableData alloc] initWithBytes:seq.first length:(seq.second - seq.first)];
    }    
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
    return nil; // Keep the compiler happy.
}

-(NSData*) readByteSeqNoCopy
{
    NSException* nsex = nil;
    try
    {
        std::pair<const Ice::Byte*, const Ice::Byte*> seq;
        is_->readByteSeq(seq);
        return [NSData dataWithBytesNoCopy:const_cast<Ice::Byte*>(seq.first) 
                       length:(seq.second - seq.first) freeWhenDone:NO];
    }    
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
    return nil; // Keep the compiler happy.
}

-(ICEShort) readShort
{
    NSException* nsex = nil;
    try
    {
        return is_->readShort();
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
    return 0; // Keep the compiler happy.
}

-(NSMutableData*) readShortSeq
{
    NSException* nsex = nil;
    try
    {
        std::pair<const Ice::Short*, const Ice::Short*> seq;
        is_->readShortSeq(seq);
        return [[NSMutableData alloc] initWithBytes:seq.first length:(seq.second - seq.first) * sizeof(ICEShort)];
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
    return nil; // Keep the compiler happy.
}

-(ICEInt) readInt
{
    NSException* nsex = nil;
    try
    {
        return is_->readInt();
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
    return 0; // Keep the compiler happy.
}

-(NSMutableData*) readIntSeq
{
    NSException* nsex = nil;
    try
    {
        std::pair<const Ice::Int*, const Ice::Int*> seq;
        is_->readIntSeq(seq);
        return [[NSMutableData alloc] initWithBytes:seq.first length:(seq.second - seq.first) * sizeof(ICEInt)];
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
    return nil; // Keep the compiler happy.
}

-(ICELong) readLong
{
    NSException* nsex = nil;
    try
    {
        return is_->readLong();
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
    return 0; // Keep the compiler happy.
}

-(NSMutableData*) readLongSeq
{
    NSException* nsex = nil;
    try
    {
        std::pair<const Ice::Long*, const Ice::Long*> seq;
        is_->readLongSeq(seq);
        return [[NSMutableData alloc] initWithBytes:seq.first length:(seq.second - seq.first) * sizeof(ICELong)];
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
    return nil; // Keep the compiler happy.
}

-(ICEFloat) readFloat
{
    NSException* nsex = nil;
    try
    {
        return is_->readFloat();
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
    return nil; // Keep the compiler happy.
}

-(NSMutableData*) readFloatSeq
{
    NSException* nsex = nil;
    try
    {
        std::pair<const Ice::Float*, const Ice::Float*> seq;
        is_->readFloatSeq(seq);
        return [[NSMutableData alloc] initWithBytes:seq.first length:(seq.second - seq.first) * sizeof(ICEFloat)];
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
    return nil; // Keep the compiler happy.
}

-(ICEDouble) readDouble
{
    NSException* nsex = nil;
    try
    {
        return is_->readDouble();
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
    return nil; // Keep the compiler happy.
}

-(NSMutableData*) readDoubleSeq
{
    NSException* nsex = nil;
    try
    {
        std::pair<const Ice::Double*, const Ice::Double*> seq;
        is_->readDoubleSeq(seq);
        return [[NSMutableData alloc] initWithBytes:seq.first length:(seq.second - seq.first) * sizeof(ICEDouble)];
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
    return nil; // Keep the compiler happy.
}

-(NSMutableString*)readString
{
    NSException* nsex = nil;
    try
    {
        return toNSMutableString(is_->readString());
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
    return nil; // Keep the compiler happy.
}

-(NSMutableArray*) readStringSeq
{
    NSException* nsex = nil;
    try
    {
        return toNSArray(is_->readStringSeq());
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
    return nil; // Keep the compiler happy.
}

-(ICEInt) readEnumerator:(ICEInt)limit
{
    NSException* nsex = nil;
    ICEInt val = 0; // Keep the compiler happy.
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
        nsex = toObjCException(ex);
    }
    if(nsex != nil)
    {
        @throw nsex;
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

    NSException* nsex = nil;
    NSMutableData* ret = 0;
    try
    {
	int count = is_->readAndCheckSeqSize(minWireSize);
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
        nsex = toObjCException(ex);
    }
    if(nsex != nil)
    {
        @throw nsex;
    }
    return ret;
}

-(ICEInt) readSize
{
    NSException* nsex = nil;
    try
    {
        return is_->readSize();
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
    return nil; // Keep the compiler happy.
}

-(ICEInt) readAndCheckSeqSize:(ICEInt)minSize
{
    NSException* nsex = nil;
    try
    {
        return is_->readAndCheckSeqSize(minSize);
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
    return nil; // Keep the compiler happy.
}

-(id<ICEObjectPrx>) readProxy:(Class)type
{
    NSException* nsex = nil;
    try
    {
	Ice::ObjectPrx p = is_->readProxy();
	if(!p)
	{
	     return nil;
	}
        else
        {
            return [[type alloc] initWithObjectPrx__:p];
        }
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
    return nil; // Keep the compiler happy.
}

-(void) readObject:(ICEObject**)object typeId:(NSString*)typeId
{
    NSException* nsex = nil;
    try
    {
        is_->readObject(new IceObjC::ReadObject(object, typeId));
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    if(nsex != nil)
    {
        @throw nsex;
    }
}

-(void) readObjectWithCallback:(id<ICEReadObjectCallback>)callback typeId:(NSString*)typeId
{
    NSException* nsex = nil;
    try
    {
        is_->readObject(new IceObjC::ReadObjectCallback(callback, typeId));
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    if(nsex != nil)
    {
        @throw nsex;
    }
}

-(NSMutableArray*) readObjectSeq:(NSString*)typeId
{
    ICEInt sz = [self readAndCheckSeqSize:4];
    NSMutableArray* arr = [[NSMutableArray alloc] initWithCapacity:sz];
    NSException* nsex = nil;
    try
    {
        int i;
        id null = [NSNull null];
        for(i = 0; i < sz; i++)
        {
            [arr addObject:null];
            is_->readObject(new IceObjC::ReadObjectAtIndex(arr, i, typeId));
        }
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    if(nsex != nil)
    {
        [arr release];
        @throw nsex;
    }
    return arr;
}

-(NSMutableDictionary*) readObjectDict:(Class)keyType typeId:(NSString*)typeId
{
    ICEInt sz = [self readAndCheckSeqSize:[keyType minWireSize] + 4];
    NSMutableDictionary* dictionary = [[NSMutableDictionary alloc] initWithCapacity:sz];
    id key = nil;
    for(int i = 0; i < sz; ++i)
    {
        @try
        {
            key = [keyType ice_readWithStream:self];
        }
        @catch(NSException *ex)
        {
            [dictionary release];
            @throw ex;
        }

        NSException* nsex = nil;
        try
        {
            is_->readObject(new IceObjC::ReadObjectForKey(dictionary, key, typeId));
        }
        catch(const std::exception& ex)
        {
            nsex = toObjCException(ex);
        }
        if(nsex != nil)
        {
            [key release];
            [dictionary release];
            @throw nsex;
        }
        [key release];
    }
    return dictionary;
}

-(NSMutableArray*) readSequence:(Class)type
{
    ICEInt sz = [self readAndCheckSeqSize:[type minWireSize]];
    NSMutableArray* arr = [[NSMutableArray alloc] initWithCapacity:sz];
    id obj = nil;
    @try
    {
	while(sz-- > 0)
	{
	    obj = [type ice_readWithStream:self];
            if(obj == nil)
            {
                [arr addObject:[NSNull null]];
            }
            else
            {
                [arr addObject:obj];
                [obj release];
            }
	}
    }
    @catch(NSException *ex)
    {
        [arr release];
	[obj release];
        @throw ex;
    }
    return arr;
}

-(NSMutableDictionary*) readDictionary:(ICEKeyValueTypeHelper)type
{
    ICEInt sz = [self readAndCheckSeqSize:[type.key minWireSize] + [type.value minWireSize]];
    NSMutableDictionary* dictionary = [[NSMutableDictionary alloc] initWithCapacity:sz];
    id key = nil;
    id value = nil;
    @try
    {
	while(sz-- > 0)
	{
	    key = [type.key ice_readWithStream:self];
	    value = [type.value ice_readWithStream:self];
            if(value == nil)
            {
                [dictionary setObject:[NSNull null] forKey:key];
            }
            else
            {
                [dictionary setObject:value forKey:key];
                [value release];
            }
	    [key release];
	}
    }
    @catch(NSException *ex)
    {
	[dictionary release];
	[key release];
	[value release];
        @throw ex;
    }
    return dictionary;
}

-(NSString*) readTypeId
{
    NSException* nsex = nil;
    try
    {
        return toNSString(is_->readTypeId());
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
    return nil; // Keep the compiler happy.
}

-(void) throwException
{
    ICEUserException* ex = nil;
    bool usesClasses = false;
    NSException* nsex = nil;
    try
    {
        usesClasses = is_->readBool();

        std::string typeId = is_->readString(false);
        for(;;)
        {
            typeId = toObjCSliceId(typeId, 
                                   [[ICECommunicator wrapperWithCxxObject:is_->communicator().get()] getPrefixTable]);
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
        nsex = toObjCException(ex);
    }
    if(nsex != nil)
    {
        @throw nsex;
    }

    if(ex)
    {
        [ex read__:self readTypeId:false];
        if(usesClasses)
        {
            [self readPendingObjects];
        }
        @throw [ex autorelease]; // NOTE: exceptions are always auto-released, no need for the caller to do it.
    }
}

-(void) startSlice
{
    NSException* nsex = nil;
    try
    {
        is_->startSlice();
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    if(nsex != nil)
    {
        @throw nsex;
    }
}

-(void) endSlice
{
    NSException* nsex = nil;
    try
    {
        is_->endSlice();
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    if(nsex != nil)
    {
        @throw nsex;
    }
}

-(void) skipSlice
{
    NSException* nsex = nil;
    try
    {
        is_->skipSlice();
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    if(nsex != nil)
    {
        @throw nsex;
    }
}

-(void) startEncapsulation
{
    NSException* nsex = nil;
    try
    {
        is_->startEncapsulation();
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    if(nsex != nil)
    {
        @throw nsex;
    }
}

-(void) endEncapsulation
{
    NSException* nsex = nil;
    try
    {
        is_->endEncapsulation();
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    if(nsex != nil)
    {
        @throw nsex;
    }
}

-(void) skipEncapsulation
{
    NSException* nsex = nil;
    try
    {
        is_->skipEncapsulation();
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    if(nsex != nil)
    {
        @throw nsex;
    }
}

-(void) readPendingObjects
{
    NSException* nsex = nil;
    try
    {
        is_->readPendingObjects();
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    if(nsex != nil)
    {
        @throw nsex;
    }
}

-(void) rewind
{
    NSException* nsex = nil;
    try
    {
        is_->rewind();
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    if(nsex != nil)
    {
        @throw nsex;
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
    objectWriters_ = 0;
    return self;
}

-(Ice::OutputStream*) os
{
    return os_;
}

-(void) dealloc
{
    if(objectWriters_)
    {
        delete objectWriters_;
    }
    [super dealloc];
}

-(void) finalize
{
    if(objectWriters_)
    {
        delete objectWriters_;
    }
    [super finalize];
}

// @protocol ICEOutputStream methods

-(id<ICECommunicator>) communicator
{
    return [ICECommunicator wrapperWithCxxObject:os_->communicator().get()];
}

-(void)writeBool:(BOOL)v
{
    NSException* nsex = nil;
    try
    {
        os_->writeBool(v);
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    if(nsex != nil)
    {
        @throw nsex;
    }
}

-(void) writeBoolSeq:(NSData*)v
{
    NSException* nsex = nil;
    try
    {
        v == nil ? os_->writeSize(0)
	         : os_->writeBoolSeq((bool*)[v bytes], (bool*)[v bytes] + [v length] / sizeof(BOOL));
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    if(nsex != nil)
    {
        @throw nsex;
    }
}

-(void) writeByte:(ICEByte)v
{
    NSException* nsex = nil;
    try
    {
        os_->writeByte(v);
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    if(nsex != nil)
    {
        @throw nsex;
    }
}

-(void) writeByteSeq:(NSData*)v
{ 
    NSException* nsex = nil;
    try
    {
        v == nil ? os_->writeSize(0)
                 : os_->writeByteSeq((ICEByte*)[v bytes], (ICEByte*)[v bytes] + [v length]);
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    if(nsex != nil)
    {
        @throw nsex;
    }
}

-(void) writeShort:(ICEShort)v
{
    NSException* nsex = nil;
    try
    {
        os_->writeShort(v);
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    if(nsex != nil)
    {
        @throw nsex;
    }
}

-(void) writeShortSeq:(NSData*)v
{
    NSException* nsex = nil;
    try
    {
        v == nil ? os_->writeSize(0)
                 : os_->writeShortSeq((ICEShort*)[v bytes], (ICEShort*)[v bytes] + [v length] / sizeof(ICEShort));
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    if(nsex != nil)
    {
        @throw nsex;
    }
}


-(void) writeInt:(ICEInt)v
{
    NSException* nsex = nil;
    try
    {
        os_->writeInt(v);
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    if(nsex != nil)
    {
        @throw nsex;
    }
}

-(void) writeIntSeq:(NSData*)v
{
    NSException* nsex = nil;
    try
    {
        v == nil ? os_->writeSize(0)
                 : os_->writeIntSeq((ICEInt*)[v bytes], (ICEInt*)[v bytes] + [v length] / sizeof(ICEInt));
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    if(nsex != nil)
    {
        @throw nsex;
    }
}

-(void) writeLong:(ICELong)v
{
    NSException* nsex = nil;
    try
    {
        os_->writeLong(v);
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    if(nsex != nil)
    {
        @throw nsex;
    }
}

-(void) writeLongSeq:(NSData*)v
{
    NSException* nsex = nil;
    try
    {
        v == nil ? os_->writeSize(0)
                 : os_->writeLongSeq((ICELong*)[v bytes], (ICELong*)[v bytes] + [v length] / sizeof(ICELong));
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    if(nsex != nil)
    {
        @throw nsex;
    }
}


-(void) writeFloat:(ICEFloat)v
{
    NSException* nsex = nil;
    try
    {
        os_->writeFloat(v);
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    if(nsex != nil)
    {
        @throw nsex;
    }
}

-(void) writeFloatSeq:(NSData*)v
{
    NSException* nsex = nil;
    try
    {
        v == nil ? os_->writeSize(0)
                 : os_->writeFloatSeq((ICEFloat*)[v bytes], (ICEFloat*)[v bytes] + [v length] / sizeof(ICEFloat));
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    if(nsex != nil)
    {
        @throw nsex;
    }
}


-(void) writeDouble:(ICEDouble)v
{
    NSException* nsex = nil;
    try
    {
        os_->writeDouble(v);
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    if(nsex != nil)
    {
        @throw nsex;
    }
}

-(void) writeDoubleSeq:(NSData*)v
{
    NSException* nsex = nil;
    try
    {
        v == nil ? os_->writeSize(0)
                 : os_->writeDoubleSeq((ICEDouble*)[v bytes],
		                           (ICEDouble*)[v bytes] + [v length] / sizeof(ICEDouble));
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    if(nsex != nil)
    {
        @throw nsex;
    }
}


-(void) writeString:(NSString*)v
{
    NSException* nsex = nil;
    try
    {
	os_->writeString(fromNSString(v));
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    if(nsex != nil)
    {
        @throw nsex;
    }
}

-(void) writeStringSeq:(NSArray*)v
{
    NSException* nsex = nil;
    try
    {
	std::vector<std::string> s;
	os_->writeStringSeq(fromNSArray(v, s));
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    if(nsex != nil)
    {
        @throw nsex;
    }
}

-(void)writeSequence:(NSArray*)arr type:(Class)type
{
    if(arr == nil)
    {
        [self writeSize:0];
        return;
    }

    [self writeSize:[arr count]];
    for(id i in arr)
    {
	[type ice_writeWithStream:(i == [NSNull null] ? nil : i) stream:self];
    }
}

-(void) writeDictionary:(NSDictionary*)dictionary type:(ICEKeyValueTypeHelper)type
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
	if(key == [NSNull null])
	{
	    @throw [ICEMarshalException marshalException:__FILE__ line:__LINE__ reason_:@"illegal NSNull value"];
	}
	[type.key ice_writeWithStream:key stream:self];
	NSObject *obj = [dictionary objectForKey:key];
	[type.value ice_writeWithStream:(obj == [NSNull null] ? nil : obj) stream:self];
    }
}

-(void) writeEnumerator:(ICEInt)v limit:(int)limit
{
    if(v >= limit)
    {
	@throw [ICEMarshalException marshalException:__FILE__ line:__LINE__ reason_:@"enumerator out of range"];
    }
    NSException* nsex = nil;
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
        nsex = toObjCException(ex);
    }
    if(nsex != nil)
    {
        @throw nsex;
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
    NSException* nsex = nil;
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
        nsex = toObjCException(ex);
    }
    if(nsex != nil)
    {
        @throw nsex;
    }
}

-(void) writeSize:(ICEInt)v
{
    NSException* nsex = nil;
    try
    {
        os_->writeSize(v);
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    if(nsex != nil)
    {
        @throw nsex;
    }
}


-(void) writeProxy:(id<ICEObjectPrx>)v
{
    NSException* nsex = nil;
    try
    {
        os_->writeProxy([(ICEObjectPrx*)v objectPrx__]);
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    if(nsex != nil)
    {
        @throw nsex;
    }
}

-(void) writeObject:(ICEObject*)v typeId:(NSString*)typeId
{
    if(v && ![v ice_isA:typeId])
    {
        NSString* actualType = [[v class] ice_staticId];
        NSString* expectedType = typeId;
        NSString* reason = [NSString stringWithFormat:@"expected to marshal object of type `%@', not `%@'",
                                     expectedType, actualType];
        
        @throw [ICEUnexpectedObjectException unexpectedObjectException:__FILE__
                                             line:__LINE__
                                             reason_:reason
                                             type:actualType
                                             expectedType:expectedType];
    }

    NSException* nsex = nil;
    try
    {
        if(v == nil)
        {
            os_->writeObject(0);
        }
        else
        {
            //
            // Ice::ObjectWriter is a subclass of Ice::Object that wraps an Objective-C object for marshaling.
            // It is possible that this Objective-C object has already been marshaled, therefore we first must
            // check the object map to see if this object is present. If so, we use the existing ObjectWriter,
            // otherwise we create a new one.
            //
            if(!objectWriters_)
            {
                objectWriters_ = new std::map<ICEObject*, Ice::ObjectPtr>();
            }
            std::map<ICEObject*, Ice::ObjectPtr>::const_iterator p = objectWriters_->find(v);
            Ice::ObjectPtr writer;
            if(p != objectWriters_->end())
            {
                writer = p->second;
            }
            else
            {
                writer = new IceObjC::ObjectWriter(v, self);
                objectWriters_->insert(std::make_pair(v, writer));
            }
            os_->writeObject(writer);
        }
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    if(nsex != nil)
    {
        @throw nsex;
    }
}

-(void) writeObjectSeq:(NSArray*)arr typeId:(NSString*)typeId
{
    if(arr == nil)
    {
        [self writeSize:0];
        return;
    }

    [self writeSize:[arr count]];
    for(id i in arr)
    {
        [self writeObject:(i == [NSNull null] ? nil : i) typeId:typeId];
    }
}

-(void) writeObjectDict:(NSDictionary*)dictionary keyType:(Class)keyType typeId:(NSString*)typeId
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
	if(key == [NSNull null])
	{
	    @throw [ICEMarshalException marshalException:__FILE__ line:__LINE__ reason_:@"illegal NSNull value"];
	}
	[keyType ice_writeWithStream:key stream:self];
	id obj = [dictionary objectForKey:key];
        [self writeObject:(obj == [NSNull null] ? nil : obj) typeId:typeId];
    }
}

-(void) writeTypeId:(NSString*)v
{
    NSException* nsex = nil;
    try
    {
        os_->writeTypeId([v UTF8String]);
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    if(nsex != nil)
    {
        @throw nsex;
    }
}

-(void) writeException:(ICEUserException*)v
{
    BOOL usesClasses = [v usesClasses__]; 
    [self writeBool:usesClasses];
    [v write__:self];
    if(usesClasses)
    {
        [self writePendingObjects];
    }
}

-(void) startSlice
{
    NSException* nsex = nil;
    try
    {
        os_->startSlice();
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    if(nsex != nil)
    {
        @throw nsex;
    }
}

-(void) endSlice
{
    NSException* nsex = nil;
    try
    {
        os_->endSlice();
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    if(nsex != nil)
    {
        @throw nsex;
    }
}

-(void) startEncapsulation
{
    NSException* nsex = nil;
    try
    {
        os_->startEncapsulation();
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    if(nsex != nil)
    {
        @throw nsex;
    }
}

-(void) endEncapsulation
{
    NSException* nsex = nil;
    try
    {
        os_->endEncapsulation();
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    if(nsex != nil)
    {
        @throw nsex;
    }
}

-(void) writePendingObjects
{
    NSException* nsex = nil;
    try
    {
        os_->writePendingObjects();
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    if(nsex != nil)
    {
        @throw nsex;
    }
}

-(NSMutableData*) finished
{
    NSException* nsex = nil;
    try
    {
        std::vector<Ice::Byte> buf;
        os_->finished(buf);
        return [NSMutableData dataWithBytes:&buf[0] length:buf.size()];
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
    return nil; // Keep the compiler happy.
}

-(void) reset:(BOOL) clearBuffer
{
    NSException* nsex = nil;
    try
    {
        os_->reset(clearBuffer);
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    if(nsex != nil)
    {
        @throw nsex;
    }
}
@end

@implementation ICEBoolHelper
+(id) ice_readWithStream:(id<ICEInputStream>)stream
{
    return [[NSNumber alloc] initWithBool:[stream readBool]];
}

+(void) ice_writeWithStream:(id)obj stream:(id<ICEOutputStream>)stream
{
    if(obj == nil)
    {
	@throw [ICEMarshalException marshalException:__FILE__ line:__LINE__ reason_:@"illegal NSNull value"];
    }
    [stream writeBool:[obj boolValue]];
}

+(ICEInt) minWireSize
{
    return 1;
}
@end

@implementation ICEByteHelper
+(id) ice_readWithStream:(id<ICEInputStream>)stream
{
    return [[NSNumber alloc] initWithUnsignedChar:[stream readByte]];
}

+(void) ice_writeWithStream:(id)obj stream:(id<ICEOutputStream>)stream
{
    if(obj == nil)
    {
	@throw [ICEMarshalException marshalException:__FILE__ line:__LINE__ reason_:@"illegal NSNull value"];
    }
    [stream writeByte:[obj unsignedCharValue]];
}

+(ICEInt) minWireSize
{
    return 1;
}
@end

@implementation ICEShortHelper
+(id) ice_readWithStream:(id<ICEInputStream>)stream
{
    return [[NSNumber alloc] initWithShort:[stream readShort]];
}

+(void) ice_writeWithStream:(id)obj stream:(id<ICEOutputStream>)stream
{
    if(obj == nil)
    {
	@throw [ICEMarshalException marshalException:__FILE__ line:__LINE__ reason_:@"illegal NSNull value"];
    }
    [stream writeShort:[obj shortValue]];
}

+(ICEInt) minWireSize
{
    return 2;
}
@end

@implementation ICEIntHelper
+(id) ice_readWithStream:(id<ICEInputStream>)stream
{
    return [[NSNumber alloc] initWithInt:[stream readInt]];
}

+(void) ice_writeWithStream:(id)obj stream:(id<ICEOutputStream>)stream
{
    if(obj == nil)
    {
	@throw [ICEMarshalException marshalException:__FILE__ line:__LINE__ reason_:@"illegal NSNull value"];
    }
    [stream writeInt:[obj intValue]];
}

+(ICEInt) minWireSize
{
    return 4;
}
@end

@implementation ICELongHelper
+(id) ice_readWithStream:(id<ICEInputStream>)stream
{
    return [[NSNumber alloc] initWithLong:[stream readLong]];
}

+(void) ice_writeWithStream:(id)obj stream:(id<ICEOutputStream>)stream
{
    if(obj == nil)
    {
	@throw [ICEMarshalException marshalException:__FILE__ line:__LINE__ reason_:@"illegal NSNull value"];
    }
    [stream writeLong:[obj longValue]];
}

+(ICEInt) minWireSize
{
    return 8;
}
@end

@implementation ICEFloatHelper
+(id) ice_readWithStream:(id<ICEInputStream>)stream
{
    return [[NSNumber alloc] initWithFloat:[stream readFloat]];
}

+(void) ice_writeWithStream:(id)obj stream:(id<ICEOutputStream>)stream
{
    if(obj == nil)
    {
	@throw [ICEMarshalException marshalException:__FILE__ line:__LINE__ reason_:@"illegal NSNull value"];
    }
    [stream writeFloat:[obj floatValue]];
}

+(ICEInt) minWireSize
{
    return 4;
}
@end

@implementation ICEDoubleHelper
+(id) ice_readWithStream:(id<ICEInputStream>)stream
{
    return [[NSNumber alloc] initWithDouble:[stream readDouble]];
}

+(void) ice_writeWithStream:(id)obj stream:(id<ICEOutputStream>)stream
{
    if(obj == nil)
    {
	@throw [ICEMarshalException marshalException:__FILE__ line:__LINE__ reason_:@"illegal NSNull value"];
    }
    [stream writeDouble:[obj doubleValue]];
}

+(ICEInt) minWireSize
{
    return 8;
}
@end

@implementation ICEStringHelper
+(id) ice_readWithStream:(id<ICEInputStream>)stream
{
    return [stream readString];
}

+(void) ice_writeWithStream:(id)obj stream:(id<ICEOutputStream>)stream
{
    if(obj == [NSNull null])
    {
        obj = nil;
    }
    [stream writeString:obj];
}

+(ICEInt) minWireSize
{
    return 1;
}
@end

@implementation ICEEnumHelper
+(id) ice_readWithStream:(id<ICEInputStream>)stream
{
    return [[NSNumber alloc] initWithInt:[stream readEnumerator:[self getLimit]]];
}

+(void) ice_writeWithStream:(id)obj stream:(id<ICEOutputStream>)stream
{
    if(obj == nil)
    {
	@throw [ICEMarshalException marshalException:__FILE__ line:__LINE__ reason_:@"illegal NSNull value"];
    }
    [stream writeEnumerator:[obj intValue] limit:[self getLimit]];
}

+(ICEInt) getLimit
{
    NSAssert(false, @"ICEEnumHelper getLimit requires override");
    return nil;
}

+(ICEInt) minWireSize
{
    ICEInt limit = [self getLimit];
    if(limit <= 0x7f)
    {
        return 1;
    }
    else if(limit <= 0x7fff)
    {
        return 2;
    }
    else
    {
        return 4;
    }
}
@end

@implementation ICEObjectPrxSequenceHelper
+(id) ice_readWithStream:(id<ICEInputStream>)stream
{
    return [stream readSequence:[ICEObjectPrx class]];
}
+(void) ice_writeWithStream:(id)obj stream:(id<ICEOutputStream>)stream
{
    return [stream writeSequence:obj type:[ICEObjectPrx class]];
}
+(ICEInt) minWireSize
{
    return 1;
}
@end

@implementation ICEObjectSequenceHelper
+(id) ice_readWithStream:(id<ICEInputStream>)stream
{
    return [stream readObjectSeq:[self getContained]];
}

+(void) ice_writeWithStream:(id)obj stream:(id<ICEOutputStream>)stream
{
    [stream writeObjectSeq:obj typeId:[self getContained]];
}

+(NSString*) getContained
{
    return @"::Ice::Object";
}

+(ICEInt) minWireSize
{
    return 1;
}
@end

@implementation ICEObjectDictionaryHelper
+(id) ice_readWithStream:(id<ICEInputStream>)stream
{
    NSString* typeId;
    Class keyClass = [self getContained:&typeId];
    return [stream readObjectDict:keyClass typeId:typeId];
}

+(void) ice_writeWithStream:(id)obj stream:(id<ICEOutputStream>)stream
{
    NSString* typeId;
    Class keyClass = [self getContained:&typeId];
    [stream writeObjectDict:obj keyType:keyClass typeId:typeId];
}

+(Class) getContained:(NSString**)typeId
{
    NSAssert(false, @"ICEObjectDictionaryHelper getContained requires override");
    *typeId = nil;
    return nil;
}
+(ICEInt) minWireSize
{
    return 1;
}
@end

@implementation ICESequenceHelper
+(id) ice_readWithStream:(id<ICEInputStream>)stream
{
    return [stream readSequence:[self getContained]];
}

+(void) ice_writeWithStream:(id)obj stream:(id<ICEOutputStream>)stream
{
    return [stream writeSequence:obj type:[self getContained]];
}

+(Class) getContained
{
    NSAssert(false, @"ICESequenceHelper getContained requires override");
    return nil;
}
+(ICEInt) minWireSize
{
    return 1;
}
@end

@implementation ICEDictionaryHelper
+(id) ice_readWithStream:(id<ICEInputStream>)stream
{
    return [stream readDictionary:[self getContained]];
}

+(void) ice_writeWithStream:(id)obj stream:(id<ICEOutputStream>)stream
{
    [stream writeDictionary:obj type:[self getContained]];
}

+(ICEKeyValueTypeHelper) getContained
{
    NSAssert(false, @"ICEDictionaryHelper getContained requires override");
    ICEKeyValueTypeHelper dummy;
    return dummy; // Keep compiler quiet
}

+(ICEInt) minWireSize
{
    return 1;
}
@end
