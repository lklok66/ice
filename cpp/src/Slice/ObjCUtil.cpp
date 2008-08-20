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
        "ICEObjectPrx",
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
        return fixId(proxy->_class()->scoped() + "Prx");
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
Slice::ObjCGenerator::isString(const TypePtr& type)
{
    if(!type)
    {
        return false;
    }
    BuiltinPtr builtin = BuiltinPtr::dynamicCast(type);
    return builtin && builtin->kind() == Builtin::KindString;
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
        switch(builtin->kind())
        {
            case Builtin::KindByte:
            {
                if(marshal)
                {
                    out << nl << "[" << stream << " writeByte:" << param << "];";
                }
                else
                {
                    out << nl << param << " = [" << stream << " readByte];";
                }
                break;
            }
            case Builtin::KindBool:
            {
                if(marshal)
                {
                    out << nl << "[" << stream << " writeBool:" << param << "];";
                }
                else
                {
                    out << nl << param << " = [" << stream << " readBool];";
                }
                break;
            }
            case Builtin::KindShort:
            {
                if(marshal)
                {
                    out << nl << "[" << stream << " writeShort:" << param << "];";
                }
                else
                {
                    out << nl << param << " = [" << stream << " readShort];";
                }
                break;
            }
            case Builtin::KindInt:
            {
                if(marshal)
                {
                    out << nl << "[" << stream << " writeInt:" << param << "];";
                }
                else
                {
                    out << nl << param << " = [" << stream << " readInt];";
                }
                break;
            }
            case Builtin::KindLong:
            {
                if(marshal)
                {
                    out << nl << "[" << stream << " writeLong:" << param << "];";
                }
                else
                {
                    out << nl << param << " = [" << stream << " readLong];";
                }
                break;
            }
            case Builtin::KindFloat:
            {
                if(marshal)
                {
                    out << nl << "[" << stream << " writeFloat:" << param << "];";
                }
                else
                {
                    out << nl << param << " = [" << stream << " readFloat];";
                }
                break;
            }
            case Builtin::KindDouble:
            {
                if(marshal)
                {
                    out << nl << "[" << stream << " writeDouble:" << param << "];";
                }
                else
                {
                    out << nl << param << " = [" << stream << " readDouble];";
                }
                break;
            }
            case Builtin::KindString:
            {
                if(marshal)
                {
                    out << nl << "[" << stream << " writeString:" << param << "];";
                }
                else
                {
                    out << nl << param << " = [" << stream << " readString];";
                }
                break;
            }
            case Builtin::KindObject:
            {
	        // TODO: deal with classes
                if(marshal)
                {
                    out << nl << stream << ".writeObject(" << param << ");";
                }
                else
                {
                    if(isOutParam)
                    {
                        out << nl << "IceInternal.ParamPatcher<Ice.Object> " << param
                            << "_PP = new IceInternal.ParamPatcher<Ice.Object>(\"::Ice::Object\");";
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
                            out <<  "(Ice.ReadObjectCallback)";
                        }
                        out << "new Patcher__(\"::Ice::Object\", " << patchParams << "));";
                    }
                }
                break;
            }
            case Builtin::KindObjectProxy:
            {
                if(marshal)
                {
                    out << nl << "[" << stream << " writeProxy:" << param << "];";
                }
                else
                {
                    out << nl << "*" << param << " = [" << stream << " readProxy];";
                }
                break;
            }
            case Builtin::KindLocalObject:
            {
                assert(false);
                break;
            }
        }
        return;
    }

    ProxyPtr prx = ProxyPtr::dynamicCast(type);
    if(prx)
    {
        // TODO
        string typeS = typeToString(type);
        if(marshal)
        {
            out << nl << typeS << "Helper.write";
            if(!streamingAPI)
            {
                out << "__";
            }
            out << "(" << stream << ", " << param << ");";
        }
        else
        {
            out << nl << param << " = " << typeS << "Helper.read";
            if(!streamingAPI)
            {
                out << "__";
            }
            out << "(" << stream << ");";
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
        if(marshal)
        {
	    out << nl << "if(" << param << " == nil)";
	    out << sb;
	    string typeS = typeToString(st);
	    out << nl << typeS << " *tmp_ = [[" << typeS << " alloc] init];";
	    out << nl << "[tmp_ " << (streamingAPI ? "ice_write" : "write__") << ":" << stream << "];";
	    out << eb;
	    out << nl << "else";
	    out << sb;
	    out << nl << "[" << param << " " << (streamingAPI ? "ice_write" : "write__") << ":" << stream << "];";
	    out << eb;
        }
        else
        {
	    out << nl << "[" << param << " " << (streamingAPI ? "ice_read" : "read__") << ":" << stream << "];";
        }
        return;
    }

    EnumPtr en = EnumPtr::dynamicCast(type);
    if(en)
    {
        string func;
        string cast;
        size_t sz = en->getEnumerators().size();
        if(sz <= 0x7f)
        {
            func = marshal ? "writeByte" : "readByte";
            cast = marshal ? string("(ICEByte)") : "(" + fixName(en) + ")";
        }
        else if(sz <= 0x7fff)
        {
            func = marshal ? "writeShort" : "readShort";
            cast = marshal ? string("(ICEShort)") : "(" + fixName(en) + ")";
        }
        else
        {
            func = marshal ? "writeInt" : "readInt";
            cast = marshal ? string("(ICEInt)") : "(" + fixName(en) + ")";
        }
        if(marshal)
        {
            out << nl << "[" << stream << " " << func << ":" << cast << param << " limit:" << sz << "];";
        }
        else
        {
            out << nl << param << " = " << cast << "[" << stream << " " << func << ":" << sz << "];";
        }
        return;
    }

    SequencePtr seq = SequencePtr::dynamicCast(type);
    if(seq)
    {
        writeSequenceMarshalUnmarshalCode(out, seq, param, marshal, streamingAPI);
        return;
    }

    assert(ConstructedPtr::dynamicCast(type));
    string typeS;
    DictionaryPtr d = DictionaryPtr::dynamicCast(type);
    if(d)
    {
        typeS = fixId(d->scope()) + d->name();
    }
    else
    {
        typeS = typeToString(type);
    }
    if(marshal)
    {
        out << nl << typeS << "Helper.write(" << stream << ", " << param << ");";
    }
    else
    {
        out << nl << param << " = " << typeS << "Helper.read(" << stream << ')' << ';';
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
        stream = streamingAPI ? "outS__" : "os__";
    }
    else
    {
        stream = streamingAPI ? "inS__" : "is__";
    }

    TypePtr type = seq->type();
    string typeS = typeToString(type);

    string genericPrefix = "clr:generic:";
    string genericType;
    string addMethod = "Add";
    bool isGeneric = seq->findMetaData(genericPrefix, genericType);
    bool isStack = false;
    bool isList = false;
    bool isLinkedList = false;
    bool isCustom = false;
    if(isGeneric)
    {
        genericType = genericType.substr(genericPrefix.size());
        if(genericType == "LinkedList")
        {
            addMethod = "AddLast";
            isLinkedList = true;
        }
        else if(genericType == "Queue")
        {
            addMethod = "Enqueue";
        }
        else if(genericType == "Stack")
        {
            addMethod = "Push";
            isStack = true;
        }
        else if(genericType == "List")
        {
            isList = true;
        }
        else
        {
            isCustom = true;
        }
    }
    bool isArray = !isGeneric && !seq->hasMetaData("clr:collection");
    string limitID = isArray ? "Length" : "Count";

    BuiltinPtr builtin = BuiltinPtr::dynamicCast(type);
    if(builtin)
    {
        switch(builtin->kind())
        {
            case Builtin::KindObject:
            case Builtin::KindObjectProxy:
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
                    if(isGeneric && !isList)
                    {
                        if(isStack)
                        {
                            //
                            // If the collection is a stack, write in bottom-to-top order. Stacks
                            // cannot contain Ice.Object.
                            //
                            out << nl << "Ice.ObjectPrx[] " << param << "_tmp = " << param << ".ToArray();";
                            out << nl << "for(int ix__ = " << param << "_tmp.Length - 1; ix__ >= 0; --ix__)";
                            out << sb;
                            out << nl << stream << ".writeProxy(" << param << "_tmp[ix__]);";
                            out << eb;
                        }
                        else
                        {
                            out << nl << "_System.Collections.Generic.IEnumerator<" << typeS
                                << "> e__ = " << param << ".GetEnumerator();";
                            out << nl << "while(e__.MoveNext())";
                            out << sb;
                            string func = builtin->kind() == Builtin::KindObject ? "writeObject" : "writeProxy";
                            out << nl << stream << '.' << func << "(e__.Current);";
                            out << eb;
                        }
                    }
                    else
                    {
                        out << nl << "for(int ix__ = 0; ix__ < " << param << '.' << limitID << "; ++ix__)";
                        out << sb;
                        string func = builtin->kind() == Builtin::KindObject ? "writeObject" : "writeProxy";
                        out << nl << stream << '.' << func << '(' << param << "[ix__]);";
                        out << eb;
                    }
                    out << eb;
                }
                else
                {
                    out << nl << "int " << param << "_lenx = " << stream << ".readSize();";
                    if(!streamingAPI)
                    {
                        if(builtin->isVariableLength())
                        {
                            out << nl << stream << ".startSeq(" << param << "_lenx, "
                                << static_cast<unsigned>(builtin->minWireSize()) << ");";
                        }
                        else
                        {
                            out << nl << stream << ".checkFixedSeq(" << param << "_lenx, "
                                << static_cast<unsigned>(builtin->minWireSize()) << ");";
                        }
                    }
                    out << nl << param << " = new ";
                    if(builtin->kind() == Builtin::KindObject)
                    {
                        if(isArray)
                        {
                            out << "Ice.Object[" << param << "_lenx];";
                        }
                        else if(isCustom)
                        {
                            out << genericType << "<Ice.Object>();";
                        }
                        else if(isGeneric)
                        {
                            out << "_System.Collections.Generic." << genericType << "<Ice.Object>(";
                            if(!isLinkedList)
                            {
                                out << param << "_lenx";
                            }
                            out << ");";
                        }
                        else
                        {
                            out << typeToString(seq) << "(" << param << "_lenx);";
                        }
                        out << nl << "for(int ix__ = 0; ix__ < " << param << "_lenx; ++ix__)";
                        out << sb;
                        out << nl << stream << ".readObject(";
                        if(streamingAPI)
                        {
                            out << "(ReadObjectCallback)";
                        }
                        string patcherName;
                        if(isCustom)
                        {
                            patcherName = "CustomSeq";
                        }
                        else if(isList)
                        {
                            patcherName = "List";
                        }
                        else if(isArray)
                        {
                            patcherName = "Array";
                        }
                        else
                        {
                            patcherName = "Sequence";
                        }
                        out << "new IceInternal." << patcherName << "Patcher<Ice.Object>(\"::Ice::Object\", "
                            << param << ", ix__));";
                    }
                    else
                    {
                        if(isArray)
                        {
                            out << "Ice.ObjectPrx[" << param << "_lenx];";
                        }
                        else if(isGeneric)
                        {
                            out << "_System.Collections.Generic." << genericType << "<Ice.ObjectPrx>(";
                            if(!isLinkedList)
                            {
                                out << param << "_lenx";
                            }
                            out << ");";
                        }
                        else
                        {
                            out << typeToString(seq) << "(" << param << "_lenx);";
                        }
                        out << nl << "for(int ix__ = 0; ix__ < " << param << "_lenx; ++ix__)";
                        out << sb;
                        if(isArray)
                        {
                            out << nl << param << "[ix__] = " << stream << ".readProxy();";
                        }
                        else
                        {
                            out << nl << "Ice.ObjectPrx val__ = new Ice.ObjectPrxHelperBase();";
                            out << nl << "val__ = " << stream << ".readProxy();";
                            out << nl << param << "." << addMethod << "(val__);";
                        }
                    }
                    if(!streamingAPI && builtin->isVariableLength())
                    {
                        out << nl << stream << ".checkSeq();";
                        out << nl << stream << ".endElement();";
                    }
                    out << eb;
                    if(!streamingAPI && builtin->isVariableLength())
                    {
                        out << nl << stream << ".endSeq(" << param << "_lenx);";
                    }
                }
                break;
            }
            default:
            {
                typeS[0] = toupper(typeS[0]);
                if(marshal)
                {
                    out << nl << stream << ".write" << typeS << "Seq(";
                    if(isArray)
                    {
                        out << param << ");";
                    }
                    else if(!isGeneric)
                    {
                        out << param << " == null ? null : " << param << ".ToArray());";
                    }
                    else
                    {
                        out << param << " == null ? 0 : " << param << ".Count, " << param << ");";
                    }
                }
                else
                {
                    if(isArray)
                    {
                        out << nl << param << " = " << stream << ".read" << typeS << "Seq();";
                    }
                    else if(isCustom)
                    {
                        out << sb;
                        out << nl << param << " = new " << genericType << "<"
                            << typeToString(type) << ">();";
                        out << nl << "int szx__ = " << stream << ".readSize();";
                        out << nl << "for(int ix__ = 0; ix__ < szx__; ++ix__)";
                        out << sb;
                        out << nl << param << ".Add(" << stream << ".read" << typeS << "());";
                        out << eb;
                        out << eb;
                    }
                    else if(isGeneric)
                    {
                        out << nl << stream << ".read" << typeS << "Seq(out " << param << ");";
                    }
                    else
                    {
                        out << nl << param << " = new " << fixId(seq->scoped())
                            << '(' << stream << ".read" << typeS << "Seq());";
                    }
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
            if(isGeneric && !isList)
            {
                //
                // Stacks cannot contain class instances, so there is no need to marshal a
                // stack bottom-up here.
                //
                out << nl << "_System.Collections.Generic.IEnumerator<" << typeS
                    << "> e__ = " << param << ".GetEnumerator();";
                out << nl << "while(e__.MoveNext())";
                out << sb;
                out << nl << stream << ".writeObject(e__.Current);";
                out << eb;
            }
            else
            {
                out << nl << "for(int ix__ = 0; ix__ < " << param << '.' << limitID << "; ++ix__)";
                out << sb;
                out << nl << stream << ".writeObject(" << param << "[ix__]);";
                out << eb;
            }
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
            if(isArray)
            {
                out << toArrayAlloc(typeS + "[]", "szx__");
            }
            else if(isCustom)
            {
                out << genericType << "<" << typeS << ">()";
            }
            else if(isGeneric)
            {
                out << "_System.Collections.Generic." << genericType << "<" << typeS << ">(";
                if(!isLinkedList)
                {
                    out << "szx__";
                }
                out << ")";
            }
            else
            {
                out << fixId(seq->scoped()) << "(szx__)";
            }
            out << ';';
            out << nl << "for(int ix__ = 0; ix__ < szx__; ++ix__)";
            out << sb;

            string patcherName;
            if(isCustom)
            {
                patcherName = "CustomSeq";
            }
            else if(isList)
            {
                patcherName = "List";
            }
            else if(isArray)
            {
                patcherName = "Array";
            }
            else
            {
                patcherName = "Sequence";
            }
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
            out << nl << "if(" << param << " == null)";
            out << sb;
            out << nl << stream << ".writeSize(0);";
            out << eb;
            out << nl << "else";
            out << sb;
            out << nl << stream << ".writeSize(" << param << '.' << limitID << ");";
            if(isGeneric && !isList)
            {
                //
                // Stacks are marshaled bottom-up.
                //
                if(isStack)
                {
                    out << nl << typeS << "[] " << param << "_tmp = " << param << ".ToArray();";
                    out << nl << "for(int ix__ = " << param << "_tmp.Length - 1; ix__ >= 0; --ix__)";
                }
                else
                {
                    out << nl << "_System.Collections.Generic.IEnumerator<" << typeS
                        << "> e__ = " << param << ".GetEnumerator();";
                    out << nl << "while(e__.MoveNext())";
                }
            }
            else
            {
                out << nl << "for(int ix__ = 0; ix__ < " << param << '.' << limitID << "; ++ix__)";
            }
            out << sb;
            string call;
            if(isGeneric && !isList && !isStack)
            {
                if(isValueType(type))
                {
                    call = "e__.Current";
                }
                else
                {
                    call = "(e__.Current == null ? new ";
                    call += typeS + "() : e__.Current)";
                }
            }
            else
            {
                if(isValueType(type))
                {
                    call = param;
                    if(isStack)
                    {
                        call += "_tmp";
                    }
                }
                else
                {
                    call = "(";
                    call += param;
                    if(isStack)
                    {
                        call += "_tmp";
                    }
                    call += " == null ? new " + typeS + "() : " + param;
                    if(isStack)
                    {
                        call += "_tmp";
                    }
                }
                call += "[ix__]";
                if(!isValueType(type))
                {
                    call += ")";
                }
            }
            call += ".";
            call += streamingAPI ? "ice_write" : "write__";
            call += "(" + stream + ");";
            out << nl << call;
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
            if(isArray)
            {
                out << toArrayAlloc(typeS + "[]", "szx__") << ";";
            }
            else if(isCustom)
            {
                out << genericType << "<" << typeS << ">();";
            }
            else if(isGeneric)
            {
                out << "_System.Collections.Generic." << genericType << "<" << typeS << ">(";
                if(!isLinkedList)
                {
                    out << "szx__";
                }
                out << ");";
            }
            else
            {
                out << fixId(seq->scoped()) << "(szx__);";
            }
            out << nl << "for(int ix__ = 0; ix__ < szx__; ++ix__)";
            out << sb;
            if(isArray)
            {
                if(!isValueType(st))
                {
                    out << nl << param << "[ix__] = new " << typeS << "();";
                }
                if(streamingAPI)
                {
                    out << nl << param << "[ix__].ice_read(" << stream << ");";
                }
                else
                {
                    out << nl << param << "[ix__].read__(" << stream << ");";
                }
            }
            else
            {
                out << nl << typeS << " val__ = new " << typeS << "();";
                if(streamingAPI)
                {
                    out << nl << "val__.ice_read(" << stream << ");";
                }
                else
                {
                    out << nl << "val__.read__(" << stream << ");";
                }
                out << nl << param << "." << addMethod << "(val__);";
            }
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

    EnumPtr en = EnumPtr::dynamicCast(type);
    if(en)
    {
        size_t sz = en->getEnumerators().size();
        string dataType;
        string func = marshal ? "write" : "read";
        if(sz <= 0x7f)
        {
            func += "Byte";
            dataType = "byte";
        }
        else if(sz <= 0x7fff)
        {
            func += "Short";
            dataType = "short";
        }
        else
        {
            func += "Int";
            dataType = "int";
        }
        if(marshal)
        {
            out << nl << "if(" << param << " == null)";
            out << sb;
            out << nl << stream << ".writeSize(0);";
            out << eb;
            out << nl << "else";
            out << sb;
            out << nl << stream << ".writeSize(" << param << '.'<< limitID << ");";
            if(isGeneric && !isList)
            {
                //
                // Stacks are marshaled bottom-up.
                //
                if(isStack)
                {
                    out << nl << typeS << "[] " << param << "_tmp = " << param << ".ToArray();";
                    out << nl << "for(int ix__ = " << param << "_tmp.Length - 1; ix__ >= 0; --ix__)";
                    out << sb;
                    out << nl << stream << '.' << func << "((" << dataType << ")" << param << "_tmp[ix__]";
                    if(!streamingAPI)
                    {
                        out << ", " << sz;
                    }
                    out << ");";
                    out << eb;
                }
                else
                {
                    out << nl << "_System.Collections.Generic.IEnumerator<" << typeS
                        << "> e__ = " << param << ".GetEnumerator();";
                    out << nl << "while(e__.MoveNext())";
                    out << sb;
                    out << nl << stream << '.' << func << "((" << dataType << ")e__.Current";
                    if(!streamingAPI)
                    {
                        out << ", " << sz;
                    }
                    out << ");";
                    out << eb;
                }
            }
            else
            {
                out << nl << "for(int ix__ = 0; ix__ < " << param << '.' << limitID << "; ++ix__)";
                out << sb;
                out << nl << stream << '.' << func << "((" << dataType << ")" << param << "[ix__]";
                if(!streamingAPI)
                {
                    out << ", " << sz;
                }
                out << ");";
                out << eb;
            }
            out << eb;
        }
        else
        {
            out << sb;
            out << nl << "int szx__ = " << stream << ".readSize();";
            if(!streamingAPI)
            {
                out << nl << stream << ".checkFixedSeq(szx__, " << static_cast<unsigned>(type->minWireSize()) << ");";
            }
            out << nl << param << " = new ";
            if(isArray)
            {
                out << toArrayAlloc(typeS + "[]", "szx__") << ";";
            }
            else if(isCustom)
            {
                out << genericType << "<" << typeS << ">();";
            }
            else if(isGeneric)
            {
                out << "_System.Collections.Generic." << genericType << "<" << typeS << ">(";
                if(!isLinkedList)
                {
                    out << "szx__";
                }
                out << ");";
            }
            else
            {
                out << fixId(seq->scoped()) << "(szx__);";
            }
            out << nl << "for(int ix__ = 0; ix__ < szx__; ++ix__)";
            out << sb;
            if(isArray)
            {
                out << nl << param << "[ix__] = (" << typeS << ')' << stream << "." << func << "(";
                if(!streamingAPI)
                {
                    out << sz;
                }
                out << ");";
            }
            else
            {
                out << nl << param << "." << addMethod << "((" << typeS << ')' << stream << "." << func << "(";
                if(!streamingAPI)
                {
                    out << sz;
                }
                out << "));";
            }
            out << eb;
            out << eb;
        }
        return;
    }

    string helperName;
    if(ProxyPtr::dynamicCast(type))
    {
        helperName = fixId(ProxyPtr::dynamicCast(type)->_class()->scoped() + "PrxHelper");
    }
    else
    {
        helperName = fixId(ContainedPtr::dynamicCast(type)->scoped() + "Helper");
    }

    string func;
    if(marshal)
    {
        func = "write";
        if(!streamingAPI && ProxyPtr::dynamicCast(type))
        {
           func += "__";
        }
        out << nl << "if(" << param << " == null)";
        out << sb;
        out << nl << stream << ".writeSize(0);";
        out << eb;
        out << nl << "else";
        out << sb;
        out << nl << stream << ".writeSize(" << param << '.' << limitID << ");";
        if(isGeneric && !isList)
        {
            //
            // Stacks are marshaled bottom-up.
            //
            if(isStack)
            {
                out << nl << typeS << "[] " << param << "_tmp = " << param << ".ToArray();";
                out << nl << "for(int ix__ = " << param << "_tmp.Length - 1; ix__ >= 0; --ix__)";
                out << sb;
                out << nl << helperName << '.' << func << '(' << stream << ", " << param << "_tmp[ix__]);";
                out << eb;
            }
            else
            {
                out << nl << "_System.Collections.Generic.IEnumerator<" << typeS
                    << "> e__ = " << param << ".GetEnumerator();";
                out << nl << "while(e__.MoveNext())";
                out << sb;
                out << nl << helperName << '.' << func << '(' << stream << ", e__.Current);";
                out << eb;
            }
        }
        else
        {
            out << nl << "for(int ix__ = 0; ix__ < " << param << '.' << limitID << "; ++ix__)";
            out << sb;
            out << nl << helperName << '.' << func << '(' << stream << ", " << param << "[ix__]);";
            out << eb;
        }
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
        if(isArray)
        {
            out << toArrayAlloc(typeS + "[]", "szx__") << ";";
        }
        else if(isCustom)
        {
            out << genericType << "<" << typeS << ">();";
        }
        else if(isGeneric)
        {
            out << "_System.Collections.Generic." << genericType << "<" << typeS << ">();";
        }
        else
        {
            out << fixId(seq->scoped()) << "(szx__);";
        }
        out << nl << "for(int ix__ = 0; ix__ < szx__; ++ix__)";
        out << sb;
        if(isArray)
        {
            out << nl << param << "[ix__] = " << helperName << '.' << func << '(' << stream << ");";
        }
        else
        {
            out << nl << param << "." << addMethod << "(" << helperName << '.' << func << '(' << stream << "));";
        }
        if(!streamingAPI && type->isVariableLength())
        {
            if(!SequencePtr::dynamicCast(type))
            {
                out << nl << stream << ".checkSeq();";
            }
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
