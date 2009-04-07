// **********************************************************************
//
// copyright (c) 2003-2008 zeroc, inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <IceUtil/DisableWarnings.h>
#include <IceUtil/Functional.h>
#include <Gen.h>
#include <limits>
#include <sys/stat.h>
#ifndef _WIN32
#include <unistd.h>
#else
#include <direct.h>
#endif
#include <IceUtil/Iterator.h>
#include <IceUtil/UUID.h>
#include <Slice/Checksum.h>
#include <Slice/ObjCNames.h>
#include <Slice/FileTracker.h>
#include <Slice/Util.h>

using namespace std;
using namespace Slice;

//
// Don't use "using namespace IceUtil", or VC++ 6.0 complains about
// ambigious symbols for constructs like
// "IceUtil::constMemFun(&Slice::Exception::isLocal)".
//
using IceUtilInternal::Output;
using IceUtilInternal::nl;
using IceUtilInternal::sp;
using IceUtilInternal::sb;
using IceUtilInternal::eb;
using IceUtilInternal::spar;
using IceUtilInternal::epar;

static string // Should be an anonymous namespace, but VC++ 6 can't handle that.
sliceModeToIceMode(Operation::Mode opMode)
{
    string mode;
    switch(opMode)
    {
        case Operation::Normal:
        {
            mode = "ICENormal";
            break;
        }
        case Operation::Nonmutating:
        {
            mode = "ICENonmutating";
            break;
        }
        case Operation::Idempotent:
        {
            mode = "ICEIdempotent";
            break;
        }
        default:
        {
            assert(false);
            break;
        }
    }
    return mode;
}

static void
emitDeprecate(const ContainedPtr& p1, const ContainedPtr& p2, Output& out, const string& type)
{
    string deprecateMetadata;
    if(p1->findMetaData("deprecate", deprecateMetadata) || 
       (p2 != 0 && p2->findMetaData("deprecate", deprecateMetadata)))
    {
        string deprecateReason = "This " + type + " has been deprecated.";
        if(deprecateMetadata.find("deprecate:") == 0 && deprecateMetadata.size() > 10)
        {
            deprecateReason = deprecateMetadata.substr(10);
        }
        out << nl << "[System.Obsolete(\"" << deprecateReason << "\")]";
    }
}

Slice::ObjCVisitor::ObjCVisitor(Output& h, Output& m) : _H(h), _M(m)
{
}

Slice::ObjCVisitor::~ObjCVisitor()
{
}

void
Slice::ObjCVisitor::writeInheritedOperations(const ClassDefPtr& p)
{
    ClassList bases = p->bases();
    if(!bases.empty() && !bases.front()->isInterface())
    {
        bases.pop_front();
    }
    if(!bases.empty())
    {
        OperationList allOps;
        for(ClassList::const_iterator q = bases.begin(); q != bases.end(); ++q)
        {
            OperationList tmp = (*q)->allOperations();
            allOps.splice(allOps.end(), tmp);
        }
        allOps.sort();
        allOps.unique();
        for(OperationList::const_iterator op = allOps.begin(); op != allOps.end(); ++op)
        {
            ClassDefPtr containingClass = ClassDefPtr::dynamicCast((*op)->container());
            bool amd = containingClass->hasMetaData("amd") || (*op)->hasMetaData("amd");
            string name = fixId((*op)->name(), ObjC::NSObject, true); // TODO 2nd param
            if(!amd)
            {
                vector<string> params;// = getParams(*op);
                vector<string> args;// = getArgs(*op);
                string retS = typeToString((*op)->returnType());

                _M << sp << nl << "public " << retS << ' ' << name << spar << params << epar;
                _M << sb;
                _M << nl;
                if((*op)->returnType())
                {
                    _M << "return ";
                }
                _M << name << spar << args << "Ice.ObjectImpl.defaultCurrent" << epar << ';';
                _M << eb;

                _M << sp << nl << "public abstract " << retS << ' ' << name << spar << params;
                if(!containingClass->isLocal())
                {
                    _M << "Ice.Current current__";
                }
                _M << epar << ';';
            }
            else
            {
                vector<string> params;// = getParamsAsync(*op, true);
                vector<string> args;// = getArgsAsync(*op);

                _M << sp << nl << "public void " << name << "_async" << spar << params << epar;
                _M << sb;
                _M << nl << name << "_async" << spar << args << epar << ';';
                _M << eb;

                _M << sp << nl << "public abstract void " << name << "_async"
                     << spar << params << "Ice.Current current__" << epar << ';';
            }
        }
    }
}

void
Slice::ObjCVisitor::writeDispatchAndMarshalling(const ClassDefPtr& p, bool stream)
{
    string name = fixName(p);
    string scoped = p->scoped();
    ClassList allBases = p->allBases();
    StringList ids;

    transform(allBases.begin(), allBases.end(), back_inserter(ids), ::IceUtil::constMemFun(&Contained::scoped));

    StringList other;
    other.push_back(p->scoped());
    other.push_back("::Ice::Object");
    other.sort();
    ids.merge(other);
    ids.unique();

    StringList::const_iterator firstIter = ids.begin();
    StringList::const_iterator scopedIter = find(ids.begin(), ids.end(), scoped);
    assert(scopedIter != ids.end());
    StringList::difference_type scopedPos = IceUtilInternal::distance(firstIter, scopedIter);
    
    _M << sp << nl << "static NSString *" << name << "_ids__[] = ";
    _M << sb;
    {
        StringList::const_iterator q = ids.begin();
        while(q != ids.end())
        {
            _M << nl << "@\"" << *q << '"';
            if(++q != ids.end())
            {
                _M << ',';
            }
        }
    }
    _M << eb << ";";

    OperationList ops = p->operations();
    OperationList::const_iterator r;
    for(r = ops.begin(); r != ops.end(); ++r)
    {
        OperationPtr op = *r;
        ContainerPtr container = op->container();
        ClassDefPtr cl = ClassDefPtr::dynamicCast(container);
        assert(cl);

        string opName = fixId(op->name());
        _M << sp << nl << "+(BOOL)" << opName << "___:(ICEObject<" << name
	   << "> *)servant current:(ICECurrent *)current " 
           << "is:(id<ICEInputStream>)is_ os:(id<ICEOutputStream>)os_";
        _M << sb;

        bool amd = false; //p->hasMetaData("amd") || op->hasMetaData("amd");
        if(!amd)
        {
	    _M << nl << "[servant checkModeAndSelector__:" << sliceModeToIceMode(op->mode()) << " selector:@selector(";
	    string selector = getSelector(op);
	    if(!selector.empty())
	    {
	        _M << selector << "current:";
	    }
            else
            {
                _M << opName << ":";
            }
	    _M << ") current:current];";
            TypeStringList inParams;
            TypeStringList outParams;
            ParamDeclList paramList = op->parameters();
            for(ParamDeclList::const_iterator pli = paramList.begin(); pli != paramList.end(); ++pli)
            {
                if(!(*pli)->isOutParam())
                {
                    inParams.push_back(make_pair((*pli)->type(), (*pli)->name()));
                }
		else
		{
		    outParams.push_back(make_pair((*pli)->type(), (*pli)->name()));
		}
            }
	    ExceptionList throws = op->throws();
	    throws.sort();
	    throws.unique();
	    throws.sort(Slice::DerivedToBaseCompare());

            bool inReferenceParams = false;
	    for(TypeStringList::const_iterator inp = inParams.begin(); inp != inParams.end(); ++inp)
	    {
                _M << nl << outTypeToString(inp->first) << " ";
                if(mapsToPointerType(inp->first))
                {
                    _M << "*";
                }
                _M << fixId(inp->second);
                if(!isValueType(inp->first))
                {
                    inReferenceParams = true;
                    _M << " = nil";
                }
                _M << ";";
            }
            if(inReferenceParams || !throws.empty())
            {
                _M << nl << "@try";
                _M << sb;
            }
	    for(TypeStringList::const_iterator inp = inParams.begin(); inp != inParams.end(); ++inp)
	    {
		writeMarshalUnmarshalCode(_M, inp->first, fixId(inp->second), false, false, false);
	    }
            if(op->sendsClasses())
            {
                _M << nl << "[is_ readPendingObjects];";
            }
	    for(TypeStringList::const_iterator outp = outParams.begin(); outp != outParams.end(); ++outp)
	    {
	        _M << nl << typeToString(outp->first) << " ";
		if(mapsToPointerType(outp->first))
		{
		    _M << "*";
		}
		_M << fixId(outp->second) << ";";
	    }
	    TypePtr returnType = op->returnType();
	    if(returnType)
	    {
	        _M << nl << typeToString(returnType) << " ";
		if(mapsToPointerType(returnType))
		{
		    _M << "*";
		}
		_M << "ret_ = ";
	    }
            else
            {
                _M << nl;
            }
	    string args = getServerArgs(op);
	    _M << "[servant " << opName << args;
	    if(!args.empty())
	    {
	        _M << " current";
	    }
	    _M << ":current];";
	    for(TypeStringList::const_iterator outp = outParams.begin(); outp != outParams.end(); ++outp)
	    {
		writeMarshalUnmarshalCode(_M, outp->first, fixId(outp->second), true, false, false);
	    }
	    if(returnType)
	    {
		writeMarshalUnmarshalCode(_M, returnType, "ret_", true, false, false);
	    }
            if(op->returnsClasses())
            {
                _M << nl << "[os_ writePendingObjects];";
            }
            if(inReferenceParams || !throws.empty())
            {
                _M << eb;
                if(!throws.empty())
                {
                    ExceptionList::const_iterator t;
                    for(t = throws.begin(); t != throws.end(); ++t)
                    {
                        string exS = fixName(*t);
                        _M << nl << "@catch(" << exS << " *ex)";
                        _M << sb;
                        _M << nl << "[os_ writeException:ex];";
                        _M << nl << "return NO;";
                        _M << eb;
                    }
                }
                _M << nl << "@finally";
                _M << sb;
                for(TypeStringList::const_iterator inp = inParams.begin(); inp != inParams.end(); ++inp)
                {
                    if(!isValueType(inp->first))
                    {
                        _M << nl << "[(id<NSObject>)" << fixId(inp->second) << " release];";
                    }
                }
                _M << eb;
            }
	    _M << nl << "return YES;";
        }
        else
        {
	    // TODO: amd
            TypeStringList inParams;
            ParamDeclList paramList = op->parameters();
            for(ParamDeclList::const_iterator pli = paramList.begin(); pli != paramList.end(); ++pli)
            {
                if(!(*pli)->isOutParam())
                {
                    inParams.push_back(make_pair((*pli)->type(), (*pli)->name()));
                }
            }
            
            TypeStringList::const_iterator q;
            _M << nl << "checkMode__(" << sliceModeToIceMode(op->mode()) << ", current__.mode);";
    
            if(!inParams.empty())
            {
                //
                // Unmarshal 'in' parameters.
                //
                _M << nl << "IceInternal.BasicStream is__ = inS__.istr();";
                _M << nl << "is__.startReadEncaps();";
                for(q = inParams.begin(); q != inParams.end(); ++q)
                {
                    string param = fixId(q->second);
                    string typeS = typeToString(q->first);
                    if(!isClass(q->first))
                    {
                        _M << nl << typeS << ' ' << param << ';';
                        StructPtr st = StructPtr::dynamicCast(q->first);
                        if(st)
                        {
                            if(isValueType(q->first))
                            {
                                _M << nl << param << " = new " << typeS << "();";
                            }
                            else
                            {
                                _M << nl << param << " = null;";
                            }
                        }
                    }
                    writeMarshalUnmarshalCode(_M, q->first, fixId(q->second), false, false, true);
                }
                if(op->sendsClasses())
                {
                    _M << nl << "is__.readPendingObjects();";
                }
                _M << nl << "is__.endReadEncaps();";
            }
            else
            {
                _M << nl << "inS__.istr().skipEmptyEncaps();";
            }

            //
            // Call on the servant.
            //
            string classNameAMD = "AMD_" + p->name();
            _M << nl << classNameAMD << '_' << opName << " cb__ = new _" << classNameAMD << '_' << opName
                 << "(inS__);";
            _M << nl << "try";
            _M << sb;
            _M << nl << "obj__.";
            if(amd)
            {
                _M << opName << "_async";
            }
            else
            {
                _M << fixId(opName, ObjC::NSObject, true); // TODO: 2nd param
            }
            _M << spar;
            if(amd)
            {
                _M << "cb__";
            }
            for(q = inParams.begin(); q != inParams.end(); ++q)
            {
                string arg;
                if(isClass(q->first))
                {
                    arg += "(" + typeToString(q->first) + ")";
                }
                arg += fixId(q->second);
                if(isClass(q->first))
                {
                    arg += "_PP.value";
                }
                _M << arg;
            }
            _M << "current__" << epar << ';';
            _M << eb;
            _M << nl << "catch(_System.Exception ex)";
            _M << sb;
            _M << nl << "cb__.ice_exception(ex);";
            _M << eb;
            _M << nl << "return Ice.DispatchStatus.DispatchAsync;";

            _M << eb;
        }
	_M << eb;
    }

    OperationList allOps = p->allOperations();
    if(!allOps.empty())
    {
        map<string, string> allOpNames;
        for(OperationList::const_iterator p = allOps.begin(); p != allOps.end(); ++p)
        {
            allOpNames.insert(make_pair((*p)->name(), fixName(ClassDefPtr::dynamicCast((*p)->container()))));
        }

        allOpNames["ice_id"] = "ICEObject";
        allOpNames["ice_ids"] = "ICEObject";
        allOpNames["ice_isA"] = "ICEObject";
        allOpNames["ice_ping"] = "ICEObject";

        map<string, string>::const_iterator q;

        _M << sp << nl << "static NSString *" << name << "_all__[] =";
        _M << sb;
        q = allOpNames.begin();
        while(q != allOpNames.end())
        {
            _M << nl << "@\"" << q->first << '"';
            if(++q != allOpNames.end())
            {
                _M << ',';
            }
        }
        _M << eb << ';';

        _M << sp << nl << "-(BOOL) dispatch__:(ICECurrent *)current is:(id<ICEInputStream>)is "
	   << "os:(id<ICEOutputStream>)os";
        _M << sb;
	_M << nl << "switch(ICEInternalLookupString(" << name << "_all__, sizeof(" << name
	   << "_all__) / sizeof(NSString*), current.operation))";
	_M << sb;
        int i = 0;
        for(q = allOpNames.begin(); q != allOpNames.end(); ++q)
        {
            _M << nl << "case " << i++ << ':';
	    _M.inc();
	    _M << nl << "return [" << q->second << " " << q->first << "___:(ICEObject<" << q->second
	       << "> *)self current:current is:is os:os];";
            _M.dec();
        }
	_M << nl << "default:";
	_M.inc();
	_M << nl << "@throw [ICEOperationNotExistException operationNotExistException:";
	_M.useCurrentPosAsIndent();
	_M << "__FILE__";
	_M << nl << "line:__LINE__";
	_M << nl << "id_:current.id_";
	_M << nl << "facet:current.facet";
	_M << nl << "operation:current.operation];";
	_M.restoreIndent();
	_M.dec();
        _M << eb;
	_M << eb;
    }

    _M << sp << nl << "-(NSArray*) ice_ids:(ICECurrent*)current";
    _M << sb;
    _M << nl << "int count = sizeof(" << name << "_ids__) / sizeof(NSString *);";
    _M << nl << "return [NSArray arrayWithObjects:" << name << "_ids__ " << "count:count];";
    _M << eb;

    _M << sp << nl << "-(NSString*) ice_id:(ICECurrent*)current";
    _M << sb;
    _M << nl << "return " << name << "_ids__[" << scopedPos << "];";
    _M << eb;

    _M << sp << nl << "+(NSString*) ice_staticId";
    _M << sb;
    _M << nl << "return " << name << "_ids__[" << scopedPos << "];";
    _M << eb;

    _M << sp << nl << "-(BOOL) ice_isA:(NSString*)typeId current:(ICECurrent*)current";
    _M << sb;
    _M << nl << "int count = sizeof(" << name << "_ids__) / sizeof(NSString *);";
    _M << nl << "return ICEInternalLookupString(" << name << "_ids__, count, typeId) >= 0;";
    _M << eb;
}

