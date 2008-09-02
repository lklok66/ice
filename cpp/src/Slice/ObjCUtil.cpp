// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Slice/ObjCUtil.h>
#include <Slice/ObjCNames.h>
#include <IceUtil/Functional.h>
#include <IceUtil/StringUtil.h>

#include <sys/types.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <direct.h>
#endif

#ifndef _WIN32
#include <unistd.h>
#endif

using namespace std;
using namespace Slice;
using namespace IceUtil;
using namespace IceUtilInternal;

Slice::ObjCGenerator::ModuleMap Slice::ObjCGenerator::_modules;

static string
lookupKwd(const string& name, int baseTypes, bool mangleCasts = false)
{
    //
    // Keyword list. *Must* be kept in alphabetical order.
    //
    static const string keywordList[] =
    {
	"BOOL", "NO", "Nil", "SEL", "YES", "auto", "break", "bycopy", "byref", "case", "char", "const", "continue",
	"default", "do", "double", "else", "enum", "extern", "float", "for", "goto", "id", "if", "in", "inout", "int",
	"long", "nil", "nil", "oneway", "out", "register", "return", "self", "short", "signed", "sizeof", "static",
	"struct", "super", "switch", "typedef", "union", "unsigned", "void", "volatile", "while"
    };
    bool found = binary_search(&keywordList[0],
                               &keywordList[sizeof(keywordList) / sizeof(*keywordList)],
                               name,
                               Slice::CICompare());
    if(found)
    {
        return name + "_";
    }
    if(mangleCasts && (name == "checkedCast" || name == "uncheckedCast"))
    {
        return string(ObjC::manglePrefix) + name;
    }
    return Slice::ObjC::mangleName(name, baseTypes);
    return name;
}

bool
Slice::ObjCGenerator::addModule(const ModulePtr& m, const string& name)
{
    string scoped = m->scoped();
    ModuleMap::const_iterator i = _modules.find(scoped);
    if(i != _modules.end())
    {
	if(i->second.name != name)
	{
	    return false;
	}
    }
    else
    {
	ModulePrefix mp;
	mp.m = m;
	mp.name = name;
	_modules[scoped] = mp;
    }
    return true;
}

Slice::ObjCGenerator::ModulePrefix
Slice::ObjCGenerator::modulePrefix(const ModulePtr& m)
{
    return _modules[m->scoped()];
}

string
Slice::ObjCGenerator::moduleName(const ModulePtr& m)
{
    return _modules[m->scoped()].name;
}

ModulePtr
Slice::ObjCGenerator::findModule(const ContainedPtr& cont, int baseTypes, bool mangleCasts)
{
    ModulePtr m = ModulePtr::dynamicCast(cont);
    ContainerPtr container = cont->container();
    while(container && !m)
    {
	ContainedPtr contained = ContainedPtr::dynamicCast(container);
	container = contained->container();
	m = ModulePtr::dynamicCast(contained);
    }
    assert(m);
    return m;
}

//
// If the passed name is a scoped name, return the identical scoped name,
// but with all components that are Objective-C keywords replaced by
// their prefixed version; otherwise, if the passed name is
// not scoped, but an Objective-C keyword, return the prefixed name;
// otherwise, check if the name is one of the method names of baseTypes;
// if so, prefix it with ICE; otherwise, return the name unchanged.
//
string
Slice::ObjCGenerator::fixId(const string& name, int baseTypes, bool mangleCasts)
{
    if(name.empty())
    {
        return name;
    }
    if(name[0] != ':')
    {
        return lookupKwd(name, baseTypes, mangleCasts);
    }
    StringList ids = splitScopedName(name);
    StringList newIds;
    for(StringList::const_iterator i = ids.begin(); i != ids.end(); ++i)
    {
        newIds.push_back(lookupKwd(*i, baseTypes));
    }
    stringstream result;
    for(StringList::const_iterator j = newIds.begin(); j != newIds.end(); ++j)
    {
        if(j != newIds.begin())
        {
            result << '.';
        }
        result << *j;
    }
    return result.str();
}

string
Slice::ObjCGenerator::fixId(const ContainedPtr& cont, int baseTypes, bool mangleCasts)
{
    ContainerPtr container = cont->container();
    ContainedPtr contained = ContainedPtr::dynamicCast(container);
    return fixId(cont->name(), baseTypes, mangleCasts);
}

