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

    // TODO
    if(optional)
    {
        return "Ice.Optional<" + typeToString(type, false) + ">";
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
        return "__ice_HashMap";
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

    const StringList ids = fixIds(splitScopedName(fixedScope));

    //
    // Return local scope for "::A::B::C" as _mod_A_B_C
    //
    stringstream result;
    result << "_mod";
    for(StringList::const_iterator i = ids.begin(); i != ids.end(); ++i)
    {
        result << '_' << *i;
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
    string stream;

    if(marshal)
    {
        stream = "__os";
    }
    else
    {
        stream = "__is";
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
                    out << nl << stream << ".writeByte(" << param << ");";
                }
                else
                {
                    out << nl << param << " = " << stream << ".readByte()" << ';';
                }
                break;
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
                break;
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
                break;
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
                break;
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
                break;
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
                break;
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
                break;
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
                break;
            }
            case Builtin::KindObject:
            {
                if(marshal)
                {
                    out << nl << stream << ".writeObject(" << param << ");";
                }
                else
                {
                    out << nl << stream << ".readObject(" << param << ");";
                }
                break;
            }
            case Builtin::KindObjectProxy:
            {
                string typeS = typeToString(type);
                if(marshal)
                {
                    out << nl << stream << ".writeProxy(" << param << ");";
                }
                else
                {
                    out << nl << param << " = " << stream << ".readProxy()" << ';';
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

    ClassDeclPtr cl = ClassDeclPtr::dynamicCast(type);
    if(cl)
    {
        if(marshal)
        {
            out << nl << stream << ".writeObject(" << param << ");";
        }
        else
        {
            const string thisSuffix = "this.";
            string p = param;
            if(p.find(thisSuffix) == 0)
            {
                p = "self." + p.substr(thisSuffix.size());
            }
            out << nl << stream << ".readObject(function(obj){ " << p << " = obj; }, " << typeToString(type) << ");";
        }
        return;
    }

    StructPtr st = StructPtr::dynamicCast(type);
    if(st)
    {
        if(marshal)
        {
            out << nl << "__ice_StreamHelpers.writeStruct(" << stream << ", " << param << ");";
        }
        else
        {
            out << nl << "if(" << param << " == null)";
            out << sb;
            out << nl << param << " = new " << typeToString(type) << "();"; // TODO: Fix reference to type
            out << eb;
            out << nl << param << ".__read(" << stream << ");";
        }
        return;
    }

    EnumPtr en = EnumPtr::dynamicCast(type);
    if(en)
    {
        if(marshal)
        {
            out << nl << stream << ".writeEnum(" << param << ", " << en->maxValue() << ");";
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
#if 0 // TODO
    string stream;

    if(marshal)
    {
        stream = streamingAPI ? "outS__" : "os__";
    }
    else
    {
        stream = streamingAPI ? "inS__" : "is__";
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
                    out << nl << stream << ".writeByte(" << tag << ", " << param << ");";
                }
                else
                {
                    out << nl << param << " = " << stream << ".readByte(" << tag << ");";
                }
                break;
            }
            case Builtin::KindBool:
            {
                if(marshal)
                {
                    out << nl << stream << ".writeBool(" << tag << ", " << param << ");";
                }
                else
                {
                    out << nl << param << " = " << stream << ".readBool(" << tag << ");";
                }
                break;
            }
            case Builtin::KindShort:
            {
                if(marshal)
                {
                    out << nl << stream << ".writeShort(" << tag << ", " << param << ");";
                }
                else
                {
                    out << nl << param << " = " << stream << ".readShort(" << tag << ");";
                }
                break;
            }
            case Builtin::KindInt:
            {
                if(marshal)
                {
                    out << nl << stream << ".writeInt(" << tag << ", " << param << ");";
                }
                else
                {
                    out << nl << param << " = " << stream << ".readInt(" << tag << ");";
                }
                break;
            }
            case Builtin::KindLong:
            {
                if(marshal)
                {
                    out << nl << stream << ".writeLong(" << tag << ", " << param << ");";
                }
                else
                {
                    out << nl << param << " = " << stream << ".readLong(" << tag << ");";
                }
                break;
            }
            case Builtin::KindFloat:
            {
                if(marshal)
                {
                    out << nl << stream << ".writeFloat(" << tag << ", " << param << ");";
                }
                else
                {
                    out << nl << param << " = " << stream << ".readFloat(" << tag << ");";
                }
                break;
            }
            case Builtin::KindDouble:
            {
                if(marshal)
                {
                    out << nl << stream << ".writeDouble(" << tag << ", " << param << ");";
                }
                else
                {
                    out << nl << param << " = " << stream << ".readDouble(" << tag << ");";
                }
                break;
            }
            case Builtin::KindString:
            {
                if(marshal)
                {
                    out << nl << stream << ".writeString(" << tag << ", " << param << ");";
                }
                else
                {
                    out << nl << param << " = " << stream << ".readString(" << tag << ");";
                }
                break;
            }
            case Builtin::KindObject:
            {
                if(marshal)
                {
                    out << nl << stream << ".writeObject(" << tag << ", " << param << ");";
                }
                else
                {
                    out << nl << stream << ".readObject(" << tag << ", " << param << ");";
                }
                break;
            }
            case Builtin::KindObjectProxy:
            {
                string typeS = typeToString(type);
                if(marshal)
                {
                    out << nl << stream << ".writeProxy(" << tag << ", " << param << ");";
                }
                else
                {
                    out << nl << param << " = new Ice.Optional<Ice.ObjectPrx>(" << stream << ".readProxy(" << tag
                        << "));";
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
        if(marshal)
        {
            out << nl << "if(" << param << ".HasValue && " << stream << ".writeOpt(" << tag
                << ", Ice.OptionalFormat.FSize))";
            out << sb;
            out << nl << stream << ".startSize();";
            writeMarshalUnmarshalCode(out, type, param + ".Value", marshal, streamingAPI);
            out << nl << stream << ".endSize();";
            out << eb;
        }
        else
        {
            out << nl << "if(" << stream << ".readOpt(" << tag << ", Ice.OptionalFormat.FSize))";
            out << sb;
            out << nl << stream << ".skip(4);";
            string tmp = "tmpVal__";
            string typeS = typeToString(type);
            out << nl << typeS << ' ' << tmp << ';';
            writeMarshalUnmarshalCode(out, type, tmp, marshal, streamingAPI);
            out << nl << param << " = new Ice.Optional<" << typeS << ">(" << tmp << ");";
            out << eb;
            out << nl << "else";
            out << sb;
            out << nl << param << " = new Ice.Optional<" << typeS << ">();";
            out << eb;
        }
        return;
    }

    ClassDeclPtr cl = ClassDeclPtr::dynamicCast(type);
    if(cl)
    {
        if(marshal)
        {
            out << nl << stream << ".writeObject(" << tag << ", " << param << ");";
        }
        else
        {
            out << nl << stream << ".readObject(" << tag << ", " << param << ");";
        }
        return;
    }

    StructPtr st = StructPtr::dynamicCast(type);
    if(st)
    {
        if(marshal)
        {
            out << nl << "if(" << param << ".HasValue && " << stream << ".writeOpt(" << tag << ", "
                << getOptionalFormat(st) << "))";
            out << sb;
            if(st->isVariableLength())
            {
                out << nl << stream << ".startSize();";
            }
            else
            {
                out << nl << stream << ".writeSize(" << st->minWireSize() << ");";
            }
            writeMarshalUnmarshalCode(out, type, param + ".Value", marshal, streamingAPI);
            if(st->isVariableLength())
            {
                out << nl << stream << ".endSize();";
            }
            out << eb;
        }
        else
        {
            out << nl << "if(" << stream << ".readOpt(" << tag << ", " << getOptionalFormat(st) << "))";
            out << sb;
            if(st->isVariableLength())
            {
                out << nl << stream << ".skip(4);";
            }
            else
            {
                out << nl << stream << ".skipSize();";
            }
            string typeS = typeToString(type);
            string tmp = "tmpVal__";
            if(isValueType(st))
            {
                out << nl << typeS << ' ' << tmp << " = new " << typeS << "();";
            }
            else
            {
                out << nl << typeS << ' ' << tmp << " = null;";
            }
            writeMarshalUnmarshalCode(out, type, tmp, marshal, streamingAPI);
            out << nl << param << " = new Ice.Optional<" << typeS << ">(" << tmp << ");";
            out << eb;
            out << nl << "else";
            out << sb;
            out << nl << param << " = new Ice.Optional<" << typeS << ">();";
            out << eb;
        }
        return;
    }

    EnumPtr en = EnumPtr::dynamicCast(type);
    if(en)
    {
        size_t sz = en->getEnumerators().size();
        if(marshal)
        {
            out << nl << "if(" << param << ".HasValue)";
            out << sb;
            out << nl << stream << ".writeEnum(" << tag << ", (int)" << param << ".Value, " << sz << ");";
            out << eb;
        }
        else
        {
            out << nl << "if(" << stream << ".readOpt(" << tag << ", Ice.OptionalFormat.Size))";
            out << sb;
            string typeS = typeToString(type);
            string tmp = "tmpVal__";
            out << nl << typeS << ' ' << tmp << ';';
            writeMarshalUnmarshalCode(out, type, tmp, marshal, streamingAPI);
            out << nl << param << " = new Ice.Optional<" << typeS << ">(" << tmp << ");";
            out << eb;
            out << nl << "else";
            out << sb;
            out << nl << param << " = new Ice.Optional<" << typeS << ">();";
            out << eb;
        }
        return;
    }

    SequencePtr seq = SequencePtr::dynamicCast(type);
    if(seq)
    {
        writeOptionalSequenceMarshalUnmarshalCode(out, seq, param, tag, marshal, streamingAPI);
        return;
    }

    DictionaryPtr d = DictionaryPtr::dynamicCast(type);
    assert(d);
    TypePtr keyType = d->keyType();
    TypePtr valueType = d->valueType();
    if(marshal)
    {
        out << nl << "if(" << param << ".HasValue && " << stream << ".writeOpt(" << tag << ", "
            << getOptionalFormat(d) << "))";
        out << sb;
        if(keyType->isVariableLength() || valueType->isVariableLength())
        {
            out << nl << stream << ".startSize();";
        }
        else
        {
            out << nl << stream << ".writeSize(" << param << ".Value == null ? 1 : " << param << ".Value.Count * "
                << (keyType->minWireSize() + valueType->minWireSize()) << " + (" << param
                << ".Value.Count > 254 ? 5 : 1));";
        }
        writeMarshalUnmarshalCode(out, type, param + ".Value", marshal, streamingAPI);
        if(keyType->isVariableLength() || valueType->isVariableLength())
        {
            out << nl << stream << ".endSize();";
        }
        out << eb;
    }
    else
    {
        out << nl << "if(" << stream << ".readOpt(" << tag << ", " << getOptionalFormat(d) << "))";
        out << sb;
        if(keyType->isVariableLength() || valueType->isVariableLength())
        {
            out << nl << stream << ".skip(4);";
        }
        else
        {
            out << nl << stream << ".skipSize();";
        }
        string typeS = typeToString(type);
        string tmp = "tmpVal__";
        out << nl << typeS << ' ' << tmp << " = new " << typeS << "();";
        writeMarshalUnmarshalCode(out, type, tmp, marshal, streamingAPI);
        out << nl << param << " = new Ice.Optional<" << typeS << ">(" << tmp << ");";
        out << eb;
        out << nl << "else";
        out << sb;
        out << nl << param << " = new Ice.Optional<" << typeS << ">();";
        out << eb;
    }
#endif
}

bool
isObjectType(const TypePtr& type)
{
    BuiltinPtr builtin = BuiltinPtr::dynamicCast(type);

    return (builtin && builtin == Builtin::KindObject) || ClassDeclPtr::dynamicCast(type);
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
                return "StreamHelpers.ByteHelper";
            }
            case Builtin::KindBool:
            {
                return "StreamHelpers.BoolHelper";
            }
            case Builtin::KindShort:
            {
                return "StreamHelpers.ShortHelper";
            }
            case Builtin::KindInt:
            {
                return "StreamHelpers.IntHelper";
            }
            case Builtin::KindLong:
            {
                return "StreamHelpers.LongHelper";
            }
            case Builtin::KindFloat:
            {
                return "StreamHelpers.FloatHelper";
            }
            case Builtin::KindDouble:
            {
                return "StreamHelpers.DoubleHelper";
            }
            case Builtin::KindString:
            {
                return "StreamHelpers.StringHelper";
            }
            case Builtin::KindObject:
            {
                // Uses generateObjectSequenceHelper bellow
                break;
            }
            case Builtin::KindObjectProxy:
            {
                return "StreamHelpers.ProxyHelper";
            }
            case Builtin::KindLocalObject:
            {
                assert(false);
                break;
            }
        }
    }
    
    EnumPtr en = EnumPtr::dynamicCast(type);
    if(en)
    {
        stringstream os;
        os << "StreamHelpers.generateEnumHelper(" << typeToString(type) << ")";
        return os.str();
    }
    
    ClassDeclPtr cls = ClassDeclPtr::dynamicCast(type);
    if(cls || (builtin && builtin == Builtin::KindLocalObject))
    {
        stringstream os;
        os << "StreamHelpers.generateObjectSequenceHelper(" << typeToString(type) <<")";
        return os.str();
    }
    
    ProxyPtr prx = ProxyPtr::dynamicCast(type);
    if(prx)
    {
        return "StreamHelpers.ProxyHelper";
    }
    
    SequencePtr seq = SequencePtr::dynamicCast(type);
    if(seq)
    {
        if(isObjectType(seq->type()))
        {
            stringstream os;
            os << "StreamHelpers.generateObjectSequenceHelper(" << typeToString(seq->type()) <<")";
            return os.str();
        }
        else
        {
            return "StreamHelpers.SequenceHelper";
        }
    }
    
    DictionaryPtr d = DictionaryPtr::dynamicCast(type);
    if(d)
    {
        if(isObjectType(d->valueType()))
        {
            stringstream os;
            os << "StreamHelpers.generateObjectDictionaryHelper("
               << getHelper(d->keyType())
               << typeToString(d->valueType()) << ")";
            return os.str();
        }
        else
        {
            return "StreamHelpers.DictionaryHelper";
        }
    }
    assert(false);
    return "???";
}

void
Slice::JsGenerator::writeDictionaryHelper(Output& out, const TypePtr& keyType, const TypePtr& valueType)
{
    out << ",";
    out << nl << "{";
    out.inc();
    out << nl << "key:" << getHelper(keyType) << ", "
        << nl << "value:" << getHelper(valueType);
    out.dec();
    out << nl << "}";
    
    SequencePtr seq = SequencePtr::dynamicCast(valueType);
    if(seq)
    {
        writeHelpers(out, seq->type(), false);
        return;
    }
    
    DictionaryPtr dict = DictionaryPtr::dynamicCast(valueType);
    if(dict)
    {
        writeHelpers(out, dict->valueType(), false);
        return;
    }
}

void
Slice::JsGenerator::writeHelpers(Output& out, const TypePtr& type, bool first)
{
    if(!first)
    {
        out << ",";
    }
    out << nl << getHelper(type);
    
    SequencePtr seq = SequencePtr::dynamicCast(type);
    if(seq)
    {
        if(!isObjectType(seq->type()))
        {
            writeHelpers(out, seq->type(), false);
        }
        return;
    }
    
    DictionaryPtr d = DictionaryPtr::dynamicCast(type);
    if(d)
    {
        if(!isObjectType(d->valueType()))
        {
            writeDictionaryHelper(out, d->keyType(), d->valueType());
        }
        return;
    }
}

void
Slice::JsGenerator::writeSequenceMarshalUnmarshalCode(Output& out, const SequencePtr& seq, const string& param,
                                                      bool marshal)
{
    string stream = marshal ? "os__" : "is__";

    if(isObjectType(seq->type()))
    {
        // Use ObjectSequenceHelper
        if(marshal)
        {
            out << nl << "StreamHelpers.generateObjectSequenceHelper(" << typeToString(seq->type()) <<" ).write(" 
                << stream << ", " << param << ");";
        }
        else
        {
            out << nl << param << " = " << "StreamHelpers.generateObjectSequenceHelper(" << typeToString(seq->type()) 
                << ").read(" << stream << ");";
        }
    }
    else
    {
        // Use SequenceHelper
        if(marshal)
        {
            out << nl << "StreamHelpers.SequenceHelper.write(" << stream << ", " << param << ", [";
            out.inc();
            writeHelpers(out, seq->type(), true);
            out.dec();
            out << nl << "]);";
        }
        else
        {
            out << nl << param << " = " << "StreamHelpers.SequenceHelper.read(" << stream << ", [";
            out.inc();
            writeHelpers(out, seq->type(), true);
            out.dec();
            out << nl << "]);";
        }
    }
}

void
Slice::JsGenerator::writeDictionaryMarshalUnmarshalCode(Output& out, const DictionaryPtr& dict, const string& param,
                                                      bool marshal)
{
    string stream = marshal ? "os__" : "is__";
    if(isObjectType(dict->valueType()))
    {
        // Use ObjectDictionaryHelper
        if(marshal)
        {
            out << nl << "StreamHelpers.generateObjectDictionaryHelper(" 
                << getHelper(dict->keyType()) << ", "    
                << typeToString(dict->valueType()) 
                <<" ).write(" 
                << stream << ", " << param << ");";
        }
        else
        {
            out << nl << param << " = " << "StreamHelpers.generateObjectDictionaryHelper(" 
                << getHelper(dict->keyType()) << ", "
                << typeToString(dict->valueType()) 
                << ").read(" << stream << ");";
        }
    }
    else
    {
        // Use DictionaryHelper
        if(marshal)
        {
            out << nl << "StreamHelpers.DictionaryHelper.write(" << stream << ", " << param << ", [";
            out.inc();
            writeDictionaryHelper(out, dict->keyType(), dict->valueType());
            out.dec();
            out << nl << "]);";
        }
        else
        {
            out << nl << param << " = " << "StreamHelpers.DictionaryHelper.read(" << stream << ", [";
            out.inc();
            writeDictionaryHelper(out, dict->keyType(), dict->valueType());
            out.dec();
            out << nl << "]);";
        }
    }
}

void
Slice::JsGenerator::writeOptionalSequenceMarshalUnmarshalCode(Output& out,
                                                              const SequencePtr& seq,
                                                              const string& param,
                                                              int tag,
                                                              bool marshal)
{
#if 0 // TODO
    string stream;
    if(marshal)
    {
        stream = streamingAPI ? "outS__" : "os__";
    }
    else
    {
        stream = streamingAPI ? "inS__" : "is__";
    }

    const TypePtr type = seq->type();
    const string typeS = typeToString(type);
    const string seqS = typeToString(seq);

    string meta;
    const bool isArray = !seq->findMetaData("clr:generic:", meta) && !seq->hasMetaData("clr:collection");
    const string length = isArray ? param + ".Value.Length" : param + ".Value.Count";

    BuiltinPtr builtin = BuiltinPtr::dynamicCast(type);
    if(builtin)
    {
        switch(builtin->kind())
        {
        case Builtin::KindByte:
        case Builtin::KindBool:
        case Builtin::KindShort:
        case Builtin::KindInt:
        case Builtin::KindFloat:
        case Builtin::KindLong:
        case Builtin::KindDouble:
        case Builtin::KindString:
        {
            string func = typeS;
            func[0] = toupper(static_cast<unsigned char>(typeS[0]));
            const bool isSerializable = seq->findMetaData("clr:serializable:", meta);

            if(marshal)
            {
                if(isSerializable)
                {
                    out << nl << "if(" << param << ".HasValue && " << stream << ".writeOpt(" << tag
                        << ", Ice.OptionalFormat.VSize))";
                    out << sb;
                    out << nl << stream << ".writeSerializable(" << param << ".Value);";
                    out << eb;
                }
                else if(isArray)
                {
                    out << nl << stream << ".write" << func << "Seq(" << tag << ", " << param << ");";
                }
                else
                {
                    out << nl << "if(" << param << ".HasValue)";
                    out << sb;
                    out << nl << stream << ".write" << func << "Seq(" << tag << ", " << param
                        << ".Value == null ? 0 : " << param << ".Value.Count, " << param << ".Value);";
                    out << eb;
                }
            }
            else
            {
                out << nl << "if(" << stream << ".readOpt(" << tag << ", " << getOptionalFormat(seq) << "))";
                out << sb;
                if(builtin->isVariableLength())
                {
                    out << nl << stream << ".skip(4);";
                }
                else if(builtin->kind() != Builtin::KindByte && builtin->kind() != Builtin::KindBool)
                {
                    out << nl << stream << ".skipSize();";
                }
                string tmp = "tmpVal__";
                out << nl << seqS << ' ' << tmp << ';';
                writeSequenceMarshalUnmarshalCode(out, seq, tmp, marshal, streamingAPI, true);
                out << nl << param << " = new Ice.Optional<" << seqS << ">(" << tmp << ");";
                out << eb;
                out << nl << "else";
                out << sb;
                out << nl << param << " = new Ice.Optional<" << seqS << ">();";
                out << eb;
            }
            break;
        }

        case Builtin::KindObject:
        case Builtin::KindObjectProxy:
        {
            if(marshal)
            {
                out << nl << "if(" << param << ".HasValue && " << stream << ".writeOpt(" << tag << ", "
                    << getOptionalFormat(seq) << "))";
                out << sb;
                out << nl << stream << ".startSize();";
                writeSequenceMarshalUnmarshalCode(out, seq, param + ".Value", marshal, streamingAPI, true);
                out << nl << stream << ".endSize();";
                out << eb;
            }
            else
            {
                out << nl << "if(" << stream << ".readOpt(" << tag << ", " << getOptionalFormat(seq) << "))";
                out << sb;
                out << nl << stream << ".skip(4);";
                string tmp = "tmpVal__";
                out << nl << seqS << ' ' << tmp << ';';
                writeSequenceMarshalUnmarshalCode(out, seq, tmp, marshal, streamingAPI, true);
                out << nl << param << " = new Ice.Optional<" << seqS << ">(" << tmp << ");";
                out << eb;
                out << nl << "else";
                out << sb;
                out << nl << param << " = new Ice.Optional<" << seqS << ">();";
                out << eb;
            }
            break;
        }

        case Builtin::KindLocalObject:
            assert(false);
        }

        return;
    }

    StructPtr st = StructPtr::dynamicCast(type);
    if(st)
    {
        if(marshal)
        {
            out << nl << "if(" << param << ".HasValue && " << stream << ".writeOpt(" << tag << ", "
                << getOptionalFormat(seq) << "))";
            out << sb;
            if(st->isVariableLength())
            {
                out << nl << stream << ".startSize();";
            }
            else if(st->minWireSize() > 1)
            {
                out << nl << stream << ".writeSize(" << param << ".Value == null ? 1 : " << length << " * "
                    << st->minWireSize() << " + (" << length << " > 254 ? 5 : 1));";
            }
            writeSequenceMarshalUnmarshalCode(out, seq, param + ".Value", marshal, streamingAPI, true);
            if(st->isVariableLength())
            {
                out << nl << stream << ".endSize();";
            }
            out << eb;
        }
        else
        {
            out << nl << "if(" << stream << ".readOpt(" << tag << ", " << getOptionalFormat(seq) << "))";
            out << sb;
            if(st->isVariableLength())
            {
                out << nl << stream << ".skip(4);";
            }
            else if(st->minWireSize() > 1)
            {
                out << nl << stream << ".skipSize();";
            }
            string tmp = "tmpVal__";
            out << nl << seqS << ' ' << tmp << ';';
            writeSequenceMarshalUnmarshalCode(out, seq, tmp, marshal, streamingAPI, true);
            out << nl << param << " = new Ice.Optional<" << seqS << ">(" << tmp << ");";
            out << eb;
            out << nl << "else";
            out << sb;
            out << nl << param << " = new Ice.Optional<" << seqS << ">();";
            out << eb;
        }
        return;
    }

    //
    // At this point, all remaining element types have variable size.
    //
    if(marshal)
    {
        out << nl << "if(" << param << ".HasValue && " << stream << ".writeOpt(" << tag << ", "
            << getOptionalFormat(seq) << "))";
        out << sb;
        out << nl << stream << ".startSize();";
        writeSequenceMarshalUnmarshalCode(out, seq, param + ".Value", marshal, streamingAPI, true);
        out << nl << stream << ".endSize();";
        out << eb;
    }
    else
    {
        out << nl << "if(" << stream << ".readOpt(" << tag << ", " << getOptionalFormat(seq) << "))";
        out << sb;
        out << nl << stream << ".skip(4);";
        string tmp = "tmpVal__";
        out << nl << seqS << ' ' << tmp << ';';
        writeSequenceMarshalUnmarshalCode(out, seq, tmp, marshal, streamingAPI, true);
        out << nl << param << " = new Ice.Optional<" << seqS << ">(" << tmp << ");";
        out << eb;
        out << nl << "else";
        out << sb;
        out << nl << param << " = new Ice.Optional<" << seqS << ">();";
        out << eb;
    }
#endif
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