string
Slice::ObjCVisitor::getSelector(const OperationPtr& op) const
{
    string result;
    ParamDeclList paramList = op->parameters();
    for(ParamDeclList::const_iterator q = paramList.begin(); q != paramList.end(); ++q)
    {
	if(q == paramList.begin())
        {
            result += fixId(op->name()) + ":";
        }
        else
        {
	    result += fixId((*q)->name()) + ":";
	}
    }
    return result;
}

string
Slice::ObjCVisitor::getParams(const OperationPtr& op) const
{
    string result;
    ParamDeclList paramList = op->parameters();
    for(ParamDeclList::const_iterator q = paramList.begin(); q != paramList.end(); ++q)
    {
	TypePtr type = (*q)->type();
	string typeString = (*q)->isOutParam() ? outTypeToString(type) : typeToString(type);
        string name = fixId((*q)->name());

	if(q != paramList.begin())
	{
	    result += " " + name;
	}
	result += ":(" + typeString;
	if(mapsToPointerType(type))
	{
	    result += " *";
            if((*q)->isOutParam())
            {
                result += "*";
            }
	}
        else if((*q)->isOutParam())
	{
	    result += " *";
	}
	result += ")" + name;
    }
    return result;
}

string
Slice::ObjCVisitor::getServerParams(const OperationPtr& op) const
{
    string result;
    ParamDeclList paramList = op->parameters();
    for(ParamDeclList::const_iterator q = paramList.begin(); q != paramList.end(); ++q)
    {
	TypePtr type = (*q)->type();
	string typeString = (*q)->isOutParam() ? typeToString(type) : outTypeToString(type);
        string name = fixId((*q)->name());

	if(q != paramList.begin())
	{
	    result += " " + name;
	}
	result += ":(" + typeString;
	if(mapsToPointerType(type))
	{
	    result += " *";
            if((*q)->isOutParam())
            {
                result += "*";
            }
	}
        else if((*q)->isOutParam())
	{
	    result += " *";
	}
	result += ")" + name;
    }
    return result;
}

string
Slice::ObjCVisitor::getParamsAsync(const OperationPtr& op, bool sent)
{
    ParamDeclList paramList = op->parameters();
    string result = ":(id)target_ response:(SEL)response_ exception:(SEL)exception_";
    if(sent)
    {
        result += " sent:(SEL)sent_";
    }
    for(ParamDeclList::const_iterator q = paramList.begin(); q != paramList.end(); ++q)
    {
        if(!(*q)->isOutParam())
        {
            TypePtr type = (*q)->type();
            string name = fixId((*q)->name());
            result += " " + name + ":(" + typeToString(type);
            if(mapsToPointerType(type))
            {
                result += " *";
            }
            result += ")" + name;
        }
    }
    return result;
}

string
Slice::ObjCVisitor::getParamsAsyncCB(const OperationPtr& op)
{
    string result = ":(id)target_ response:(SEL)response_ exception:(SEL)exception_";
    
    TypePtr ret = op->returnType();
    if(ret)
    {
        result += " ret:(" + typeToString(ret);
        if(mapsToPointerType(ret))
        {
            result += " *";
        }
        result += ")ret_";
    }

    ParamDeclList paramList = op->parameters();
    for(ParamDeclList::const_iterator q = paramList.begin(); q != paramList.end(); ++q)
    {
        if((*q)->isOutParam())
        {
            TypePtr type = (*q)->type();
            string name = fixId((*q)->name());
            result += " " + name + ":(" + typeToString(type);
            if(mapsToPointerType(type))
            {
                result += " *";
            }
            result += ")" + name;
        }
    }

    return result;
}

string
Slice::ObjCVisitor::getArgs(const OperationPtr& op) const
{
    string result;
    ParamDeclList paramList = op->parameters();
    for(ParamDeclList::const_iterator q = paramList.begin(); q != paramList.end(); ++q)
    {
        string name = fixId((*q)->name());

	if(q != paramList.begin())
	{
	    result += " " + name;
	}
	result += ":" + name;
    }
    return result;
}

string
Slice::ObjCVisitor::getServerArgs(const OperationPtr& op) const
{
    string result;
    ParamDeclList paramList = op->parameters();
    for(ParamDeclList::const_iterator q = paramList.begin(); q != paramList.end(); ++q)
    {
        string name = fixId((*q)->name());

	if(q != paramList.begin())
	{
	    result += " " + name;
	}
	result += ":";
	if((*q)->isOutParam())
	{
	    result += "&";
	}
	result += name;
    }
    return result;
}

string
Slice::ObjCVisitor::getArgsAsync(const OperationPtr& op, bool sent)
{
    string result;

    result += ":target_ response:response_ exception:exception_ ";
    if(sent)
    {
        result += "sent:sent_ ";
    }
    else
    {
        result += "sent:nil ";
    }

    ParamDeclList paramList = op->parameters();
    for(ParamDeclList::const_iterator q = paramList.begin(); q != paramList.end(); ++q)
    {
        if(!(*q)->isOutParam())
        {
            string name = fixId((*q)->name());
            result += " " + name + ":" +name;
        }
    }
    return result;
}

string
Slice::ObjCVisitor::getArgsAsyncCB(const OperationPtr& op)
{
    string result;

    TypePtr ret = op->returnType();
    if(ret)
    {
        result += "ret_";
    }

    ParamDeclList paramList = op->parameters();
    bool first = !ret;
    for(ParamDeclList::const_iterator q = paramList.begin(); q != paramList.end(); ++q)
    {
        if((*q)->isOutParam())
        {
            string name = fixId((*q)->name());
            
            if(first)
            {
                first = false;
            }
            else
            {
                result += ", ";
            }
            result += name;
        }
    }

    return result;
}

string
Slice::ObjCVisitor::getSigAsyncCB(const OperationPtr& op)
{
    string result = "void(*)(id, SEL";

    TypePtr ret = op->returnType();
    if(ret)
    {
        result += ", " + typeToString(ret);
        if(mapsToPointerType(ret))
        {
            result += " *";
        }
    }

    ParamDeclList paramList = op->parameters();
    for(ParamDeclList::const_iterator q = paramList.begin(); q != paramList.end(); ++q)
    {
        if((*q)->isOutParam())
        {
            TypePtr type = (*q)->type();
            result += ", " + typeToString(type);
            if(mapsToPointerType(type))
            {
                result += " *";
            }
        }
    }
    result += ")";
    return result;
}

void
Slice::ObjCVisitor::emitAttributes(const ContainedPtr& p)
{
    StringList metaData = p->getMetaData();
    for(StringList::const_iterator i = metaData.begin(); i != metaData.end(); ++i)
    {
        static const string prefix = "cs:attribute:";
        if(i->find(prefix) == 0)
        {
            _M << nl << '[' << i->substr(prefix.size()) << ']';
        }
    }
}

string
Slice::ObjCVisitor::writeValue(const TypePtr& type)
{
    assert(type);

    BuiltinPtr builtin = BuiltinPtr::dynamicCast(type);
    if(builtin)
    {
        switch(builtin->kind())
        {
            case Builtin::KindBool:
            {
                return "false";
                break;
            }
            case Builtin::KindByte:
            case Builtin::KindShort:
            case Builtin::KindInt:
            case Builtin::KindLong:
            {
                return "0";
                break;
            }
            case Builtin::KindFloat:
            {
                return "0.0f";
                break;
            }
            case Builtin::KindDouble:
            {
                return "0.0";
                break;
            }
            default:
            {
                return "null";
                break;
            }
        }
    }

    EnumPtr en = EnumPtr::dynamicCast(type);
    if(en)
    {
        return fixId(en->scoped()) + "." + fixId((*en->getEnumerators().begin())->name());
    }

    StructPtr st = StructPtr::dynamicCast(type);
    if(st)
    {
        return st->hasMetaData("clr:class") ? string("null") : "new " + fixId(st->scoped()) + "()";
    }

    return "null";
}


Slice::Gen::Gen(const string& name, const string& base, const string& include, const vector<string>& includePaths,
                const string& dir, bool impl, bool implTie, bool stream)
    : _base(base),
      _include(include),
      _includePaths(includePaths),
      _impl(impl),
      _stream(stream)
{
    for(vector<string>::iterator p = _includePaths.begin(); p != _includePaths.end(); ++p)
    {
        *p = fullPath(*p);
    }

    string::size_type pos = _base.find_last_of("/\\");
    if(pos != string::npos)
    {
        _base.erase(0, pos + 1);
    }

    string fileH = _base + ".h";
    string fileM = _base + ".m";
    string fileImplH = _base + "I.h";
    string fileImplM = _base + "I.m";

    if(!dir.empty())
    {
        fileH = dir + '/' + fileH;
        fileM = dir + '/' + fileM;
        fileImplH = dir + '/' + fileImplH;
        fileImplM = dir + '/' + fileImplM;
    }

    if(impl || implTie)
    {
        struct stat st;
        if(stat(fileImplH.c_str(), &st) == 0)
        {
            ostringstream os;
            os << fileImplH << "' already exists - will not overwrite";
            throw FileException(__FILE__, __LINE__, os.str());
        }

        if(stat(fileImplM.c_str(), &st) == 0)
        {
            ostringstream os;
            os << fileImplM << "' already exists - will not overwrite";
            throw FileException(__FILE__, __LINE__, os.str());
        }

        _implH.open(fileImplH.c_str());
        if(!_implH)
        {
            ostringstream os;
            os << "cannot open `" << fileImplH << "': " << strerror(errno);
            throw FileException(__FILE__, __LINE__, os.str());
        }
        FileTracker::instance()->addFile(fileImplH);

        _implM.open(fileImplM.c_str());
        if(!_implM)
        {
            ostringstream os;
            os << "cannot open `" << fileImplM << "': " << strerror(errno);
            throw FileException(__FILE__, __LINE__, os.str());
        }
        FileTracker::instance()->addFile(fileImplM);
    }

    _H.open(fileH.c_str());
    if(!_H)
    {
        ostringstream os;
        os << "cannot open `" << fileH << "': " << strerror(errno);
        throw FileException(__FILE__, __LINE__, os.str());
    }
    FileTracker::instance()->addFile(fileH);
    printHeader(_H);
    _H << nl << "// Generated from file `" << _base << ".ice'";

    _H << sp << nl << "#import <Ice/Config.h>";
    _H << nl << "#import <Ice/Proxy.h>";
    _H << nl << "#import <Ice/Object.h>";
    _H << nl << "#import <Ice/Current.h>";
    _H << nl << "#import <Ice/Exception.h>";
    _H << nl << "#import <Ice/Stream.h>";

    _M.open(fileM.c_str());
    if(!_M)
    {
        ostringstream os;
        os << "cannot open `" << fileM << "': " << strerror(errno);
        throw FileException(__FILE__, __LINE__, os.str());
    }
    FileTracker::instance()->addFile(fileM);
    printHeader(_M);
    _M << nl << "// Generated from file `" << _base << ".ice'";
}