string
Slice::ObjCGenerator::fixName(const ContainedPtr& cont, int baseTypes, bool mangleCasts)
{
    return moduleName(findModule(cont, baseTypes, mangleCasts)) + fixId(cont->name());
}

string
Slice::ObjCGenerator::typeToString(const TypePtr& type)
{
    if(!type)
    {
        return "void";
    }

    static const char* builtinTable[] =
    {
        "ICEByte",
        "BOOL",
        "ICEShort",
        "ICEInt",
        "ICELong",
        "ICEFloat",
        "ICEDouble",
        "NSString",
        "ICEObject",
        "id<ICEObjectPrx>",
        "id"		// Dummy--we don't support Slice local Object
    };

    BuiltinPtr builtin = BuiltinPtr::dynamicCast(type);
    if(builtin)
    {
        return builtinTable[builtin->kind()];
    }

    ProxyPtr proxy = ProxyPtr::dynamicCast(type);
    if(proxy)
    {
	string mName = moduleName(findModule(proxy->_class()));
        return "id<" + mName + (proxy->_class()->name()) + "Prx>";
    }

    SequencePtr seq = SequencePtr::dynamicCast(type);
    if(seq)
    {
	return fixName(seq);
    }

    DictionaryPtr d = DictionaryPtr::dynamicCast(type);
    if(d)
    {
        return "NSDictionary";
    }

    ContainedPtr contained = ContainedPtr::dynamicCast(type);
    if(contained)
    {
        return fixName(contained);
    }

    return "???";
}

string
Slice::ObjCGenerator::outTypeToString(const TypePtr& type)
{
    if(!type)
    {
        return "void";
    }

    BuiltinPtr builtin = BuiltinPtr::dynamicCast(type);
    if(builtin && builtin->kind() == Builtin::KindString)
    {
	return "NSMutableString";
    }

    SequencePtr seq = SequencePtr::dynamicCast(type);
    if(seq)
    {
	string prefix = moduleName(findModule(seq));
	return prefix + "Mutable" + seq->name();
    }

    DictionaryPtr d = DictionaryPtr::dynamicCast(type);
    if(d)
    {
	string prefix = moduleName(findModule(d));
	return prefix + "Mutable" + d->name();
    }

    return typeToString(type);
}

bool
Slice::ObjCGenerator::isValueType(const TypePtr& type)
{
    if(!type)
    {
        return true;
    }
    BuiltinPtr builtin = BuiltinPtr::dynamicCast(type);
    if(builtin)
    {
        switch(builtin->kind())
        {
            case Builtin::KindString:
            case Builtin::KindObject:
            case Builtin::KindObjectProxy:
            case Builtin::KindLocalObject:
            {
                return false;
                break;
            }
            default:
            {
                return true;
                break;
            }
        }
    }
    if(EnumPtr::dynamicCast(type))
    {
        return true;
    }
    return false;
}

bool
Slice::ObjCGenerator::isProtocolType(const TypePtr& type)
{
    if(!type)
    {
        return true;
    }
    BuiltinPtr builtin = BuiltinPtr::dynamicCast(type);
    if(builtin)
    {
        switch(builtin->kind())
        {
            case Builtin::KindObjectProxy:
            case Builtin::KindObject:
            case Builtin::KindLocalObject:
            {
                return true;
                break;
            }
            default:
            {
                return false;
                break;
            }
        }
    }
    if(ProxyPtr::dynamicCast(type) || ClassDefPtr::dynamicCast(type))
    {
        return true;
    }
    return false;
}

bool
Slice::ObjCGenerator::isString(const TypePtr& type)
{
    if(!type)
    {
        return false;
    }
    BuiltinPtr builtin = BuiltinPtr::dynamicCast(type);
    return builtin && builtin->kind() == Builtin::KindString;
}

bool
Slice::ObjCGenerator::mapsToPointerType(const TypePtr& type)
{
    if(isValueType(type))
    {
        return false;
    }
    BuiltinPtr builtin = BuiltinPtr::dynamicCast(type);
    if(builtin && builtin->kind() == Builtin::KindObjectProxy)
    {
       return false;
    }
    return !ProxyPtr::dynamicCast(type);
}

