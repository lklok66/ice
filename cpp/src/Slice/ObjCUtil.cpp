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
    // Keyword list. *Must* be kept in case-insensitive alphabetical order.
    //
    static string keywordList[] =
    {
	"auto", "BOOL", "break", "bycopy", "byref", "case", "char", "Class", "const", "continue",
	"default", "do", "double", "else", "enum", "extern", "float", "for", "goto",
	"id", "if", "IMP", "in", "inout", "int", "long", "nil", "NO", "oneway", "out",
	"register", "return", "SEL", "self", "short", "signed", "sizeof", "static", "struct", "super", "switch",
	"typedef", "union", "unsigned", "void", "volatile", "while", "YES"
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
        return string(ObjC::manglePrefix) + name + string(ObjC::mangleSuffix);
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
#if 0
    if(name[0] != ':')
    {
#endif
        return lookupKwd(name, baseTypes, mangleCasts);
#if 0
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
#endif
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
    if(ProxyPtr::dynamicCast(type) || ClassDeclPtr::dynamicCast(type))
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
Slice::ObjCGenerator::isClass(const TypePtr& type)
{
    BuiltinPtr builtin = BuiltinPtr::dynamicCast(type);
    return builtin && builtin->kind() == Builtin::KindObject || ClassDeclPtr::dynamicCast(type);
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
        if(builtin->kind() == Builtin::KindObject)
        {
            if(marshal)
            {
                out << nl << "[" << stream << " writeObject:" << param << "];";
            }
            else
            {
                out << nl << "[" << stream << " readObject:&" << param << "];";
            }
            return;
        }

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
	    if(builtin->kind() == Builtin::KindObject)
	    {
		out << nl << "[" << stream << " readObject:&" << param << "];";
	    }
	    else
	    {
		out << nl << param << " = [" << stream << " " << selector << "];";
	    }
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
	    out << nl << param << " = (id<" << name << ">)[" << stream 
                << " readProxy:objc_getClass(\"" << name << "\")];";
	}
        return;
    }

    ClassDeclPtr cl = ClassDeclPtr::dynamicCast(type);
    if(cl)
    {
        if(marshal)
        {
	    // Cast avoids warning for forward-declared classes.
            out << nl << "[" << stream << " writeObject:(ICEObject*)" << param << "];";
        }
        else
        {
            out << nl << "[" << stream << " readObject:(ICEObject**)&" << param << "];";
//             if(isOutParam)
//             {
//                 //out << nl << "IceInternal.ParamPatcher<" << typeToString(type) << ">" << param
//                     //<< "_PP = new IceInternal.ParamPatcher<" << typeToString(type) << ">(\""
//                     //<< cl->scoped() << "\");";
//                 out << nl << "[" << stream << " readObject:&" << param << "];"; // TODO: instantiate patcher
// 		//out << "(Ice.ReadObjectCallback)";
//                 //out << param << "_PP);";
//             }
//             else
//             {
//                 out << nl << "[" << stream << " readObject:nil];"; // TODO, instantiate callback
// 		//out << "(Ice.ReadObjectCallback)";
//                 //out << "new Patcher__(\"" << cl->scoped() << "\", " << patchParams << "));";
//             }
        }
        return;
    }

    StructPtr st = StructPtr::dynamicCast(type);
    if(st)
    {
	string typeS = typeToString(st);
	if(marshal)
	{
	    out << nl << "[" << typeS << " ice_writeWithStream:" << param << " stream:" << stream << "];";
	}
	else
	{
	    out << nl << param << " = [" << typeS << " ice_readWithStream:" << stream << "];";
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
                    out << nl << param << " = [" << stream << " readSequence:[ICEObjectPrx class]];";
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
                    out << nl << param << " = [" << stream << " " << selector << "];";
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
		out << nl << param << " = [" << stream << " readEnumSeq:" << sz << "];";
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
		out << nl << param << " = [" << stream << " readSequence:["
		    << mName + prx->_class()->name() + "Prx class]];";
	    }
	    return;
	}

	string prefix = moduleName(findModule(seq));
	string name =  prefix + seq->name() + "Helper";
	if(marshal)
	{
	    out << nl << "[" << name << " ice_writeWithStream:" << param << " stream:" << stream << "];";
	}
	else
	{
	    out << nl << param << " = [" << name << " ice_readWithStream:" << stream << "];";
	}
	return;
    }

    DictionaryPtr d = DictionaryPtr::dynamicCast(type);
    assert(d);
    string prefix = moduleName(findModule(d));
    string name = prefix + d->name() + "Helper";
    if(marshal)
    {
        out << nl << "[" + name << " ice_writeWithStream:" << param << " stream:" << stream << "];";
    }
    else
    {
        out << nl << param << " = [" << name << " ice_readWithStream:" << stream << "];";
    } 
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