Slice::Gen::~Gen()
{
    if(_H.isOpen())
    {
        _H << '\n';
        _M << '\n';
    }
    if(_implH.isOpen())
    {
        _implH << '\n';
        _implM << '\n';
    }
}

bool
Slice::Gen::operator!() const
{
    if(!_H || !_M)
    {
        return true;
    }
    if(_impl && (!_implH || !_implM))
    {
        return true;
    }
    return false;
}

void
Slice::Gen::generate(const UnitPtr& p)
{
    ObjCGenerator::validateMetaData(p);

    _M << nl << "\n#import <Ice/LocalException.h>";
    _M << nl << "#import <Ice/Stream.h>";
    _M << nl << "#import <Ice/Internal.h>";

    _M << nl << "#import <";
    if(!_include.empty())
    {
        _M << _include << "/";
    }
    _M << _base << ".h>";

    _M << nl;
    if(p->hasContentsWithMetaData("ami"))
    {
        _M << "\n#import <objc/message.h>"; // For objc_msgSend.
    }

    StringList includes = p->includeFiles();
    for(StringList::const_iterator q = includes.begin(); q != includes.end(); ++q)
    {
        _H << "\n#import <" << changeInclude(*q, _includePaths) << ".h>";
    }

    UnitVisitor unitVisitor(_H, _M, _stream);
    p->visit(&unitVisitor, false);

    ObjectDeclVisitor objectDeclVisitor(_H, _M);
    p->visit(&objectDeclVisitor, false);

    ProxyDeclVisitor proxyDeclVisitor(_H, _M);
    p->visit(&proxyDeclVisitor, false);

    TypesVisitor typesVisitor(_H, _M, _stream);
    p->visit(&typesVisitor, false);

    ProxyVisitor proxyVisitor(_H, _M);
    p->visit(&proxyVisitor, false);

    DelegateMVisitor delegateMVisitor(_H, _M);
    p->visit(&delegateMVisitor, false);

    HelperVisitor HelperVisitor(_H, _M, _stream);
    p->visit(&HelperVisitor, false);
}

void
Slice::Gen::generateTie(const UnitPtr& p)
{
    //TieVisitor tieVisitor(_M);
    //p->visit(&tieVisitor, false);
}

void
Slice::Gen::generateImpl(const UnitPtr& p)
{
    //ImplVisitor implVisitor(_impl);
    //p->visit(&implVisitor, false);
}

void
Slice::Gen::generateImplTie(const UnitPtr& p)
{
    //ImplTieVisitor implTieVisitor(_impl);
    //p->visit(&implTieVisitor, false);
}

#if 0
void
Slice::Gen::generateChecksums(const UnitPtr& u)
{
    ChecksumMap map = createChecksums(u);
    if(!map.empty())
    {
        string className = "X" + IceUtil::generateUUID();
        for(string::size_type pos = 1; pos < className.size(); ++pos)
        {
            if(!isalnum(className[pos]))
            {
                className[pos] = '_';
            }
        }

        _M << sp << nl << "namespace IceInternal";
        _M << sb;
        _M << nl << "namespace SliceChecksums";
        _M << sb;
        _M << nl << "public sealed class " << className;
        _M << sb;
        _M << nl << "public readonly static System.Collections.Hashtable map = new System.Collections.Hashtable();";
        _M << sp << nl << "static " << className << "()";
        _M << sb;
        for(ChecksumMap::const_iterator p = map.begin(); p != map.end(); ++p)
        {
            _M << nl << "map.Add(\"" << p->first << "\", \"";
            ostringstream str;
            str.flags(ios_base::hex);
            str.fill('0');
            for(vector<unsigned char>::const_iterator q = p->second.begin(); q != p->second.end(); ++q)
            {
                str << (int)(*q);
            }
            _M << str.str() << "\");";
        }
        _M << eb;
        _M << eb << ';';
        _M << eb;
        _M << eb;
    }
}
#endif

void
Slice::Gen::closeOutput()
{
    _H.close();
    _M.close();
    _implH.close();
    _implM.close();
}

void
Slice::Gen::printHeader(Output& o)
{
    static const char* header =
"// **********************************************************************\n"
"//\n"
"// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.\n"
"//\n"
"// This copy of Ice is licensed to you under the terms described in the\n"
"// ICE_TOUCH_LICENSE file included in this distribution.\n"
"//\n"
"// **********************************************************************\n"
        ;

    o << header;
    o << "\n// Ice version " << ICE_STRING_VERSION;
}

Slice::Gen::UnitVisitor::UnitVisitor(Output& H, Output& M, bool stream)
    : ObjCVisitor(H, M), _stream(stream), _globalMetaDataDone(false)
{
}

bool
Slice::Gen::UnitVisitor::visitModuleStart(const ModulePtr& p)
{
#if 0
    if(!_globalMetaDataDone)
    {
        DefinitionContextPtr dc = p->definitionContext();
        StringList globalMetaData = dc->getMetaData();

        static const string attributePrefix = "cs:attribute:";

        if(!globalMetaData.empty())
        {
            _M << sp;
        }
        for(StringList::const_iterator q = globalMetaData.begin(); q != globalMetaData.end(); ++q)
        {
            string::size_type pos = q->find(attributePrefix);
            if(pos == 0)
            {
                string attrib = q->substr(pos + attributePrefix.size());
                _M << nl << '[' << attrib << ']';
            }
        }
        _globalMetaDataDone = true; // Do this only once per source file.
    }
#endif
    string dummy;
    if(p->findMetaData("objc:prefix", dummy))
    {
        _prefixes.push_back(modulePrefix(p));
    }
    return true;
}

void
Slice::Gen::UnitVisitor::visitUnitEnd(const UnitPtr& unit)
{
    string uuid = IceUtil::generateUUID();
    for(string::size_type pos = 0; pos < uuid.size(); ++pos)
    {
        if(!isalnum(uuid[pos]))
        {
            uuid[pos] = '_';
        }
    }

    if(!_prefixes.empty())
    {
        _M << sp << nl << "@implementation ICEInternalPrefixTable(C" << uuid << ")";
        _M << nl << "-(void)addPrefixes_C" << uuid << ":(NSMutableDictionary*)prefixTable";
        _M << sb;
        for(vector<Slice::ObjCGenerator::ModulePrefix>::const_iterator p = _prefixes.begin(); p != _prefixes.end(); ++p)
        {
            _M << nl << "[prefixTable setObject:@\"" << p->name << "\" forKey:@\"" << p->m->scoped() << "\"];";
        }
        _M << eb;
        _M << nl << "@end";
    }
}

Slice::Gen::ObjectDeclVisitor::ObjectDeclVisitor(Output& H, Output& M)
    : ObjCVisitor(H, M)
{
}

void
Slice::Gen::ObjectDeclVisitor::visitClassDecl(const ClassDeclPtr& p)
{
    _H << sp << nl << "@class " << fixName(p) << ";";
    _H << nl << "@protocol " << fixName(p) << ";";
}

Slice::Gen::ProxyDeclVisitor::ProxyDeclVisitor(Output& H, Output& M)
    : ObjCVisitor(H, M)
{
}

void
Slice::Gen::ProxyDeclVisitor::visitClassDecl(const ClassDeclPtr& p)
{
    _H << sp << nl << "@class " << fixName(p) << "Prx;";
    _H << nl << "@protocol " << fixName(p) << "Prx;";
}

Slice::Gen::TypesVisitor::TypesVisitor(Output& H, Output& M, bool stream)
    : ObjCVisitor(H, M), _stream(stream)
{
}

bool
Slice::Gen::TypesVisitor::visitModuleStart(const ModulePtr& p)
{
    string suffix;
    StringList names = splitScopedName(p->scoped());
    for(StringList::const_iterator i = names.begin(); i != names.end(); ++i)
    {
	if(i != names.begin())
	{
	    suffix += "_";
	}
        suffix += *i;
    }
    string symbol = "ICE_MODULE_PREFIX_";
    symbol += suffix;
    // TODO: generate checksum so we can catch mismatched module prefixes across different compilation units
    // that are compiled by separate invocations of slice2objc
    #if 0
    _H << nl << nl << "#if defined(" << symbol << ") && (" << symbol << " != " << moduleName(p) << ")";
    _H << nl << "#error inconsistent prefix metadata for Slice module " << p->scoped();
    _H << nl << "#else";
    _H << nl << "#define " << symbol << " " << moduleName(p);
    _H << nl << "#endif";
    #endif
    return true;
}

void
Slice::Gen::TypesVisitor::visitModuleEnd(const ModulePtr&)
{
}

bool
Slice::Gen::TypesVisitor::visitClassDefStart(const ClassDefPtr& p)
{
    string name = fixName(p);
    ClassList bases = p->bases();

    // TODO :if(_stream)

    _H << sp << nl << "@protocol " << name;
    if(bases.empty())
    {
        _H << " <ICEObject>";
    }
    else
    {
        _H << " <";
        for(ClassList::const_iterator i = bases.begin(); i != bases.end(); ++i)
        {
            string baseName = fixName(*i);
            if(i != bases.begin())
            {
                _H << ", ";
            }
            _H << baseName;
        }
        _H << ">";
    }

    _M << sp << nl << "@implementation " << name;

    return true;
}

void
Slice::Gen::TypesVisitor::visitClassDefEnd(const ClassDefPtr& p)
{
    string name = fixName(p);
    ClassList bases = p->bases();
    bool hasBaseClass = !bases.empty() && !bases.front()->isInterface();
    string baseName = hasBaseClass ? fixName(bases.front()) : "ICEObject";
    DataMemberList baseDataMembers;
    if(hasBaseClass)
    {
        baseDataMembers = bases.front()->allDataMembers();
    }
    DataMemberList dataMembers = p->dataMembers();
    DataMemberList allDataMembers = p->allDataMembers();
    DataMemberList::const_iterator q;

    _H << nl << "@end";

    _H << sp << nl << "@interface " << name << " : " << baseName;

    if(!dataMembers.empty())
    {
	//
	// Data member declarations.
	//
        _H << sb;
	writeMembers(dataMembers, 0); // TODO fix second paramater
	_H << eb;
	_H << sp;

	_M << sp;
    }

    //
    // @property and @synthesize for each data member.
    //
    writeProperties(dataMembers, 0); // TODO fix second parameter
    writeSynthesize(dataMembers, 0); // TODO fix second parameter

    //
    // Constructor.
    //
    if(!dataMembers.empty())
    {
        _H << sp;
    }
    if(!p->isInterface() && !dataMembers.empty())
    {
	_H << nl << "-(id) init";
	_M << sp << nl << "-(id) init";
	writeMemberSignature(allDataMembers, 0, Other); // TODO fix second parameter
	_H << ";";
	_M << sb;
	_M << nl << "if(![super init";
	writeMemberCall(baseDataMembers, 0, Other, WithEscape); // TODO
	_M << "])";
	_M << sb;
	_M << nl << "return nil;";
	_M << eb;
	writeMemberInit(dataMembers, 0); // TODO
	_M << nl << "return self;";
	_M << eb;
    }

    //
    // Convenience constructors.
    //
    if(!allDataMembers.empty())
    {
	string lowerCaseName = fixId(p->name());
	*(lowerCaseName.begin()) = tolower(*lowerCaseName.begin());

	_H << nl << "+(id) " << lowerCaseName;
	_M << sp << nl << "+(id) " << lowerCaseName;
	writeMemberSignature(allDataMembers, 0, Other); // TODO fix second parameter
	_H << ";";
	_M << sb;

	//
	// The cast avoids a compiler warning that is emitted if different structs
	// have members with the same name but different types.
	//
	_M << nl << name << " *s__ = [(" << name << " *)[" << name << " alloc] init";
	writeMemberCall(allDataMembers, 0, Other, WithEscape); // TODO
	_M << "];";
	_M << nl << "[s__ autorelease];";
	_M << nl << "return s__;";
	_M << eb;

	_H << nl << "+(id) " << lowerCaseName << ";";
	_M << sp << nl << "+(id) " << lowerCaseName;
	_M << sb;
	_M << nl << name << " *s__ = [[" << name << " alloc] init];";
	_M << nl << "[s__ autorelease];";
	_M << nl << "return s__;";
	_M << eb;
    }

    if(!p->isInterface())
    {
	//
	//  copyWithZone and dealloc
	//
	if(!dataMembers.empty())
	{
	    _M << sp << nl << "-(id) copyWithZone:(NSZone *)zone_p";
	    _M << sb;
	    _M << nl << "return [(" << name << " *)[[self class] allocWithZone:zone_p] init";
	    writeMemberCall(allDataMembers, 0, Other, NoEscape); // TODO
	    _M << "];";
	    _M << eb;
	    _H << nl << "// This class also overrides copyWithZone:.";

	    writeMemberDealloc(dataMembers, 0); // TODO fix second parameter
	}
    }

    //
    // Operations
    //
    OperationList ops = p->operations();
    OperationList::const_iterator r;
    for(r = ops.begin(); r != ops.end(); ++r)
    {
        OperationPtr op = *r;
        _H << nl << "+(BOOL)" << fixId(op->name()) << "___:(ICEObject<" << name
	   << "> *)servant current:(ICECurrent *)current " 
           << "is:(id<ICEInputStream>)is_ os:(id<ICEOutputStream>)os_;";
    }

    //
    // Marshaling/unmarshaling
    //

    _M << sp << nl << "-(void) write__:(id<ICEOutputStream>)os_";
    _M << sb;
    _M << nl << "[os_ writeTypeId:@\"" << p->scoped() << "\"];";
    _M << nl << "[os_ startSlice];";
    writeMemberMarshal("self->", dataMembers, 0); // TODO fix second parameter
    _M << nl << "[os_ endSlice];";
    _M << nl << "[super write__:os_];";
    _M << eb;

    _H << nl << "@end";

    _M << sp << nl << "-(void) read__:(id<ICEInputStream>)is_ readTypeId:(BOOL)rid_";
    _M << sb;
    _M << nl << "if(rid_)";
    _M << sb;
    _M << nl << "[[is_ readTypeId] release];";
    _M << eb;
    _M << nl << "[is_ startSlice];";
    writeMemberUnmarshal("self->", dataMembers, 0); // TODO fix second parameter
    _M << nl << "[is_ endSlice];";
    _M << nl << "[super read__:is_ readTypeId:YES];";
    _M << eb;

    writeDispatchAndMarshalling(p, _stream);

    _M << nl << "@end";
}