string
Slice::ObjCGenerator::getBuiltinName(const BuiltinPtr& builtin)
{
    switch(builtin->kind())
    {
	case Builtin::KindByte:
	{
	    return "Byte";
	}
	case Builtin::KindBool:
	{
	    return "Bool";
	}
	case Builtin::KindShort:
	{
	    return "Short";
	}
	case Builtin::KindInt:
	{
	    return "Int";
	}
	case Builtin::KindLong:
	{
	    return "Long";
	}
	case Builtin::KindFloat:
	{
	    return "Float";
	}
	case Builtin::KindDouble:
	{
	    return "Double";
	}
	case Builtin::KindString:
	{
	    return "String";
	}
	case Builtin::KindObject:
	{
	    return "Object";
	}
	case Builtin::KindObjectProxy:
	{
	    return "Proxy";
	}
	default:
	{
	    assert(false);
	}
    }
    return "NO__SUCH__TYPE";
}

string
Slice::ObjCGenerator::getBuiltinSelector(const BuiltinPtr& builtin, bool marshal)
{
    string rw = marshal ? "write" : "read";
    return rw + getBuiltinName(builtin);
}

//
// Split a scoped name into its components and return the components as a list of (unscoped) identifiers.
//
StringList
Slice::ObjCGenerator::splitScopedName(const string& scoped)
{
    assert(scoped[0] == ':');
    StringList ids;
    string::size_type next = 0;
    string::size_type pos;
    while((pos = scoped.find("::", next)) != string::npos)
    {
        pos += 2;
        if(pos != scoped.size())
        {
            string::size_type endpos = scoped.find("::", pos);
            if(endpos != string::npos)
            {
                ids.push_back(scoped.substr(pos, endpos - pos));
            }
        }
        next = pos;
    }
    if(next != scoped.size())
    {
        ids.push_back(scoped.substr(next));
    }
    else
    {
        ids.push_back("");
    }

    return ids;
}

