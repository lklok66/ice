// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICETOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Ice/Config.h>

#import <Foundation/NSException.h>

#include <exception>
#include <vector>
#include <map>
#include <string>


void rethrowObjCException(const std::exception& ex);
void rethrowCxxException(NSException* ex, bool = false);

//
// The toXXX methods don't auto release the returned object: the caller
// must assume ownership of the returned object.
//

inline id toObjC(bool v)      { return [[NSNumber alloc] initWithBool:v]; }
inline id toObjC(ICEByte v)   { return [[NSNumber alloc] initWithUnsignedChar:v]; }
inline id toObjC(ICEShort v)  { return [[NSNumber alloc] initWithShort:v]; }
inline id toObjC(ICEInt v)    { return [[NSNumber alloc] initWithInt:v]; }
inline id toObjC(ICELong v)   { return [[NSNumber alloc] initWithLongLong:v]; }
inline id toObjC(ICEFloat v)  { return [[NSNumber alloc] initWithFloat:v]; }
inline id toObjC(ICEDouble v) { return [[NSNumber alloc] initWithDouble:v]; }

inline void fromObjC(id object, bool& v)      { v = [object boolValue]; }
inline void fromObjC(id object, ICEByte& v)   { v = [object unsignedCharValue]; }
inline void fromObjC(id object, ICEShort& v)  { v = [object shortValue]; }
inline void fromObjC(id object, ICEInt& v)    { v = [object intValue];}
inline void fromObjC(id object, ICELong& v)   { v = [object longLongValue]; }
inline void fromObjC(id object, ICEFloat& v)  { v = [object floatValue]; }
inline void fromObjC(id object, ICEDouble& v) { v = [object doubleValue]; }

inline id
toObjC(const std::string& s)
{
    return [[NSString alloc] initWithUTF8String:s.c_str()];
}

inline void
fromObjC(id object, std::string& s)
{
    s = object == [NSNull null] ? ::std::string() : [object UTF8String];
}

inline NSMutableArray*
toNSArray(const char* arr[], size_t size)
{
    NSMutableArray* array = [[NSMutableArray alloc] initWithCapacity:size];
    for(size_t i = 0; i < size; ++i)
    {
        NSObject* obj = [[NSString alloc] initWithUTF8String:arr[i]];
        [array addObject:obj];
        [obj release];
    }
    return array;
}

template<typename T> NSMutableArray*
toNSArray(const std::vector<T>& seq)
{
    NSMutableArray* array = [[NSMutableArray alloc] initWithCapacity:seq.size()];
    for(typename std::vector<T>::const_iterator p = seq.begin(); p != seq.end(); ++p)
    {
        NSObject* obj = toObjC(*p);
        [array addObject:obj];
        [obj release];
    }
    return array;
}

template<typename T> NSMutableData*
toNSData(const std::vector<T>& seq)
{
    NSMutableData* array = [[NSMutableData alloc] initWithLength:seq.size() * sizeof(T)];
    T* target = (T*)[array bytes];
    for(typename std::vector<T>::const_iterator p = seq.begin(); p != seq.end(); ++p)
    {
	*target++ = *p;
    }
    return array;
}

template<typename T> std::vector<T>&
fromNSArray(NSArray* array, std::vector<T>& seq)
{
    if(array != nil)
    {
        seq.reserve([array count]);
        NSEnumerator* enumerator = [array objectEnumerator]; 
        id obj = nil; 
        while((obj = [enumerator nextObject])) 
        { 
            T v;
	    fromObjC(obj, v);
            seq.push_back(v);
        }
    }
    return seq;
}

template<typename T> std::vector<T>&
fromNSData(NSData* array, std::vector<T>& seq)
{
    if(array != nil)
    {
	int len = [array length] / sizeof(T);
        seq.reserve(len);
	T* src = (T*)[array bytes];
	while(len-- > 0)
	{
            seq.push_back(*src++);
	}
    }
    return seq;
}

template<typename K, typename V> NSMutableDictionary*
toNSDictionary(const std::map<K, V>& dict)
{
    NSMutableDictionary* dictionary = [[NSMutableDictionary alloc] initWithCapacity:dict.size()];
    for(typename std::map<K, V>::const_iterator p = dict.begin(); p != dict.end(); ++p)
    {
        NSObject* key = toObjC(p->first);
        NSObject* value = toObjC(p->second);
        [dictionary setObject:value forKey:key];
        [key release];
        [value release];
    }
    return dictionary;
}

template<typename K, typename V> std::map<K, V>&
fromNSDictionary(NSDictionary* dictionary, std::map<K, V>& dict)
{
    if(dictionary != nil)
    {
        NSEnumerator* enumerator = [dictionary keyEnumerator]; 
        id obj = nil; 
        while((obj = [enumerator nextObject])) 
        { 
            K k;
            fromObjC(obj, k);
            V v;
            fromObjC([dictionary objectForKey:obj], v);
            dict.insert(std::pair<K, V>(k, v));
        }
    }
    return dict;
}

inline NSString*
toNSString(const char* s)
{
    return [[NSString alloc] initWithCString:s encoding:[NSString defaultCStringEncoding]];
}

inline NSString*
toNSString(const std::string& s)
{
    return [[NSString alloc] initWithUTF8String:s.c_str()];
}

inline NSMutableString*
toNSMutableString(const std::string& s)
{
    return [[NSMutableString alloc] initWithUTF8String:s.c_str()];
}

inline std::string
fromNSString(NSString* s)
{
    return s == nil ? std::string() : [s UTF8String];
}

std::string toObjCSliceId(const std::string&, NSDictionary*);

namespace IceObjC
{

class Exception : public std::exception
{
public:

    Exception(const char*, int, const std::string&);
    virtual ~Exception() throw();

    virtual const char* what() const throw();
    
private:
    
    ::std::string _str;
};

}