void
Slice::Gen::TypesVisitor::visitOperation(const OperationPtr& p)
{
    // TODO deal with deprecate metadata.

    string name = fixId(p->name());
    TypePtr returnType = p->returnType();
    string retString = typeToString(returnType);
    string params = getServerParams(p);

    _H << nl << "-(" << retString;
    if(mapsToPointerType(returnType))
    {
        _H << " *";
    }
    _H << ") " << name << params;
    if(!params.empty())
    {
        _H << " current";
    }
    _H << ":(ICECurrent *)current;";

    // TODO: deal with AMI
}

void
Slice::Gen::TypesVisitor::visitSequence(const SequencePtr& p)
{
    string prefix = moduleName(findModule(p));
    string name = fixName(p);

    emitDeprecate(p, 0, _M, "type");

    if(isValueType(p->type()))
    {
	_H << sp << nl << "typedef NSData " << name << ";";
	_H << nl << "typedef NSMutableData " << prefix << "Mutable" << p->name() << ";";
    }
    else
    {
	_H << sp << nl << "typedef NSArray " << name << ";";
	_H << nl << "typedef NSMutableArray " << prefix << "Mutable" << p->name() << ";";
    }
}

bool
Slice::Gen::TypesVisitor::visitExceptionStart(const ExceptionPtr& p)
{
    string name = fixName(p);
    ExceptionPtr base = p->base();
    DataMemberList dataMembers = p->dataMembers();

    _H << sp;

    emitDeprecate(p, 0, _M, "type");

    _H << nl << "@interface " << name << " : ";
    if(base)
    {
        _H << fixName(base);
    }
    else
    {
        _H << (p->isLocal() ? "ICELocalException" : "ICEUserException");
    }
    if(!dataMembers.empty())
    {
	_H << sb;
    }

    _M << sp << nl << "@implementation " << name;

    return true;
}

void
Slice::Gen::TypesVisitor::visitExceptionEnd(const ExceptionPtr& p)
{
    string name = fixName(p);

    string lowerCaseName = fixId(p->name());
    *(lowerCaseName.begin()) = tolower(*lowerCaseName.begin());

    DataMemberList dataMembers = p->dataMembers();
    DataMemberList allDataMembers = p->allDataMembers();
    DataMemberList::const_iterator q;
    
    DataMemberList baseDataMembers;
    if(p->base())
    {
        baseDataMembers = p->base()->allDataMembers();
    }

    const int baseTypes = p->isLocal() ? ObjC::NSLocalException : ObjC::NSUserException;

    // TODO: deprecate metadata

    if(!dataMembers.empty())
    {
	//
	// Data member declarations.
	//
	writeMembers(dataMembers, baseTypes);

	_H << eb;
	_H << sp;
	_M << sp;

	//
	// @property and @synthesize for each data member.
	//
	writeProperties(dataMembers, baseTypes);
	writeSynthesize(dataMembers, baseTypes);
	_H << sp;
    }

    //
    // ice_name
    //
    _H << nl << "-(NSString *) ice_name;";
    _M << sp << nl << "-(NSString *) ice_name";
    _M << sb;
    _M << nl << "return @\"" << p->scoped().substr(2) << "\";";
    _M << eb;

    //
    // Constructors.
    //
    if(p->isLocal() && !dataMembers.empty())
    {
	_H << nl << "-(id) init:(const char*)file__p line:(int)line__p;";
	_M << sp << nl << "-(id) init:(const char*)file__p line:(int)line__p";
	_M << sb;
	_M << nl << "if(![super init:file__p line:line__p])";
	_M << sb;
	_M << nl << "return nil;";
	_M << eb;
	_M << nl << "return self;";
	_M << eb;
    }
    if(!dataMembers.empty())
    {
	_H << nl << "-(id) init";
	_M << sp << nl << "-(id) init";
	writeMemberSignature(allDataMembers, baseTypes, p->isLocal() ? LocalException : Other);
	_H << ";";
	_M << sb;
	if(!p->base())
	{
	    _M << nl << "if(![super initWithName:[self ice_name] reason:nil userInfo:nil])";
	}
	else
	{
	    _M << nl << "if(![super init";
	    if(p->isLocal())
	    {
	        _M << ":file__p line:line__p ";
	    }
	    writeMemberCall(baseDataMembers, baseTypes, p->isLocal() ? LocalException : Other, WithEscape);
	    _M << "])";
	}
	_M << sb;
	_M << nl << "return nil;";
	_M << eb;
	if(!dataMembers.empty())
	{
	    writeMemberInit(dataMembers, baseTypes);
	}
	_M << nl << "return self;";
	_M << eb;
    }

    //
    // Convenience constructors.
    //
    _H << nl << "+(id) " << lowerCaseName;
    _M << sp << nl << "+(id) " << lowerCaseName;
    writeMemberSignature(allDataMembers, baseTypes, p->isLocal() ? LocalException : Other);
    _H << ";";

    //
    // The cast avoids a compiler warning that is emitted if different exceptions
    // have members with the same name but different types.
    //
    _M << sb;
    _M << nl << name << " *s__ = [(" << name << " *)[" << name << " alloc] init";
    if(p->isLocal())
    {
        _M << ":file__p line:line__p";
    }
    writeMemberCall(allDataMembers, baseTypes, p->isLocal() ? LocalException : Other, WithEscape);
    _M << "];";
    _M << nl << "[s__ autorelease];";
    _M << nl << "return s__;";
    _M << eb;

    if(!allDataMembers.empty())
    {
	_H << nl << "+(id) " << lowerCaseName;
	_M << sp << nl << "+(id) " << lowerCaseName;
	if(p->isLocal())
	{
	    _H << ":(const char*)file__p line:(int)line__p";
	    _M << ":(const char*)file__p line:(int)line__p";
	}
	_H << ";";
	_M << sb;
	_M << nl << name << " *s__ = [[" << name << " alloc] init";
	if(p->isLocal())
	{
	    _M << ":file__p line:line__p";
	}
	_M << "];";
	_M << nl << "[s__ autorelease];";
	_M << nl << "return s__;";
	_M << eb;
    }

    //
    // copyWithZone and dealloc
    //
    if(!dataMembers.empty())
    {
	_M << sp << nl << "-(id) copyWithZone:(NSZone *)zone_p";
	_M << sb;
	_M << nl << "return [(" << name << " *)[[self class] allocWithZone:zone_p] init";
	if(p->isLocal())
	{
	    _M << ":file line:line";
	}
	writeMemberCall(allDataMembers, baseTypes, p->isLocal() ? LocalException : Other, NoEscape);
	_M << "];";
	_M << eb;
	_H << nl << "// This class also overrides copyWithZone:.";

	writeMemberDealloc(dataMembers, baseTypes);
    }

    //
    // Marshaling/unmarshaling
    //
    ExceptionPtr base = p->base();
    if(!p->allClassDataMembers().empty())
    {
	if(!base || (base && !base->usesClasses()))
	{
	    _M << sp << nl << "-(BOOL) usesClasses__";
	    _M << sb;
	    _M << nl << "return YES;";
	    _M << eb;
	}
    }

    if(!p->isLocal())
    {
	_M << sp << nl << "-(void) write__:(id<ICEOutputStream>)os_";
	_M << sb;
	_M << nl << "[os_ writeString:@\"" << p->scoped() << "\"];";
	_M << nl << "[os_ startSlice];";
	writeMemberMarshal("self->", dataMembers, baseTypes);
	_M << nl << "[os_ endSlice];";
	if(base)
	{
	    _M << nl << "[super write__:os_];";
	}
	_M << eb;

	_M << sp << nl << "-(void) read__:(id<ICEInputStream>)is_ readTypeId:(BOOL)rid_";
	_M << sb;
	_M << nl << "if(rid_)";
	_M << sb;
	_M << nl << "[[is_ readString] release];";
	_M << eb;
	_M << nl << "[is_ startSlice];";
	writeMemberUnmarshal("self->", dataMembers, baseTypes);
	_M << nl << "[is_ endSlice];";
	if(base)
	{
	    _M << nl << "[super read__:is_ readTypeId:YES];";
	}
	_M << eb;
    }

    _H << nl << "@end";
    _M << nl << "@end";
}

bool
Slice::Gen::TypesVisitor::visitStructStart(const StructPtr& p)
{
    string name = fixName(p);

    // TODO: if(_stream)

    _H << sp;

    emitDeprecate(p, 0, _M, "type");

    _H << nl << "@interface " << name << " : NSObject <NSCopying>";
    _H << sb;
    _H << nl << "@private";
    _H.inc();

    _M << sp << nl << "@implementation " << name << sp;

    return true;
}