void
Slice::ObjCGenerator::writeMarshalUnmarshalCode(Output &out,
                                              const TypePtr& type,
                                              const string& param,
                                              bool marshal,
                                              bool streamingAPI,
                                              bool isOutParam,
                                              const string& patchParams) const
{
    string stream;

    if(marshal)
    {
        stream = streamingAPI ? "outS_" : "os_";
    }
    else
    {
        stream = streamingAPI ? "inS_" : "is_";
    }

    BuiltinPtr builtin = BuiltinPtr::dynamicCast(type);
    if(builtin)
    {
	string selector;
        if(builtin->kind() == Builtin::KindObjectProxy)
        {
            selector = marshal ? "writeProxy" : "readProxy:[ICEObjectPrx class]";
        }
        else
        {
            selector = getBuiltinSelector(builtin, marshal);
        }

	if(marshal)
	{
	    out << nl << "[" << stream << " " << selector << ":" << param << "];";
	}
	else
	{
	    out << nl << param << " = [";
	    if(!isValueType(builtin))
	    {
	        out << "[";
	    }
	    out << stream << " " << selector;
	    if(!isValueType(builtin))
	    {
	        out << "] autorelease";
	    }
	    out << "];";
	}
        return;
    }

    ProxyPtr prx = ProxyPtr::dynamicCast(type);
    if(prx)
    {
        if(marshal)
        {
	    out << nl << "[" << stream << " writeProxy:(id<ICEObjectPrx>)" << param << "];";
	}
	else
	{
	    string mName = moduleName(findModule(prx->_class()));
	    string name = mName + prx->_class()->name() + "Prx";
            //
            // We use objc_getClass to get the proxy class instead of [name class]. This is to avoid
            // a warning if the proxy is forward declared.
	    out << nl << param << " = (id<" << name << ">)[[" << stream << " readProxy:objc_getClass(\""
	        << name << "\")] autorelease];";
	}
        return;
    }

    ClassDeclPtr cl = ClassDeclPtr::dynamicCast(type);
    if(cl)
    {
        if(marshal)
        {
            out << nl << stream << ".writeObject(" << param << ");";
        }
        else
        {
            if(isOutParam)
            {
                out << nl << "IceInternal.ParamPatcher<" << typeToString(type) << ">" << param
                    << "_PP = new IceInternal.ParamPatcher<" << typeToString(type) << ">(\""
                    << cl->scoped() << "\");";
                out << nl << stream << ".readObject(";
                if(streamingAPI)
                {
                    out << "(Ice.ReadObjectCallback)";
                }
                out << param << "_PP);";
            }
            else
            {
                out << nl << stream << ".readObject(";
                if(streamingAPI)
                {
                    out << "(Ice.ReadObjectCallback)";
                }
                out << "new Patcher__(\"" << cl->scoped() << "\", " << patchParams << "));";
            }
        }
        return;
    }

    StructPtr st = StructPtr::dynamicCast(type);
    if(st)
    {
	string typeS = typeToString(st);
	if(marshal)
	{
	    out << nl << "[" << typeS << " writeWithStream:" << param << " stream:" << stream << "];";
	}
	else
	{
	    out << nl << param << " = (" << typeS << "*)[[" << typeS << " readWithStream:" << stream
	        << "] autorelease];";
	}
        return;
    }

    EnumPtr en = EnumPtr::dynamicCast(type);
    if(en)
    {
        size_t sz = en->getEnumerators().size();
        if(marshal)
        {
            out << nl << "[" << stream << " writeEnumerator:" << param << " limit:" << sz << "];";
        }
        else
        {
            out << nl << param << " = " << "[" << stream << " readEnumerator:" << sz << "];";
        }
        return;
    }

    SequencePtr seq = SequencePtr::dynamicCast(type);
    if(seq)
    {
        BuiltinPtr builtin = BuiltinPtr::dynamicCast(seq->type());
	if(builtin)
	{
            if(builtin->kind() == Builtin::KindObjectProxy)
            {
                if(marshal)
                {
                    out << nl << "[" << stream << " writeSequence:" << param << " c:[ICEObjectPrx class]];";
                }
                else
                {
                    out << nl << param << " = [[" << stream << " readSequence:[ICEObjectPrx class]] autorelease];";
                }
            }
            else
            {
                string selector = getBuiltinSelector(builtin, marshal) + "Seq";
                if(marshal)
                {
                    out << nl << "[" << stream << " " << selector << ":" << param << "];";
                }
                else
                {
                    out << nl << param << " = [[" << stream << " " << selector << "] autorelease];";
                }
            }
	    return;
	}

	EnumPtr en = EnumPtr::dynamicCast(seq->type());
	if(en)
	{
	    size_t sz = en->getEnumerators().size();
	    string func = marshal ? "writeEnumSeq" : "readEnumSeq";
	    if(marshal)
	    {
		out << nl << "[" << stream << " writeEnumSeq:" << param << " limit:" << sz << "];";
	    }
	    else
	    {
		out << nl << param << " = [[" << stream << " readEnumSeq:" << sz << "] autorelease];";
	    }
	    return;
	}

	ProxyPtr prx = ProxyPtr::dynamicCast(seq->type());
	if(ProxyPtr::dynamicCast(seq))
	{
	    if(marshal)
	    {
		string mName = moduleName(findModule(prx->_class()));
		out << nl << "[" << stream << " writeSequence:" << param << " c:["
		    << mName + prx->_class()->name() + "Prx class]];";
	    }
	    else
	    {
		string mName = moduleName(findModule(prx->_class()));
		out << nl << param << " = [[" << stream << " readSequence:["
		    << mName + prx->_class()->name() + "Prx class]] autorelease];";
	    }
	    return;
	}

	string prefix = moduleName(findModule(seq));
	string name =  prefix + seq->name() + "Helper";
	if(marshal)
	{
	    out << nl << "[" << name << " writeWithStream:" << param << " stream:" << stream << "];";
	}
	else
	{
	    out << nl << param << " = [[" << name << " readWithStream:" << stream << "] autorelease];";
	}
	return;
    }

    DictionaryPtr d = DictionaryPtr::dynamicCast(type);
    assert(d);
    string prefix = moduleName(findModule(d));
    string name = prefix + d->name() + "Helper";
    if(marshal)
    {
        out << nl << "[" + name << " writeWithStream:" << param << " stream:" << stream << "];";
    }
    else
    {
        out << nl << param << " = [[" << name << " readWithStream:" << stream << "] autorelease];";
    }
}

