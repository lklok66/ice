// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

//
// Disable unreferenced formal parameter warnings
// for VC90 binary_search.
//
#if defined(_MSC_VER) && (_MSC_VER <= 1500)
#  pragma warning( push )
#  pragma warning( disable : 4100 )
#  include <algorithm>
#  pragma warning( pop )
#endif

#include <JsUtil.h>
#include <Slice/Util.h>
#include <IceUtil/Functional.h>

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

static string
lookupKwd(const string& name, bool mangleCasts = false)
{
    //
    // Keyword list. *Must* be kept in alphabetical order.
    //
    static const string keywordList[] =
    {
        "break", "case", "catch", "class", "const", "continue", "debugger", "default", "delete", "do", "else",
        "enum", "export", "extends", "false", "finally", "for", "function", "if", "implements", "import", "in",
        "instanceof", "interface", "let", "new", "null", "package", "private", "protected", "public", "return",
        "static", "super", "switch", "this", "throw", "true", "try", "typeof", "var", "void", "while", "with",
        "yield"
    };
    bool found = binary_search(&keywordList[0],
                               &keywordList[sizeof(keywordList) / sizeof(*keywordList)],
                               name,
                               Slice::CICompare());
    if(found)
    {
        return "_" + name;
    }
    if(mangleCasts && (name == "checkedCast" || name == "uncheckedCast"))
    {
        return "_" + name;
    }
    return name;
}

//
// Split a scoped name into its components and return the components as a list of (unscoped) identifiers.
//
static StringList
splitScopedName(const string& scoped)
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

static StringList
fixIds(const StringList& ids)
{
    StringList newIds;
    for(StringList::const_iterator i = ids.begin(); i != ids.end(); ++i)
    {
        newIds.push_back(lookupKwd(*i));
    }
    return newIds;
}

static string
fixSuffix(const string& param)
{
    const string thisSuffix = "this.";
    string p = param;
    if(p.find(thisSuffix) == 0)
    {
        p = "self." + p.substr(thisSuffix.size());
    }
    return p;
}

bool
Slice::JsGenerator::isClassType(const TypePtr& type)
{
    BuiltinPtr builtin = BuiltinPtr::dynamicCast(type);
    return (builtin && builtin->kind() == Builtin::KindObject) || ClassDeclPtr::dynamicCast(type);
}