void
Slice::Gen::TypesVisitor::visitStructEnd(const StructPtr& p)
{
    string name = fixName(p);
    DataMemberList dataMembers = p->dataMembers();
    DataMemberList::const_iterator q;
    const int baseTypes = ObjC::NSObject | ObjC::NSCopying;

    // TODO: deprecate metadata

    //
    // Data member declarations.
    //
    writeMembers(dataMembers, 0); // TODO fix second paramater

    _H.dec();
    _H << eb;

    _H << sp;

    //
    // @property and @synthesize for each data member.
    //
    writeProperties(dataMembers, baseTypes);
    writeSynthesize(dataMembers, baseTypes);

    //
    // Constructor.
    //
    _H << sp << nl << "-(id) init";
    _M << sp << nl << "-(id) init";
    writeMemberSignature(dataMembers, baseTypes, Other);
    _H << ";";
    _M << sb;
    _M << nl << "if(![super init])";
    _M << sb;
    _M << nl << "return nil;";
    _M << eb;
    writeMemberInit(dataMembers, baseTypes);
    _M << nl << "return self;";
    _M << eb;

    //
    // Convenience constructor.
    //
    string lowerCaseName = fixId(p->name());
    *(lowerCaseName.begin()) = tolower(*lowerCaseName.begin());

    _H << nl << "+(id) " << lowerCaseName;
    _M << sp << nl << "+(id) " << lowerCaseName;
    writeMemberSignature(dataMembers, baseTypes, Other);
    _H << ";";
    _M << sb;

    //
    // The cast avoids a compiler warning that is emitted if different structs
    // have members with the same name but different types.
    //
    _M << nl << name << " *s__ = [(" << name << "* )[" << name << " alloc] init";
    writeMemberCall(dataMembers, baseTypes, Other, WithEscape);
    _M << "];";
    _M << nl << "[s__ autorelease];";
    _M << nl << "return s__;";
    _M << eb;

    _H << nl << "+(id) " << lowerCaseName << ";";
    _M << sp << nl << "+(id) " << lowerCaseName;
    _M << sb;
    _M << nl << name << " *s__ = [[" << name << " alloc] init];";
    _M << nl << "[s__ autorelease];";
    _M << nl << "return s__;";
    _M << eb;

    //
    // copyWithZone
    //
    _M << sp << nl << "-(id) copyWithZone:(NSZone *)zone_p";
    _M << sb;
    _M << nl << "return [(" << name << " *)[[self class] allocWithZone:zone_p] init";
    writeMemberCall(dataMembers, baseTypes, Other, NoEscape);
    _M << "];";
    _M << eb;

    //
    // hash
    //
    writeMemberHashCode(dataMembers, baseTypes);

    //
    // isEqual
    //
    _M << sp << nl << "-(BOOL) isEqual:(id)o_";
    _M << sb;
    _M << nl << "if(self == o_)";
    _M << sb;
    _M << nl << "return YES;";
    _M << eb;
    _M << nl << "if(!o_ || ![o_ isKindOfClass:[self class]])";
    _M << sb;
    _M << nl << "return NO;";
    _M << eb;
    writeMemberEquals(dataMembers, baseTypes);
    _M << eb;

    _H << nl << "// This class also overrides copyWithZone:, hash, and isEqual:";

    //
    // dealloc
    //
    writeMemberDealloc(dataMembers, 0); // TODO fix second parameter

    //
    // Marshaling/unmarshaling
    //
    _H << nl << "+(void) ice_writeWithStream:(id)obj stream:(id<ICEOutputStream>)stream;";
    _M << sp << nl << "+(void) ice_writeWithStream:(id)obj stream:(id<ICEOutputStream>)os_";
    _M << sb;
    _M << nl << name << "*" << " p = (" << name << "*)obj;";
    _M << nl << "if(p == nil)";
    _M << sb;
    _M << nl << "p = [[[self class] alloc] init];";
    _M << eb;
    _M << nl << "@try";
    _M << sb;
    writeMemberMarshal("p->", dataMembers, 0); // TODO fix second parameter
    _M << eb;
    _M << nl << "@finally";
    _M << sb;
    _M << nl << "if(obj == nil)";
    _M << sb;
    _M << nl << "[p release];";
    _M << eb;
    _M << eb;
    _M << eb;

    _H << nl << "+(id) ice_readWithStream:(id<ICEInputStream>)stream;";
    _M << sp << nl << "+(id) ice_readWithStream:(id<ICEInputStream>)is_";
    _M << sb;
    _M << nl << name << "*" << " p = [[[self class] alloc] init];";
    _M << nl << "@try";
    _M << sb;
    writeMemberUnmarshal("p->", dataMembers, 0); // TODO fix second parameter
    _M << eb;
    _M << nl << "@catch(NSException *ex)";
    _M << sb;
    _M << nl << "[p release];";
    _M << nl << "@throw ex;";
    _M << eb;
    _M << nl << "return p;";
    _M << eb;

    _H << nl << "@end";

    _M << nl << "@end";
}

void
Slice::Gen::TypesVisitor::visitDictionary(const DictionaryPtr& p)
{
    string prefix = moduleName(findModule(p));
    string name = fixName(p);

    emitDeprecate(p, 0, _M, "type");

    _H << sp << nl << "typedef NSDictionary " << name << ";";
    _H << nl << "typedef NSMutableDictionary " << prefix << "Mutable" << p->name() << ";";
}

void
Slice::Gen::TypesVisitor::visitEnum(const EnumPtr& p)
{
    string name = fixName(p);
    EnumeratorList enumerators = p->getEnumerators();
    _H << sp;

    emitDeprecate(p, 0, _M, "type");

    _H << nl << "typedef enum";
    _H << sb;
    EnumeratorList::const_iterator en = enumerators.begin();
    while(en != enumerators.end())
    {
        _H << nl << fixName(*en);
        if(++en != enumerators.end())
        {
            _H << ',';
        }
    }
    _H << eb << " " << name << ";";

    // TODO: if(_stream)
}

void
Slice::Gen::TypesVisitor::visitConst(const ConstPtr& p)
{
    _H << sp;
    if(isString(p->type()))
    {
	_H << nl << "static NSString * const";
    }
    else
    {
        _H << nl << "static const " << typeToString(p->type());
    }
    _H << " " << fixName(p) << " = ";

    if(isString(p->type()))
    {
        //
        // Expand strings into the basic source character set. We can't use isalpha() and the like
        // here because they are sensitive to the current locale.
        //
        static const string basicSourceChars = "abcdefghijklmnopqrstuvwxyz"
                                               "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                               "0123456789"
                                               "_{}[]#()<>%:;.?*+-/^&|~!=,\\\"' ";
        static const set<char> charSet(basicSourceChars.begin(), basicSourceChars.end());

        _H << "@\"";                                      // Opening @"

        const string val = p->value();
        for(string::const_iterator c = val.begin(); c != val.end(); ++c)
        {
            if(charSet.find(*c) == charSet.end())
            {
                unsigned char uc = *c;                  // char may be signed, so make it positive
                ostringstream s;
                s << "\\";                              // Print as octal if not in basic source character set
                s.width(3);
                s.fill('0');
                s << oct;
                s << static_cast<unsigned>(uc);
                _H << s.str();
            }
            else
            {
                _H << *c;                                // Print normally if in basic source character set
            }
        }

        _H << "\"";                                      // Closing "
    }
    else
    {
        EnumPtr ep = EnumPtr::dynamicCast(p->type());
        if(ep)
        {
	    string prefix = moduleName(findModule(ep));
            _H << prefix << p->value();
        }
        else
        {
	    string value = p->value();
	    if(value == "true")
	    {
	        value = "YES";
	    }
	    else if(value == "false")
	    {
	        value = "NO";
	    }
            _H << value;
        }
    }

    _H << ';';
}

void
Slice::Gen::TypesVisitor::writeMembers(const DataMemberList& dataMembers, int baseTypes) const
{
    for(DataMemberList::const_iterator q = dataMembers.begin(); q != dataMembers.end(); ++q)
    {
	TypePtr type = (*q)->type();
	string typeString = typeToString(type);
        string name = fixId((*q)->name(), baseTypes);

	_H << nl << typeString << " ";
	if(mapsToPointerType(type))
	{
	    _H << "*";
	}
	_H << name << ";";
    }
}

void
Slice::Gen::TypesVisitor::writeMemberSignature(const DataMemberList& dataMembers, int baseTypes,
                                               ContainerType ct) const
{
    if(ct == LocalException)
    {
	_H << ":(const char*)file__p line:(int)line__p";
        _M << ":(const char*)file__p line:(int)line__p";
    }
    for(DataMemberList::const_iterator q = dataMembers.begin(); q != dataMembers.end(); ++q)
    {
	TypePtr type = (*q)->type();
	string typeString = typeToString(type);
        string name = fixId((*q)->name(), baseTypes);

	if(q != dataMembers.begin() || ct == LocalException)
	{
	    _H << " " << name;
	    _M << " " << name;
	}
	_H << ":(" << typeString;
	_M << ":(" << typeString;
	if(mapsToPointerType(type))
	{
	    _H << " *";
	    _M << " *";
	}
	_H << ")" << fixId((*q)->name());
	_M << ")" << fixId((*q)->name()) << "_p";

    }
}

void
Slice::Gen::TypesVisitor::writeMemberCall(const DataMemberList& dataMembers, int baseTypes,
                                          ContainerType ct, Escape esc) const
{
    for(DataMemberList::const_iterator q = dataMembers.begin(); q != dataMembers.end(); ++q)
    {
        string name = (*q)->name();

	if(q != dataMembers.begin() || ct == LocalException)
	{
	    _M << " " << fixId(name, baseTypes);
	}
	if(esc == NoEscape)
	{
	    _M << ":" << fixId(name, baseTypes);
	}
	else
	{
	    _M << ":" << fixId(name) << "_p";
	}
    }
}

void
Slice::Gen::TypesVisitor::writeMemberInit(const DataMemberList& dataMembers, int baseTypes) const
{
    for(DataMemberList::const_iterator q = dataMembers.begin(); q != dataMembers.end(); ++q)
    {
	TypePtr type = (*q)->type();
	string typeString = typeToString(type);
	string name = fixId((*q)->name());

	_M << nl << fixId((*q)->name(), baseTypes) << " = ";
	if(isValueType(type))
	{
	    _M << name << "_p;";
	}
	else
	{
	   _M << "[" << name << "_p retain];";
	}
    }
}

void
Slice::Gen::TypesVisitor::writeProperties(const DataMemberList& dataMembers, int baseTypes) const
{
    for(DataMemberList::const_iterator q = dataMembers.begin(); q != dataMembers.end(); ++q)
    {
	TypePtr type = (*q)->type();
        string name = fixId((*q)->name(), baseTypes);
	string typeString = typeToString(type);
	bool isValue = isValueType(type);

	_H << nl << "@property(nonatomic, ";
	if(isValue)
	{
	    _H << "assign";
	}
	else
	{
	    _H << "retain";
	}
	_H << ") " << typeString << " ";
	if(mapsToPointerType(type))
	{
	    _H << "*";
	}
	_H << name << ";";
    }
}

void
Slice::Gen::TypesVisitor::writeSynthesize(const DataMemberList& dataMembers, int baseTypes) const
{
    for(DataMemberList::const_iterator q = dataMembers.begin(); q != dataMembers.end(); ++q)
    {
        string name = fixId((*q)->name(), baseTypes);
	_M << nl << "@synthesize " << name << ";";
    }
}

void
Slice::Gen::TypesVisitor::writeMemberHashCode(const DataMemberList& dataMembers, int baseTypes) const
{
    _M << sp << nl << "-(NSUInteger) hash";
    _M << sb;
    _M << nl << "NSUInteger h_ = 0;";
    for(DataMemberList::const_iterator q = dataMembers.begin(); q != dataMembers.end(); ++q)
    {
	TypePtr type = (*q)->type();
        string name = fixId((*q)->name());

	_M << nl << "h_ = (h_ << 1) ^ ";
	if(isValueType(type))
	{
            BuiltinPtr builtin = BuiltinPtr::dynamicCast(type);
            if(builtin)
            {
                if(builtin->kind() == Builtin::KindFloat)
                {
                    _M << "ICEInternalHashFloat(" << name << ");";
                }
                else if(builtin->kind() == Builtin::KindDouble)
                {
                    _M << "ICEInternalHashDouble(" << name << ");";
                }
                else
                {
                    _M << name << ";";
                }
            }
            else
            {
                _M << name << ";";
            }
	}
	else
	{
	    _M << "[" << name << " hash];";
	}
    }
    _M << nl << "return h_;";
    _M << eb;
}

void
Slice::Gen::TypesVisitor::writeMemberEquals(const DataMemberList& dataMembers, int baseTypes) const
{
    if(!dataMembers.empty())
    {
	ContainerPtr container = (*dataMembers.begin())->container();
	ContainedPtr contained = ContainedPtr::dynamicCast(container);
	string containerName = fixName(contained);
	_M << nl << containerName << " *obj_ = (" << containerName << " *)o_;";
	for(DataMemberList::const_iterator q = dataMembers.begin(); q != dataMembers.end(); ++q)
	{
	    TypePtr type = (*q)->type();
	    string name = fixId((*q)->name());

	    if(isValueType(type))
	    {
		_M << nl << "if(" << name << " != obj_->" << name << ")";
		_M << sb;
		_M << nl << "return NO;";
		_M << eb;
	    }
	    else
	    {
		_M << nl << "if(!" << name << ")";
		_M << sb;
		_M << nl << "if(obj_->" << name << ")";
		_M << sb;
		_M << nl << "return NO;";
		_M << eb;
		_M << eb;
		_M << nl << "else";
		_M << sb;
		_M << nl << "if(![" << name << " ";
		_M << (isString(type) ? "isEqualToString" : "isEqual");
		_M << ":obj_->" << name << "])";
		_M << sb;
		_M << nl << "return NO;";
		_M << eb;
		_M << eb;
	    }
	}
    }
    _M << nl << "return YES;";
}

void
Slice::Gen::TypesVisitor::writeMemberDealloc(const DataMemberList& dataMembers, int baseTypes) const
{
    bool once = false;
    for(DataMemberList::const_iterator q = dataMembers.begin(); q != dataMembers.end(); ++q)
    {
	TypePtr type = (*q)->type();
	if(!isValueType(type))
	{
	    if(!once)
	    {
		 once = true;
		_M << sp << nl << "-(void) dealloc;";
		_M << sb;
	    }

	    bool isValue = isValueType(type);
	    if(!isValue)
	    {
		_M << nl << "[" << fixId((*q)->name(), baseTypes) << " release];";
	    }
	}
    }
    if(once)
    {
	_M << nl << "[super dealloc];";
	_M << eb;
	_H << nl << "// This class also overrides dealloc.";
    }
}

void
Slice::Gen::TypesVisitor::writeMemberMarshal(const string& instance, const DataMemberList& dataMembers,
                                             int baseTypes) const
{
    for(DataMemberList::const_iterator q = dataMembers.begin(); q != dataMembers.end(); ++q)
    {
	writeMarshalUnmarshalCode(_M, (*q)->type(), instance + fixId((*q)->name(), baseTypes), true, false, false);
    }
}