void
Slice::ObjCGenerator::writeSequenceMarshalUnmarshalCode(Output& out,
                                                      const SequencePtr& seq,
                                                      const string& param,
                                                      bool marshal,
                                                      bool streamingAPI) const
{
    string stream;
    if(marshal)
    {
        stream = streamingAPI ? "outS__" : "os_";
    }
    else
    {
        stream = streamingAPI ? "inS__" : "is_";
    }

    TypePtr type = seq->type();
    string typeS = typeToString(type);

    string addMethod = "Add";

    BuiltinPtr builtin = BuiltinPtr::dynamicCast(type);
    bool isNSDataSeq = builtin && isValueType(builtin);
    string limitID = isNSDataSeq ? "length" : "count";

    if(builtin)
    {
        switch(builtin->kind())
        {
	    // TODO: adjust this for ObjC
            case Builtin::KindObject:
            case Builtin::KindObjectProxy:
            {
                if(marshal)
                {
		    string func = builtin->kind() == Builtin::KindObject ? "writeObject" : "writeProxy";
		    string type = builtin->kind() == Builtin::KindObject ? "ICEObject *" : "ICEObjectPrx *";
                    out << nl << "if(" << param << " == nil)";
                    out << sb;
                    out << nl << "[" << stream << " writeSize:0];";
                    out << eb;
		    out << nl << "else";
		    out << sb;
		    out << nl << "for(" << type << "elmt_ in v)";
		    out << sb;
		    out << nl << "[" << stream << " " << func << ":elmt_];";
		    out << eb;
		    out << eb;
                }
                else
                {
                    out << nl << "int " << param << "_lenx = [" << stream << " readSize];";
                    if(!streamingAPI)
                    {
                        if(builtin->isVariableLength())
                        {
                            out << nl << "[" <<  stream << " startSeq:" << param << "_lenx minSize:"
                                << static_cast<unsigned>(builtin->minWireSize()) << "];";
                        }
                        else
                        {
                            out << nl << "[" << stream << " checkFixedSeq:" << param << "_lenx elemSize:"
                                << static_cast<unsigned>(builtin->minWireSize()) << "];";
                        }
                    }
                    out << nl << param << " = ";
                    if(builtin->kind() == Builtin::KindObject)
                    {
			out << "[" << outTypeToString(seq) << " arrayWithCapacity:" << param << "_lenx];";
			out << nl << "int ix__;";
                        out << nl << "for(ix__ = 0; ix__ < " << param << "_lenx; ++ix__)";
                        out << sb;
#if 0
// TODO: adjust this
                        out << nl << "[" << stream << " readObject:";
                        if(streamingAPI)
                        {
                            out << "(ReadObjectCallback *)";
                        }
                        string patcherName;
			patcherName = "Sequence";
                        out << "new IceInternal." << patcherName << "Patcher<Ice.Object>(\"::Ice::Object\", "
                            << param << ", ix__));";
#endif
                    }
                    else
                    {
			out << "[" << outTypeToString(seq) << " arrayWithCapacity:" << param << "_lenx];";
			out << nl << "int ix__;";
                        out << nl << "for(ix__ = 0; ix__ < " << param << "_lenx; ++ix__)";
                        out << sb;
			out << nl << "[" << param << " addObject:[[" << stream
			    << " readProxy:[ICEObjectPrx class]] autorelease]];";
                    }
                    if(!streamingAPI && builtin->isVariableLength())
                    {
                        out << nl << "[" << stream << " checkSeq];";
                        out << nl << "[" << stream << " endElement];";
                    }
                    out << eb;
                    if(!streamingAPI && builtin->isVariableLength())
                    {
                        out << nl << "[" << stream << " endSeq:" << param << "_lenx];";
                    }
                }
                break;
            }
	    case Builtin::KindBool:
	    {
                if(marshal)
                {
                    out << nl << "[" << stream << " writeBoolSeq:" << param << "];";
                }
                else
                {
		    out << nl << param << " = [[" << stream << " readBoolSeq] autorelease];";
                }
                break;
	    }
	    case Builtin::KindString:
	    {
                if(marshal)
                {
                    out << nl << "[" << stream << " writeStringSeq:" << param << "];";
                }
                else
                {
		    out << nl << param << " = [[" << stream << " readStringSeq] autorelease];";
                }
                break;
	    }
            default:
            {
	        assert(typeS.compare(0, 3, "ICE") == 0);
		typeS = typeS.substr(3);
                typeS[0] = toupper(typeS[0]);
                if(marshal)
                {
                    out << nl << "[" << stream << " write" << typeS << "Seq:" << param << "];";
                }
                else
                {
		    out << nl << param << " = [[" << stream << " read" << typeS << "Seq] autorelease];";
                }
                break;
            }
        }
        return;
    }

    ClassDeclPtr cl = ClassDeclPtr::dynamicCast(type);
    if(cl)
    {
        if(marshal)
        {
            out << nl << "if(" << param << " == null)";
            out << sb;
            out << nl << stream << ".writeSize(0);";
            out << eb;
            out << nl << "else";
            out << sb;
            out << nl << stream << ".writeSize(" << param << '.' << limitID << ");";
	    out << nl << "for(int ix__ = 0; ix__ < " << param << '.' << limitID << "; ++ix__)";
	    out << sb;
	    out << nl << stream << ".writeObject(" << param << "[ix__]);";
	    out << eb;
            out << eb;
        }
        else
        {
            out << sb;
            out << nl << "int szx__ = " << stream << ".readSize();";
            if(!streamingAPI)
            {
                if(type->isVariableLength())
                {
                    out << nl << stream << ".startSeq(szx__, " << static_cast<unsigned>(type->minWireSize()) << ");";
                }
                else
                {
                    out << nl << stream << ".checkFixedSeq(szx__, " << static_cast<unsigned>(type->minWireSize()) << ");";
                }
            }
            out << nl << param << " = new ";
	    out << fixId(seq->scoped()) << "(szx__)";
            out << ';';
            out << nl << "for(int ix__ = 0; ix__ < szx__; ++ix__)";
            out << sb;

            string patcherName;
	    patcherName = "Sequence";
            string scoped = ContainedPtr::dynamicCast(type)->scoped();
            out << nl << "IceInternal." << patcherName << "Patcher<" << typeS << "> spx = new IceInternal."
                << patcherName << "Patcher<" << typeS << ">(\"" << scoped << "\", " << param << ", ix__);";
            out << nl << stream << ".readObject(";
            if(streamingAPI)
            {
                out << "(Ice.ReadObjectCallback)";
            }
            out << "spx);";
            if(!streamingAPI && type->isVariableLength())
            {
                out << nl << stream << ".checkSeq();";
                out << nl << stream << ".endElement();";
            }
            out << eb;
            if(!streamingAPI && type->isVariableLength())
            {
                out << nl << stream << ".endSeq(szx__);";
            }
            out << eb;
        }
        return;
    }

    StructPtr st = StructPtr::dynamicCast(type);
    if(st)
    {
        if(marshal)
        {
            out << nl << "[" << stream << " writeSequence: " << param << " class: [" << typeS << " class]];";
        }
        else
        {
            out << nl << "v = [" << stream << " readSequence: [" << typeS << " class]];";
        }
        return;
    }

    EnumPtr en = EnumPtr::dynamicCast(type);
    if(en)
    {
        size_t sz = en->getEnumerators().size();
        string func = marshal ? "writeEnumeSeq" : "readEnumSeq";
        if(marshal)
        {
	    out << nl << "[" << stream << " writeEnumSeq:" << param << " limit:" << sz << "];";
        }
        else
        {
	    out << nl << param << " = [[" << stream << " readEnumSeq:" << sz << "] autorelease];";
        }
        return;
    }

    string helperName;
    if(ProxyPtr::dynamicCast(type))
    {
        helperName = fixId(ProxyPtr::dynamicCast(type)->_class()->scoped() + "PrxHelper"); // TODO: wrong name
    }
    else
    {
	helperName = typeS + "Helper";
    }

    string func;
    if(marshal)
    {
        func = "write";
        if(!streamingAPI && ProxyPtr::dynamicCast(type))
        {
           func += "__";
        }
        out << nl << "if(" << param << " == nil)";
        out << sb;
        out << nl << "[" << stream << " writeSize:0];";
        out << eb;
        out << nl << "else";
        out << sb;
        out << nl << "[" << stream << " writeSize:[" << param << " count]];";
	out << nl << "for(" << typeS << " * elmt_ in " << param << ")";
	out << sb;
	out << nl << "[" << helperName << " write:" << stream << " v:elmt_];";
	out << eb;
        out << eb;
    }
    else
    {
        func = "read";
        if(!streamingAPI && ProxyPtr::dynamicCast(type))
        {
           func += "__";
        }
        out << sb;
        out << nl << "int szx__ = [" << stream << " readSize];";
        if(!streamingAPI)
        {
            if(type->isVariableLength())
            {
                out << nl << "[" << stream << " startSeq:szx__ minSize:"
		    << static_cast<unsigned>(type->minWireSize()) << "];";
            }
            else
            {
                out << nl << "[" << stream << " checkFixedSeq:szx__ elemSize:"
		    << static_cast<unsigned>(type->minWireSize()) << "];";
            }
        }
	string mName = moduleName(findModule(seq)) + "Mutable" + seq->name();
        out << nl << param << " = [" << mName << " arrayWithCapacity:" << "szx__];";
	out << nl << "int ix__;";
        out << nl << "for(ix__ = 0; ix__ < szx__; ++ix__)";
        out << sb;
	out << nl << "[" << param << " addObject:[" << helperName << " " << func << ":" << stream << "]];";
        if(!streamingAPI && type->isVariableLength())
        {
            if(!SequencePtr::dynamicCast(type))
            {
                out << nl << "[" << stream << " checkSeq];";
            }
            out << nl << "[" << stream << " endElement];";
        }
        out << eb;
        if(!streamingAPI && type->isVariableLength())
        {
            out << nl << "[" << stream << " endSeq:szx__];";
        }
        out << eb;
    }

    return;
}

