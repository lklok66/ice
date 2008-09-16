// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
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
#include <Slice/SignalHandler.h>
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

//
// Callback for Crtl-C signal handling
//
static Gen* _gen = 0;

static void closeCallback()
{
    if(_gen != 0)
    {
        _gen->closeOutput();
    }
}

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
        _M << sp << nl << "+(BOOL)" << opName << "___:(id)servant current:(ICECurrent *)current " 
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
	    _M << nl << "return [" << q->second << " " << q->first << "___:self current:current is:is os:os];";
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

    _M << sp << nl << "+(NSString **) staticIds__:(int*)count idIndex:(int*)idx";
    _M << sb;
    _M << nl << "*count = sizeof(" << name << "_ids__) / sizeof(NSString *);";
    _M << nl << "*idx = " << scopedPos << ";";
    _M << nl << "return " << name << "_ids__;";
    _M << eb;

#if 0
    // Marshalling support
    DataMemberList allClassMembers = p->allClassDataMembers();
    DataMemberList::const_iterator d;
    DataMemberList members = p->dataMembers();
    DataMemberList classMembers = p->classDataMembers();
    ClassList bases = p->bases();
    bool hasBaseClass = !bases.empty() && !bases.front()->isInterface();

    _M << sp << nl << "#region Marshaling support";

    _M << sp << nl << "public override void write__(IceInternal.BasicStream os__)";
    _M << sb;
    _M << nl << "os__.writeTypeId(ice_staticId());";
    _M << nl << "os__.startWriteSlice();";
    for(d = members.begin(); d != members.end(); ++d)
    {
        writeMarshalUnmarshalCode(_M, (*d)->type(), fixId(*d, ObjC::NSObject, true),
                                  true, false, false); // TODO: 2nd param
    }
    _M << nl << "os__.endWriteSlice();";
    _M << nl << "base.write__(os__);";
    _M << eb;

    if(allClassMembers.size() != 0)
    {
        _M << sp << nl << "public sealed ";
        if(hasBaseClass && bases.front()->declaration()->usesClasses())
        {
            _M << "new ";
        }
        _M << "class Patcher__ : IceInternal.Patcher<" << name << ">";
        _M << sb;
        _M << sp << nl << "internal Patcher__(string type, Ice.ObjectImpl instance";
        if(allClassMembers.size() > 1)
        {
            _M << ", int member";
        }
        _M << ") : base(type)";
        _M << sb;
        _M << nl << "_instance = (" << name << ")instance;";
        if(allClassMembers.size() > 1)
        {
            _M << nl << "_member = member;";
        }
        _M << eb;

        _M << sp << nl << "public override void patch(Ice.Object v)";
        _M << sb;
        _M << nl << "try";
        _M << sb;
        if(allClassMembers.size() > 1)
        {
            _M << nl << "switch(_member)";
            _M << sb;
        }
        int memberCount = 0;
        for(d = allClassMembers.begin(); d != allClassMembers.end(); ++d)
        {
            if(allClassMembers.size() > 1)
            {
                _M.dec();
                _M << nl << "case " << memberCount << ":";
                _M.inc();
            }
            string memberName = fixId((*d)->name(), ObjC::NSObject, true); // TODO: 2nd param
            string memberType = typeToString((*d)->type());
            _M << nl << "_instance." << memberName << " = (" << memberType << ")v;";
            ContainedPtr contained = ContainedPtr::dynamicCast((*d)->type());
            string sliceId = contained ? contained->scoped() : string("::Ice::Object");
            _M << nl << "_typeId = \"" << sliceId << "\";";
            if(allClassMembers.size() > 1)
            {
                _M << nl << "break;";
            }
            memberCount++;
        }
        if(allClassMembers.size() > 1)
        {
            _M << eb;
        }
        _M << eb;
        _M << nl << "catch(System.InvalidCastException)";
        _M << sb;
        _M << nl << "IceInternal.Ex.throwUOE(_typeId, v.ice_id());";
        _M << eb;
        _M << eb;

        _M << sp << nl << "private " << name << " _instance;";
        if(allClassMembers.size() > 1)
        {
            _M << nl << "private int _member;";
        }
        _M << nl << "private string _typeId;";
        _M << eb;
    }

    _M << sp << nl << "public override void read__(IceInternal.BasicStream is__, bool rid__)";
    _M << sb;
    _M << nl << "if(rid__)";
    _M << sb;
    _M << nl << "/* string myId = */ is__.readTypeId();";
    _M << eb;
    _M << nl << "is__.startReadSlice();";
    int classMemberCount = static_cast<int>(allClassMembers.size() - classMembers.size());
    for(d = members.begin(); d != members.end(); ++d)
    {
        ostringstream patchParams;
        patchParams << "this";
	if(isClass((*d)->type())
        {
            if(classMembers.size() > 1 || allClassMembers.size() > 1)
            {
                patchParams << ", " << classMemberCount++;
            }
        }
        writeMarshalUnmarshalCode(_M, (*d)->type(), fixId(*d, ObjC::NSObject, true),
                                  false, false, false, patchParams.str()); // TODO: ObjC::NSObject
    }
    _M << nl << "is__.endReadSlice();";
    _M << nl << "base.read__(is__, true);";
    _M << eb;

    //
    // Write streaming API.
    //
    if(stream)
    {
        _M << sp << nl << "public override void write__(Ice.OutputStream outS__)";
        _M << sb;
        _M << nl << "outS__.writeTypeId(ice_staticId());";
        _M << nl << "outS__.startSlice();";
        for(d = members.begin(); d != members.end(); ++d)
        {
            writeMarshalUnmarshalCode(_M, (*d)->type(), fixId(*d, ObjC::NSObject, true),
                                      true, true, false); // TODO: Objc::NSObject
        }
        _M << nl << "outS__.endSlice();";
        _M << nl << "base.write__(outS__);";
        _M << eb;

        _M << sp << nl << "public override void read__(Ice.InputStream inS__, bool rid__)";
        _M << sb;
        _M << nl << "if(rid__)";
        _M << sb;
        _M << nl << "/* string myId = */ inS__.readTypeId();";
        _M << eb;
        _M << nl << "inS__.startSlice();";
        for(d = members.begin(); d != members.end(); ++d)
        {
            ostringstream patchParams;
            patchParams << "this";
            if(isClass((*d)->type()))
            {
                if(classMembers.size() > 1 || allClassMembers.size() > 1)
                {
                    patchParams << ", " << classMemberCount++;
                }
            }
            writeMarshalUnmarshalCode(_M, (*d)->type(), fixId(*d, ObjC::NSObject, true),
                                      false, true, false, patchParams.str()); // TODO: ObjC::NSObject
        }
        _M << nl << "inS__.endSlice();";
        _M << nl << "base.read__(inS__, true);";
        _M << eb;
    }
    else
    {
        //
        // Emit placeholder functions to catch errors.
        //
        string scoped = p->scoped();
        _M << sp << nl << "public override void write__(Ice.OutputStream outS__)";
        _M << sb;
        _M << nl << "Ice.MarshalException ex = new Ice.MarshalException();";
        _M << nl << "ex.reason = \"type " << scoped.substr(2) << " was not generated with stream support\";";
        _M << nl << "throw ex;";
        _M << eb;

        _M << sp << nl << "public override void read__(Ice.InputStream inS__, bool rid__)";
        _M << sb;
        _M << nl << "Ice.MarshalException ex = new Ice.MarshalException();";
        _M << nl << "ex.reason = \"type " << scoped.substr(2) << " was not generated with stream support\";";
        _M << nl << "throw ex;";
        _M << eb;
    }

    _M << sp << nl << "#endregion"; // Marshalling support
#endif
}