void
Slice::Gen::TypesVisitor::writeMemberUnmarshal(const string& instance, const DataMemberList& dataMembers,
                                               int baseTypes) const
{
    for(DataMemberList::const_iterator q = dataMembers.begin(); q != dataMembers.end(); ++q)
    {
	writeMarshalUnmarshalCode(_M, (*q)->type(), instance + fixId((*q)->name(), baseTypes),
	                          false, false, false);
    }
}

Slice::Gen::ProxyVisitor::ProxyVisitor(Output& H, Output& M)
    : ObjCVisitor(H, M)
{
}

bool
Slice::Gen::ProxyVisitor::visitClassDefStart(const ClassDefPtr& p)
{
    string name = fixName(p);
    ClassList bases = p->bases();

    _H << sp << nl << "@protocol " << name << "Prx <";
    if(bases.empty())
    {
        _H << "ICEObjectPrx";
    }
    else
    {
        ClassList::const_iterator q = bases.begin();
        while(q != bases.end())
        {
            _H << fixName(*q) + "Prx";
            if(++q != bases.end())
            {
                _H << ", ";
            }
        }
    }
    _H << ">";

    return true;
}

void
Slice::Gen::ProxyVisitor::visitClassDefEnd(const ClassDefPtr&)
{
    _H << nl << "@end";
}

void
Slice::Gen::ProxyVisitor::visitOperation(const OperationPtr& p)
{
    // TODO deal with deprecate metadata.

    string name = fixId(p->name());
    TypePtr returnType = p->returnType();
    string retString = outTypeToString(returnType);
    bool retIsPointer = mapsToPointerType(returnType);
    string params = getParams(p);

    //
    // Write two versions of the operation--with and without a
    // context parameter.
    //
    _H << nl << "-(" << retString;
    if(retIsPointer)
    {
        _H << " *";
    }
    _H << ") " << name << params << ";";

    _H << nl << "-(" << retString;
    if(retIsPointer)
    {
        _H << " *";
    }
    _H << ") " << name << params;
    if(!params.empty())
    {
        _H << " context";
    }
    _H << ":(ICEContext *)context;";

    // TODO: deal with AMI
    ContainerPtr container = p->container();
    ClassDefPtr cl = ClassDefPtr::dynamicCast(container);
    if(cl->hasMetaData("ami") || p->hasMetaData("ami"))
    {
        string params = getParamsAsync(p, false); // Without sent callback.
        assert(!params.empty());
        _H << nl << "-(BOOL) " << name << "_async" << params << ";";
        _H << nl << "-(BOOL) " << name << "_async" << params << " context:(ICEContext *)context;";

        params = getParamsAsync(p, true); // With sent callback.
        assert(!params.empty());
        _H << nl << "-(BOOL) " << name << "_async" << params << ";";
        _H << nl << "-(BOOL) " << name << "_async" << params << " context:(ICEContext *)context;";
    }
}

Slice::Gen::HelperVisitor::HelperVisitor(Output& H, Output& M, bool stream)
    : ObjCVisitor(H, M), _stream(stream)
{
}

void
Slice::Gen::HelperVisitor::visitEnum(const EnumPtr& p)
{
    string prefix = moduleName(findModule(p));
    string name = prefix + p->name() + "Helper";

    _H << sp << nl << "@interface " << name << " : ICEEnumHelper";
    _H << nl << "+(ICEInt) getLimit;";
    _H << nl << "@end";

    _M << sp << nl << "@implementation " << name;
    _M << nl << "+(ICEInt) getLimit";
    _M << sb;
    _M << nl << "return " << p->getEnumerators().size() << ";";
    _M << eb;
    _M << nl << "@end";
}

void
Slice::Gen::HelperVisitor::visitSequence(const SequencePtr& p)
{
    string prefix = moduleName(findModule(p));
    string name = prefix + p->name() + "Helper";

    BuiltinPtr builtin = BuiltinPtr::dynamicCast(p->type());
    if(builtin)
    {
        if(builtin->kind() == Builtin::KindObjectProxy)
        {
            _H << sp << nl << "typedef ICEObjectPrxSequenceHelper " << name << ";";
            return;
        }
        else if(builtin->kind() == Builtin::KindObject)
        {
            _H << sp << nl << "typedef ICEObjectSequenceHelper " << name << ";";
            return;
        }

	_H << sp << nl << "@interface " << name << " : NSObject";
	_H << nl << "+(id) ice_readWithStream:(id<ICEInputStream>)stream;";
	_H << nl << "+(void) ice_writeWithStream:(id)obj stream:(id<ICEOutputStream>)stream;";
	_H << nl << "@end";

	_M << sp << nl << "@implementation " << name;
	_M << nl << "+(id) ice_readWithStream:(id<ICEInputStream>)stream";
	_M << sb;
        _M << nl << "return [stream read" << getBuiltinName(builtin) << "Seq];";
	_M << eb;

	_M << sp << nl << "+(void) ice_writeWithStream:(id)obj stream:(id<ICEOutputStream>)stream";
	_M << sb;
        _M << nl << "[stream write" << getBuiltinName(builtin) << "Seq:obj];";
	_M << eb;
	_M << nl << "@end";
	
	return;
    }

    EnumPtr en = EnumPtr::dynamicCast(p->type());
    if(en)
    {
	_H << sp << nl << "@interface " << name << " : NSObject";
	_H << nl << "+(id) ice_readWithStream:(id<ICEInputStream>)stream;";
	_H << nl << "+(void) ice_writeWithStream:(id)obj stream:(id<ICEOutputStream>)stream;";
	_H << nl << "@end";

	string typeS = typeToString(en);
	int limit = en->getEnumerators().size();
	_M << sp << nl << "@implementation " << name;
	_M << nl << "+(id) ice_readWithStream:(id<ICEInputStream>)stream";
	_M << sb;
	_M << nl << "return [stream readEnumSeq:" << limit << "];";
	_M << eb;

	_M << sp << nl << "+(void) ice_writeWithStream:(id)obj stream:(id<ICEOutputStream>)stream";
	_M << sb;
	_M << nl << "[stream writeEnumSeq:obj limit:" << limit << "];";
	_M << eb;
	_M << nl << "@end";

        return;
    }

    ClassDeclPtr cl = ClassDeclPtr::dynamicCast(p->type());
    if(cl)
    {
        _H << sp << nl << "@interface " << name << " : ICEObjectSequenceHelper";
        _H << nl << "+(NSString*) getContained;";
        _H << nl << "@end";

        _M << sp << nl << "@implementation " << name;
        _M << nl << "+(NSString*) getContained";
        _M << sb;
        _M << nl << "return @\"" << cl->scoped() << "\";";
        _M << eb;
        _M << nl << "@end";
        return;
    }

    ContainedPtr contained = ContainedPtr::dynamicCast(p->type());
    ProxyPtr proxy = ProxyPtr::dynamicCast(p->type());

    assert(contained || proxy);
    _H << sp << nl << "@interface " << name << " : ICESequenceHelper";
    _H << nl << "+(Class) getContained;";
    _H << nl << "@end";

    _M << sp << nl << "@implementation " << name;
    _M << nl << "+(Class) getContained";
    _M << sb;
    if(proxy)
    {
        _M << nl << "return [" << moduleName(findModule(proxy->_class())) << (proxy->_class()->name()) << "Prx class];";
    }
    else if(SequencePtr::dynamicCast(contained) || DictionaryPtr::dynamicCast(contained))
    {
        _M << nl << "return [" << moduleName(findModule(contained)) + contained->name() + "Helper class];";
    }
    else
    {
        _M << nl << "return [" << moduleName(findModule(contained)) + contained->name() + " class];";
    }
    _M << eb;
    _M << nl << "@end";
}

void
Slice::Gen::HelperVisitor::visitDictionary(const DictionaryPtr& p)
{
    string prefix = moduleName(findModule(p));
    string name = prefix + p->name() + "Helper";

    TypePtr keyType = p->keyType();
    string keyS;
    BuiltinPtr keyBuiltin = BuiltinPtr::dynamicCast(keyType);
    EnumPtr keyEnum = EnumPtr::dynamicCast(keyType);
    if(keyBuiltin)
    {
        if(keyBuiltin->kind() == Builtin::KindObjectProxy)
        {
            keyS = "ICEObjectPrx";
        }
        else if(keyBuiltin->kind() == Builtin::KindObject)
        {
            keyS = "ICEObject";
        }
        else
        {
            keyS = "ICE" + getBuiltinName(BuiltinPtr::dynamicCast(keyType)) + "Helper";
        }
    }
    else if(keyEnum)
    {
        keyS = moduleName(findModule(keyEnum)) + keyEnum->name() + "Helper";
    }
    else
    {
	StructPtr contained = StructPtr::dynamicCast(keyType);
	string prefix = moduleName(findModule(contained));
        keyS = moduleName(findModule(contained)) + contained->name();
        if(SequencePtr::dynamicCast(contained) || DictionaryPtr::dynamicCast(contained))
        {
            keyS += "Helper";
        }
    }

    TypePtr valueType = p->valueType();
    string valueS;
    BuiltinPtr valueBuiltin = BuiltinPtr::dynamicCast(valueType);
    EnumPtr valueEnum = EnumPtr::dynamicCast(valueType);
    if(valueBuiltin)
    {
        if(valueBuiltin->kind() == Builtin::KindObjectProxy)
        {
            valueS = "ICEObjectPrx";
        }
        else if(valueBuiltin->kind() == Builtin::KindObject)
        {
            valueS = "ICEObject";
        }
        else
        {
            valueS = "ICE" + getBuiltinName(BuiltinPtr::dynamicCast(valueType)) + "Helper";
        }
    }
    else if(valueEnum)
    {
        valueS = moduleName(findModule(valueEnum)) + valueEnum->name() + "Helper";
    }
    else
    {
	ContainedPtr contained = ContainedPtr::dynamicCast(valueType);
	string prefix = moduleName(findModule(contained));
        valueS = moduleName(findModule(contained)) + contained->name();
        if(SequencePtr::dynamicCast(contained) || DictionaryPtr::dynamicCast(contained))
        {
            valueS += "Helper";
        }
    }


    ClassDeclPtr valueClass = ClassDeclPtr::dynamicCast(valueType);
    if(valueBuiltin && valueBuiltin->kind() == Builtin::KindObject || valueClass)
    {
        _H << sp << nl << "@interface " << name << " : ICEObjectDictionaryHelper";
        _H << nl << "+(Class) getContained:(NSString**)typeId;";
        _H << nl << "@end";
        
        _M << sp << nl << "@implementation " << name;
        _M << nl << "+(Class) getContained:(NSString**)typeId";
        _M << sb;
        if(valueBuiltin)
        {
            _M << nl << "*typeId = @\"::Ice::Object\";";
        }
        else
        {
            _M << nl << "*typeId = @\"" << valueClass->scoped() <<  "\";";
        }
        _M << nl << "return [" << keyS << " class];";
        _M << eb;
        _M << nl << "@end";
        return;
    }

    
    _H << sp << nl << "@interface " << name << " : ICEDictionaryHelper";
    _H << nl << "+(ICEKeyValueTypeHelper) getContained;";
    _H << nl << "@end";

    _M << sp << nl << "@implementation " << name;
    _M << nl << "+(ICEKeyValueTypeHelper) getContained";
    _M << sb;
    _M << nl << "ICEKeyValueTypeHelper c;";
    _M << nl << "c.key = [" << keyS << " class];";
    _M << nl << "c.value = [" << valueS << " class];";
    _M << nl << "return c;";
    _M << eb;
    _M << nl << "@end";
}

Slice::Gen::DelegateMVisitor::DelegateMVisitor(Output& H, Output& M)
    : ObjCVisitor(H, M)
{
}

bool
Slice::Gen::DelegateMVisitor::visitModuleStart(const ModulePtr& p)
{
    return true;
}

void
Slice::Gen::DelegateMVisitor::visitModuleEnd(const ModulePtr&)
{
}