string
Slice::ObjCGenerator::toArrayAlloc(const string& decl, const string& sz)
{
    int count = 0;
    string::size_type pos = decl.size();
    while(pos > 1 && decl.substr(pos - 2, 2) == "[]")
    {
        ++count;
        pos -= 2;
    }
    assert(count > 0);

    ostringstream o;
    o << decl.substr(0, pos) << '[' << sz << ']' << decl.substr(pos + 2);
    return o.str();
}

void
Slice::ObjCGenerator::validateMetaData(const UnitPtr& u)
{
    MetaDataVisitor visitor;
    u->visit(&visitor, true);
}

const string Slice::ObjCGenerator::MetaDataVisitor::_objcPrefix = "objc:";
const string Slice::ObjCGenerator::MetaDataVisitor::_msg = "ignoring invalid metadata";

Slice::ObjCGenerator::MetaDataVisitor::MetaDataVisitor()
    : _globalMetaDataDone(false)
{
}

bool
Slice::ObjCGenerator::MetaDataVisitor::visitModuleStart(const ModulePtr& p)
{
#if 0
    if(!_globalMetaDataDone)
    {
        //
        // Validate global metadata.
        //
        DefinitionContextPtr dc = p->definitionContext();
        assert(dc);
        StringList globalMetaData = dc->getMetaData();
        string file = dc->filename();
        static const string prefix = "cs:";
        for(StringList::const_iterator q = globalMetaData.begin(); q != globalMetaData.end(); ++q)
        {
            string s = *q;
            if(_history.count(s) == 0)
            {
                if(s.find(prefix) == 0)
                {
                    static const string attributePrefix = "cs:attribute:";
                    if(s.find(attributePrefix) != 0 || s.size() == attributePrefix.size())
                    {
                        cout << file << ": warning: ignoring invalid global metadata `" << s << "'" << endl;
                    }
                }
                _history.insert(s);
            }
        }
        _globalMetaDataDone = true;
    }
#endif
    validate(p);
    return true;
}

