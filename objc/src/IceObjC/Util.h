// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Foundation/NSException.h>
#import <Foundation/NSArray.h>
#import <Foundation/NSDictionary.h>
#import <Foundation/NSString.h>

#include <exception>
#include <vector>
#include <map>
#include <string>

NSException* rethrowObjCException(const std::exception& ex);

//
// The toXXX methods don't auto release the returned object: the caller
// must assume ownership of the returned object.
//

inline id
toObjC(const std::string& s)
{
    return [[NSString alloc] initWithUTF8String:s.c_str()];
}

inline void
fromObjC(id object, std::string& s)
{
    s = [object UTF8String];
}

template<typename T> NSArray*
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

template<typename T> void
fromNSArray(NSArray* array, std::vector<T>& seq)
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

template<typename K, typename V> NSDictionary*
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

template<typename K, typename V> void
fromNSDictionary(NSDictionary* dictionary, std::map<K, V>& dict)
{
    dict.reserve([dictionary count]);
    NSEnumerator* enumerator = [dictionary keyEnumerator]; 
    id obj = nil; 
    while((obj = [enumerator nextObject])) 
    { 
        K k;
        fromObjC(obj, k);
        V v;
        fromObjC([dictionary objectForKey:k], v);
        dict.insert(std::pair<K, V>(k, v));
    }
}