bool
Slice::Gen::DelegateMVisitor::visitClassDefStart(const ClassDefPtr& p)
{
    string name = fixName(p);
    ClassList bases = p->bases();
    OperationList ops = p->allOperations();
    OperationList::const_iterator r;

    _H << sp << nl << "@interface " << name << "Prx : ICEObjectPrx <" << name << "Prx>";
    _H << nl << "+(NSString *) ice_staticId;";
    
    _M << sp << nl << "@implementation " << name << "Prx";
    for(r = ops.begin(); r != ops.end(); ++r)
    {
        string opName = fixId((*r)->name());
	TypePtr returnType = (*r)->returnType();
	string retString = outTypeToString(returnType);
	bool retIsPointer = mapsToPointerType(returnType);
	string params = getParams(*r);
	string args = getArgs(*r);

	ContainerPtr container = (*r)->container();
	ClassDefPtr cl = ClassDefPtr::dynamicCast(container);
	string className = fixName(cl);

	//
	// Write context-less operation that forwards to the version with a context.
	//
	_M << sp << nl << "-(" << retString;
	if(retIsPointer)
	{
	    _M << " *";
	}
	_M << ") " << opName << params;
	_M << sb;
	_M << nl;
	if(returnType)
	{
	    _M << "return ";
	}
	_M << "[" << className << "Prx " << opName << "___" << args;
	if(!args.empty())
	{
	    _M << " prx";
	}
	_M << ":self context:nil];";
	_M << eb;

	//
	// Write version with context.
	//
	_M << sp << nl << "-(" << retString;
	if(retIsPointer)
	{
	    _M << " *";
	}
	_M << ") " << opName << params;
	if(!params.empty())
	{
	    _M << " context";
	}
	_M << ":(ICEContext *)ctx_";
	_M << sb;
	_M << nl;
	if(returnType)
	{
	    _M << "return ";
	}
	_M << "[" << className << "Prx " << opName << "___" << args;
	if(!args.empty())
	{
	    _M << " prx";
	}
	_M << ":self context:ctx_];";
	_M << eb;


        if(cl->hasMetaData("ami") || (*r)->hasMetaData("ami"))
        {
            bool sent[] = { false, true };
            for(int i = 0; i < 2; ++i)
            {
                params = getParamsAsync(*r, sent[i]);
                args = getArgsAsync(*r, sent[i]);
                
                //
                // Write context-less operation that forwards to the version with a context.
                //
                _M << sp << nl << "-(BOOL) " << opName << "_async" << params;
                _M << sb;
                _M << nl;
                _M << "return [" << className << "Prx " << opName << "_async___" << args << " prx:self context:nil];";
                _M << eb;
                
                //
                // Write version with context.
                //
                _M << sp << nl << "-(BOOL) " << opName << "_async" << params << " context:(ICEContext *)ctx_";
                _M << sb;
                _M << nl;
                _M << "return [" << className << "Prx " << opName << "_async___" << args << " prx:self context:ctx_];";
                _M << eb;
            }
        }
    }

    _M << sp << nl << "+(NSString *) ice_staticId";
    _M << sb;
    _M << nl << "return @\"" << p->scoped() << "\";";
    _M << eb;

    return true;
}

void
Slice::Gen::DelegateMVisitor::visitClassDefEnd(const ClassDefPtr& p)
{
    _H << nl << "@end";
    _M << nl << "@end";
}

void
Slice::Gen::DelegateMVisitor::visitOperation(const OperationPtr& p)
{
    string name = fixId(p->name());
    TypePtr returnType = p->returnType();
    string retString = outTypeToString(returnType);
    bool retIsPointer = mapsToPointerType(returnType);
    string params = getParams(p);
    string args = getParams(p);

    TypeStringList inParams;
    TypeStringList outParams;
    ParamDeclList paramList = p->parameters();
    for(ParamDeclList::const_iterator pli = paramList.begin(); pli != paramList.end(); ++pli)
    {
	if((*pli)->isOutParam())
	{
	   outParams.push_back(make_pair((*pli)->type(), (*pli)->name()));
	}
	else
	{
	   inParams.push_back(make_pair((*pli)->type(), (*pli)->name()));
	}
    }

    //
    // Write class method to invoke each operation.
    //
    ContainerPtr container = p->container();
    ClassDefPtr cl = ClassDefPtr::dynamicCast(container);
    string className = fixName(cl);
    _H << nl << "+(" << retString;
    if(retIsPointer)
    {
        _H << " *";
    }
    _H << ") " << name << "___" << params;
    if(!params.empty())
    {
        _H << " prx";
    }
    _H << ":(ICEObjectPrx <" << className << "Prx> *)prx context:(ICEContext *)ctx;";

    _M << sp << nl << "+(" << retString;
    if(retIsPointer)
    {
        _M << " *";
    }
    _M << ") " << name << "___" << params;
    if(!params.empty())
    {
        _M << " prx";
    }
    _M << ":(ICEObjectPrx <" << className << "Prx> *)prx_ context:(ICEContext *)ctx_";
    _M << sb;
    if(p->returnsData())
    {
        _M << nl << "[prx_ checkTwowayOnly__:@\"" << name <<  "\"];";
    }
    _M << nl << "id<ICEOutputStream> os_ = [prx_ createOutputStream__];";
    _M << nl << "id<ICEInputStream> is_ = nil;";
    if(returnType)
    {
        _M << nl << retString << " " << (retIsPointer ? "*ret_" : "ret_");
        if(!isValueType(returnType))
        {
            _M << " = nil;";
        }
        else
        {
            _M << ";";
        }
    }
    if(p->returnsData())
    {
	for(TypeStringList::const_iterator op = outParams.begin(); op != outParams.end(); ++op)
	{
            if(!isValueType(op->first))
            {
                _M << nl << "*" << fixId(op->second) << " = nil;";
            }
	}
    }
    _M << nl << "@try";
    _M << sb;
    for(TypeStringList::const_iterator ip = inParams.begin(); ip != inParams.end(); ++ip)
    {
	writeMarshalUnmarshalCode(_M, ip->first, fixId(ip->second), true, false, false);
    }
    if(p->sendsClasses())
    {
        _M << nl << "[os_ writePendingObjects];";
    }
    _M << nl << "[prx_ invoke__:@\"" << name <<  "\" mode:" << sliceModeToIceMode(p->sendMode())
       << " os:os_ is:&is_ context:ctx_];";
    if(p->returnsData())
    {
        // _M << nl << "[is_ startEncapsulation];";
	for(TypeStringList::const_iterator op = outParams.begin(); op != outParams.end(); ++op)
	{
	    writeMarshalUnmarshalCode(_M, op->first, "*" + fixId(op->second), false, false, true, "");
	}
        if(returnType)
        {
	   writeMarshalUnmarshalCode(_M, returnType, "ret_", false, false, true, "");
	}
	if(p->returnsClasses())
	{
 	    _M << nl << "[is_ readPendingObjects];";
	    // TODO: assign to parameters from patcher
	}
	// _M << nl << "[is_ endEncapsulation];";
    }
    else
    {
        // _M << nl << "[is_ skipEncapsulation];";
    }

    _M << eb;

    //
    // Arrange exceptions into most-derived to least-derived order. If we don't
    // do this, a base exception handler can appear before a derived exception
    // handler, causing compiler warnings and resulting in the base exception
    // being marshaled instead of the derived exception.
    //
    ExceptionList throws = p->throws();
    throws.sort();
    throws.unique();
    throws.sort(Slice::DerivedToBaseCompare());

    for(ExceptionList::const_iterator e = throws.begin(); e != throws.end(); ++e)
    {
        _M << nl << "@catch(" << fixName(*e) << " *ex_)";
	_M << sb;
	_M << nl << "@throw;";
	_M << eb;
    }
    _M << nl << "@catch(ICEUserException *ex_)";
    _M << sb;
    _M << nl << "@throw [ICEUnknownUserException unknownUserException:__FILE__ line:__LINE__ unknown:[ex_ ice_name]];";
    _M << eb;
    _M << nl << "@finally";
    _M << sb;
    if(p->returnsData())
    {
	for(TypeStringList::const_iterator op = outParams.begin(); op != outParams.end(); ++op)
	{
            if(!isValueType(op->first))
            {
                _M << nl << "[(" << (isClass(op->first) ? "NSObject*" : "id<NSObject>")
		   << ")*" + fixId(op->second) << " autorelease];";
            }
        }
        if(returnType && !isValueType(returnType))
        {
            _M << nl << "[(" << (isClass(returnType) ? "NSObject*" : "id<NSObject>") << ")ret_ autorelease];";
        }
    }
    _M << nl << "[os_ release];";
    _M << nl << "[is_ release];";
    _M << eb;
    if(returnType)
    {
        _M << nl << "return ret_;";
    }
    _M << eb;

    if(cl->hasMetaData("ami") || p->hasMetaData("ami"))
    {
        params = getParamsAsync(p, true);
        _H << nl << "+(BOOL) " << name << "_async___" << params << " prx:(ICEObjectPrx <" << className << "Prx> *)prx ";
        _H << "context:(ICEContext *)ctx;";

        _M << sp << nl << "+(BOOL) " << name << "_async___" << params << " prx:(ICEObjectPrx <" << className;
        _M << "Prx> *)prx_ context:(ICEContext *)ctx_";
        _M << sb;
        if(p->returnsData())
        {
            _M << nl << "[prx_ checkTwowayOnly__:@\"" << name <<  "\"];";
        }
        _M << nl << "SEL finished_ = @selector(" << name << "_async_finished___:response:exception:ok:is:);";

        _M << nl << "id<ICEOutputStream> os_ = [prx_ createOutputStream__];";
        _M << nl << "@try";
        _M << sb;
        for(TypeStringList::const_iterator ip = inParams.begin(); ip != inParams.end(); ++ip)
        {
            writeMarshalUnmarshalCode(_M, ip->first, fixId(ip->second), true, false, false);
        }
        if(p->sendsClasses())
        {
            _M << nl << "[os_ writePendingObjects];";
        }
        _M << nl << "return ";
        _M << "[prx_ invoke_async__:target_ response:response_ exception:exception_ sent:sent_ ";
        _M << "finishedClass:self finished:finished_" << " operation:@\"" << name <<  "\""; 
        _M << " mode:" << sliceModeToIceMode(p->sendMode()) << " os:os_ context:ctx_];";
        _M << eb;

        _M << nl << "@finally";
        _M << sb;
        _M << nl << "[os_ release];";
        _M << eb;
        _M << nl << "return FALSE; // Keep the compiler happy.";
        _M << eb;

        params = getParamsAsyncCB(p);
        _H << nl << "+(void) " << name << "_async_finished___:(id)target response:(SEL)response ";
        _H << "exception:(SEL)exception ok:(BOOL)ok is:(id<ICEInputStream>)is;";

        _M << sp << nl << "+(void) " << name << "_async_finished___:(id)target_ response:(SEL)response_ ";
        _M << "exception:(SEL)exception_ ok:(BOOL)ok_ is:(id<ICEInputStream>)is_";
        _M << sb;

        bool outReferenceParams = false;
        if(!outParams.empty() || returnType)
        {
            for(TypeStringList::const_iterator op = outParams.begin(); op != outParams.end(); ++op)
            {
                _M << nl << outTypeToString(op->first) << " ";
                if(mapsToPointerType(op->first))
                {
                    _M << "*";
                }
                _M << fixId(op->second);

                if(!isValueType(op->first))
                {
                    outReferenceParams = true;
                    _M << " = nil";
                }
                _M << ";";
            }
            if(returnType)
            {
                _M << nl << outTypeToString(returnType) << " ";
                if(mapsToPointerType(returnType))
                {
                    _M << "*";
                }
                _M << "ret_";
                if(!isValueType(returnType))
                {
                    outReferenceParams = true;
                    _M << " = nil";
                }
                _M << ";";
            }
        }

        _M << nl << "@try";
        _M << sb;
        _M << nl << "if(!ok_)";
        _M << sb;
        _M << nl << "[is_ throwException];";
        _M << eb;

        if(!outParams.empty() || returnType)
        {
            // _M << nl << "[is_ startEncapsulation];";
            for(TypeStringList::const_iterator op = outParams.begin(); op != outParams.end(); ++op)
            {
                writeMarshalUnmarshalCode(_M, op->first, fixId(op->second), false, false, true, "");
            }
            if(returnType)
            {
                writeMarshalUnmarshalCode(_M, returnType, "ret_", false, false, true, "");
            }
            if(p->returnsClasses())
            {
                _M << nl << "[is_ readPendingObjects];";
            }
            // _M << nl << "[is_ endEncapsulation];";

            //
            // NOTE: it's necessary to cast the objc_msgSend function to the type of the callback.
            // Otherwise, wrong parameter types are used to call the Objective-C method (this occurs
            // when calling a function with float parameters for instance).
            //
            _M << nl << "((" << getSigAsyncCB(p) << ")objc_msgSend)(target_, response_, " << getArgsAsyncCB(p) << ");";
        }
        else
        {
            // _M << nl << "[is_ skipEncapsulation];";
            _M << nl << "objc_msgSend(target_, response_);";
        }
        _M << eb;

        //
        // Arrange exceptions into most-derived to least-derived order. If we don't
        // do this, a base exception handler can appear before a derived exception
        // handler, causing compiler warnings and resulting in the base exception
        // being marshaled instead of the derived exception.
        //
        ExceptionList throws = p->throws();
        throws.sort();
        throws.unique();
        throws.sort(Slice::DerivedToBaseCompare());
        
        for(ExceptionList::const_iterator e = throws.begin(); e != throws.end(); ++e)
        {
            _M << nl << "@catch(" << fixName(*e) << " *ex_)";
            _M << sb;
            _M << nl << "objc_msgSend(target_, exception_, ex_);";
            _M << eb;
        }
        _M << nl << "@catch(ICEUserException *ex_)";
        _M << sb;
        _M << nl << "ICEUnknownUserException* uuex_;";
        _M << nl << "uuex_ = [ICEUnknownUserException unknownUserException:__FILE__ line:__LINE__ ";
        _M << "unknown:[ex_ ice_name]];";
        _M << nl << "objc_msgSend(target_, exception_, uuex_);";
        _M << eb;
        if(outReferenceParams)
        {
            _M << nl << "@finally";
            _M << sb;
            {
                for(TypeStringList::const_iterator op = outParams.begin(); op != outParams.end(); ++op)
                {
                    if(!isValueType(op->first))
                    {
                        _M << nl << "[(" << (isClass(op->first) ? "ICEObject*" : "id<NSObject>") << ")"
			   << fixId(op->second) << " release];";
                    }
                }
                if(returnType && !isValueType(returnType))
                {
                    _M << nl << "[(" << (isClass(returnType) ? "ICEObject*" : "id<NSObject>") << ")ret_ release];";
                }
            }
            _M << eb;
        }

        _M << eb;
    }
}