void
Slice::ObjCGenerator::MetaDataVisitor::visitModuleEnd(const ModulePtr&)
{
}

void
Slice::ObjCGenerator::MetaDataVisitor::visitClassDecl(const ClassDeclPtr& p)
{
    validate(p);
}

bool
Slice::ObjCGenerator::MetaDataVisitor::visitClassDefStart(const ClassDefPtr& p)
{
    validate(p);
    return true;
}

void
Slice::ObjCGenerator::MetaDataVisitor::visitClassDefEnd(const ClassDefPtr&)
{
}

bool
Slice::ObjCGenerator::MetaDataVisitor::visitExceptionStart(const ExceptionPtr& p)
{
    validate(p);
    return true;
}

void
Slice::ObjCGenerator::MetaDataVisitor::visitExceptionEnd(const ExceptionPtr&)
{
}

bool
Slice::ObjCGenerator::MetaDataVisitor::visitStructStart(const StructPtr& p)
{
    validate(p);
    return true;
}

void
Slice::ObjCGenerator::MetaDataVisitor::visitStructEnd(const StructPtr&)
{
}

void
Slice::ObjCGenerator::MetaDataVisitor::visitOperation(const OperationPtr& p)
{
#if 0
    if(p->hasMetaData("UserException"))
    {
        ClassDefPtr cl = ClassDefPtr::dynamicCast(p->container());
        if(!cl->isLocal())
        {
            cout << p->definitionContext()->filename() << ":" << p->line()
                 << ": warning: metdata directive `UserException' applies only to local operations "
                 << "but enclosing " << (cl->isInterface() ? "interface" : "class") << "`" << cl->name()
                 << "' is not local" << endl;
        }
    }
#endif
    validate(p);
}