#if 0
string
Slice::ObjCVisitor::getParamAttributes(const ParamDeclPtr& p)
{
    string result;
    StringList metaData = p->getMetaData();
    for(StringList::const_iterator i = metaData.begin(); i != metaData.end(); ++i)
    {
        static const string prefix = "cs:attribute:";
        if(i->find(prefix) == 0)
        {
            result += "[" + i->substr(prefix.size()) + "] ";
        }
    }
    return result;
}
#endif

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
    _gen = this;
    SignalHandler::setCallback(closeCallback);

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
    SignalHandler::addFile(fileH);
    SignalHandler::addFile(fileM);
    SignalHandler::addFile(fileImplH);
    SignalHandler::addFile(fileImplM);

    _H.open(fileH.c_str());
    if(!_H)
    {
        cerr << name << ": can't open `" << fileH << "' for writing" << endl;
        return;
    }
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
        cerr << name << ": can't open `" << fileM << "' for writing" << endl;
        return;
    }
    printHeader(_M);
    _M << nl << "// Generated from file `" << _base << ".ice'";

    if(impl || implTie)
    {
        struct stat st;
        if(stat(fileImplH.c_str(), &st) == 0)
        {
            cerr << name << ": `" << fileImplH << "' already exists--will not overwrite" << endl;
            return;
        }
        _implH.open(fileImplH.c_str());
        if(!_implH)
        {
            cerr << name << ": can't open `" << fileImplH << "' for writing" << endl;
            return;
        }

        if(stat(fileImplM.c_str(), &st) == 0)
        {
            cerr << name << ": `" << fileImplM << "' already exists--will not overwrite" << endl;
            return;
        }
        _implM.open(fileImplM.c_str());
        if(!_implM)
        {
            cerr << name << ": can't open `" << fileImplM << "' for writing" << endl;
            return;
        }
    }
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

    SignalHandler::setCallback(0);
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

    ProxyHelperVisitor proxyHelperVisitor(_H, _M, _stream);
    p->visit(&proxyHelperVisitor, false);

    HelperVisitor HelperVisitor(_H, _M, _stream);
    p->visit(&HelperVisitor, false);

#if 0
    OpsVisitor opsVisitor(_M);
    p->visit(&opsVisitor, false);


    DelegateVisitor delegateVisitor(_M);
    p->visit(&delegateVisitor, false);


    DelegateDVisitor delegateDVisitor(_M);
    p->visit(&delegateDVisitor, false);

    DispatcherVisitor dispatcherVisitor(_M, _stream);
    p->visit(&dispatcherVisitor, false);

    AsyncVisitor asyncVisitor(_M);
    p->visit(&asyncVisitor, false);
#endif
}