//
// If the passed name is a scoped name, return the identical scoped name,
// but with all components that are JS keywords replaced by
// their "_"-prefixed version; otherwise, if the passed name is
// not scoped, but a JS keyword, return the "_"-prefixed name.
//
string
Slice::JsGenerator::fixId(const string& name, bool mangleCasts)
{
    if(name.empty())
    {
        return name;
    }
    if(name[0] != ':')
    {
        return lookupKwd(name, mangleCasts);
    }

    const StringList ids = splitScopedName(name);
    const StringList newIds = fixIds(ids);

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
Slice::JsGenerator::fixId(const ContainedPtr& cont, bool mangleCasts)
{
    return fixId(cont->name(), mangleCasts);
}

string
Slice::JsGenerator::getOptionalFormat(const TypePtr& type)
{
    BuiltinPtr bp = BuiltinPtr::dynamicCast(type);
    if(bp)
    {
        switch(bp->kind())
        {
        case Builtin::KindByte:
        case Builtin::KindBool:
        {
            return "Ice.OptionalFormat.F1";
        }
        case Builtin::KindShort:
        {
            return "Ice.OptionalFormat.F2";
        }
        case Builtin::KindInt:
        case Builtin::KindFloat:
        {
            return "Ice.OptionalFormat.F4";
        }
        case Builtin::KindLong:
        case Builtin::KindDouble:
        {
            return "Ice.OptionalFormat.F8";
        }
        case Builtin::KindString:
        {
            return "Ice.OptionalFormat.VSize";
        }
        case Builtin::KindObject:
        {
            return "Ice.OptionalFormat.Class";
        }
        case Builtin::KindObjectProxy:
        {
            return "Ice.OptionalFormat.FSize";
        }
        case Builtin::KindLocalObject:
        {
            assert(false);
            break;
        }
        }
    }

    if(EnumPtr::dynamicCast(type))
    {
        return "Ice.OptionalFormat.Size";
    }

    SequencePtr seq = SequencePtr::dynamicCast(type);
    if(seq)
    {
        return seq->type()->isVariableLength() ? "Ice.OptionalFormat.FSize" : "Ice.OptionalFormat.VSize";
    }

    DictionaryPtr d = DictionaryPtr::dynamicCast(type);
    if(d)
    {
        return (d->keyType()->isVariableLength() || d->valueType()->isVariableLength()) ?
            "Ice.OptionalFormat.FSize" : "Ice.OptionalFormat.VSize";
    }

    StructPtr st = StructPtr::dynamicCast(type);
    if(st)
    {
        return st->isVariableLength() ? "Ice.OptionalFormat.FSize" : "Ice.OptionalFormat.VSize";
    }

    if(ProxyPtr::dynamicCast(type))
    {
        return "Ice.OptionalFormat.FSize";
    }

    ClassDeclPtr cl = ClassDeclPtr::dynamicCast(type);
    assert(cl);
    return "Ice.OptionalFormat.Class";
}

string
Slice::JsGenerator::getStaticId(const TypePtr& type)
{
    BuiltinPtr b = BuiltinPtr::dynamicCast(type);
    ClassDeclPtr cl = ClassDeclPtr::dynamicCast(type);

    assert((b && b->kind() == Builtin::KindObject) || cl);

    if(b)
    {
        return "Ice.ObjectImpl.ice_staticId()";
    }
    else if(cl->isInterface())
    {
        ContainedPtr cont = ContainedPtr::dynamicCast(cl->container());
        assert(cont);
        return fixId(cont->scoped()) + "." + cl->name() + "Disp_.ice_staticId()";
    }
    else
    {
        return fixId(cl->scoped()) + ".ice_staticId()";
    }
}

string
Slice::JsGenerator::typeToString(const TypePtr& type, bool optional)
{
    if(!type)
    {
        return "void";
    }

    static const char* builtinTable[] =
    {
        "Number",           // byte
        "Boolean",          // bool
        "Number",           // short
        "Number",           // int
        "Number",           // long
        "Number",           // float
        "Number",           // double
        "String",
        "Ice.Object",       // TODO: Can we use "Object"?
        "Ice.ObjectPrx",
        "Object"
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
        return typeToString(seq->type()) + "[]";
    }

    DictionaryPtr d = DictionaryPtr::dynamicCast(type);
    if(d)
    {
        return "Ice.HashMap";
    }

    ContainedPtr contained = ContainedPtr::dynamicCast(type);
    if(contained)
    {
        return fixId(contained->scoped());
    }

    return "???";
}

string
Slice::JsGenerator::getLocalScope(const string& scope)
{
    assert(!scope.empty());

    //
    // Remove trailing "::" if present.
    //
    string fixedScope;
    if(scope[scope.size() - 1] == ':')
    {
        assert(scope[scope.size() - 2] == ':');
        fixedScope = scope.substr(0, scope.size() - 2);
    }
    else
    {
        fixedScope = scope;
    }
    if(fixedScope.empty())
    {
        return "";
    }
    const StringList ids = fixIds(splitScopedName(fixedScope));

    //
    // Return local scope for "::A::B::C" as A.B.C
    //
    stringstream result;
    for(StringList::const_iterator i = ids.begin(); i != ids.end(); ++i)
    {
        if(i != ids.begin())
        {
            result << '.';
        }
        result << *i;
    }
    return result.str();
}

string
Slice::JsGenerator::getReference(const string& scope, const string& target)
{
    //
    // scope and target should be fully-qualified symbols.
    //
    assert(!scope.empty() && scope[0] == ':' && !target.empty() && target[0] == ':');

    //
    // Check whether the target is in the given scope.
    //
    if(target.find(scope) == 0)
    {
        //
        // Remove scope from target, but keep the leading "::".
        //
        const string rem = target.substr(scope.size() - 2);
        assert(!rem.empty());
        const StringList ids = fixIds(splitScopedName(rem));
        stringstream result;
        result << getLocalScope(scope);
        for(StringList::const_iterator i = ids.begin(); i != ids.end(); ++i)
        {
            result << '.' << *i;
        }
        return result.str();
    }
    else
    {
        return fixId(target);
    }
}

void
Slice::JsGenerator::writeMarshalUnmarshalCode(Output &out,
                                              const TypePtr& type,
                                              const string& param,
                                              bool marshal)
{
    string stream = marshal ? "__os" : "__is";

    BuiltinPtr builtin = BuiltinPtr::dynamicCast(type);
    if(builtin)
    {
        switch(builtin->kind())
        {
            case Builtin::KindByte:
            {
                if(marshal)
                {
                    out << nl << stream << ".writeByte(" << param << ");";
                }
                else
                {
                    out << nl << param << " = " << stream << ".readByte()" << ';';
                }
                return;
            }
            case Builtin::KindBool:
            {
                if(marshal)
                {
                    out << nl << stream << ".writeBool(" << param << ");";
                }
                else
                {
                    out << nl << param << " = " << stream << ".readBool()" << ';';
                }
                return;
            }
            case Builtin::KindShort:
            {
                if(marshal)
                {
                    out << nl << stream << ".writeShort(" << param << ");";
                }
                else
                {
                    out << nl << param << " = " << stream << ".readShort()" << ';';
                }
                return;
            }
            case Builtin::KindInt:
            {
                if(marshal)
                {
                    out << nl << stream << ".writeInt(" << param << ");";
                }
                else
                {
                    out << nl << param << " = " << stream << ".readInt()" << ';';
                }
                return;
            }
            case Builtin::KindLong:
            {
                if(marshal)
                {
                    out << nl << stream << ".writeLong(" << param << ");";
                }
                else
                {
                    out << nl << param << " = " << stream << ".readLong()" << ';';
                }
                return;
            }
            case Builtin::KindFloat:
            {
                if(marshal)
                {
                    out << nl << stream << ".writeFloat(" << param << ");";
                }
                else
                {
                    out << nl << param << " = " << stream << ".readFloat()" << ';';
                }
                return;
            }
            case Builtin::KindDouble:
            {
                if(marshal)
                {
                    out << nl << stream << ".writeDouble(" << param << ");";
                }
                else
                {
                    out << nl << param << " = " << stream << ".readDouble()" << ';';
                }
                return;
            }
            case Builtin::KindString:
            {
                if(marshal)
                {
                    out << nl << stream << ".writeString(" << param << ");";
                }
                else
                {
                    out << nl << param << " = " << stream << ".readString()" << ';';
                }
                return;
            }
            case Builtin::KindObject:
            {
                // Handle by isClassType below.
                break;
            }
            case Builtin::KindObjectProxy:
            {
                if(marshal)
                {
                    out << nl << stream << ".writeProxy(" << param << ");";
                }
                else
                {
                    out << nl << param << " = " << stream << ".readProxy();";
                }
                return;
            }
            case Builtin::KindLocalObject:
            {
                assert(false);
                return;
            }
        }
    }

    if(ProxyPtr::dynamicCast(type))
    {
        if(marshal)
        {
            out << nl << typeToString(type) << ".write(" << stream << ", " << param << ");";
        }
        else
        {
            out << nl << param << " = " << typeToString(type) << ".read(" << stream << ");";
        }
        return;
    }

    if(isClassType(type))
    {
        if(marshal)
        {
            out << nl << stream << ".writeObject(" << param << ");";
        }
        else
        {
            out << nl << stream << ".readObject(function(obj){ " << fixSuffix(param) << " = obj; }, " << typeToString(type) << ");";
        }
        return;
    }

    StructPtr st = StructPtr::dynamicCast(type);
    if(st)
    {
        if(marshal)
        {
            out << nl << stream << ".writeStruct(" << param << ");";
        }
        else
        {
            out << nl << param << " = " << stream << ".readStruct(" << typeToString(type) << ");";
        }
        return;
    }

    EnumPtr en = EnumPtr::dynamicCast(type);
    if(en)
    {
        if(marshal)
        {
            out << nl << stream << ".writeEnum(" << param << ");";
        }
        else
        {
            out << nl << param << " = " << stream << ".readEnum(" << typeToString(en) << ");";
        }
        return;
    }


    SequencePtr seq = SequencePtr::dynamicCast(type);
    if(seq)
    {
        writeSequenceMarshalUnmarshalCode(out, seq, param, marshal);
        return;
    }

    DictionaryPtr d = DictionaryPtr::dynamicCast(type);
    if(d)
    {
        writeDictionaryMarshalUnmarshalCode(out, d, param, marshal);
        return;
    }
    assert(false);
}

void
Slice::JsGenerator::writeOptionalMarshalUnmarshalCode(Output &out,
                                                      const TypePtr& type,
                                                      const string& param,
                                                      int tag,
                                                      bool marshal)
{
    string stream = marshal ? "__os" : "__is";

    BuiltinPtr builtin = BuiltinPtr::dynamicCast(type);
    if(builtin)
    {
        switch(builtin->kind())
        {
            case Builtin::KindByte:
            {
                if(marshal)
                {
                    out << nl << stream << ".writeOptByte(" << tag << ", " << param << ");";
                }
                else
                {
                    out << nl << param << " = " << stream << ".readOptByte(" << tag << ");";
                }
                return;
            }
            case Builtin::KindBool:
            {
                if(marshal)
                {
                    out << nl << stream << ".writeOptBool(" << tag << ", " << param << ");";
                }
                else
                {
                    out << nl << param << " = " << stream << ".readOptBool(" << tag << ");";
                }
                return;
            }
            case Builtin::KindShort:
            {
                if(marshal)
                {
                    out << nl << stream << ".writeOptShort(" << tag << ", " << param << ");";
                }
                else
                {
                    out << nl << param << " = " << stream << ".readOptShort(" << tag << ");";
                }
                return;
            }
            case Builtin::KindInt:
            {
                if(marshal)
                {
                    out << nl << stream << ".writeOptInt(" << tag << ", " << param << ");";
                }
                else
                {
                    out << nl << param << " = " << stream << ".readOptInt(" << tag << ");";
                }
                return;
            }
            case Builtin::KindLong:
            {
                if(marshal)
                {
                    out << nl << stream << ".writeOptLong(" << tag << ", " << param << ");";
                }
                else
                {
                    out << nl << param << " = " << stream << ".readOptLong(" << tag << ");";
                }
                return;
            }
            case Builtin::KindFloat:
            {
                if(marshal)
                {
                    out << nl << stream << ".writeOptFloat(" << tag << ", " << param << ");";
                }
                else
                {
                    out << nl << param << " = " << stream << ".readOptFloat(" << tag << ");";
                }
                return;
            }
            case Builtin::KindDouble:
            {
                if(marshal)
                {
                    out << nl << stream << ".writeOptDouble(" << tag << ", " << param << ");";
                }
                else
                {
                    out << nl << param << " = " << stream << ".readOptDouble(" << tag << ");";
                }
                return;
            }
            case Builtin::KindString:
            {
                if(marshal)
                {
                    out << nl << stream << ".writeOptString(" << tag << ", " << param << ");";
                }
                else
                {
                    out << nl << param << " = " << stream << ".readOptString(" << tag << ");";
                }
                return;
            }
            case Builtin::KindObject:
            {
                // Handle by isClassType below.
                break;
            }
            case Builtin::KindObjectProxy:
            {
                break;
            }
            case Builtin::KindLocalObject:
            {
                assert(false);
                return;
            }
        }
    }
    
    if(isClassType(type))
    {
        if(marshal)
        {
            out << nl << stream << ".writeOptObject(" << tag << ", " << param << ");";
        }
        else
        {
            out << nl << stream << ".readOptObject(" << tag << ", function(obj){ " << fixSuffix(param)
                << " = obj; }, " << typeToString(type) << ");";
        }
        return;
    }
    
    if(ProxyPtr::dynamicCast(type) || (builtin && builtin == Builtin::KindObjectProxy))
    {
        if(marshal)
        {
            out << nl << typeToString(type) << ".writeOpt(" << stream << ", " << tag << ", " << param << ");";
        }
        else
        {
            out << nl << param << " = " << typeToString(type) << ".readOpt(" << stream << ", " << tag << ");";
        }
        return;
    }
    
    StructPtr st = StructPtr::dynamicCast(type);
    if(st)
    {
        if(marshal)
        {
            out << nl << typeToString(type) << ".writeOpt(" << stream << ", " << tag << ", " << param << ");";
        }
        else
        {
            out << nl << param << " = " << typeToString(type) << ".readOpt(" << stream << ", " << tag << ");";
        }
        return;
    }

    EnumPtr en = EnumPtr::dynamicCast(type);
    if(en)
    {
        if(marshal)
        {
            out << nl << stream << ".writeOptEnum(" << tag << ", " << param << ");";
        }
        else
        {
            out << nl << param << " = " << stream << ".readOptEnum(" << tag << ", " << typeToString(en) << ");";
        }
        return;
    }


    SequencePtr seq = SequencePtr::dynamicCast(type);
    if(seq)
    {
        writeOptionalSequenceMarshalUnmarshalCode(out, seq, param, tag, marshal);
        return;
    }

    DictionaryPtr d = DictionaryPtr::dynamicCast(type);
    if(d)
    {
        writeOptionalDictionaryMarshalUnmarshalCode(out, d, param, tag, marshal);
        return;
    }
    assert(false);
}

std::string
Slice::JsGenerator::getHelper(const TypePtr& type)
{
    BuiltinPtr builtin = BuiltinPtr::dynamicCast(type);
    if(builtin)
    {
        switch(builtin->kind())
        {
            case Builtin::KindByte:
            {
                return "Ice.StreamHelpers.ByteHelper";
            }
            case Builtin::KindBool:
            {
                return "Ice.StreamHelpers.BoolHelper";
            }
            case Builtin::KindShort:
            {
                return "Ice.StreamHelpers.ShortHelper";
            }
            case Builtin::KindInt:
            {
                return "Ice.StreamHelpers.IntHelper";
            }
            case Builtin::KindLong:
            {
                return "Ice.StreamHelpers.LongHelper";
            }
            case Builtin::KindFloat:
            {
                return "Ice.StreamHelpers.FloatHelper";
            }
            case Builtin::KindDouble:
            {
                return "Ice.StreamHelpers.DoubleHelper";
            }
            case Builtin::KindString:
            {
                return "Ice.StreamHelpers.StringHelper";
            }
            case Builtin::KindObject:
            {
                // Uses SequenceHelper/DictionaryHelper below
                break;
            }
            case Builtin::KindObjectProxy:
            {
                return "Ice.ObjectPrx";
            }
            case Builtin::KindLocalObject:
            {
                assert(false);
                break;
            }
        }
    }
    
    if(ProxyPtr::dynamicCast(type))
    {
        return typeToString(type);
    }
    
    StructPtr st = StructPtr::dynamicCast(type);
    if(st)
    {
        return typeToString(type);
    }
    
    else if(EnumPtr::dynamicCast(type))
    {
        return typeToString(type);
    }
    
    SequencePtr seq = SequencePtr::dynamicCast(type);
    if(seq)
    {
        stringstream s;
        s << getLocalScope(seq->scoped()) << "Helper";
        return s.str();
    }
    
    DictionaryPtr dict = DictionaryPtr::dynamicCast(type);
    if(dict)
    {
        stringstream s;
        s << getLocalScope(dict->scoped()) << "Helper";
        return s.str();
    }
    assert(false);
    return "???";
}

void
Slice::JsGenerator::writeSequenceMarshalUnmarshalCode(Output& out, const SequencePtr& seq, const string& param,
                                                      bool marshal)
{
    string stream = marshal ? "__os" : "__is";
    if(marshal)
    {
        out << nl << getHelper(seq) <<".write(" << stream << ", " << param << ");";
    }
    else
    {
        out << nl << param << " = " << getHelper(seq) << ".read(" << stream << ");";
    }
}

void
Slice::JsGenerator::writeDictionaryMarshalUnmarshalCode(Output& out, const DictionaryPtr& dict, const string& param,
                                                      bool marshal)
{
    string stream = marshal ? "__os" : "__is";
    if(marshal)
    {
        out << nl << getHelper(dict) <<".write(" << stream << ", " << param << ");";
    }
    else
    {
        out << nl << param << " = " << getHelper(dict) << ".read(" << stream << ");";
    }
}

void
Slice::JsGenerator::writeOptionalSequenceMarshalUnmarshalCode(Output& out,
                                                              const SequencePtr& seq,
                                                              const string& param,
                                                              int tag,
                                                              bool marshal)
{
    string stream = marshal ? "__os" : "__is";
    if(marshal)
    {
        out << nl << getHelper(seq) <<".writeOpt(" << stream << ", " << tag << ", " << param << ");";
    }
    else
    {
        out << nl << param << " = " << getHelper(seq) << ".readOpt(" << stream << ", " << tag << ");";
    }
}

void
Slice::JsGenerator::writeOptionalDictionaryMarshalUnmarshalCode(Output& out, 
                                                                const DictionaryPtr& dict, 
                                                                const string& param,
                                                                int tag,
                                                                bool marshal)
{
    string stream = marshal ? "__os" : "__is";
    if(marshal)
    {
        out << nl << getHelper(dict) <<".writeOpt(" << stream << ", " << tag << ", " << param << ");";
    }
    else
    {
        out << nl << param << " = " << getHelper(dict) << ".readOpt(" << stream << ", " << tag << ");";
    }
}

void
Slice::JsGenerator::validateMetaData(const UnitPtr& u)
{
    MetaDataVisitor visitor;
    u->visit(&visitor, true);
}

bool
Slice::JsGenerator::MetaDataVisitor::visitUnitStart(const UnitPtr& p)
{
    //
    // Validate global metadata in the top-level file and all included files.
    //
    StringList files = p->allFiles();

    for(StringList::iterator q = files.begin(); q != files.end(); ++q)
    {
        string file = *q;
        DefinitionContextPtr dc = p->findDefinitionContext(file);
        assert(dc);
        StringList globalMetaData = dc->getMetaData();

        static const string jsPrefix = "js:";
        for(StringList::const_iterator r = globalMetaData.begin(); r != globalMetaData.end(); ++r)
        {
            string s = *r;
            if(_history.count(s) == 0)
            {
                if(s.find(jsPrefix) == 0)
                {
                    emitWarning(file, -1, "ignoring invalid global metadata `" + s + "'");
                    _history.insert(s);
                }
            }
        }
    }
    return true;
}

bool
Slice::JsGenerator::MetaDataVisitor::visitModuleStart(const ModulePtr& p)
{
    validate(p);
    return true;
}

void
Slice::JsGenerator::MetaDataVisitor::visitModuleEnd(const ModulePtr&)
{
}

void
Slice::JsGenerator::MetaDataVisitor::visitClassDecl(const ClassDeclPtr& p)
{
    validate(p);
}

bool
Slice::JsGenerator::MetaDataVisitor::visitClassDefStart(const ClassDefPtr& p)
{
    validate(p);
    return true;
}

void
Slice::JsGenerator::MetaDataVisitor::visitClassDefEnd(const ClassDefPtr&)
{
}

bool
Slice::JsGenerator::MetaDataVisitor::visitExceptionStart(const ExceptionPtr& p)
{
    validate(p);
    return true;
}

void
Slice::JsGenerator::MetaDataVisitor::visitExceptionEnd(const ExceptionPtr&)
{
}

bool
Slice::JsGenerator::MetaDataVisitor::visitStructStart(const StructPtr& p)
{
    validate(p);
    return true;
}

void
Slice::JsGenerator::MetaDataVisitor::visitStructEnd(const StructPtr&)
{
}

void
Slice::JsGenerator::MetaDataVisitor::visitOperation(const OperationPtr& p)
{
    if(p->hasMetaData("UserException"))
    {
        ClassDefPtr cl = ClassDefPtr::dynamicCast(p->container());
        if(!cl->isLocal())
        {
            ostringstream os;
            os << "ignoring invalid metadata `UserException': directive applies only to local operations "
               << "but enclosing " << (cl->isInterface() ? "interface" : "class") << "`" << cl->name()
               << "' is not local";
            emitWarning(p->file(), p->line(), os.str());
        }
    }
    validate(p);

    ParamDeclList params = p->parameters();
    for(ParamDeclList::const_iterator i = params.begin(); i != params.end(); ++i)
    {
        visitParamDecl(*i);
    }
}

void
Slice::JsGenerator::MetaDataVisitor::visitParamDecl(const ParamDeclPtr& p)
{
    validate(p);
}

void
Slice::JsGenerator::MetaDataVisitor::visitDataMember(const DataMemberPtr& p)
{
    validate(p);
}

void
Slice::JsGenerator::MetaDataVisitor::visitSequence(const SequencePtr& p)
{
    validate(p);
}

void
Slice::JsGenerator::MetaDataVisitor::visitDictionary(const DictionaryPtr& p)
{
    validate(p);
}

void
Slice::JsGenerator::MetaDataVisitor::visitEnum(const EnumPtr& p)
{
    validate(p);
}

void
Slice::JsGenerator::MetaDataVisitor::visitConst(const ConstPtr& p)
{
    validate(p);
}

void
Slice::JsGenerator::MetaDataVisitor::validate(const ContainedPtr& cont)
{
    const string msg = "ignoring invalid metadata";

    StringList localMetaData = cont->getMetaData();

    for(StringList::const_iterator p = localMetaData.begin(); p != localMetaData.end(); ++p)
    {
#if 0 // TODO
        string s = *p;

        string prefix = "clr:";
        if(_history.count(s) == 0)
        {
            if(s.find(prefix) == 0)
            {
                SequencePtr seq = SequencePtr::dynamicCast(cont);
                if(seq)
                {
                    if(s.substr(prefix.size()) == "collection")
                    {
                        continue;
                    }
                    static const string clrGenericPrefix = prefix + "generic:";
                    if(s.find(clrGenericPrefix) == 0)
                    {
                        string type = s.substr(clrGenericPrefix.size());
                        if(type == "LinkedList" || type == "Queue" || type == "Stack")
                        {
                            ClassDeclPtr cd = ClassDeclPtr::dynamicCast(seq->type());
                            BuiltinPtr builtin = BuiltinPtr::dynamicCast(seq->type());
                            if(!cd && !(builtin && builtin->kind() == Builtin::KindObject))
                            {
                                continue;
                            }
                        }
                        else if(!type.empty())
                        {
                            continue; // Custom type or List<T>
                        }
                    }
                    static const string clrSerializablePrefix = prefix + "serializable:";
                    if(s.find(clrSerializablePrefix) == 0)
                    {
                        string meta;
                        if(cont->findMetaData(prefix + "collection", meta)
                           || cont->findMetaData(prefix + "generic:", meta))
                        {
                            emitWarning(cont->file(), cont->line(), msg + " `" + meta + "':\n" +
                                        "serialization can only be used with the array mapping for byte sequences");
                        }
                        string type = s.substr(clrSerializablePrefix.size());
                        BuiltinPtr builtin = BuiltinPtr::dynamicCast(seq->type());
                        if(!type.empty() && builtin && builtin->kind() == Builtin::KindByte)
                        {
                            continue;
                        }
                    }
                }
                else if(StructPtr::dynamicCast(cont))
                {
                    if(s.substr(prefix.size()) == "class")
                    {
                        continue;
                    }
                    if(s.substr(prefix.size()) == "property")
                    {
                        continue;
                    }
                }
                else if(ClassDefPtr::dynamicCast(cont))
                {
                    if(s.substr(prefix.size()) == "property")
                    {
                        continue;
                    }
                }
                else if(DictionaryPtr::dynamicCast(cont))
                {
                    if(s.substr(prefix.size()) == "collection")
                    {
                        continue;
                    }
                    static const string clrGenericPrefix = prefix + "generic:";
                    if(s.find(clrGenericPrefix) == 0)
                    {
                        string type = s.substr(clrGenericPrefix.size());
                        if(type == "SortedDictionary" ||  type == "SortedList")
                        {
                            continue;
                        }
                    }
                }
                emitWarning(cont->file(), cont->line(), msg + " `" + s + "'");
                _history.insert(s);
            }
        }

        prefix = "cs:";
        if(_history.count(s) == 0)
        {
            if(s.find(prefix) == 0)
            {
                static const string csAttributePrefix = prefix + "attribute:";
                if(s.find(csAttributePrefix) == 0 && s.size() > csAttributePrefix.size())
                {
                    continue;
                }
                emitWarning(cont->file(), cont->line(), msg + " `" + s + "'");
                _history.insert(s);
            }
        }
#endif
    }
}
