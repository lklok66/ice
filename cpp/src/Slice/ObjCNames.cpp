// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Slice/ObjCNames.h>
#include <ctype.h>
#include <cstring>

using namespace std;

namespace Slice
{

namespace ObjC
{

struct Node
{
    const char** names;
    const Node** parents;
};

// TODO: adjust these as appropriate

// TODO: what to do about methods with parameters, such as performSelector:WithObject: ?

static const char* NSObjectNames[] =
                    {
                        "autorelease", "class", "conformsToProtocol:", "description",
                        "hash", "isEqual:", "isKindOfClass:", "isMemberOfClass:", "isProxy",
			"performSelector:", "performSelector:withObject:", "performSelector:withObject:withObject:",
			"release", "respondstoSelector:", "retain", "retainCount", "self", "superclass", "zone", 0
                    };
static const Node* NSObjectParents[] =
                    {
                        0
                    };
static const Node NSObjectNode =
                    {
                        NSObjectNames, &NSObjectParents[0]
                    };

static const char* NSCopyingNames[] =
                    {
		        "copyWithZone:", 0
		    };
static const Node* NSCopyingParents[] =
                    {
		        0
		    };
static const Node NSCopyingNode =
                    {
		        NSCopyingNames, &NSCopyingParents[0]
		    };

static const char* NSCodingNames[] =
                    {
		        "encodeWithCoder:", "initWithCoder::", 0
		    };
static const Node* NSCodingParents[] =
                    {
		        0
		    };
static const Node NSCodingNode =
                    {
		        NSCodingNames, &NSCodingParents[0]
		    };

static const char* ICloneableNames[] =
                    {
                        "Clone", 0
                    };
static const Node* ICloneableParents[] =
                    {
                        &NSObjectNode, 0
                    };
static const Node ICloneableNode =
                    {
                        ICloneableNames, &ICloneableParents[0]
                    };

static const char* NSExceptionNames[] =
                    {
                        "callStackReturnAddresses", "initWithName:reason:userInfo:", "name", "raise",
			"reason", "userInfo", 0
                    };
static const Node* NSExceptionParents[] =
                    {
                        &NSObjectNode, &NSCopyingNode, &NSCodingNode, 0
                    };
static const Node NSExceptionNode =
                    {
                        NSExceptionNames, &NSExceptionParents[0]
                    };

//
// Must be kept in same order as definition of BaseType in header file!
//
static const Node* nodes[] =
                    {
                        &NSObjectNode, &NSCopyingNode, &NSExceptionNode
                    };

static bool
ciEquals(const string& s, const char* p)
{
    if(s.size() != strlen(p))
    {
        return false;
    }
    string::const_iterator i = s.begin();
    while(i != s.end())
    {
        if(tolower(*i++) != tolower(*p++))
        {
            return false;
        }
    }
    return true;
}

const char* manglePrefix = "";
const char* mangleSuffix = "_";

static bool
mangle(const string& s, const Node* np, string& newName)
{
    const char** namep = np->names;
    while(*namep)
    {
        if(ciEquals(s, *namep))
        {
            newName = manglePrefix + s + mangleSuffix;
            return true;
        }
        ++namep;
    }
    const Node** parentp = np->parents;
    while(*parentp)
    {
        if(mangle(s, *parentp, newName))
        {
            return true;
        }
        ++parentp;
    }
    return false;
}

}

}

string
Slice::ObjC::mangleName(const string& s, int baseTypes)
{
    if(baseTypes == 0)
    {
        return s;
    }
    string newName;
    for(unsigned int mask = 1, i=0; mask < END; mask <<= 1, ++i)
    {
        if(baseTypes & mask)
        {
            if(mangle(s, nodes[i], newName))
            {
                return newName;
            }
        }
    }
    return s;
}