void
Slice::Gen::generateTie(const UnitPtr& p)
{
    TieVisitor tieVisitor(_M);
    p->visit(&tieVisitor, false);
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
"// ICE_LICENSE file included in this distribution.\n"
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
    return false;
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
    // Constructors.
    //
    if(!dataMembers.empty())
    {
        _H << sp;
    }
    _H << nl << "-(id) init";
    _M << sp << nl << "-(id) init";
    writeMemberSignature(allDataMembers, 0, Other, HAndM); // TODO fix second parameter
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

    if(!allDataMembers.empty())
    {
	string lowerCaseName = fixId(p->name());
	*(lowerCaseName.begin()) = tolower(*lowerCaseName.begin());

	_H << nl << "+(id) " << lowerCaseName;
	_M << sp << nl << "+(id) " << lowerCaseName;
	writeMemberSignature(allDataMembers, 0, Other, HAndM); // TODO fix second parameter
	_H << ";";
	_M << sb;

	//
	// The cast avoids a compiler warning that is emitted if different structs
	// have members with the same name but different types.
	//
	_M << nl << name << " *s__ = [((" << name << " *)[" << name << " alloc]) init";
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
	// copy__
	//
	if(!dataMembers.empty())
	{
	    _H << nl << "-(void) copy__:(" << name << " *)copy_;";
	    _M << sp << nl << "-(void) copy__:(" << name << "*)copy_";
	    _M << sb;
	    _M << nl << "[super copy__:copy_];";
	    writeMemberCopy(p, dataMembers, 0); // TODO: fix third parameter
	    _M << eb;
	}

	//
	// dealloc
	//
	writeMemberDealloc(dataMembers, 0); // TODO fix second parameter
    }

    //
    // Operations
    //
    OperationList ops = p->operations();
    OperationList::const_iterator r;
    for(r = ops.begin(); r != ops.end(); ++r)
    {
        OperationPtr op = *r;
        _H << nl << "+(BOOL)" << fixId(op->name()) << "___:(id)servant current:(ICECurrent *)current " 
           << "is:(id<ICEInputStream>)is_ os:(id<ICEOutputStream>)os_;";
    }

    //
    // Marshaling/unmarshaling
    //
    _H << nl << "-(void) write__:(id<ICEOutputStream>)stream;";

    _M << sp << nl << "-(void) write__:(id<ICEOutputStream>)os_";
    _M << sb;
    _M << nl << "[os_ writeTypeId:@\"" << p->scoped() << "\"];";
    _M << nl << "[os_ startSlice];";
    writeMemberMarshal("self->", dataMembers, 0); // TODO fix second parameter
    _M << nl << "[os_ endSlice];";
    _M << nl << "[super write__:os_];";
    _M << eb;

    _H << nl << "-(void) read__:(id<ICEInputStream>)stream readTypeId:(BOOL)rid_;";
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

#if 0
    DataMemberList classMembers = p->classDataMembers();
    DataMemberList allClassMembers = p->allClassDataMembers();
    DataMemberList dataMembers = p->dataMembers();
    DataMemberList allDataMembers = p->allDataMembers();
    DataMemberList::const_iterator d;

    if(!p->isInterface())
    {
        if(!allDataMembers.empty())
        {
            _M << sp << nl << "public " << name << spar << epar;
            if(hasBaseClass)
            {
                _M << " : base()";
            }
            _M << sb;
            _M << eb;

            _M << sp << nl << "public " << name << spar;
            vector<string> paramDecl;
            for(d = allDataMembers.begin(); d != allDataMembers.end(); ++d)
            {
                string memberName = fixId((*d)->name());
                string memberType = typeToString((*d)->type());
                paramDecl.push_back(memberType + " " + memberName);
            }
            _M << paramDecl << epar;
            if(hasBaseClass && allDataMembers.size() != dataMembers.size())
            {
                _M << " : base" << spar;
                vector<string> baseParamNames;
                DataMemberList baseDataMembers = bases.front()->allDataMembers();
                for(d = baseDataMembers.begin(); d != baseDataMembers.end(); ++d)
                {
                    baseParamNames.push_back(fixId((*d)->name()));
                }
                _M << baseParamNames << epar;
            }
            _M << sb;
            vector<string> paramNames;
            for(d = dataMembers.begin(); d != dataMembers.end(); ++d)
            {
                paramNames.push_back(fixId((*d)->name()));
            }
            for(vector<string>::const_iterator i = paramNames.begin(); i != paramNames.end(); ++i)
            {
                _M << nl << "this." << *i << " = " << *i << ';';
            }
            _M << eb;

            _M << sp << nl << "#endregion"; // Constructors
        }

        writeInheritedOperations(p);
    }
#endif

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
	_H << nl << "@private";
	_H.inc();
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

    const int baseTypes = ObjC::NSException;

    // TODO: deprecate metadata

    if(!dataMembers.empty())
    {
	//
	// Data member declarations.
	//
	writeMembers(dataMembers, baseTypes);

	_H.dec();
	_H << eb;
	_H << sp;
	_M << sp;

	//
	// @property and @synthesize for each data member.
	//
	writeProperties(dataMembers, baseTypes);
	writeSynthesize(dataMembers, baseTypes);
    }

    if(!dataMembers.empty())
    {
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
    if(p->isLocal())
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
    if(!allDataMembers.empty())
    {
	_H << nl << "-(id) init";
	_M << sp << nl << "-(id) init";
	writeMemberSignature(allDataMembers, baseTypes, p->isLocal() ? LocalException : Other, HAndM);
	_H << ";";
	_M << sb;
	if(!p->base())
	{
	    _M << nl << "if(![super initWithName:[self ice_name] reason:nil userInfo:nil])";
	}
	else
	{
	    _M << nl << "if(![super init";
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
    writeMemberSignature(allDataMembers, baseTypes, p->isLocal() ? LocalException : Other, HAndM);
    _H << ";";

    //
    // The cast avoids a compiler warning that is emitted if different exceptions
    // have members with the same name but different types.
    //
    _M << sb;
    _M << nl << name << " *s__ = [((" << name << " *)[" << name << " alloc]) init";
    writeMemberCall(allDataMembers, baseTypes, p->isLocal() ? LocalException : Other, WithEscape);
    _M << "];";
    _M << nl << "[s__ autorelease];";
    _M << nl << "return s__;";
    _M << eb;

    if (!allDataMembers.empty())
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
    // copy__
    //
    if(!dataMembers.empty())
    {
	_H << nl << "-(void) copy__:(" << name << " *)copy_;";
	_M << sp << nl << "-(void) copy__:(" << name << "*)copy_";
	_M << sb;
	_M << nl << "[super copy__:copy_];";
	writeMemberCopy(p, dataMembers, baseTypes);
	_M << eb;
    }

    //
    // dealloc
    //
    writeMemberDealloc(dataMembers, baseTypes);

    //
    // Marshaling/unmarshaling
    //
    ExceptionPtr base = p->base();
    if(!p->allClassDataMembers().empty())
    {
	if(!base || (base && !base->usesClasses()))
	{
	    _H << nl << "-(BOOL) usesClasses__;";
	    _M << sp << nl << "-(BOOL) usesClasses__";
	    _M << sb;
	    _M << nl << "return YES;";
	    _M << eb;
	}
    }

    if(!p->isLocal())
    {
	_H << nl << "-(void) write__:(id<ICEOutputStream>)stream;";
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

	_H << nl << "-(void) read__:(id<ICEInputStream>)stream readTypeId:(BOOL)rid_;";
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
    // Constructors.
    //
    _H << sp << nl << "-(id) init";
    _M << sp << nl << "-(id) init";
    writeMemberSignature(dataMembers, baseTypes, Other, HAndM);
    _H << ";";
    _M << sb;
    _M << nl << "if(![super init])";
    _M << sb;
    _M << nl << "return nil;";
    _M << eb;
    writeMemberInit(dataMembers, baseTypes);
    _M << nl << "return self;";
    _M << eb;

    string lowerCaseName = fixId(p->name());
    *(lowerCaseName.begin()) = tolower(*lowerCaseName.begin());

    _H << nl << "+(id) " << lowerCaseName;
    _M << sp << nl << "+(id) " << lowerCaseName;
    writeMemberSignature(dataMembers, baseTypes, Other, HAndM);
    _H << ";";
    _M << sb;

    //
    // The cast avoids a compiler warning that is emitted if different structs
    // have members with the same name but different types.
    //
    _M << nl << name << " *s__ = [((" << name << " *)[" << name << " alloc]) init";
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
    _H << nl << "-(id) copyWithZone:(NSZone *)zone;";

    _M << sp << nl << "-(id) copyWithZone:(NSZone *)zone";
    _M << sb;
    _M << nl << name << " *copy_ = [" << name << " allocWithZone:zone];";
    writeMemberCopy(p, dataMembers, baseTypes); 
    _M << nl << "return copy_;";
    _M << eb;

    //
    // hash
    //
    writeMemberHashCode(dataMembers, baseTypes);

    //
    // isEqual
    //
    _H << nl << "-(BOOL) isEqual:(id)anObject;";

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
    _M << nl << "p = [[" << name << " alloc] init];";
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
    _M << nl << name << "*" << " p = [[self alloc] init];";
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
    _H << nl << "static const " << typeToString(p->type()) << " ";
    if(mapsToPointerType(p->type()))
    {
        _H << "*";
    }
    _H << fixName(p) << " = ";

    BuiltinPtr bp = BuiltinPtr::dynamicCast(p->type());
    if(bp && bp->kind() == Builtin::KindString)
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
Slice::Gen::TypesVisitor::visitDataMember(const DataMemberPtr& p)
{
#if 0
    int baseTypes = 0;
    bool isClass = false;
    bool propertyMapping = false;
    bool isValue = false;
    bool isProtected = false;
    ContainedPtr cont = ContainedPtr::dynamicCast(p->container());
    assert(cont);
    if(StructPtr::dynamicCast(cont))
    {
        isValue = isValueType(StructPtr::dynamicCast(cont));
        if(!isValue || cont->hasMetaData("clr:class"))
        {
            baseTypes = ObjC::NSObject; // TODO
        }
        if(cont->hasMetaData("clr:property"))
        {
            propertyMapping = true;
        }
    }
    else if(ExceptionPtr::dynamicCast(cont))
    {
        baseTypes = ObjC::Exception;
    }
    else if(ClassDefPtr::dynamicCast(cont))
    {
        baseTypes = ObjC::NSObject; // TODO
        isClass = true;
        if(cont->hasMetaData("clr:property"))
        {
            propertyMapping = true;
        }
        isProtected = cont->hasMetaData("protected") || p->hasMetaData("protected");
    }

    _M << sp;

    emitDeprecate(p, cont, _M, "member");

    emitAttributes(p);

    string type = typeToString(p->type());
    string propertyName = fixId(p->name(), baseTypes, isClass);
    string dataMemberName = propertyName;
    if(propertyMapping)
    {
        dataMemberName += "_prop";
    }

    _M << nl;
    if(propertyMapping)
    {
        _M << "private";
    }
    else if(isProtected)
    {
        _M << "protected";
    }
    else
    {
        _M << "public";
    }
    _M << ' ' << type << ' ' << dataMemberName << ';';

    if(!propertyMapping)
    {
        return;
    }

    _M << nl << (isProtected ? "protected" : "public");
    if(!isValue)
    {
        _M << " virtual";
    }
    _M << ' ' << type << ' ' << propertyName;
    _M << sb;
    _M << nl << "get";
    _M << sb;
    _M << nl << "return " << dataMemberName << ';';
    _M << eb;
    _M << nl << "set";
    _M << sb;
    _M << nl << dataMemberName << " = value;";
    _M << eb;
    _M << eb;
#endif
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
                                               ContainerType ct, Destination d) const
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
	    _M << " " << name;
	}
	_M << ":(" << typeString;
	if(mapsToPointerType(type))
	{
	    _M << " *";
	}
	_M << ")" << fixId((*q)->name()) << "_p";

	if(d == HAndM)
	{
	    if(q != dataMembers.begin() || ct == LocalException)
	    {
		_H << " " << name;
	    }
	    _H << ":(" << typeString;
	    if(mapsToPointerType(type))
	    {
		_H << " *";
	    }
	    _H << ")" << fixId((*q)->name());
	}
    }
}

void
Slice::Gen::TypesVisitor::writeMemberCall(const DataMemberList& dataMembers, int baseTypes,
                                          ContainerType ct, Escape esc) const
{
    if(ct == LocalException)
    {
        _M << ":file__p line:line__p";
    }
    for(DataMemberList::const_iterator q = dataMembers.begin(); q != dataMembers.end(); ++q)
    {
        string name = (*q)->name();

	if(q != dataMembers.begin() || ct == LocalException)
	{
	    _M << " " << fixId(name, baseTypes);
	}
	_M << ":" << fixId(name);
	if(esc == WithEscape)
	{
	    _M << "_p";
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
Slice::Gen::TypesVisitor::writeMemberCopy(const SyntaxTreeBasePtr& parent, const DataMemberList& dataMembers,
                                          int baseTypes) const
{
    StructPtr st = StructPtr::dynamicCast(parent);
    ClassDefPtr cl = ClassDefPtr::dynamicCast(parent);
    ExceptionPtr ex = ExceptionPtr::dynamicCast(parent);
    ClassDefPtr baseCl;
    ExceptionPtr baseEx;
    bool hasBase;
    if(st)
    {
	hasBase = false;
    }
    else if(cl)
    {
	ClassList bases = cl->bases();
	if(!bases.empty() && !bases.front()->isInterface())
	{
	    baseCl = bases.front();
	}
    }
    else if(ex)
    {
        baseEx = ex->base();
    }
    else
    {
	assert(false); // Nothing else has bases
    }

    for(DataMemberList::const_iterator q = dataMembers.begin(); q != dataMembers.end(); ++q)
    {
	string name = fixId((*q)->name(), baseTypes);
	if(isValueType((*q)->type()))
	{
	    _M << nl << "copy_->" << name << " = " << name << ";";
	}
	else if(isProtocolType((*q)->type()))
	{
	    // Cast to NSObject to prevent warning (copy isn't part of the NSObject protocol).
	    _M << nl << "copy_->" << name << " = [(NSObject*)" << name << " retain];";
	}
	else
	{
	    _M << nl << "copy_->" << name << " = [" << name << " copy];";
	}
    }
}

void
Slice::Gen::TypesVisitor::writeMemberHashCode(const DataMemberList& dataMembers, int baseTypes) const
{
    _H << nl << "-(NSUInteger) hash;";

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
	    _M << name << ";";
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
		_H << nl << "-(void) dealloc;";
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
    }
}

void
Slice::Gen::TypesVisitor::writeMemberMarshal(const string& instance, const DataMemberList& dataMembers,
                                             int baseTypes) const
{
    for(DataMemberList::const_iterator q = dataMembers.begin(); q != dataMembers.end(); ++q)
    {
	writeMarshalUnmarshalCode(_M, (*q)->type(), instance + fixId((*q)->name()), true, false, false);
    }
}

void
Slice::Gen::TypesVisitor::writeMemberUnmarshal(const string& instance, const DataMemberList& dataMembers,
                                               int baseTypes) const
{
    for(DataMemberList::const_iterator q = dataMembers.begin(); q != dataMembers.end(); ++q)
    {
	writeMarshalUnmarshalCode(_M, (*q)->type(), instance + fixId((*q)->name()
	                          /* TODO: base classes */), false, false, false);
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
    _H << nl << "@end;";
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

Slice::Gen::OpsVisitor::OpsVisitor(Output& out)
    : ObjCVisitor(out, out)
{
}

bool
Slice::Gen::OpsVisitor::visitModuleStart(const ModulePtr& p)
{
    if(!p->hasAbstractClassDefs())
    {
        return false;
    }

    _M << sp << nl << "namespace " << fixId(p->name());
    _M << sb;
    return true;
}

void
Slice::Gen::OpsVisitor::visitModuleEnd(const ModulePtr&)
{
    _M << eb;
}

bool
Slice::Gen::OpsVisitor::visitClassDefStart(const ClassDefPtr& p)
{
    //
    // Don't generate Operations interfaces for non-abstract classes.
    //
    if(!p->isAbstract())
    {
        return false;
    }

    if(!p->isLocal())
    {
        writeOperations(p, false);
    }
    writeOperations(p, true);

    return false;
}

void
Slice::Gen::OpsVisitor::writeOperations(const ClassDefPtr& p, bool noCurrent)
{
    string name = p->name();
    string scoped = fixId(p->scoped());
    ClassList bases = p->bases();
    string opIntfName = "Operations";
    if(noCurrent || p->isLocal())
    {
        opIntfName += "NC";
    }

    _M << sp << nl << "public interface " << name << opIntfName << '_';
    if((bases.size() == 1 && bases.front()->isAbstract()) || bases.size() > 1)
    {
        _M << " : ";
        ClassList::const_iterator q = bases.begin();
        bool first = true;
        while(q != bases.end())
        {
            if((*q)->isAbstract())
            {
                if(!first)
                {
                    _M << ", ";
                }
                else
                {
                    first = false;
                }
                string s = (*q)->scoped();
                s += "Operations";
                if(noCurrent)
                {
                    s += "NC";
                }
                _M << fixId(s) << '_';
            }
            ++q;
        }
    }
    _M << sb;

    OperationList ops = p->operations();
    OperationList::const_iterator r;
    for(r = ops.begin(); r != ops.end(); ++r)
    {
        OperationPtr op = *r;
        bool amd = !p->isLocal() && (p->hasMetaData("amd") || op->hasMetaData("amd"));
        string opname = amd ? (op->name() + "_async") : fixId(op->name(), ObjC::NSObject, true); // TODO: NSObject
        
        TypePtr ret;
        vector<string> params;

        if(amd)
        {
            //params = getParamsAsync(op, true);
        }
        else
        {
            //params = getParams(op);
            ret = op->returnType();
        }

        _M << sp;

        emitDeprecate(op, p, _M, "operation");

        emitAttributes(op);
        string retS = typeToString(ret);
        _M << nl << retS << ' ' << opname << spar << params;
        if(!noCurrent && !p->isLocal())
        { 
            _M << "Ice.Current current__";
        }
        _M << epar << ';';
    }

    _M << eb;
}

Slice::Gen::ProxyHelperVisitor::ProxyHelperVisitor(Output& H, Output& M, bool stream)
    : ObjCVisitor(H, M), _stream(stream)
{
}

bool
Slice::Gen::ProxyHelperVisitor::visitClassDefStart(const ClassDefPtr& p)
{
#if 0
    string name = fixName(p) + "PrxHelper";
    ClassList bases = p->bases();

    _H << sp << nl << "@interface " << name;
    _H << nl << "+(void) ice_writeWithStream:(id<ICEOutputStream>)os proxy:(" << fixName(p) + "Prx" << " *)proxy;";
    _H << nl << "+(void) ice_readWithStream:(id<ICEInputStream>)is;";
    _H << nl << "@end";

    _M << sp << nl << "@implementation " << name;
    _M << nl << "+(void) ice_writeWithStream:(id<ICEOutputStream>)os proxy:(" << fixName(p) + "Prx" << " *)proxy";
    _M << sb;
    _M << eb;

    _M << sp << nl << "+(void) ice_readWithStream:(id<ICEInputStream>)is";
    _M << sb;
    _M << eb;


    // TODO: if(_stream)
#endif
    return true;
}

void
Slice::Gen::ProxyHelperVisitor::visitClassDefEnd(const ClassDefPtr&)
{
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

void
Slice::Gen::ProxyHelperVisitor::visitDictionary(const DictionaryPtr& p)
{
}

Slice::Gen::DelegateVisitor::DelegateVisitor(Output& out)
    : ObjCVisitor(out, out)
{
}

bool
Slice::Gen::DelegateVisitor::visitModuleStart(const ModulePtr& p)
{
    if(!p->hasNonLocalClassDecls())
    {
        return false;
    }

    _M << sp << nl << "namespace " << fixId(p->name());
    _M << sb;
    return true;
}

void
Slice::Gen::DelegateVisitor::visitModuleEnd(const ModulePtr&)
{
    _M << eb;
}

bool
Slice::Gen::DelegateVisitor::visitClassDefStart(const ClassDefPtr& p)
{
    if(p->isLocal())
    {
        return false;
    }

    string name = p->name();
    ClassList bases = p->bases();

    _M << sp << nl << "public interface " << name << "Del_ : ";
    if(bases.empty())
    {
        _M << "Ice.ObjectDel_";
    }
    else
    {
        ClassList::const_iterator q = bases.begin();
        while(q != bases.end())
        {
            string s = (*q)->scoped();
            s += "Del_";
            _M << fixId(s);
            if(++q != bases.end())
            {
                _M << ", ";
            }
        }
    }

    _M << sb;

    OperationList ops = p->operations();

    OperationList::const_iterator r;
    for(r = ops.begin(); r != ops.end(); ++r)
    {
        OperationPtr op = *r;
        string opName = fixId(op->name(), ObjC::NSObject, true); // TODO: NSObject
        TypePtr ret = op->returnType();
        string retS = typeToString(ret);
        vector<string> params;// = getParams(op);

        _M << sp << nl << retS << ' ' << opName << spar << params
             << "_System.Collections.Generic.Dictionary<string, string> context__" << epar << ';';
    }

    return true;
}

void
Slice::Gen::DelegateVisitor::visitClassDefEnd(const ClassDefPtr&)
{
    _M << eb;
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
        _M << nl << retString << " ";
        if(retIsPointer)
        {
            _M << "*ret_ = nil;";
        }
        else
        {
            _M << "ret_;";
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

Slice::Gen::DelegateDVisitor::DelegateDVisitor(Output& out)
    : ObjCVisitor(out, out)
{
}

bool
Slice::Gen::DelegateDVisitor::visitModuleStart(const ModulePtr& p)
{
    if(!p->hasNonLocalClassDecls())
    {
        return false;
    }

    _M << sp << nl << "namespace " << fixId(p->name());
    _M << sb;
    return true;
}

void
Slice::Gen::DelegateDVisitor::visitModuleEnd(const ModulePtr&)
{
    _M << eb;
}

bool
Slice::Gen::DelegateDVisitor::visitClassDefStart(const ClassDefPtr& p)
{
    if(p->isLocal())
    {
        return false;
    }

    string name = p->name();
    ClassList bases = p->bases();

    _M << sp << nl << "public sealed class " << name << "DelD_ : Ice.ObjectDelD_, " << name << "Del_";
    _M << sb;

    OperationList ops = p->allOperations();

    OperationList::const_iterator r;
    for(r = ops.begin(); r != ops.end(); ++r)
    {
        OperationPtr op = *r;
        string opName = fixId(op->name(), ObjC::NSObject, true); // TODO: NSObject
        TypePtr ret = op->returnType();
        string retS = typeToString(ret);
        ClassDefPtr containingClass = ClassDefPtr::dynamicCast(op->container());

        ExceptionList throws = op->throws();
        throws.sort();
        throws.unique();

        //
        // Arrange exceptions into most-derived to least-derived order. If we don't
        // do this, a base exception handler can appear before a derived exception
        // handler, causing compiler warnings.
        //
#if defined(__SUNPRO_CC)
        throws.sort(Slice::derivedToBaseCompare);
#else
        throws.sort(Slice::DerivedToBaseCompare());
#endif

        vector<string> params;// = getParams(op);
        vector<string> args;// = getArgs(op);

        _M << sp;
        _M << nl << "public " << retS << ' ' << opName << spar << params
             << "_System.Collections.Generic.Dictionary<string, string> context__" << epar;
        _M << sb;
        if(containingClass->hasMetaData("amd") || op->hasMetaData("amd"))
        {
            _M << nl << "throw new Ice.CollocationOptimizationException();";
        }
        else
        {
            _M << nl << "Ice.Current current__ = new Ice.Current();";
            _M << nl << "initCurrent__(ref current__, \"" << op->name() << "\", " 
                 << sliceModeToIceMode(op->sendMode())
                 << ", context__);";
            
          
            //
            // Create out holders and delArgs
            //
            vector<string> delArgs;
            vector<string> outHolders;

            const ParamDeclList paramList = op->parameters();
            for(ParamDeclList::const_iterator q = paramList.begin(); q != paramList.end(); ++q)
            {
                string arg = fixId((*q)->name());
                if((*q)->isOutParam())
                {
                    _M << nl << typeToString((*q)->type()) << " " << arg << "Holder__ = " << writeValue((*q)->type()) << ";";
                    outHolders.push_back(arg);
                    arg = "out " + arg + "Holder__";
                }
                delArgs.push_back(arg);
            }
            
            if(ret)
            {
                _M << nl << retS << " result__ = " << writeValue(ret) << ";";
            }
       
            if(!throws.empty())
            {
                _M << nl << "Ice.UserException userException__ = null;";
            }

            _M << nl << "IceInternal.Direct.RunDelegate run__ = delegate(Ice.Object obj__)";
            _M << sb;
            _M << nl << fixId(name) << " servant__ = null;";
            _M << nl << "try";
            _M << sb;
            _M << nl << "servant__ = (" << fixId(name) << ")obj__;";
            _M << eb;
            _M << nl << "catch(_System.InvalidCastException)";
            _M << sb;
            _M << nl << "throw new Ice.OperationNotExistException(current__.id, current__.facet, current__.operation);";
            _M << eb;

            if(!throws.empty())
            {
                _M << nl << "try";
                _M << sb;
            }

            _M << nl;
            if(ret)
            {
                _M << "result__ = ";
            }
           
            _M << "servant__." << opName << spar << delArgs << "current__" << epar << ';';
            _M << nl << "return Ice.DispatchStatus.DispatchOK;";
            
            if(!throws.empty())
            {
                _M << eb;
                _M << nl << "catch(Ice.UserException ex__)";
                _M << sb;
                _M << nl << "userException__ = ex__;";
                _M << nl << "return Ice.DispatchStatus.DispatchUserException;";
                _M << eb;
            }

            _M << eb;
            _M << ";";

            _M << nl << "IceInternal.Direct direct__ = null;";
            _M << nl << "try";
            _M << sb;
            _M << nl << "direct__ = new IceInternal.Direct(current__, run__);";

            _M << nl << "try";
            _M << sb;
            
            _M << nl << "Ice.DispatchStatus status__ = direct__.servant().collocDispatch__(direct__);";
            if(!throws.empty())
            {
                _M << nl << "if(status__ == Ice.DispatchStatus.DispatchUserException)";
                _M << sb;
                _M << nl << "throw userException__;";
                _M << eb;
            }
            _M << nl << "_System.Diagnostics.Debug.Assert(status__ == Ice.DispatchStatus.DispatchOK);";

            _M << eb;
            _M << nl << "finally";
            _M << sb;
            _M << nl << "direct__.destroy();";
            _M << eb;
            _M << eb;

            for(ExceptionList::const_iterator i = throws.begin(); i != throws.end(); ++i)
            {
                _M << nl << "catch(" << fixId((*i)->scoped()) << ')';
                _M << sb;
                _M << nl << "throw;";
                _M << eb;
            }
            _M << nl << "catch(Ice.SystemException)";
            _M << sb;
            _M << nl << "throw;";
            _M << eb;
            _M << nl << "catch(System.Exception ex__)";
            _M << sb;
            _M << nl << "IceInternal.LocalExceptionWrapper.throwWrapper(ex__);";
            _M << eb;
            
            //
            //
            // Set out parameters
            //
            for(vector<string>::iterator s = outHolders.begin(); s != outHolders.end(); ++s)
            {
                _M << nl << (*s) << " = " << (*s) << "Holder__;";
            }
            if(ret && !containingClass->hasMetaData("amd") && !op->hasMetaData("amd"))
            {
                _M << nl << "return result__;";
            }
        }
        _M << eb;
    }

    return true;
}

void
Slice::Gen::DelegateDVisitor::visitClassDefEnd(const ClassDefPtr&)
{
    _M << eb;
}

Slice::Gen::DispatcherVisitor::DispatcherVisitor(Output &out, bool stream)
    : ObjCVisitor(out, out), _stream(stream)
{
}

bool
Slice::Gen::DispatcherVisitor::visitModuleStart(const ModulePtr& p)
{
    if(!p->hasNonLocalClassDecls())
    {
        return false;
    }

    _M << sp << nl << "namespace " << fixId(p->name());
    _M << sb;
    return true;
}

void
Slice::Gen::DispatcherVisitor::visitModuleEnd(const ModulePtr&)
{
    _M << eb;
}

bool
Slice::Gen::DispatcherVisitor::visitClassDefStart(const ClassDefPtr& p)
{
    if(p->isLocal() || !p->isInterface())
    {
        return false;
    }

    string name = p->name();

    _M << sp << nl << "public abstract class " << name << "Disp_ : Ice.ObjectImpl, " << fixId(name);
    _M << sb;

    OperationList ops = p->operations();
    if(!ops.empty())
    {
        _M << sp << nl << "#region Slice operations";
    }

    for(OperationList::const_iterator op = ops.begin(); op != ops.end(); ++op)
    {
        bool amd = p->hasMetaData("amd") || (*op)->hasMetaData("amd");

        string opname = (*op)->name();
        vector<string> params;
        vector<string> args;
        TypePtr ret;

        if(amd)
        {
            opname = opname + "_async";
            //params = getParamsAsync(*op, true);
            //args = getArgsAsync(*op);
        }
        else
        {
            opname = fixId(opname, ObjC::NSObject, true); // TODO: NSObject
            //params = getParams(*op);
            ret = (*op)->returnType();
            //args = getArgs(*op);
        }

        _M << sp << nl << "public " << typeToString(ret) << " " << opname << spar << params << epar;
        _M << sb << nl;
        if(ret)
        {
            _M << "return ";
        }
        _M << opname << spar << args << "Ice.ObjectImpl.defaultCurrent" << epar << ';';
        _M << eb;

        _M << sp << nl << "public abstract " << typeToString(ret) << " " << opname << spar << params;
        if(!p->isLocal())
        {
            _M << "Ice.Current current__";
        }
        _M << epar << ';';
    }

    if(!ops.empty())
    {
        _M << sp << nl << "#endregion"; // Slice operations
    }

    writeInheritedOperations(p);

    writeDispatchAndMarshalling(p, _stream);

    _M << eb;

    return true;
}

Slice::Gen::AsyncVisitor::AsyncVisitor(Output &out)
    : ObjCVisitor(out, out)
{
}

bool
Slice::Gen::AsyncVisitor::visitModuleStart(const ModulePtr& p)
{
    if(!p->hasAsyncOps())
    {
        return false;
    }

    _M << sp << nl << "namespace " << fixId(p->name());
    _M << sb;
    return true;
}

void
Slice::Gen::AsyncVisitor::visitModuleEnd(const ModulePtr&)
{
    _M << eb;
}

bool
Slice::Gen::AsyncVisitor::visitClassDefStart(const ClassDefPtr& p)
{
    return true;
}

void
Slice::Gen::AsyncVisitor::visitClassDefEnd(const ClassDefPtr&)
{
}

void
Slice::Gen::AsyncVisitor::visitOperation(const OperationPtr& p)
{
    ContainerPtr container = p->container();
    ClassDefPtr cl = ClassDefPtr::dynamicCast(container);
    
    if(cl->isLocal())
    {
        return;
    }

    string name = p->name();
    
    if(cl->hasMetaData("ami") || p->hasMetaData("ami"))
    {

        TypePtr ret = p->returnType();
        string retS = typeToString(ret);

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

        ExceptionList throws = p->throws();
        throws.sort();
        throws.unique();

        //
        // Arrange exceptions into most-derived to least-derived order. If we don't
        // do this, a base exception handler can appear before a derived exception
        // handler, causing compiler warnings and resulting in the base exception
        // being marshaled instead of the derived exception.
        //
#if defined(__SUNPRO_CC)
        throws.sort(Slice::derivedToBaseCompare);
#else
        throws.sort(Slice::DerivedToBaseCompare());
#endif

        TypeStringList::const_iterator q;

        vector<string> params;// = getParamsAsyncCB(p);
        vector<string> args;// = getArgsAsyncCB(p);

        vector<string> paramsInvoke;// = getParamsAsync(p, false);

        _M << sp << nl << "public abstract class AMI_" << cl->name() << '_'
             << name << " : IceInternal.OutgoingAsync";
        _M << sb;
        _M << sp;
        _M << nl << "public abstract void ice_response" << spar << params << epar << ';';
        
        _M << sp << nl << "public bool invoke__" << spar << "Ice.ObjectPrx prx__"
             << paramsInvoke << "_System.Collections.Generic.Dictionary<string, string> ctx__" << epar;
        _M << sb;
        _M << nl << "acquireCallback__(prx__);";
        _M << nl << "try";
        _M << sb;
        if(p->returnsData())
        {
            _M << nl << "((Ice.ObjectPrxHelperBase)prx__).checkTwowayOnly__(\"" << p->name() << "\");";
        }
        _M << nl << "prepare__(prx__, \"" << name << "\", " << sliceModeToIceMode(p->sendMode()) << ", ctx__);";
        for(q = inParams.begin(); q != inParams.end(); ++q)
        {
            string typeS = typeToString(q->first);
            writeMarshalUnmarshalCode(_M, q->first, fixId(q->second), true, false, false);
        }
        if(p->sendsClasses())
        {
            _M << nl << "os__.writePendingObjects();";
        }
        _M << nl << "os__.endWriteEncaps();";
        _M << nl << "return send__();";
        _M << eb;
        _M << nl << "catch(Ice.LocalException ex__)";
        _M << sb;
        _M << nl << "releaseCallback__(ex__);";
        _M << nl << "return false;";
        _M << eb;
        _M << eb;

        _M << sp << nl << "protected override void response__(bool ok__)";
        _M << sb;
        for(q = outParams.begin(); q != outParams.end(); ++q)
        {
            string param = fixId(q->second);
            string typeS = typeToString(q->first);
            _M << nl << typeS << ' ' << param << ';';
        }
        if(ret)
        {
            _M << nl << retS << " ret__;";
        }
        _M << nl << "try";
        _M << sb;
        _M << nl << "if(!ok__)";
        _M << sb;
        _M << nl << "try";
        _M << sb;
        _M << nl << "throwUserException__();";
        _M << eb;
        for(ExceptionList::const_iterator r = throws.begin(); r != throws.end(); ++r)
        {
            _M << nl << "catch(" << fixId((*r)->scoped()) << " ex__)";
            _M << sb;
            _M << nl << "exception__(ex__);";
            _M << eb;
        }
        _M << nl << "catch(Ice.UserException ex__)";
        _M << sb;
        _M << nl << "throw new Ice.UnknownUserException(ex__.ice_name(), ex__);";
        _M << eb;
        _M << "return;";
        _M << eb;
        if(p->returnsData())
        {
            _M << nl << "is__.startReadEncaps();";
            for(q = outParams.begin(); q != outParams.end(); ++q)
            {
                string param = fixId(q->second);
                StructPtr st = StructPtr::dynamicCast(q->first);
                if(st)
		{
                    if(isValueType(st))
                    {
                        _M << nl << param << " = new " << typeToString(q->first) << "();";
                    }
                    else
                    {
                        _M << nl << param << " = null;";
                    }
		}
                writeMarshalUnmarshalCode(_M, q->first, fixId(q->second), false, false, true);
            }
            if(ret)
            {
                StructPtr st = StructPtr::dynamicCast(ret);
                if(st)
                {
                    if(isValueType(ret))
                    {
                        _M << nl << "ret__ = new " << retS << "();";
                    }
                    else
                    {
                        _M << nl << "ret__ = null;";
                    }
                }
                writeMarshalUnmarshalCode(_M, ret, "ret__", false, false, true);
            }
            if(p->returnsClasses())
            {
                _M << nl << "is__.readPendingObjects();";
            }
            _M << nl << "is__.endReadEncaps();";
            for(q = outParams.begin(); q != outParams.end(); ++q)
            {
                string param = fixId(q->second);
		if(isClass(q->first))
                {
                    string type = typeToString(q->first);
                    _M << nl << param << " = (" << type << ")" << param << "_PP.value;";
                }
            }
            if(ret)
            {
		if(isClass(ret))
                {
                    string type = typeToString(ret);
                    _M << nl << "ret__ = (" << retS << ")ret___PP.value;";
                }
            }
        }
        else
        {
            _M << nl << "is__.skipEmptyEncaps();";
        }
        _M << eb;
        _M << nl << "catch(Ice.LocalException ex__)";
        _M << sb;
        _M << nl << "finished__(ex__);";
        _M << nl << "return;";
        _M << eb;
        _M << nl << "ice_response" << spar << args << epar << ';';
        _M << nl << "releaseCallback__();";
        _M << eb;
        _M << eb;
    }

    if(cl->hasMetaData("amd") || p->hasMetaData("amd"))
    {
        string classNameAMD = "AMD_" + cl->name();
        string classNameAMDI = "_AMD_" + cl->name();

        vector<string> paramsAMD;// = getParamsAsyncCB(p);

        _M << sp << nl << "public interface " << classNameAMD << '_' << name;
        _M << sb;
        _M << sp << nl << "void ice_response" << spar << paramsAMD << epar << ';';
        _M << sp << nl << "void ice_exception(_System.Exception ex);";
        _M << eb;
    
        TypePtr ret = p->returnType();
        
        TypeStringList outParams;
        ParamDeclList paramList = p->parameters();
        for(ParamDeclList::const_iterator pli = paramList.begin(); pli != paramList.end(); ++pli)
        {
            if((*pli)->isOutParam())
            {
                outParams.push_back(make_pair((*pli)->type(), (*pli)->name()));
            }
        }
        
        ExceptionList throws = p->throws();
        throws.sort();
        throws.unique();

        //
        // Arrange exceptions into most-derived to least-derived order. If we don't
        // do this, a base exception handler can appear before a derived exception
        // handler, causing compiler warnings and resulting in the base exception
        // being marshaled instead of the derived exception.
        //
#if defined(__SUNPRO_CC)
        throws.sort(Slice::derivedToBaseCompare);
#else
        throws.sort(Slice::DerivedToBaseCompare());
#endif

        TypeStringList::const_iterator q;
        _M << sp << nl << "class " << classNameAMDI << '_' << name
            << " : IceInternal.IncomingAsync, " << classNameAMD << '_' << name;
        _M << sb;

        _M << sp << nl << "public " << classNameAMDI << '_' << name << "(IceInternal.Incoming inc) : base(inc)";
        _M << sb;
        _M << eb;

        _M << sp << nl << "public void ice_response" << spar << paramsAMD << epar;
        _M << sb;
        _M << nl << "if(validateResponse__(true))";
        _M << sb;
        if(ret || !outParams.empty())
        {
            _M << nl << "try";
            _M << sb;
            _M << nl << "IceInternal.BasicStream os__ = this.os__();";
            for(q = outParams.begin(); q != outParams.end(); ++q)
            {
                string typeS = typeToString(q->first);
                writeMarshalUnmarshalCode(_M, q->first, fixId(q->second), true, false, false);
            }
            if(ret)
            {
                string retS = typeToString(ret);
                writeMarshalUnmarshalCode(_M, ret, "ret__", true, false, false);
            }
            if(p->returnsClasses())
            {
                _M << nl << "os__.writePendingObjects();";
            }
            _M << eb;
            _M << nl << "catch(Ice.LocalException ex__)";
            _M << sb;
            _M << nl << "ice_exception(ex__);";
            _M << eb;
        }
        _M << nl << "response__(true);";
        _M << eb;
        _M << eb;

        _M << sp << nl << "public void ice_exception(_System.Exception ex)";
        _M << sb;
        if(throws.empty())
        {
            _M << nl << "if(validateException__(ex))";
            _M << sb;
            _M << nl << "exception__(ex);";
            _M << eb;
        }
        else
        {
            _M << nl << "try";
            _M << sb;
            _M << nl << "throw ex;";
            _M << eb;
            ExceptionList::const_iterator r;
            for(r = throws.begin(); r != throws.end(); ++r)
            {
                string exS = fixId((*r)->scoped());
                _M << nl << "catch(" << exS << " ex__)";
                _M << sb;
                _M << nl << "if(validateResponse__(false))";
                _M << sb;
                _M << nl << "os__().writeUserException(ex__);";
                _M << nl << "response__(false);";
                _M << eb;
                _M << eb;
            }
            _M << nl << "catch(_System.Exception ex__)";
            _M << sb;
            _M << nl << "exception__(ex__);";
            _M << eb;
        }
        _M << eb;

        _M << eb;
    }
}

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