void
Slice::ObjCGenerator::MetaDataVisitor::visitParamDecl(const ParamDeclPtr& p)
{
    validate(p);
}

void
Slice::ObjCGenerator::MetaDataVisitor::visitDataMember(const DataMemberPtr& p)
{
    validate(p);
}

void
Slice::ObjCGenerator::MetaDataVisitor::visitSequence(const SequencePtr& p)
{
    validate(p);
}

void
Slice::ObjCGenerator::MetaDataVisitor::visitDictionary(const DictionaryPtr& p)
{
    validate(p);
}

void
Slice::ObjCGenerator::MetaDataVisitor::visitEnum(const EnumPtr& p)
{
    validate(p);
}

void
Slice::ObjCGenerator::MetaDataVisitor::visitConst(const ConstPtr& p)
{
    validate(p);
}

void
Slice::ObjCGenerator::MetaDataVisitor::validate(const ContainedPtr& cont)
{
    ModulePtr m = ModulePtr::dynamicCast(cont);
    if(m)
    {
	bool error = false;
	bool foundPrefix = false;

	StringList meta = getMetaData(m);
	StringList::const_iterator p;

	for(p = meta.begin(); p != meta.end(); ++p)
	{
	    const string prefix = "prefix:";
	    string name; 
	    if(p->substr(_objcPrefix.size(), prefix.size()) == prefix)
	    {
	        foundPrefix = true;
		name = trim(p->substr(_objcPrefix.size() + prefix.size()));
		if(name.empty())
		{
		    if(_history.count(*p) == 0)
		    {
			string file = m->definitionContext()->filename();
			cout << file << ":" << m->line() << ": warning: " << _msg << " `" << *p << "'" << endl;
			_history.insert(*p);
		    }
		    error = true;
		}
		else
		{
		    if(!addModule(m, name))
		    {
			modulePrefixError(m, *p);
		    }
		}
	    }
	    else
	    {
		if(_history.count(*p) == 0)
		{
		    string file = m->definitionContext()->filename();
		    cout << file << ":" << m->line() << ": warning: " << _msg << " `" << *p << "'" << endl;
		    _history.insert(*p);
		}
		error = true;
	    }
	}

	if(!error && !foundPrefix)
	{
	    StringList names = splitScopedName(m->scoped());
	    string name;
	    for(StringList::const_iterator i = names.begin(); i != names.end(); ++i)
	    {
		name += *i;
	    }
	    if(!addModule(m, name))
	    {
	        modulePrefixError(m, "");
	    }
	}
    }
}

StringList
Slice::ObjCGenerator::MetaDataVisitor::getMetaData(const ContainedPtr& cont)
{
    StringList ret;
    StringList localMetaData = cont->getMetaData();
    StringList::const_iterator p;

    for(p = localMetaData.begin(); p != localMetaData.end(); ++p)
    {
        if(p->find(_objcPrefix) != string::npos)
	{
	    ret.push_back(*p);
	}
    }

    return ret;
}

void
Slice::ObjCGenerator::MetaDataVisitor::modulePrefixError(const ModulePtr& m, const string& metadata)
{
    string file = m->definitionContext()->filename();
    string line = m->line();
    ModulePrefix mp = modulePrefix(m);
    string old_file = mp.m->definitionContext()->filename();
    string old_line = mp.m->line();
    cout << file << ":" << line << ": warning:";
    if(!metadata.empty())
    {
        cout << " " << _msg << " `" << metadata << "':";
    }
    cout << " inconsistent module prefix previously defined ";
    if(old_file != file)
    {
	 cout << "in " << old_file << ":";
    }
    else
    {
	cout << "at line ";
    }
    cout << line;
    cout << " as `" << mp.name << "'" << endl;
}