#if 0
Slice::Gen::TieVisitor::TieVisitor(Output& out)
    : ObjCVisitor(out, out)
{
}

bool
Slice::Gen::TieVisitor::visitModuleStart(const ModulePtr& p)
{
    if(!p->hasClassDefs())
    {
        return false;
    }

    _M << sp << nl << "namespace " << fixId(p->name());
    _M << sb;

    return true;
}

void
Slice::Gen::TieVisitor::visitModuleEnd(const ModulePtr&)
{
    _M << eb;
}

bool
Slice::Gen::TieVisitor::visitClassDefStart(const ClassDefPtr& p)
{
    if(!p->isAbstract())
    {
        return false;
    }
    
    string name = p->name();
    string opIntfName = "Operations";
    if(p->isLocal())
    {
        opIntfName += "NC";
    }

    _M << sp << nl << "public class " << name << "Tie_ : ";
    if(p->isInterface())
    {
        if(p->isLocal())
        {
            _M << fixId(name) << ", Ice.TieBase";
        }
        else
        {
            _M << name << "Disp_, Ice.TieBase";
        }
    }
    else
    {
        _M << fixId(name) << ", Ice.TieBase";
    }
    _M << sb;

    _M << sp << nl << "public " << name << "Tie_()";
    _M << sb;
    _M << eb;

    _M << sp << nl << "public " << name << "Tie_(" << name << opIntfName << "_ del)";
    _M << sb;
    _M << nl << "_ice_delegate = del;";
    _M << eb;

    _M << sp << nl << "public object ice_delegate()";
    _M << sb;
    _M << nl << "return _ice_delegate;";
    _M << eb;

    _M << sp << nl << "public void ice_delegate(object del)";
    _M << sb;
    _M << nl << "_ice_delegate = (" << name << opIntfName << "_)del;";
    _M << eb;

    _M << sp << nl << "public ";
    if(!p->isInterface() || !p->isLocal())
    {
        _M << "override ";
    }
    _M << "int ice_hash()";

    _M << sb;
    _M << nl << "return GetHashCode();";
    _M << eb;

    _M << sp << nl << "public override int GetHashCode()";
    _M << sb;
    _M << nl << "return _ice_delegate == null ? 0 : _ice_delegate.GetHashCode();";
    _M << eb;

    _M << sp << nl << "public override bool Equals(object rhs)";
    _M << sb;
    _M << nl << "if(object.ReferenceEquals(this, rhs))";
    _M << sb;
    _M << nl << "return true;";
    _M << eb;
    _M << nl << "if(!(rhs is " << name << "Tie_))";
    _M << sb;
    _M << nl << "return false;";
    _M << eb;
    _M << nl << "if(_ice_delegate == null)";
    _M << sb;
    _M << nl << "return ((" << name << "Tie_)rhs)._ice_delegate == null;";
    _M << eb;
    _M << nl << "return _ice_delegate.Equals(((" << name << "Tie_)rhs)._ice_delegate);";
    _M << eb;

    OperationList ops = p->operations();
    OperationList::const_iterator r;
    for(r = ops.begin(); r != ops.end(); ++r)
    {
        bool hasAMD = p->hasMetaData("amd") || (*r)->hasMetaData("amd");
        string opName = hasAMD ? (*r)->name() + "_async" : fixId((*r)->name(), ObjC::NSObject, true); // TODO: NSObject

        TypePtr ret = (*r)->returnType();
        string retS = typeToString(ret);

        vector<string> params;
        vector<string> args;
        if(hasAMD)
        {
            //params = getParamsAsync((*r), true);
            //args = getArgsAsync(*r);
        }
        else
        {
            //params = getParams(*r);
            //args = getArgs(*r);
        }

        _M << sp << nl << "public ";
        if(!p->isInterface() || !p->isLocal())
        {
            _M << "override ";
        }
        _M << (hasAMD ? string("void") : retS) << ' ' << opName << spar << params;
        if(!p->isLocal())
        {
            _M << "Ice.Current current__";
        }
        _M << epar;
        _M << sb;
        _M << nl;
        if(ret && !hasAMD)
        {
            _M << "return ";
        }
        _M << "_ice_delegate." << opName << spar << args;
        if(!p->isLocal())
        {
            _M << "current__";
        }
        _M << epar << ';';
        _M << eb;
    }

    NameSet opNames;
    ClassList bases = p->bases();
    for(ClassList::const_iterator i = bases.begin(); i != bases.end(); ++i)
    {
        writeInheritedOperationsWithOpNames(*i, opNames);
    }

    _M << sp << nl << "private " << name << opIntfName << "_ _ice_delegate;";

    return true;
}

void
Slice::Gen::TieVisitor::visitClassDefEnd(const ClassDefPtr&)
{
    _M << eb;
}

void
Slice::Gen::TieVisitor::writeInheritedOperationsWithOpNames(const ClassDefPtr& p, NameSet& opNames)
{
    OperationList ops = p->operations();
    OperationList::const_iterator r;
    for(r = ops.begin(); r != ops.end(); ++r)
    {
        bool hasAMD = p->hasMetaData("amd") || (*r)->hasMetaData("amd");
        string opName = hasAMD ? (*r)->name() + "_async" : fixId((*r)->name(), ObjC::NSObject, true); // TODO: NSObject
        if(opNames.find(opName) != opNames.end())
        {
            continue;
        }
        opNames.insert(opName);

        TypePtr ret = (*r)->returnType();
        string retS = typeToString(ret);

        vector<string> params;
        vector<string> args;
        if(hasAMD)
        {
            //params = getParamsAsync((*r), true);
            //args = getArgsAsync(*r);
        }
        else
        {
            //params = getParams(*r);
            //args = getArgs(*r);
        }

        _M << sp << nl << "public ";
        if(!p->isInterface() || !p->isLocal())
        {
            _M << "override ";
        }
        _M << (hasAMD ? string("void") : retS) << ' ' << opName << spar << params;
        if(!p->isLocal())
        {
            _M << "Ice.Current current__";
        }
        _M << epar;
        _M << sb;
        _M << nl;
        if(ret && !hasAMD)
        {
            _M << "return ";
        }
        _M << "_ice_delegate." << opName << spar << args;
        if(!p->isLocal())
        {
            _M << "current__";
        }
        _M << epar << ';';
        _M << eb;
    }

    ClassList bases = p->bases();
    for(ClassList::const_iterator i = bases.begin(); i != bases.end(); ++i)
    {
        writeInheritedOperationsWithOpNames(*i, opNames);
    }
}

Slice::Gen::BaseImplVisitor::BaseImplVisitor(Output& out)
    : ObjCVisitor(out, out)
{
}

void
Slice::Gen::BaseImplVisitor::writeOperation(const OperationPtr& op, bool comment, bool forTie)
{
    ClassDefPtr cl = ClassDefPtr::dynamicCast(op->container());
    string opName = op->name();
    TypePtr ret = op->returnType();
    string retS = typeToString(ret);
    ParamDeclList params = op->parameters();

    if(comment)
    {
        _M << nl << "// ";
    }
    else
    {
        _M << sp << nl;
    }

        ParamDeclList::const_iterator i;
    if(!cl->isLocal() && (cl->hasMetaData("amd") || op->hasMetaData("amd")))
    {
        ParamDeclList::const_iterator i;
        vector<string> pDecl;// = getParamsAsync(op, true);

        _M << "public ";
        if(!forTie)
        {
            _M << "override ";
        }
        _M << "void " << opName << "_async" << spar << pDecl << "Ice.Current current__" << epar;

        if(comment)
        {
            _M << ';';
            return;
        }

        _M << sb;
        if(ret)
        {
            _M << nl << typeToString(ret) << " ret__ = " << writeValue(ret) << ';';
        }
        for(i = params.begin(); i != params.end(); ++i)
        {
            if((*i)->isOutParam())
            {
                string name = fixId((*i)->name());
                TypePtr type = (*i)->type();
                _M << nl << typeToString(type) << ' ' << name << " = " << writeValue(type) << ';';
            }
        }
        _M << nl << "cb__.ice_response" << spar;
        if(ret)
        {
            _M << "ret__";
        }
        for(i = params.begin(); i != params.end(); ++i)
        {
            if((*i)->isOutParam())
            {
                _M << fixId((*i)->name());
            }
        }
        _M << epar << ';';
        _M << eb;
    }
    else
    {
        vector<string> pDecls;// = getParams(op);

        _M << "public ";
        if(!forTie && !cl->isLocal())
        {
            _M << "override ";
        }
        _M << retS << ' ' << fixId(opName, ObjC::NSObject, true) << spar << pDecls; // TODO: NSObject
        if(!cl->isLocal())
        {
            _M << "Ice.Current current__";
        }
        _M << epar;
        if(comment)
        {
            _M << ';';
            return;
        }
        _M << sb;
        for(ParamDeclList::const_iterator i = params.begin(); i != params.end(); ++i)
        {
            if((*i)->isOutParam())
            {
                string name = fixId((*i)->name());
                TypePtr type = (*i)->type();
                _M << nl << name << " = " << writeValue(type) << ';';
            }
        }
        if(ret)
        {
            _M << nl << "return " << writeValue(ret) << ';';
        }
        _M << eb;
    }
}


Slice::Gen::ImplVisitor::ImplVisitor(Output& out)
    : BaseImplVisitor(out)
{
}

bool
Slice::Gen::ImplVisitor::visitModuleStart(const ModulePtr& p)
{
    if(!p->hasClassDefs())
    {
        return false;
    }

    _M << sp << nl << "namespace " << fixId(p->name());
    _M << sb;

    return true;
}

void
Slice::Gen::ImplVisitor::visitModuleEnd(const ModulePtr&)
{
    _M << eb;
}

bool
Slice::Gen::ImplVisitor::visitClassDefStart(const ClassDefPtr& p)
{
    if(!p->isAbstract())
    {
        return false;
    }

    string name = p->name();

    _M << sp << nl << "public sealed class " << name << 'I';
    if(p->isInterface())
    {
        if(p->isLocal())
        {
            _M << " : " << fixId(name);
        }
        else
        {
            _M << " : " << name << "Disp_";
        }
    }
    else
    {
        _M << " : " << fixId(name);
    }
    _M << sb;

    OperationList ops = p->allOperations();
    for(OperationList::const_iterator r = ops.begin(); r != ops.end(); ++r)
    {
        writeOperation(*r, false, false);
    }

    return true;
}

void
Slice::Gen::ImplVisitor::visitClassDefEnd(const ClassDefPtr&)
{
    _M << eb;
}

Slice::Gen::ImplTieVisitor::ImplTieVisitor(Output& out)
    : BaseImplVisitor(out)
{
}

bool
Slice::Gen::ImplTieVisitor::visitModuleStart(const ModulePtr& p)
{
    if(!p->hasClassDefs())
    {
        return false;
    }

    _M << sp << nl << "namespace " << fixId(p->name());
    _M << sb;

    return true;
}

void
Slice::Gen::ImplTieVisitor::visitModuleEnd(const ModulePtr&)
{
    _M << eb;
}

bool
Slice::Gen::ImplTieVisitor::visitClassDefStart(const ClassDefPtr& p)
{
    if(!p->isAbstract())
    {
        return false;
    }

    string name = p->name();
    ClassList bases = p->bases();

    //
    // Use implementation inheritance in the following situations:
    //
    // * if a class extends another class
    // * if a class implements a single interface
    // * if an interface extends only one interface
    //
    bool inheritImpl = (!p->isInterface() && !bases.empty() && !bases.front()->isInterface()) || (bases.size() == 1);

    _M << sp << nl << "public class " << name << "I : ";
    if(inheritImpl)
    {
        if(bases.front()->isAbstract())
        {
            _M << bases.front()->name() << 'I';
        }
        else
        {
            _M << fixId(bases.front()->name());
        }
        _M << ", ";
    }
    _M << name << "Operations";
    if(p->isLocal())
    {
        _M << "NC";
    }
    _M << '_';
    _M << sb;

    _M << nl << "public " << name << "I()";
    _M << sb;
    _M << eb;

    OperationList ops = p->allOperations();
    ops.sort();

    OperationList baseOps;
    if(inheritImpl)
    {
        baseOps = bases.front()->allOperations();
        baseOps.sort();
    }

    OperationList::const_iterator r;
    for(r = ops.begin(); r != ops.end(); ++r)
    {
        if(inheritImpl && binary_search(baseOps.begin(), baseOps.end(), *r))
        {
            _M << sp;
            _M << nl << "// ";
            _M << nl << "// Implemented by " << bases.front()->name() << 'I';
            _M << nl << "//";
            writeOperation(*r, true, true);
        }
        else
        {
            writeOperation(*r, false, true);
        }
    }

    _M << eb;

    return true;
}
#endif
