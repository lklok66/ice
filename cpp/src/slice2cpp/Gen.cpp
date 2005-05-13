// **********************************************************************
//
// Copyright (c) 2003-2005 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Gen.h>
#include <Slice/CPlusPlusUtil.h>
#include <IceUtil/Functional.h>
#include <IceUtil/Iterator.h>
#include <Slice/Checksum.h>

#include <limits>
#include <sys/stat.h>

using namespace std;
using namespace Slice;
using namespace IceUtil;

Slice::Gen::Gen(const string& name, const string& base,	const string& headerExtension,
	        const string& sourceExtension, const string& include, const vector<string>& includePaths,
		const string& dllExport, const string& dir, bool imp, bool checksum, bool stream) :
    _base(base),
    _headerExtension(headerExtension),
    _sourceExtension(sourceExtension),
    _include(include),
    _includePaths(includePaths),
    _dllExport(dllExport),
    _impl(imp),
    _checksum(checksum),
    _stream(stream)
{
    for(vector<string>::iterator p = _includePaths.begin(); p != _includePaths.end(); ++p)
    {
	if(p->length() && (*p)[p->length() - 1] != '/')
	{
	    *p += '/';
	}
    }

    string::size_type pos = _base.rfind('/');
    if(pos == string::npos)
    {
        pos = _base.rfind('\\');
    }
    if(pos != string::npos)
    {
	_base.erase(0, pos + 1);
    }

    if(_impl)
    {
        string fileImplH = _base + "I." + _headerExtension;
        string fileImplC = _base + "I." + _sourceExtension;
        if(!dir.empty())
        {
            fileImplH = dir + '/' + fileImplH;
            fileImplC = dir + '/' + fileImplC;
        }

        struct stat st;
        if(stat(fileImplH.c_str(), &st) == 0)
        {
            cerr << name << ": `" << fileImplH << "' already exists - will not overwrite" << endl;
            return;
        }
        if(stat(fileImplC.c_str(), &st) == 0)
        {
            cerr << name << ": `" << fileImplC << "' already exists - will not overwrite" << endl;
            return;
        }

        implH.open(fileImplH.c_str());
        if(!implH)
        {
            cerr << name << ": can't open `" << fileImplH << "' for writing" << endl;
            return;
        }
        
        implC.open(fileImplC.c_str());
        if(!implC)
        {
            cerr << name << ": can't open `" << fileImplC << "' for writing" << endl;
            return;
        }

        string s = fileImplH;
        if(_include.size())
        {
            s = _include + '/' + s;
        }
        transform(s.begin(), s.end(), s.begin(), ToIfdef());
        implH << "#ifndef __" << s << "__";
        implH << "\n#define __" << s << "__";
        implH << '\n';
    }

    string fileH = _base + "." + _headerExtension;
    string fileC = _base + "." + _sourceExtension;
    if(!dir.empty())
    {
	fileH = dir + '/' + fileH;
	fileC = dir + '/' + fileC;
    }

    H.open(fileH.c_str());
    if(!H)
    {
	cerr << name << ": can't open `" << fileH << "' for writing" << endl;
	return;
    }
    
    C.open(fileC.c_str());
    if(!C)
    {
	cerr << name << ": can't open `" << fileC << "' for writing" << endl;
	return;
    }

    printHeader(H);
    printHeader(C);
    H << "\n// Generated from file `" << changeInclude(_base, _includePaths) << ".ice'\n";
    C << "\n// Generated from file `" << changeInclude(_base, _includePaths) << ".ice'\n";

    string s = fileH;
    if(_include.size())
    {
	s = _include + '/' + s;
    }
    transform(s.begin(), s.end(), s.begin(), ToIfdef());
    H << "\n#ifndef __" << s << "__";
    H << "\n#define __" << s << "__";
    H << '\n';
}

Slice::Gen::~Gen()
{
    H << "\n\n#endif\n";
    C << '\n';

    if(_impl)
    {
        implH << "\n\n#endif\n";
        implC << '\n';
    }
}

bool
Slice::Gen::operator!() const
{
    if(!H || !C)
    {
        return true;
    }
    if(_impl && (!implH || !implC))
    {
        return true;
    }
    return false;
}

void
Slice::Gen::generate(const UnitPtr& p)
{
    validateMetaData(p);

    C << "\n#include <";
    if(_include.size())
    {
	C << _include << '/';
    }
    C << _base << "." << _headerExtension << ">";

    H << "\n#include <Ice/LocalObjectF.h>";
    H << "\n#include <Ice/ProxyF.h>";
    H << "\n#include <Ice/ObjectF.h>";
    H << "\n#include <Ice/Exception.h>";
    H << "\n#include <Ice/LocalObject.h>";

    if(p->usesProxies())
    {
	H << "\n#include <Ice/Proxy.h>";
    }

    if(p->hasNonLocalClassDefs())
    {
	H << "\n#include <Ice/Object.h>";
	H << "\n#include <Ice/Outgoing.h>";
	if(p->hasContentsWithMetaData("ami"))
	{
	    H << "\n#include <Ice/OutgoingAsync.h>";
	}
	H << "\n#include <Ice/Incoming.h>";
	if(p->hasContentsWithMetaData("amd"))
	{
	    H << "\n#include <Ice/IncomingAsync.h>";
	}
	H << "\n#include <Ice/Direct.h>";
	C << "\n#include <Ice/LocalException.h>";
	C << "\n#include <Ice/ObjectFactory.h>";
    }

    if(p->hasNonLocalExceptions())
    {
	H << "\n#include <Ice/UserExceptionFactory.h>";
    }

    if(p->hasDataOnlyClasses() || p->hasNonLocalExceptions())
    {
	H << "\n#include <Ice/FactoryTable.h>";
    }

    if(p->usesNonLocals())
    {
	C << "\n#include <Ice/BasicStream.h>";
	C << "\n#include <Ice/Object.h>";
    }

    if(_stream || p->hasNonLocalClassDefs() || p->hasNonLocalExceptions())
    {
	H << "\n#include <Ice/StreamF.h>";

	if(!p->hasNonLocalClassDefs())
	{
	    C << "\n#include <Ice/LocalException.h>";
	}

	if(_stream)
	{
	    C << "\n#include <Ice/Stream.h>";
	}
    }

    if(_checksum)
    {
        C << "\n#include <Ice/SliceChecksums.h>";
    }

    StringList includes = p->includeFiles();

    for(StringList::const_iterator q = includes.begin(); q != includes.end(); ++q)
    {
	H << "\n#include <" << changeInclude(*q, _includePaths) << "." << _headerExtension << ">";
    }

    printVersionCheck(H);
    printVersionCheck(C);

    printDllExportStuff(H, _dllExport);
    if(_dllExport.size())
    {
	_dllExport += " ";
    }

    ProxyDeclVisitor proxyDeclVisitor(H, C, _dllExport);
    p->visit(&proxyDeclVisitor, false);

    ObjectDeclVisitor objectDeclVisitor(H, C, _dllExport);
    p->visit(&objectDeclVisitor, false);

    IceInternalVisitor iceInternalVisitor(H, C, _dllExport);
    p->visit(&iceInternalVisitor, false);

    HandleVisitor handleVisitor(H, C, _dllExport, _stream);
    p->visit(&handleVisitor, false);

    TypesVisitor typesVisitor(H, C, _dllExport, _stream);
    p->visit(&typesVisitor, false);

    AsyncVisitor asyncVisitor(H, C, _dllExport);
    p->visit(&asyncVisitor, false);

    AsyncImplVisitor asyncImplVisitor(H, C, _dllExport);
    p->visit(&asyncImplVisitor, false);

    ProxyVisitor proxyVisitor(H, C, _dllExport);
    p->visit(&proxyVisitor, false);

    DelegateVisitor delegateVisitor(H, C, _dllExport);
    p->visit(&delegateVisitor, false);

    DelegateMVisitor delegateMVisitor(H, C, _dllExport);
    p->visit(&delegateMVisitor, false);

    DelegateDVisitor delegateDVisitor(H, C, _dllExport);
    p->visit(&delegateDVisitor, false);

    ObjectVisitor objectVisitor(H, C, _dllExport, _stream);
    p->visit(&objectVisitor, false);

    if(_impl)
    {
        implH << "\n#include <";
        if(_include.size())
        {
            implH << _include << '/';
        }
        implH << _base << ".h>";

        implC << "#include <";
        if(_include.size())
        {
            implC << _include << '/';
        }
        implC << _base << "I.h>";

        ImplVisitor implVisitor(implH, implC, _dllExport);
        p->visit(&implVisitor, false);
    }

    if(_checksum)
    {
        ChecksumMap map = createChecksums(p);
        if(!map.empty())
        {
            C << sp << nl << "static const char* __sliceChecksums[] =";
            C << sb;
            for(ChecksumMap::const_iterator p = map.begin(); p != map.end(); ++p)
            {
                C << nl << "\"" << p->first << "\", \"";
                ostringstream str;
                str.flags(ios_base::hex);
                str.fill('0');
                for(vector<unsigned char>::const_iterator q = p->second.begin(); q != p->second.end(); ++q)
                {
                    str << (int)(*q);
                }
                C << str.str() << "\",";
            }
            C << nl << "0";
            C << eb << ';';
            C << nl << "static IceInternal::SliceChecksumInit __sliceChecksumInit(__sliceChecksums);";
        }
    }
}

Slice::Gen::TypesVisitor::TypesVisitor(Output& h, Output& c, const string& dllExport, bool stream) :
    H(h), C(c), _dllExport(dllExport), _stream(stream)
{
}

bool
Slice::Gen::TypesVisitor::visitModuleStart(const ModulePtr& p)
{
    string name = fixKwd(p->name());
    
    H << sp << nl << "namespace " << name << nl << '{';

    return true;
}

void
Slice::Gen::TypesVisitor::visitModuleEnd(const ModulePtr& p)
{
    H << sp << nl << '}';
}

bool
Slice::Gen::TypesVisitor::visitClassDefStart(const ClassDefPtr&)
{
    return false;
}

bool
Slice::Gen::TypesVisitor::visitExceptionStart(const ExceptionPtr& p)
{
    string name = fixKwd(p->name());
    string scoped = fixKwd(p->scoped());
    ExceptionPtr base = p->base();

    H << sp << nl << "class " << _dllExport << name << " : ";
    H.useCurrentPosAsIndent();
    if(!base)
    {
	if(p->isLocal())
	{
	    H << "public ::Ice::LocalException";
	}
	else
	{
	    H << "public ::Ice::UserException";
	}
    }
    else
    {
	H << "public " << fixKwd(base->scoped());
    }
    H.restoreIndent();
    H << sb;

    H.dec();
    H << nl << "public:";
    H.inc();

    H << sp;
    if(p->isLocal())
    {
	H << nl << name << "(const char*, int);";
	C << sp << nl << scoped.substr(2) << "::" << name << "(const char* file, int line) : ";
	C.inc();
	if(!base)
	{
	    C << nl << "::Ice::LocalException(file, line)";
	}
	else
	{
	    C << nl << fixKwd(base->scoped()) << "(file, line)";
	}
	C.dec();
	C << sb;
	C << eb;
    }

    H << nl << "virtual const ::std::string& ice_name() const;";

    string flatName = p->flattenedScope() + p->name() + "_name";

    C << sp << nl << "static const ::std::string " << flatName << " = \"" << p->scoped().substr(2) << "\";";
    C << sp << nl << "const ::std::string&" << nl << scoped.substr(2) << "::ice_name() const";
    C << sb;
    C << nl << "return " << flatName << ';';
    C << eb;
    
    if(p->isLocal())
    {
	H << nl << "virtual void ice_print(::std::ostream&) const;";
    }

    H << nl << "virtual ::Ice::Exception* ice_clone() const;";
    C << sp << nl << "::Ice::Exception*" << nl << scoped.substr(2) << "::ice_clone() const";
    C << sb;
    C << nl << "return new " << name << "(*this);";
    C << eb;

    H << nl << "virtual void ice_throw() const;";
    C << sp << nl << "void" << nl << scoped.substr(2) << "::ice_throw() const";
    C << sb;
    C << nl << "throw *this;";
    C << eb;

    if(!p->isLocal())
    {
	H << sp << nl << "static const ::IceInternal::UserExceptionFactoryPtr& ice_factory();";
    }
    return true;
}

void
Slice::Gen::TypesVisitor::visitExceptionEnd(const ExceptionPtr& p)
{
    string name = fixKwd(p->name());
    string scope = fixKwd(p->scope());
    string scoped = fixKwd(p->scoped());
    DataMemberList dataMembers = p->dataMembers();
    DataMemberList::const_iterator q;
	
    string factoryName;

    if(!p->isLocal())
    {
	ExceptionPtr base = p->base();
    
	H << sp << nl << "virtual void __write(::IceInternal::BasicStream*) const;";
	H << nl << "virtual void __read(::IceInternal::BasicStream*, bool);";

        H << sp << nl << "virtual void __write(const ::Ice::OutputStreamPtr&) const;";
        H << nl << "virtual void __read(const ::Ice::InputStreamPtr&, bool);";

	TypeStringList memberList;
	for(q = dataMembers.begin(); q != dataMembers.end(); ++q)
	{
	    memberList.push_back(make_pair((*q)->type(), (*q)->name()));
	}
	C << sp << nl << "void" << nl << scoped.substr(2) << "::__write(::IceInternal::BasicStream* __os) const";
	C << sb;
	C << nl << "__os->write(::std::string(\"" << p->scoped() << "\"));";
	C << nl << "__os->startWriteSlice();";
	writeMarshalCode(C, memberList, 0);
	C << nl << "__os->endWriteSlice();";
	if(base)
	{
	    C.zeroIndent();
	    C << nl << "#if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug"; // COMPILERBUG
	    C.restoreIndent();
	    C << nl << fixKwd(base->name()) << "::__write(__os);";
	    C.zeroIndent();
	    C << nl << "#else";
	    C.restoreIndent();
	    C << nl << fixKwd(base->scoped()) << "::__write(__os);";
	    C.zeroIndent();
	    C << nl << "#endif";
	    C.restoreIndent();
	}
	C << eb;

	C << sp << nl << "void" << nl << scoped.substr(2) << "::__read(::IceInternal::BasicStream* __is, bool __rid)";
	C << sb;
	C << nl << "if(__rid)";
	C << sb;
	C << nl << "::std::string myId;";
	C << nl << "__is->read(myId);";
	C << eb;
	C << nl << "__is->startReadSlice();";
	writeUnmarshalCode(C, memberList, 0);
	C << nl << "__is->endReadSlice();";
	if(base)
	{
	    C.zeroIndent();
	    C << nl << "#if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug"; // COMPILERBUG
	    C.restoreIndent();
	    C << nl << fixKwd(base->name()) << "::__read(__is, true);";
	    C.zeroIndent();
	    C << nl << "#else";
	    C.restoreIndent();
	    C << nl << fixKwd(base->scoped()) << "::__read(__is, true);";
	    C.zeroIndent();
	    C << nl << "#endif";
	    C.restoreIndent();
	}
	C << eb;

        if(_stream)
        {
            C << sp << nl << "void" << nl << scoped.substr(2) << "::__write(const ::Ice::OutputStreamPtr& __outS) const";
            C << sb;
            C << nl << "__outS->writeString(::std::string(\"" << p->scoped() << "\"));";
            C << nl << "__outS->startSlice();";
            writeStreamMarshalCode(C, memberList, 0);
            C << nl << "__outS->endSlice();";
            if(base)
            {
                C.zeroIndent();
                C << nl << "#if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug"; // COMPILERBUG
                C.restoreIndent();
                C << nl << fixKwd(base->name()) << "::__write(__outS);";
                C.zeroIndent();
                C << nl << "#else";
                C.restoreIndent();
                C << nl << fixKwd(base->scoped()) << "::__write(__outS);";
                C.zeroIndent();
                C << nl << "#endif";
                C.restoreIndent();
            }
            C << eb;

            C << sp << nl << "void" << nl << scoped.substr(2)
              << "::__read(const ::Ice::InputStreamPtr& __inS, bool __rid)";
            C << sb;
            C << nl << "if(__rid)";
            C << sb;
            C << nl << "__inS->readString();";
            C << eb;
            C << nl << "__inS->startSlice();";
            writeStreamUnmarshalCode(C, memberList, 0);
            C << nl << "__inS->endSlice();";
            if(base)
            {
                C.zeroIndent();
                C << nl << "#if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug"; // COMPILERBUG
                C.restoreIndent();
                C << nl << fixKwd(base->name()) << "::__read(__inS, true);";
                C.zeroIndent();
                C << nl << "#else";
                C.restoreIndent();
                C << nl << fixKwd(base->scoped()) << "::__read(__inS, true);";
                C.zeroIndent();
                C << nl << "#endif";
                C.restoreIndent();
            }
            C << eb;
        }
        else
        {
            //
            // Emit placeholder functions to catch errors.
            //
            C << sp << nl << "void" << nl << scoped.substr(2) << "::__write(const ::Ice::OutputStreamPtr&) const";
            C << sb;
            C << nl << "Ice::MarshalException ex(__FILE__, __LINE__);";
            C << nl << "ex.reason = \"exception " << scoped.substr(2) << " was not generated with stream support\";";
            C << nl << "throw ex;";
            C << eb;

            C << sp << nl << "void" << nl << scoped.substr(2) << "::__read(const ::Ice::InputStreamPtr&, bool)";
            C << sb;
            C << nl << "Ice::MarshalException ex(__FILE__, __LINE__);";
            C << nl << "ex.reason = \"exception " << scoped .substr(2)<< " was not generated with stream support\";";
            C << nl << "throw ex;";
            C << eb;
        }

	if(p->usesClasses())
	{
	    if(!base || (base && !base->usesClasses()))
	    {
		H << nl << "virtual bool __usesClasses() const;";

		C << sp << nl << "bool";
		C << nl << scoped.substr(2) << "::__usesClasses() const";
		C << sb;
		C << nl << "return true;";
		C << eb;
	    }
	}

	factoryName = "__F" + p->flattenedScope() + p->name();

	C << sp << nl << "struct " << factoryName << " : public ::IceInternal::UserExceptionFactory";
	C << sb;
	C << sp << nl << "virtual void";
	C << nl << "createAndThrow()";
	C << sb;
	C << nl << "throw " << scoped << "();";
	C << eb;
	C << eb << ';';

	C << sp << nl << "static ::IceInternal::UserExceptionFactoryPtr " << factoryName
	  << "__Ptr = new " << factoryName << ';';

	C << sp << nl << "const ::IceInternal::UserExceptionFactoryPtr&";
	C << nl << scoped.substr(2) << "::ice_factory()";
	C << sb;
	C << nl << "return " << factoryName << "__Ptr;";
	C << eb;

	C << sp << nl << "class " << factoryName << "__Init";
	C << sb;
	C.dec();
	C << nl << "public:";
	C.inc();
	C << sp << nl << factoryName << "__Init()";
	C << sb;
	C << nl << "::Ice::factoryTable->addExceptionFactory(\"" << p->scoped() << "\", " << scoped
	  << "::ice_factory());";
	C << eb;
	C << sp << nl << "~" << factoryName << "__Init()";
	C << sb;
	C << nl << "::Ice::factoryTable->removeExceptionFactory(\"" << p->scoped() << "\");";
	C << eb;
	C << eb << ';';
	C << sp << nl << "static " << factoryName << "__Init "<< factoryName << "__i;";
	C << sp << nl << "#ifdef __APPLE__";
	
	string initfuncname = "__F" + p->flattenedScope() + p->name() + "__initializer";
	C << nl << "extern \"C\" { void " << initfuncname << "() {} }";
	C << nl << "#endif";
    }
    H << eb << ';';

    if(!p->isLocal())
    {
	H << sp << nl << "static " << name << " __" << p->name() << "_init;";
    }
}

bool
Slice::Gen::TypesVisitor::visitStructStart(const StructPtr& p)
{
    string name = fixKwd(p->name());

    H << sp << nl << "struct " << name;
    H << sb;

    return true;
}

void
Slice::Gen::TypesVisitor::visitStructEnd(const StructPtr& p)
{
    string name = fixKwd(p->name());
    string scoped = fixKwd(p->scoped());
    string scope = fixKwd(p->scope());

    H << sp;
    H << nl << _dllExport << "bool operator==(const " << name << "&) const;";
    H << nl << _dllExport << "bool operator!=(const " << name << "&) const;";
    H << nl << _dllExport << "bool operator<(const " << name << "&) const;";
    
    DataMemberList dataMembers = p->dataMembers();
    DataMemberList::const_iterator q;
    C << sp << nl << "bool" << nl << scoped.substr(2) << "::operator==(const " << name << "& __rhs) const";
    C << sb;
    C << nl << "return !operator!=(__rhs);";
    C << eb;
    C << sp << nl << "bool" << nl << scoped.substr(2) << "::operator!=(const " << name << "& __rhs) const";
    C << sb;
    C << nl << "if(this == &__rhs)";
    C << sb;
    C << nl << "return false;";
    C << eb;
    for(q = dataMembers.begin(); q != dataMembers.end(); ++q)
    {
	C << nl << "if(" << fixKwd((*q)->name()) << " != __rhs." << fixKwd((*q)->name()) << ')';
	C << sb;
	C << nl << "return true;";
	C << eb;
    }
    C << nl << "return false;";
    C << eb;
    C << sp << nl << "bool" << nl << scoped.substr(2) << "::operator<(const " << name << "& __rhs) const";
    C << sb;
    C << nl << "if(this == &__rhs)";
    C << sb;
    C << nl << "return false;";
    C << eb;
    for(q = dataMembers.begin(); q != dataMembers.end(); ++q)
    {
	C << nl << "if(" << fixKwd((*q)->name()) << " < __rhs." << fixKwd((*q)->name()) << ')';
	C << sb;
	C << nl << "return true;";
	C << eb;
	C << nl << "else if(__rhs." << fixKwd((*q)->name()) << " < " << fixKwd((*q)->name()) << ')';
	C << sb;
	C << nl << "return false;";
	C << eb;
    }
    C << nl << "return false;";
    C << eb;
    
    if(!p->isLocal())
    {
	//
	// None of these member functions is virtual!
	//
	H << sp << nl << _dllExport << "void __write(::IceInternal::BasicStream*) const;";
	H << nl << _dllExport << "void __read(::IceInternal::BasicStream*);";

        if(_stream)
        {
            H << sp << nl << _dllExport << "void __write(const ::Ice::OutputStreamPtr&) const;";
            H << nl << _dllExport << "void __read(const ::Ice::InputStreamPtr&);";
        }

	TypeStringList memberList;
	for(q = dataMembers.begin(); q != dataMembers.end(); ++q)
	{
	    memberList.push_back(make_pair((*q)->type(), (*q)->name()));
	}
	C << sp << nl << "void" << nl << scoped.substr(2) << "::__write(::IceInternal::BasicStream* __os) const";
	C << sb;
	writeMarshalCode(C, memberList, 0);
	C << eb;

	C << sp << nl << "void" << nl << scoped.substr(2) << "::__read(::IceInternal::BasicStream* __is)";
	C << sb;
	writeUnmarshalCode(C, memberList, 0);
	C << eb;

	if(_stream)
	{
	    C << sp << nl << "void" << nl << scoped.substr(2) << "::__write(const ::Ice::OutputStreamPtr& __outS) const";
	    C << sb;
	    writeStreamMarshalCode(C, memberList, 0);
	    C << eb;

	    C << sp << nl << "void" << nl << scoped.substr(2) << "::__read(const ::Ice::InputStreamPtr& __inS)";
	    C << sb;
	    writeStreamUnmarshalCode(C, memberList, 0);
	    C << eb;
	}
    }

    H << eb << ';';

    if(!p->isLocal() && _stream)
    {
        H << sp << nl << _dllExport << "void ice_write" << p->name() << "(const ::Ice::OutputStreamPtr&, const "
          << name << "&);";
        H << nl << _dllExport << "void ice_read" << p->name() << "(const ::Ice::InputStreamPtr&, " << name << "&);";

        C << sp << nl << "void" << nl << scope.substr(2) << "ice_write" << p->name()
          << "(const ::Ice::OutputStreamPtr& __outS, const " << scoped << "& __v)";
        C << sb;
        C << nl << "__v.__write(__outS);";
        C << eb;

        C << sp << nl << "void" << nl << scope.substr(2) << "ice_read" << p->name()
          << "(const ::Ice::InputStreamPtr& __inS, " << scoped << "& __v)";
        C << sb;
        C << nl << "__v.__read(__inS);";
        C << eb;
    }
}

void
Slice::Gen::TypesVisitor::visitDataMember(const DataMemberPtr& p)
{
    string name = fixKwd(p->name());
    string s = typeToString(p->type());
    H << sp << nl << s << ' ' << name << ';';
}

void
Slice::Gen::TypesVisitor::visitSequence(const SequencePtr& p)
{
    string name = fixKwd(p->name());
    TypePtr type = p->type();
    string s = typeToString(type);
    if(s[0] == ':')
    {
	s.insert(0, " ");
    }
    H << sp << nl << "typedef ::std::vector<" << s << "> " << name << ';';

    BuiltinPtr builtin = BuiltinPtr::dynamicCast(type);
    if(!p->isLocal() &&
	(!builtin || builtin->kind() == Builtin::KindObject || builtin->kind() == Builtin::KindObjectProxy))
    {
	string scoped = fixKwd(p->scoped());
	string scope = fixKwd(p->scope());

	H << sp << nl << "class __U__" << name << " { };";
	H << nl << _dllExport << "void __write(::IceInternal::BasicStream*, const " << name << "&, __U__" << name
	  << ");";
	H << nl << _dllExport << "void __read(::IceInternal::BasicStream*, " << name << "&, __U__" << name << ");";

        if(_stream)
        {
            H << nl << _dllExport << "void ice_write" << p->name() << "(const ::Ice::OutputStreamPtr&, const "
              << name << "&);";
            H << nl << _dllExport << "void ice_read" << p->name() << "(const ::Ice::InputStreamPtr&, " << name << "&);";
        }

	C << sp << nl << "void" << nl << scope.substr(2) << "__write(::IceInternal::BasicStream* __os, const "
	  << scoped << "& v, " << scope << "__U__" << name << ")";
	C << sb;
	C << nl << "__os->writeSize(::Ice::Int(v.size()));";
	C << nl << scoped << "::const_iterator p;";
	C << nl << "for(p = v.begin(); p != v.end(); ++p)";
	C << sb;
	writeMarshalUnmarshalCode(C, type, "(*p)", true);
	C << eb;
	C << eb;

	C << sp << nl << "void" << nl << scope.substr(2) << "__read(::IceInternal::BasicStream* __is, " << scoped
	  << "& v, " << scope << "__U__" << name << ')';
	C << sb;
	C << nl << "::Ice::Int sz;";
	C << nl << "__is->readSize(sz);";
	if(type->isVariableLength())
	{
	    C << nl << "__is->startSeq(sz, " << type->minWireSize() << ");"; // Protect against bogus sequence sizes.
	}
	else
	{
	    C << nl << "__is->checkFixedSeq(sz, " << type->minWireSize() << ");";
	}
	C << nl << "v.resize(sz);";
	C << nl << "for(int i = 0; i < sz; ++i)";
	C << sb;
	writeMarshalUnmarshalCode(C, type, "v[i]", false);

	//
	// After unmarshaling each element, check that there are still enough bytes left in the stream
	// to unmarshal the remainder of the sequence, and decrement the count of elements
	// yet to be unmarshaled for sequences with variable-length element type (that is, for sequences
	// of classes, structs, dictionaries, sequences, strings, or proxies). This allows us to
	// abort unmarshaling for bogus sequence sizes at the earliest possible moment.
	// (For fixed-length sequences, we don't need to do this because the prediction of how many
	// bytes will be taken up by the sequence is accurate.)
	//
	if(type->isVariableLength())
	{
	    if(!SequencePtr::dynamicCast(type))
	    {
		//
		// No need to check for directly nested sequences because, at the start of each
		// sequence, we check anyway.
		//
		C << nl << "__is->checkSeq();";
	    }
	    C << nl << "__is->endElement();";
	}
	C << eb;
	if(type->isVariableLength())
	{
	    C << nl << "__is->endSeq(sz);";
	}
	C << eb;

        if(_stream)
        {
            C << sp << nl << "void" << nl << scope.substr(2) << "ice_write" << p->name()
              << "(const ::Ice::OutputStreamPtr& __outS, const " << scoped << "& v)";
            C << sb;
            C << nl << "__outS->writeSize(::Ice::Int(v.size()));";
            C << nl << scoped << "::const_iterator p;";
            C << nl << "for(p = v.begin(); p != v.end(); ++p)";
            C << sb;
            writeStreamMarshalUnmarshalCode(C, type, "(*p)", true);
            C << eb;
            C << eb;

            C << sp << nl << "void" << nl << scope.substr(2) << "ice_read" << p->name()
              << "(const ::Ice::InputStreamPtr& __inS, " << scoped << "& v)";
            C << sb;
            C << nl << "::Ice::Int sz = __inS->readSize();";
            C << nl << "v.resize(sz);";
            C << nl << "for(int i = 0; i < sz; ++i)";
            C << sb;
            writeStreamMarshalUnmarshalCode(C, type, "v[i]", false);
            C << eb;
            C << eb;
        }
    }
}

void
Slice::Gen::TypesVisitor::visitDictionary(const DictionaryPtr& p)
{
    string name = fixKwd(p->name());
    TypePtr keyType = p->keyType();
    TypePtr valueType = p->valueType();
    string ks = typeToString(keyType);
    if(ks[0] == ':')
    {
	ks.insert(0, " ");
    }
    string vs = typeToString(valueType);
    H << sp << nl << "typedef ::std::map<" << ks << ", " << vs << "> " << name << ';';

    if(!p->isLocal())
    {
	string scoped = fixKwd(p->scoped());
	string scope = fixKwd(p->scope());

	H << sp << nl << "class __U__" << name << " { };";
	H << nl << _dllExport << "void __write(::IceInternal::BasicStream*, const " << name
	  << "&, __U__" << name << ");";
	H << nl << _dllExport << "void __read(::IceInternal::BasicStream*, " << name
	  << "&, __U__" << name << ");";

	if(_stream)
	{
	    H << nl << _dllExport << "void ice_write" << p->name() << "(const ::Ice::OutputStreamPtr&, const " << name
	      << "&);";
	    H << nl << _dllExport << "void ice_read" << p->name() << "(const ::Ice::InputStreamPtr&, " << name << "&);";
	}

	C << sp << nl << "void" << nl << scope.substr(2) << "__write(::IceInternal::BasicStream* __os, const "
	  << scoped << "& v, " << scope << "__U__" << name << ")";
	C << sb;
	C << nl << "__os->writeSize(::Ice::Int(v.size()));";
	C << nl << scoped << "::const_iterator p;";
	C << nl << "for(p = v.begin(); p != v.end(); ++p)";
	C << sb;
	writeMarshalUnmarshalCode(C, keyType, "p->first", true);
	writeMarshalUnmarshalCode(C, valueType, "p->second", true);
	C << eb;
	C << eb;

	C << sp << nl << "void" << nl << scope.substr(2) << "__read(::IceInternal::BasicStream* __is, " << scoped
	  << "& v, " << scope << "__U__" << name << ')';
	C << sb;
	C << nl << "::Ice::Int sz;";
	C << nl << "__is->readSize(sz);";
	C << nl << "while(sz--)";
	C << sb;
	C << nl << "::std::pair<const " << ks << ", " << vs << "> pair;";
	string pf = string("const_cast<") + ks + "&>(pair.first)";
	writeMarshalUnmarshalCode(C, keyType, pf, false);
	C << nl << scoped << "::iterator __i = v.insert(v.end(), pair);";
	writeMarshalUnmarshalCode(C, valueType, "__i->second", false);
	C << eb;
	C << eb;

	if(_stream)
	{
	    C << sp << nl << "void" << nl << scope.substr(2) << "ice_write" << p->name()
	      << "(const ::Ice::OutputStreamPtr& __outS, const " << scoped << "& v)";
	    C << sb;
	    C << nl << "__outS->writeSize(::Ice::Int(v.size()));";
	    C << nl << scoped << "::const_iterator p;";
	    C << nl << "for(p = v.begin(); p != v.end(); ++p)";
	    C << sb;
	    writeStreamMarshalUnmarshalCode(C, keyType, "p->first", true);
	    writeStreamMarshalUnmarshalCode(C, valueType, "p->second", true);
	    C << eb;
	    C << eb;

	    C << sp << nl << "void" << nl << scope.substr(2) << "ice_read" << p->name()
	      << "(const ::Ice::InputStreamPtr& __inS, " << scoped << "& v)";
	    C << sb;
	    C << nl << "::Ice::Int sz = __inS->readSize();";
	    C << nl << "while(sz--)";
	    C << sb;
	    C << nl << "::std::pair<const " << ks << ", " << vs << "> pair;";
	    string pf = string("const_cast<") + ks + "&>(pair.first)";
	    writeStreamMarshalUnmarshalCode(C, keyType, pf, false);
	    C << nl << scoped << "::iterator __i = v.insert(v.end(), pair);";
	    writeStreamMarshalUnmarshalCode(C, valueType, "__i->second", false);
	    C << eb;
	    C << eb;
	}
    }
}

void
Slice::Gen::TypesVisitor::visitEnum(const EnumPtr& p)
{
    string name = fixKwd(p->name());
    EnumeratorList enumerators = p->getEnumerators();
    H << sp << nl << "enum " << name;
    H << sb;
    EnumeratorList::const_iterator en = enumerators.begin();
    while(en != enumerators.end())
    {
	H << nl << fixKwd((*en)->name());
	if(++en != enumerators.end())
	{
	    H << ',';
	}
    }
    H << eb << ';';

    if(!p->isLocal())
    {
	string scoped = fixKwd(p->scoped());
	string scope = fixKwd(p->scope());
	
	size_t sz = enumerators.size();
	assert(sz <= 0x7fffffff); // 64-bit enums are not supported
	
	H << sp << nl << _dllExport << "void __write(::IceInternal::BasicStream*, " << name << ");";
	H << nl << _dllExport << "void __read(::IceInternal::BasicStream*, " << name << "&);";

	if(_stream)
	{
	    H << sp << nl << _dllExport << "void ice_write" << p->name()
              << "(const ::Ice::OutputStreamPtr&, " << name << ");";
	    H << nl << _dllExport << "void ice_read" << p->name() << "(const ::Ice::InputStreamPtr&, " << name << "&);";
	}

	C << sp << nl << "void" << nl << scope.substr(2) << "__write(::IceInternal::BasicStream* __os, " << scoped
	  << " v)";
	C << sb;
	if(sz <= 0x7f)
	{
	    C << nl << "__os->write(static_cast< ::Ice::Byte>(v));";
	}
	else if(sz <= 0x7fff)
	{
	    C << nl << "__os->write(static_cast< ::Ice::Short>(v));";
	}
	else
	{
	    C << nl << "__os->write(static_cast< ::Ice::Int>(v));";
	}
	C << eb;

	C << sp << nl << "void" << nl << scope.substr(2) << "__read(::IceInternal::BasicStream* __is, " << scoped
	  << "& v)";
	C << sb;
	if(sz <= 0x7f)
	{
	    C << nl << "::Ice::Byte val;";
	    C << nl << "__is->read(val);";
	    C << nl << "v = static_cast< " << scoped << ">(val);";
	}
	else if(sz <= 0x7fff)
	{
	    C << nl << "::Ice::Short val;";
	    C << nl << "__is->read(val);";
	    C << nl << "v = static_cast< " << scoped << ">(val);";
	}
	else
	{
	    C << nl << "::Ice::Int val;";
	    C << nl << "__is->read(val);";
	    C << nl << "v = static_cast< " << scoped << ">(val);";
	}
	C << eb;

	if(_stream)
	{
	    C << sp << nl << "void" << nl << scope.substr(2) << "ice_write" << p->name()
              << "(const ::Ice::OutputStreamPtr& __outS, " << scoped << " v)";
	    C << sb;
	    if(sz <= 0x7f)
	    {
		C << nl << "__outS->writeByte(static_cast< ::Ice::Byte>(v));";
	    }
	    else if(sz <= 0x7fff)
	    {
		C << nl << "__outS->writeShort(static_cast< ::Ice::Short>(v));";
	    }
	    else
	    {
		C << nl << "__outS->writeInt(static_cast< ::Ice::Int>(v));";
	    }
	    C << eb;

	    C << sp << nl << "void" << nl << scope.substr(2) << "ice_read" << p->name()
              << "(const ::Ice::InputStreamPtr& __inS, " << scoped << "& v)";
	    C << sb;
	    if(sz <= 0x7f)
	    {
		C << nl << "::Ice::Byte val = __inS->readByte();";
		C << nl << "v = static_cast< " << scoped << ">(val);";
	    }
	    else if(sz <= 0x7fff)
	    {
		C << nl << "::Ice::Short val = __inS->readShort();";
		C << nl << "v = static_cast< " << scoped << ">(val);";
	    }
	    else
	    {
		C << nl << "::Ice::Int val = __inS->readInt();";
		C << nl << "v = static_cast< " << scoped << ">(val);";
	    }
	    C << eb;
	}
    }
}

void
Slice::Gen::TypesVisitor::visitConst(const ConstPtr& p)
{
    H << sp;
    H << nl << "const " << typeToString(p->type()) << " " << fixKwd(p->name()) << " = ";

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
					       "_{}[]#()<>%:;,?*+=/^&|~!=,\\\"' \t";
    	static const set<char> charSet(basicSourceChars.begin(), basicSourceChars.end());

	H << "\"";					// Opening "

	ios_base::fmtflags originalFlags = H.flags();	// Save stream state
	streamsize originalWidth = H.width();
	ostream::char_type originalFill = H.fill();

	const string val = p->value();
	for(string::const_iterator c = val.begin(); c != val.end(); ++c)
	{
	    if(charSet.find(*c) == charSet.end())
	    {
		unsigned char uc = *c;			// char may be signed, so make it positive
		H << "\\";    				// Print as octal if not in basic source character set
		H.flags(ios_base::oct);
		H.width(3);
		H.fill('0');
		H << static_cast<unsigned>(uc);
	    }
	    else
	    {
		H << *c;				// Print normally if in basic source character set
	    }
	}

	H.fill(originalFill);				// Restore stream state
	H.width(originalWidth);
	H.flags(originalFlags);

	H << "\"";					// Closing "
    }
    else if(bp && bp->kind() == Builtin::KindLong)
    {
	H << "ICE_INT64(" << p->value() << ")";
    }
    else
    {
	EnumPtr ep = EnumPtr::dynamicCast(p->type());
	if(ep)
	{
	    H << fixKwd(p->value());
	}
	else
	{
	    H << p->value();
	}
    }

    H << ';';
}

Slice::Gen::ProxyDeclVisitor::ProxyDeclVisitor(Output& h, Output& c, const string& dllExport) :
    H(h), C(c), _dllExport(dllExport)
{
}

bool
Slice::Gen::ProxyDeclVisitor::visitUnitStart(const UnitPtr& p)
{
    if(!p->hasNonLocalClassDecls())
    {
	return false;
    }

    H << sp << nl << "namespace IceProxy" << nl << '{';

    return true;
}

void
Slice::Gen::ProxyDeclVisitor::visitUnitEnd(const UnitPtr& p)
{
    H << sp << nl << '}';
}
    
bool
Slice::Gen::ProxyDeclVisitor::visitModuleStart(const ModulePtr& p)
{
    if(!p->hasNonLocalClassDecls())
    {
	return false;
    }

    string name = fixKwd(p->name());
    
    H << sp << nl << "namespace " << name << nl << '{';

    return true;
}

void
Slice::Gen::ProxyDeclVisitor::visitModuleEnd(const ModulePtr& p)
{
    H << sp << nl << '}';
}

void
Slice::Gen::ProxyDeclVisitor::visitClassDecl(const ClassDeclPtr& p)
{
    if(p->isLocal())
    {
	return;
    }

    string name = fixKwd(p->name());

    H << sp << nl << "class " << name << ';';
    H << nl << _dllExport << "bool operator==(const " << name << "&, const " << name << "&);";
    H << nl << _dllExport << "bool operator!=(const " << name << "&, const " << name << "&);";
    H << nl << _dllExport << "bool operator<(const " << name << "&, const " << name << "&);";
}

Slice::Gen::ProxyVisitor::ProxyVisitor(Output& h, Output& c, const string& dllExport) :
    H(h), C(c), _dllExport(dllExport)
{
}

bool
Slice::Gen::ProxyVisitor::visitUnitStart(const UnitPtr& p)
{
    if(!p->hasNonLocalClassDecls())
    {
	return false;
    }

    H << sp << nl << "namespace IceProxy" << nl << '{';

    return true;
}

void
Slice::Gen::ProxyVisitor::visitUnitEnd(const UnitPtr& p)
{
    H << sp << nl << '}';
}
    
bool
Slice::Gen::ProxyVisitor::visitModuleStart(const ModulePtr& p)
{
    if(!p->hasNonLocalClassDecls())
    {
	return false;
    }

    string name = fixKwd(p->name());
    
    H << sp << nl << "namespace " << name << nl << '{';

    return true;
}

void
Slice::Gen::ProxyVisitor::visitModuleEnd(const ModulePtr& p)
{
    H << sp << nl << '}';
}

bool
Slice::Gen::ProxyVisitor::visitClassDefStart(const ClassDefPtr& p)
{
    if(p->isLocal())
    {
	return false;
    }

    string name = fixKwd(p->name());
    string scoped = fixKwd(p->scoped());
    ClassList bases = p->bases();

    H << sp << nl << "class " << _dllExport << name << " : ";
    if(bases.empty())
    {
	H << "virtual public ::IceProxy::Ice::Object";
    }
    else
    {
	H.useCurrentPosAsIndent();
	ClassList::const_iterator q = bases.begin();
	while(q != bases.end())
	{
	    H << "virtual public ::IceProxy" << fixKwd((*q)->scoped());
	    if(++q != bases.end())
	    {
		H << ',' << nl;
	    }
	}
	H.restoreIndent();
    }

    H << sb;
    H.dec();
    H << nl << "public:";
    H.inc();

    return true;
}

void
Slice::Gen::ProxyVisitor::visitClassDefEnd(const ClassDefPtr& p)
{
    string scoped = fixKwd(p->scoped());
    string scope = fixKwd(p->scope());
    
    H << nl << nl << "static const ::std::string& ice_staticId();";
    
    H.dec();
    H << sp << nl << "private: ";
    H.inc();
    H << sp << nl << "virtual ::IceInternal::Handle< ::IceDelegateM::Ice::Object> __createDelegateM();";
    H << nl << "virtual ::IceInternal::Handle< ::IceDelegateD::Ice::Object> __createDelegateD();";
    H << eb << ';';

    C << sp;
    C << nl << "const ::std::string&" << nl << "IceProxy" << scoped << "::ice_staticId()";
    C << sb;
    C << nl << "return "<< scoped << "::ice_staticId();";
    C << eb;

    C << sp << nl << "::IceInternal::Handle< ::IceDelegateM::Ice::Object>";
    C << nl << "IceProxy" << scoped << "::__createDelegateM()";
    C << sb;
    C << nl << "return ::IceInternal::Handle< ::IceDelegateM::Ice::Object>(new ::IceDelegateM" << scoped << ");";
    C << eb;
    C << sp << nl << "::IceInternal::Handle< ::IceDelegateD::Ice::Object>";
    C << nl << "IceProxy" << scoped << "::__createDelegateD()";
    C << sb;
    C << nl << "return ::IceInternal::Handle< ::IceDelegateD::Ice::Object>(new ::IceDelegateD" << scoped << ");";
    C << eb;

    C << sp;
    C << nl << "bool" << nl << "IceProxy" << scope << "operator==(const ::IceProxy" << scoped
      << "& l, const ::IceProxy" << scoped << "& r)";
    C << sb;
    C << nl << "return static_cast<const ::IceProxy::Ice::Object&>(l) == "
      << "static_cast<const ::IceProxy::Ice::Object&>(r);";
    C << eb;
    C << sp;
    C << nl << "bool" << nl << "IceProxy" << scope << "operator!=(const ::IceProxy" << scoped
      << "& l, const ::IceProxy" << scoped << "& r)";
    C << sb;
    C << nl << "return static_cast<const ::IceProxy::Ice::Object&>(l) != "
      << "static_cast<const ::IceProxy::Ice::Object&>(r);";
    C << eb;
    C << sp;
    C << nl << "bool" << nl << "IceProxy" << scope << "operator<(const ::IceProxy" << scoped
      << "& l, const ::IceProxy" << scoped << "& r)";
    C << sb;
    C << nl << "return static_cast<const ::IceProxy::Ice::Object&>(l) < "
      << "static_cast<const ::IceProxy::Ice::Object&>(r);";
    C << eb;
}

void
Slice::Gen::ProxyVisitor::visitOperation(const OperationPtr& p)
{
    string name = p->name();
    string scoped = fixKwd(p->scoped());
    string scope = fixKwd(p->scope());

    TypePtr ret = p->returnType();
    string retS = returnTypeToString(ret);

    vector<string> params;
    vector<string> paramsDecl;
    vector<string> args;

    vector<string> paramsAMI;
    vector<string> paramsDeclAMI;
    vector<string> argsAMI;

    ParamDeclList paramList = p->parameters();
    for(ParamDeclList::const_iterator q = paramList.begin(); q != paramList.end(); ++q)
    {
	string paramName = fixKwd((*q)->name());

#if defined(__SUNPRO_CC) && (__SUNPRO_CC==0x550)
	//
	// Work around for Sun CC 5.5 bug #4853566
	//
	string typeString;
	if((*q)->isOutParam())
	{
	    typeString = outputTypeToString((*q)->type());
	}
	else
	{
	    typeString = inputTypeToString((*q)->type());
	}
#else
	string typeString = (*q)->isOutParam() ? outputTypeToString((*q)->type()) : inputTypeToString((*q)->type());
#endif

	params.push_back(typeString);
	paramsDecl.push_back(typeString + ' ' + paramName);
	args.push_back(paramName);

	if(!(*q)->isOutParam())
	{
	    string inputTypeString = inputTypeToString((*q)->type());

	    paramsAMI.push_back(inputTypeString);
	    paramsDeclAMI.push_back(inputTypeString + ' ' + paramName);
	    argsAMI.push_back(paramName);
	}
    }

    string thisPointer = fixKwd(scope.substr(0, scope.size() - 2)) + "*";

    H << sp;
    H << nl << retS << ' ' << fixKwd(name) << spar << params << epar << ';';
    H << nl << retS << ' ' << fixKwd(name) << spar << params << "const ::Ice::Context&" << epar << ';';

    C << sp << nl << retS << nl << "IceProxy" << scoped << spar << paramsDecl << epar;
    C << sb;
    C << nl;
    if(ret)
    {
	C << "return ";
    }
    C << fixKwd(name) << spar << args << "__defaultContext()" << epar << ';';
    C << eb;

    C << sp << nl << retS << nl << "IceProxy" << scoped << spar << paramsDecl << "const ::Ice::Context& __ctx" << epar;
    C << sb;
    C << nl << "int __cnt = 0;";
    C << nl << "while(true)";
    C << sb;
    C << nl << "try";
    C << sb;
    if(p->returnsData())
    {
        C << nl << "__checkTwowayOnly(\"" << name << "\");";
    }
    C << nl << "::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase = __getDelegate();";
    C << nl << "::IceDelegate" << thisPointer << " __del = dynamic_cast< ::IceDelegate"
      << thisPointer << ">(__delBase.get());";
    C << nl;
    if(ret)
    {
	C << "return ";
    }
    C << "__del->" << fixKwd(name) << spar << args << "__ctx" << epar << ';';
    if(!ret)
    {
	C << nl << "return;";
    }
    C << eb;
    C << nl << "catch(const ::IceInternal::NonRepeatable& __ex)";
    C << sb;
    if(p->mode() == Operation::Idempotent || p->mode() == Operation::Nonmutating)
    {
	C << nl << "__handleException(*__ex.get(), __cnt);";
    }
    else
    {
	C << nl << "__rethrowException(*__ex.get());";
    }
    C << eb;
    C << nl << "catch(const ::Ice::LocalException& __ex)";
    C << sb;
    C << nl << "__handleException(__ex, __cnt);";
    C << eb;
    C << eb;
    C << eb;

    ContainerPtr container = p->container();
    ClassDefPtr cl = ClassDefPtr::dynamicCast(container);
    if(cl->hasMetaData("ami") || p->hasMetaData("ami"))
    {
	string classNameAMI = "AMI_" + cl->name();
	string classScope = fixKwd(cl->scope());
	string classScopedAMI = classScope + classNameAMI;

	H << nl << "void " << name << "_async" << spar << ("const" + classScopedAMI + '_' + name + "Ptr&")
	  << paramsAMI << epar << ';';
	H << nl << "void " << name << "_async" << spar << ("const" + classScopedAMI + '_' + name + "Ptr&")
	  << paramsAMI << "const ::Ice::Context&" << epar << ';';

	C << sp << nl << "void" << nl << "IceProxy" << scope << name << "_async" << spar
	  << ("const " + classScopedAMI + '_' + name + "Ptr& __cb") << paramsDeclAMI << epar;
	C << sb;
	C << nl << name << "_async" << spar << "__cb" << argsAMI << "__defaultContext()" << epar << ';';
	C << eb;

	C << sp << nl << "void" << nl << "IceProxy" << scope << name << "_async" << spar
	  << ("const " + classScopedAMI + '_' + name + "Ptr& __cb") << paramsDeclAMI << "const ::Ice::Context& __ctx"
	  << epar;
	C << sb;
	// Async requests may only be sent twoway.
	C << nl << "__checkTwowayOnly(\"" << name << "\");";
	C << nl << "__cb->__invoke" << spar << "this" << argsAMI << "__ctx" << epar << ';';
	C << eb;
    }
}

Slice::Gen::DelegateVisitor::DelegateVisitor(Output& h, Output& c, const string& dllExport) :
    H(h), C(c), _dllExport(dllExport)
{
}

bool
Slice::Gen::DelegateVisitor::visitUnitStart(const UnitPtr& p)
{
    if(!p->hasNonLocalClassDecls())
    {
	return false;
    }

    H << sp << nl << "namespace IceDelegate" << nl << '{';

    return true;
}

void
Slice::Gen::DelegateVisitor::visitUnitEnd(const UnitPtr& p)
{
    H << sp << nl << '}';
}
    
bool
Slice::Gen::DelegateVisitor::visitModuleStart(const ModulePtr& p)
{
    if(!p->hasNonLocalClassDecls())
    {
	return false;
    }

    string name = fixKwd(p->name());
    
    H << sp << nl << "namespace " << name << nl << '{';

    return true;
}

void
Slice::Gen::DelegateVisitor::visitModuleEnd(const ModulePtr& p)
{
    H << sp << nl << '}';
}

bool
Slice::Gen::DelegateVisitor::visitClassDefStart(const ClassDefPtr& p)
{
    if(p->isLocal())
    {
	return false;
    }

    string name = fixKwd(p->name());
    ClassList bases = p->bases();

    H << sp << nl << "class " << _dllExport << name << " : ";
    if(bases.empty())
    {
	H << "virtual public ::IceDelegate::Ice::Object";
    }
    else
    {
	H.useCurrentPosAsIndent();
	ClassList::const_iterator q = bases.begin();
	while(q != bases.end())
	{
	    H << "virtual public ::IceDelegate" << fixKwd((*q)->scoped());
	    if(++q != bases.end())
	    {
		H << ',' << nl;
	    }
	}
	H.restoreIndent();
    }
    H << sb;
    H.dec();
    H << nl << "public:";
    H.inc();

    return true;
}

void
Slice::Gen::DelegateVisitor::visitClassDefEnd(const ClassDefPtr& p)
{
    H << eb << ';';
}

void
Slice::Gen::DelegateVisitor::visitOperation(const OperationPtr& p)
{
    string name = fixKwd(p->name());

    TypePtr ret = p->returnType();
    string retS = returnTypeToString(ret);

    vector<string> params;

    ParamDeclList paramList = p->parameters();
    for(ParamDeclList::const_iterator q = paramList.begin(); q != paramList.end(); ++q)
    {
#if defined(__SUNPRO_CC) && (__SUNPRO_CC==0x550)
	//
	// Work around for Sun CC 5.5 bug #4853566
	//
	string typeString;
	if((*q)->isOutParam())
	{
	    typeString = outputTypeToString((*q)->type());
	}
	else
	{
	    typeString = inputTypeToString((*q)->type());
	}
#else
	string typeString = (*q)->isOutParam() ? outputTypeToString((*q)->type()) : inputTypeToString((*q)->type());
#endif

	params.push_back(typeString);
    }

    params.push_back("const ::Ice::Context&");

    H << sp << nl << "virtual " << retS << ' ' << name << spar << params << epar << " = 0;";
}

Slice::Gen::DelegateMVisitor::DelegateMVisitor(Output& h, Output& c, const string& dllExport) :
    H(h), C(c), _dllExport(dllExport)
{
}

bool
Slice::Gen::DelegateMVisitor::visitUnitStart(const UnitPtr& p)
{
    if(!p->hasNonLocalClassDecls())
    {
	return false;
    }

    H << sp << nl << "namespace IceDelegateM" << nl << '{';

    return true;
}

void
Slice::Gen::DelegateMVisitor::visitUnitEnd(const UnitPtr& p)
{
    H << sp << nl << '}';
}
    
bool
Slice::Gen::DelegateMVisitor::visitModuleStart(const ModulePtr& p)
{
    if(!p->hasNonLocalClassDecls())
    {
	return false;
    }

    string name = fixKwd(p->name());
    
    H << sp << nl << "namespace " << name << nl << '{';

    return true;
}

void
Slice::Gen::DelegateMVisitor::visitModuleEnd(const ModulePtr& p)
{
    H << sp << nl << '}';
}

bool
Slice::Gen::DelegateMVisitor::visitClassDefStart(const ClassDefPtr& p)
{
    if(p->isLocal())
    {
	return false;
    }

    string name = fixKwd(p->name());
    string scoped = fixKwd(p->scoped());
    ClassList bases = p->bases();

    H << sp << nl << "class " << _dllExport << name << " : ";
    H.useCurrentPosAsIndent();
    H << "virtual public ::IceDelegate" << scoped << ',';
    if(bases.empty())
    {
	H << nl << "virtual public ::IceDelegateM::Ice::Object";
    }
    else
    {
	ClassList::const_iterator q = bases.begin();
	while(q != bases.end())
	{
	    H << nl << "virtual public ::IceDelegateM" << fixKwd((*q)->scoped());
	    if(++q != bases.end())
	    {
		H << ',';
	    }
	}
    }
    H.restoreIndent();
    H << sb;
    H.dec();
    H << nl << "public:";
    H.inc();

    return true;
}

void
Slice::Gen::DelegateMVisitor::visitClassDefEnd(const ClassDefPtr& p)
{
    H << eb << ';';
}

void
Slice::Gen::DelegateMVisitor::visitOperation(const OperationPtr& p)
{
    string name = fixKwd(p->name());
    string scoped = fixKwd(p->scoped());

    TypePtr ret = p->returnType();
    string retS = returnTypeToString(ret);

    vector<string> params;
    vector<string> paramsDecl;

    TypeStringList inParams;
    TypeStringList outParams;
    ParamDeclList paramList = p->parameters();
    for(ParamDeclList::const_iterator q = paramList.begin(); q != paramList.end(); ++q)
    {
	string paramName = fixKwd((*q)->name());
	TypePtr type = (*q)->type();
	bool isOutParam = (*q)->isOutParam();
	string typeString;
	if(isOutParam)
	{
	    outParams.push_back(make_pair(type, paramName));
	    typeString = outputTypeToString(type);
	}
	else
	{
	    inParams.push_back(make_pair(type, paramName));
	    typeString = inputTypeToString(type);
	}

	params.push_back(typeString);
	paramsDecl.push_back(typeString + ' ' + paramName);
    }

    params.push_back("const ::Ice::Context&");
    paramsDecl.push_back("const ::Ice::Context& __context");

    H << sp << nl << "virtual " << retS << ' ' << name << spar << params << epar << ';';
    C << sp << nl << retS << nl << "IceDelegateM" << scoped << spar << paramsDecl << epar;
    C << sb;
    C << nl << "static const ::std::string __operation(\"" << p->name() << "\");";
    C << nl << "::IceInternal::Outgoing __outS(__connection.get(), __reference.get(), __operation, "
      << "static_cast< ::Ice::OperationMode>(" << p->mode() << "), __context, __compress);";
    if(!inParams.empty())
    {
	C << nl << "try";
	C << sb;
	C << nl << "::IceInternal::BasicStream* __os = __outS.os();";
	writeMarshalCode(C, inParams, 0);
	if(p->sendsClasses())
	{
	    C << nl << "__os->writePendingObjects();";
	}
	C << eb;
	C << nl << "catch(const ::Ice::LocalException& __ex)";
	C << sb;
	C << nl << "__outS.abort(__ex);";
	C << eb;
    }
    C << nl << "bool __ok = __outS.invoke();";
    C << nl << "try";
    C << sb;
    C << nl << "::IceInternal::BasicStream* __is = __outS.is();";
    C << nl << "if(!__ok)";
    C << sb;
    C << nl << "__is->throwException();";
    C << eb;
    writeAllocateCode(C, TypeStringList(), ret);
    writeUnmarshalCode(C, outParams, ret);
    if(p->returnsClasses())
    {
	C << nl << "__is->readPendingObjects();";
    }
    if(ret)
    {
	C << nl << "return __ret;";
    }
    C << eb;

    //
    // Generate a catch block for each legal user exception. This is necessary
    // to prevent an "impossible" user exception to be thrown if client and
    // and server use different exception specifications for an operation. For
    // example:
    //
    // Client compiled with:
    // exception A {};
    // exception B {};
    // interface I {
    //     void op() throws A;
    // };
    //
    // Server compiled with:
    // exception A {};
    // exception B {};
    // interface I {
    //     void op() throws B; // Differs from client
    // };
    //
    // We need the catch blocks so, if the server throws B from op(), the
    // client receives UnknownUserException instead of B.
    //
    ExceptionList throws = p->throws();
    throws.sort();
    throws.unique();
#if defined(__SUNPRO_CC)
    throws.sort(derivedToBaseCompare);
#else
    throws.sort(Slice::DerivedToBaseCompare());
#endif
    for(ExceptionList::const_iterator i = throws.begin(); i != throws.end(); ++i)
    {
	string scoped = (*i)->scoped();
	C << nl << "catch(const " << fixKwd((*i)->scoped()) << "&)";
	C << sb;
	C << nl << "throw;";
	C << eb;
    }
    C << nl << "catch(const ::Ice::UserException&)";
    C << sb;
    C << nl << "throw ::Ice::UnknownUserException(__FILE__, __LINE__);";
    C << eb;
    C << nl << "catch(const ::Ice::LocalException& __ex)";
    C << sb;
    C << nl << "throw ::IceInternal::NonRepeatable(__ex);";
    C << eb;
    C << eb;
}

Slice::Gen::DelegateDVisitor::DelegateDVisitor(Output& h, Output& c, const string& dllExport) :
    H(h), C(c), _dllExport(dllExport)
{
}

bool
Slice::Gen::DelegateDVisitor::visitUnitStart(const UnitPtr& p)
{
    if(!p->hasNonLocalClassDecls())
    {
	return false;
    }

    H << sp << nl << "namespace IceDelegateD" << nl << '{';

    return true;
}

void
Slice::Gen::DelegateDVisitor::visitUnitEnd(const UnitPtr& p)
{
    H << sp << nl << '}';
}
    
bool
Slice::Gen::DelegateDVisitor::visitModuleStart(const ModulePtr& p)
{
    if(!p->hasNonLocalClassDecls())
    {
	return false;
    }

    string name = fixKwd(p->name());
    
    H << sp << nl << "namespace " << name << nl << '{';

    return true;
}

void
Slice::Gen::DelegateDVisitor::visitModuleEnd(const ModulePtr& p)
{
    H << sp << nl << '}';
}

bool
Slice::Gen::DelegateDVisitor::visitClassDefStart(const ClassDefPtr& p)
{
    if(p->isLocal())
    {
	return false;
    }

    string name = fixKwd(p->name());
    string scoped = fixKwd(p->scoped());
    ClassList bases = p->bases();

    H << sp << nl << "class " << _dllExport << name << " : ";
    H.useCurrentPosAsIndent();
    H << "virtual public ::IceDelegate" << scoped << ',';
    if(bases.empty())
    {
	H << nl << "virtual public ::IceDelegateD::Ice::Object";
    }
    else
    {
	ClassList::const_iterator q = bases.begin();
	while(q != bases.end())
	{
	    H << nl << "virtual public ::IceDelegateD" << fixKwd((*q)->scoped());
	    if(++q != bases.end())
	    {
		H << ',';
	    }
	}
    }
    H.restoreIndent();
    H << sb;
    H.dec();
    H << nl << "public:";
    H.inc();

    return true;
}

void
Slice::Gen::DelegateDVisitor::visitClassDefEnd(const ClassDefPtr& p)
{
    H << eb << ';';
}

void
Slice::Gen::DelegateDVisitor::visitOperation(const OperationPtr& p)
{
    string name = fixKwd(p->name());
    string scoped = fixKwd(p->scoped());

    TypePtr ret = p->returnType();
    string retS = returnTypeToString(ret);

    vector<string> params;
    vector<string> paramsDecl;
    vector<string> args;

    ParamDeclList paramList = p->parameters();
    for(ParamDeclList::const_iterator q = paramList.begin(); q != paramList.end(); ++q)
    {
	string paramName = fixKwd((*q)->name());

#if defined(__SUNPRO_CC) && (__SUNPRO_CC==0x550)
	//
	// Work around for Sun CC 5.5 bug #4853566
	//
	string typeString;
	if((*q)->isOutParam())
	{
	    typeString = outputTypeToString((*q)->type());
	}
	else
	{
	    typeString = inputTypeToString((*q)->type());
	}
#else
	string typeString = (*q)->isOutParam() ? outputTypeToString((*q)->type()) : inputTypeToString((*q)->type());
#endif

	params.push_back(typeString);
	paramsDecl.push_back(typeString + ' ' + paramName);
	args.push_back(paramName);
    }
    
    params.push_back("const ::Ice::Context&");
    paramsDecl.push_back("const ::Ice::Context& __context");
    args.push_back("__current");
    
    ContainerPtr container = p->container();
    ClassDefPtr cl = ClassDefPtr::dynamicCast(container);
    string thisPointer = fixKwd(cl->scoped()) + "*";

    H << sp;

    H << nl << "virtual " << retS << ' ' << name << spar << params << epar << ';';
    bool amd = !cl->isLocal() && (cl->hasMetaData("amd") || p->hasMetaData("amd"));
    if(amd)
    {
	C << sp << nl << retS << nl << "IceDelegateD" << scoped << spar << params << epar;
	C << sb;
	C << nl << "throw ::Ice::CollocationOptimizationException(__FILE__, __LINE__);";
	C << eb;
    }
    else
    {
	C << sp << nl << retS << nl << "IceDelegateD" << scoped << spar << paramsDecl << epar;
	C << sb;
	C << nl << "::Ice::Current __current;";
	C << nl << "__initCurrent(__current, \"" << p->name()
	  << "\", static_cast< ::Ice::OperationMode>(" << p->mode() << "), __context);";
	C << nl << "while(true)";
	C << sb;
	C << nl << "::IceInternal::Direct __direct(__current);";
	C << nl << thisPointer << " __servant = dynamic_cast< " << thisPointer << ">(__direct.servant().get());";
	C << nl << "if(!__servant)";
	C << sb;
	C << nl << "::Ice::OperationNotExistException __opEx(__FILE__, __LINE__);";
	C << nl << "__opEx.id = __current.id;";
	C << nl << "__opEx.facet = __current.facet;";
	C << nl << "__opEx.operation = __current.operation;";
	C << nl << "throw __opEx;";
	C << eb;
        C << nl << "try";
        C << sb;
	C << nl;
	if(ret)
	{
	    C << "return ";
	}
	C << "__servant->" << name << spar << args << epar << ';';
	if(!ret)
	{
	    C << nl << "return;";
	}
	C << eb;
        C << nl << "catch(const ::Ice::LocalException& __ex)";
        C << sb;
        C << nl << "throw ::IceInternal::NonRepeatable(__ex);";
	C << eb;
	C << eb;
	C << eb;
    }
}

Slice::Gen::ObjectDeclVisitor::ObjectDeclVisitor(Output& h, Output& c, const string& dllExport) :
    H(h), C(c), _dllExport(dllExport)
{
}

bool
Slice::Gen::ObjectDeclVisitor::visitModuleStart(const ModulePtr& p)
{
    if(!p->hasClassDecls())
    {
	return false;
    }

    string name = fixKwd(p->name());
    
    H << sp << nl << "namespace " << name << nl << '{';

    return true;
}

void
Slice::Gen::ObjectDeclVisitor::visitModuleEnd(const ModulePtr& p)
{
    H << sp << nl << '}';
}

void
Slice::Gen::ObjectDeclVisitor::visitClassDecl(const ClassDeclPtr& p)
{
    string name = fixKwd(p->name());
    
    H << sp << nl << "class " << name << ';';
    H << nl << _dllExport << "bool operator==(const " << name << "&, const " << name << "&);";
    H << nl << _dllExport << "bool operator!=(const " << name << "&, const " << name << "&);";
    H << nl << _dllExport << "bool operator<(const " << name << "&, const " << name << "&);";
}

Slice::Gen::ObjectVisitor::ObjectVisitor(Output& h, Output& c, const string& dllExport, bool stream) :
    H(h), C(c), _dllExport(dllExport), _stream(stream)
{
}

bool
Slice::Gen::ObjectVisitor::visitModuleStart(const ModulePtr& p)
{
    if(!p->hasClassDefs())
    {
	return false;
    }

    string name = fixKwd(p->name());
    
    H << sp << nl << "namespace " << name << nl << '{';

    return true;
}

void
Slice::Gen::ObjectVisitor::visitModuleEnd(const ModulePtr& p)
{
    H << sp;
    H << nl << '}';
}

bool
Slice::Gen::ObjectVisitor::visitClassDefStart(const ClassDefPtr& p)
{
    string name = fixKwd(p->name());
    ClassList bases = p->bases();

    H << sp;
    H << nl << "class " << _dllExport << name << " : ";
    H.useCurrentPosAsIndent();
    if(bases.empty())
    {
	if(p->isLocal())
	{
	    H << "virtual public ::Ice::LocalObject";
	}
	else
	{
	    H << "virtual public ::Ice::Object";
	}
    }
    else
    {
	ClassList::const_iterator q = bases.begin();
	while(q != bases.end())
	{
	    H << "virtual public " << fixKwd((*q)->scoped());
	    if(++q != bases.end())
	    {
		H << ',' << nl;
	    }
	}
    }
    H.restoreIndent();
    H << sb;
    H.dec();
    H << nl << "public:";
    H.inc();

    if(!p->isAbstract() && !p->isLocal())
    {
	H << sp << nl << "void __copyMembers(" << fixKwd(p->scoped() + "Ptr") + ") const;";

	C << sp;
	C << nl << "void ";
	C << nl << fixKwd(p->scoped()).substr(2) << "::__copyMembers(" << fixKwd(p->scoped() + "Ptr") << " __to) const";
	C << sb;
	string winUpcall; 
	string unixUpcall; 
	if(!bases.empty() && !bases.front()->isInterface())
	{
	    winUpcall = fixKwd(bases.front()->name()) + "::__copyMembers(__to);";
	    unixUpcall = fixKwd(bases.front()->scoped()) + "::__copyMembers(__to);";
	}
	else
	{
	    winUpcall = "Object::__copyMembers(__to);";
	    unixUpcall = "::Ice::Object::__copyMembers(__to);";
	}
	C.zeroIndent();
	C << nl << "#if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug"; // COMPILERBUG
	C.restoreIndent();
	C << nl << winUpcall;
	C.zeroIndent();
	C << nl << "#else";
	C.restoreIndent();
	C << nl << unixUpcall;
	C.zeroIndent();
	C << nl << "#endif";
	C.restoreIndent();
	DataMemberList dataMembers = p->dataMembers();
	for(DataMemberList::const_iterator q = dataMembers.begin(); q != dataMembers.end(); ++q)
	{
	    C << nl << "__to->" << fixKwd((*q)->name()) << " = " << fixKwd((*q)->name()) << ';';
	}
	C << eb;

	H << nl << "virtual ::Ice::ObjectPtr ice_clone() const;";

	C << sp;
	C << nl << "::Ice::ObjectPtr";
	C << nl << fixKwd(p->scoped()).substr(2) << "::ice_clone() const";
	C << sb;
	C << nl << fixKwd(p->scope()) << p->name() << "Ptr __p = new " << fixKwd(p->scoped()) << ';';
	C.zeroIndent();
	C << nl << "#if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug"; // COMPILERBUG
	C.restoreIndent();
	C << nl << fixKwd(name) + "::__copyMembers(__p);";
	C.zeroIndent();
	C << nl << "#else";
	C.restoreIndent();
	C << nl << fixKwd(p->scoped()) + "::__copyMembers(__p);";
	C.zeroIndent();
	C << nl << "#endif";
	C.restoreIndent();
	C << nl << "return __p;";
	C << eb;
    }

    if(!p->isLocal())
    {
	ClassList allBases = p->allBases();
	StringList ids;
#if defined(__IBMCPP__) && defined(NDEBUG)
//
// VisualAge C++ 6.0 does not see that ClassDef is a Contained,
// when inlining is on. The code below issues a warning: better
// than an error!
//
	transform(allBases.begin(), allBases.end(), back_inserter(ids), ::IceUtil::constMemFun<string,ClassDef>(&Contained::scoped));
#else
	transform(allBases.begin(), allBases.end(), back_inserter(ids), ::IceUtil::constMemFun(&Contained::scoped));
#endif
	StringList other;
	other.push_back(p->scoped());
	other.push_back("::Ice::Object");
	other.sort();
	ids.merge(other);
	ids.unique();
        StringList::const_iterator firstIter = ids.begin();
        StringList::const_iterator scopedIter = find(ids.begin(), ids.end(), p->scoped());
        assert(scopedIter != ids.end());
        StringList::difference_type scopedPos = ice_distance(firstIter, scopedIter);

	StringList::const_iterator q;

	H << sp;
	H << nl << "virtual bool ice_isA"
	  << "(const ::std::string&, const ::Ice::Current& = ::Ice::Current()) const;";
	H << nl << "virtual ::std::vector< ::std::string> ice_ids"
	  << "(const ::Ice::Current& = ::Ice::Current()) const;";
	H << nl << "virtual const ::std::string& ice_id(const ::Ice::Current& = ::Ice::Current()) const;";
	H << nl << "static const ::std::string& ice_staticId();";

	string flatName = p->flattenedScope() + p->name() + "_ids";

	C << sp;
	C << nl << "static const ::std::string " << flatName << '[' << ids.size() << "] =";
	C << sb;
	q = ids.begin();
	while(q != ids.end())
	{
	    C << nl << '"' << *q << '"';
	    if(++q != ids.end())
	    {
		C << ',';
	    }
	}
	C << eb << ';';

	C << sp;
	C << nl << "bool" << nl << fixKwd(p->scoped()).substr(2)
          << "::ice_isA(const ::std::string& _s, const ::Ice::Current&) const";
	C << sb;
	C << nl << "return ::std::binary_search(" << flatName << ", " << flatName << " + " << ids.size() << ", _s);";
	C << eb;

	C << sp;
	C << nl << "::std::vector< ::std::string>" << nl << fixKwd(p->scoped()).substr(2)
	  << "::ice_ids(const ::Ice::Current&) const";
	C << sb;
	C << nl << "return ::std::vector< ::std::string>(&" << flatName << "[0], &" << flatName
	  << '[' << ids.size() << "]);";
	C << eb;

	C << sp;
	C << nl << "const ::std::string&" << nl << fixKwd(p->scoped()).substr(2)
	  << "::ice_id(const ::Ice::Current&) const";
	C << sb;
	C << nl << "return " << flatName << '[' << scopedPos << "];";
	C << eb;

	C << sp;
	C << nl << "const ::std::string&" << nl << fixKwd(p->scoped()).substr(2) << "::ice_staticId()";
	C << sb;
	C << nl << "return " << flatName << '[' << scopedPos << "];";
	C << eb;

	emitGCFunctions(p);
    }

    return true;
}

void
Slice::Gen::ObjectVisitor::visitClassDefEnd(const ClassDefPtr& p)
{
    string scoped = fixKwd(p->scoped());
    string scope = fixKwd(p->scope());

    if(!p->isLocal())
    {	
	ClassList bases = p->bases();
	ClassDefPtr base;
	if(!bases.empty() && !bases.front()->isInterface())
	{
	    base = bases.front();
	}
    
	OperationList allOps = p->allOperations();
	if(!allOps.empty())
	{
	    StringList allOpNames;
#if defined(__IBMCPP__) && defined(NDEBUG)
//
// See comment for transform above
//
	    transform(allOps.begin(), allOps.end(), back_inserter(allOpNames),
		      ::IceUtil::constMemFun<string,Operation>(&Contained::name));
#else
	    transform(allOps.begin(), allOps.end(), back_inserter(allOpNames),
                      ::IceUtil::constMemFun(&Contained::name));
#endif
	    allOpNames.push_back("ice_id");
	    allOpNames.push_back("ice_ids");
	    allOpNames.push_back("ice_isA");
	    allOpNames.push_back("ice_ping");
	    allOpNames.sort();
	    allOpNames.unique();

	    StringList::const_iterator q;
	    
	    H << sp;
	    H << nl
	      << "virtual ::IceInternal::DispatchStatus __dispatch(::IceInternal::Incoming&, const ::Ice::Current&);";

	    string flatName = p->flattenedScope() + p->name() + "_all";
	    C << sp;
	    C << nl << "static ::std::string " << flatName << "[] =";
	    C << sb;
	    q = allOpNames.begin();
	    while(q != allOpNames.end())
	    {
		C << nl << '"' << *q << '"';
		if(++q != allOpNames.end())
		{
		    C << ',';
		}
	    }
	    C << eb << ';';
	    C << sp;
	    C << nl << "::IceInternal::DispatchStatus" << nl << scoped.substr(2)
	      << "::__dispatch(::IceInternal::Incoming& in, const ::Ice::Current& current)";
	    C << sb;
	  
	    C << nl << "::std::pair< ::std::string*, ::std::string*> r = "
	      << "::std::equal_range(" << flatName << ", " << flatName << " + " << allOpNames.size()
	      << ", current.operation);";
	    C << nl << "if(r.first == r.second)";
	    C << sb;
	    C << nl << "return ::IceInternal::DispatchOperationNotExist;";
	    C << eb;
	    C << sp;
	    C << nl << "switch(r.first - " << flatName << ')';
	    C << sb;
	    int i = 0;
	    for(q = allOpNames.begin(); q != allOpNames.end(); ++q)
	    {
		C << nl << "case " << i++ << ':';
		C << sb;
		C << nl << "return ___" << *q << "(in, current);";
		C << eb;
	    }
	    C << eb;
	    C << sp;
	    C << nl << "assert(false);";
	    C << nl << "return ::IceInternal::DispatchOperationNotExist;";
	    C << eb;
	}
	
	H << sp;
	H << nl << "virtual void __write(::IceInternal::BasicStream*) const;";
	H << nl << "virtual void __read(::IceInternal::BasicStream*, bool);";
        H << nl << "virtual void __write(const ::Ice::OutputStreamPtr&) const;";
        H << nl << "virtual void __read(const ::Ice::InputStreamPtr&, bool);";

	TypeStringList memberList;
	DataMemberList dataMembers = p->dataMembers();
	for(DataMemberList::const_iterator q = dataMembers.begin(); q != dataMembers.end(); ++q)
	{
	    memberList.push_back(make_pair((*q)->type(), (*q)->name()));
	}
	C << sp;
	C << nl << "void" << nl << scoped.substr(2)
          << "::__write(::IceInternal::BasicStream* __os) const";
	C << sb;
	C << nl << "__os->writeTypeId(ice_staticId());";
	C << nl << "__os->startWriteSlice();";
	writeMarshalCode(C, memberList, 0);
	C << nl << "__os->endWriteSlice();";
	C.zeroIndent();
	C << nl << "#if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug"; // COMPILERBUG
	C.restoreIndent();
	C << nl << (base ? fixKwd(base->name()) : "Object") << "::__write(__os);";
	C.zeroIndent();
	C << nl << "#else";
	C.restoreIndent();
	C << nl << (base ? fixKwd(base->scoped()) : "::Ice::Object") << "::__write(__os);";
	C.zeroIndent();
	C << nl << "#endif";
	C.restoreIndent();
	C << eb;
	C << sp;
	C << nl << "void" << nl << scoped.substr(2) << "::__read(::IceInternal::BasicStream* __is, bool __rid)";
	C << sb;
	C << nl << "if(__rid)";
	C << sb;
	C << nl << "::std::string myId;";
	C << nl << "__is->readTypeId(myId);";
	C << eb;
	C << nl << "__is->startReadSlice();";
	writeUnmarshalCode(C, memberList, 0);
	C << nl << "__is->endReadSlice();";
	C.zeroIndent();
	C << nl << "#if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug"; // COMPILERBUG
	C.restoreIndent();
	C << nl << (base ? fixKwd(base->name()) : "Object") << "::__read(__is, true);";
	C.zeroIndent();
	C << nl << "#else";
	C.restoreIndent();
	C << nl << (base ? fixKwd(base->scoped()) : "::Ice::Object") << "::__read(__is, true);";
	C.zeroIndent();
	C << nl << "#endif";
	C.restoreIndent();
	C << eb;

	if(_stream)
	{
	    C << sp;
	    C << nl << "void" << nl << scoped.substr(2) << "::__write(const ::Ice::OutputStreamPtr& __outS) const";
	    C << sb;
	    C << nl << "__outS->writeTypeId(ice_staticId());";
	    C << nl << "__outS->startSlice();";
	    writeStreamMarshalCode(C, memberList, 0);
	    C << nl << "__outS->endSlice();";
	    C.zeroIndent();
	    C << nl << "#if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug"; // COMPILERBUG
	    C.restoreIndent();
	    C << nl << (base ? fixKwd(base->name()) : "Object") << "::__write(__outS);";
	    C.zeroIndent();
	    C << nl << "#else";
	    C.restoreIndent();
	    C << nl << (base ? fixKwd(base->scoped()) : "::Ice::Object") << "::__write(__outS);";
	    C.zeroIndent();
	    C << nl << "#endif";
	    C.restoreIndent();
	    C << eb;
	    C << sp;
	    C << nl << "void" << nl << scoped.substr(2) << "::__read(const ::Ice::InputStreamPtr& __inS, bool __rid)";
	    C << sb;
	    C << nl << "if(__rid)";
	    C << sb;
	    C << nl << "__inS->readTypeId();";
	    C << eb;
	    C << nl << "__inS->startSlice();";
	    writeStreamUnmarshalCode(C, memberList, 0);
	    C << nl << "__inS->endSlice();";
	    C.zeroIndent();
	    C << nl << "#if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug"; // COMPILERBUG
	    C.restoreIndent();
	    C << nl << (base ? fixKwd(base->name()) : "Object") << "::__read(__inS, true);";
	    C.zeroIndent();
	    C << nl << "#else";
	    C.restoreIndent();
	    C << nl << (base ? fixKwd(base->scoped()) : "::Ice::Object") << "::__read(__inS, true);";
	    C.zeroIndent();
	    C << nl << "#endif";
	    C.restoreIndent();
	    C << eb;
	}
        else
	{
            //
            // Emit placeholder functions to catch errors.
            //
	    C << sp;
	    C << nl << "void" << nl << scoped.substr(2) << "::__write(const ::Ice::OutputStreamPtr&) const";
	    C << sb;
            C << nl << "Ice::MarshalException ex(__FILE__, __LINE__);";
            C << nl << "ex.reason = \"type " << scoped.substr(2) << " was not generated with stream support\";";
            C << nl << "throw ex;";
	    C << eb;
	    C << sp;
	    C << nl << "void" << nl << scoped.substr(2) << "::__read(const ::Ice::InputStreamPtr&, bool)";
	    C << sb;
            C << nl << "Ice::MarshalException ex(__FILE__, __LINE__);";
            C << nl << "ex.reason = \"type " << scoped.substr(2) << " was not generated with stream support\";";
            C << nl << "throw ex;";
	    C << eb;
	}

	if(!p->isAbstract())
	{
	    H << sp << nl << "static const ::Ice::ObjectFactoryPtr& ice_factory();";

	    string factoryName = "__F" + p->flattenedScope() + p->name();
	    C << sp;
	    C << nl << "class " << factoryName << " : public ::Ice::ObjectFactory";
	    C << sb;
	    C.dec();
	    C << nl << "public:";
	    C.inc();
	    C << sp << nl << "virtual ::Ice::ObjectPtr" << nl << "create(const ::std::string& type)";
	    C << sb;
	    C << nl << "assert(type == " << scoped << "::ice_staticId());";
	    C << nl << "return new " << scoped << ';';
	    C << eb;
	    C << sp << nl << "virtual void" << nl << "destroy()";
	    C << sb;
	    C << eb;
	    C << eb << ';';

	    string flatName = factoryName + "_Ptr";
	    C << sp;
	    C << nl << "static ::Ice::ObjectFactoryPtr " << flatName << " = new " << factoryName << ';';

	    C << sp << nl << "const ::Ice::ObjectFactoryPtr&" << nl << scoped.substr(2) << "::ice_factory()";
	    C << sb;
	    C << nl << "return " << flatName << ';';
	    C << eb;

	    C << sp;
	    C << nl << "class " << factoryName << "__Init";
	    C << sb;
	    C.dec();
	    C << nl << "public:";
	    C.inc();
	    C << sp << nl << factoryName << "__Init()";
	    C << sb;
	    C << nl << "::Ice::factoryTable->addObjectFactory(" << scoped << "::ice_staticId(), "
	      << scoped << "::ice_factory());";
	    C << eb;
	    C << sp << nl << "~" << factoryName << "__Init()";
	    C << sb;
	    C << nl << "::Ice::factoryTable->removeObjectFactory(" << scoped << "::ice_staticId());";
	    C << eb;
	    C << eb << ';';

	    C << sp;
	    C << nl << "static " << factoryName << "__Init " << factoryName << "__i;";
	    C << sp << nl << "#ifdef __APPLE__";
	    std::string initfuncname = "__F" + p->flattenedScope() + p->name() + "__initializer";
	    C << nl << "extern \"C\" { void " << initfuncname << "() {} }";
	    C << nl << "#endif";
	}
    }

    H << eb << ';';

    if(!p->isAbstract() && !p->isLocal())
    {
	H << sp << nl << "static " << scoped << " __" << p->name() << "_init;";
    }

    if(p->isLocal())
    {
	C << sp;
	C << nl << "bool" << nl << scope.substr(2) << "operator==(const " << scoped
	  << "& l, const " << scoped << "& r)";
	C << sb;
	C << nl << "return static_cast<const ::Ice::LocalObject&>(l) == static_cast<const ::Ice::LocalObject&>(r);";
	C << eb;
	C << sp;
	C << nl << "bool" << nl << scope.substr(2) << "operator!=(const " << scoped
	  << "& l, const " << scoped << "& r)";
	C << sb;
	C << nl << "return static_cast<const ::Ice::LocalObject&>(l) != static_cast<const ::Ice::LocalObject&>(r);";
	C << eb;
	C << sp;
	C << nl << "bool" << nl << scope.substr(2) << "operator<(const " << scoped
	  << "& l, const " << scoped << "& r)";
	C << sb;
	C << nl << "return static_cast<const ::Ice::LocalObject&>(l) < static_cast<const ::Ice::LocalObject&>(r);";
	C << eb;
    }
    else
    {
	string name = p->name();

	H << sp << nl << "void " << _dllExport << "__patch__" << name << "Ptr(void*, ::Ice::ObjectPtr&);";

	C << sp << nl << "void " << _dllExport;
	C << nl << scope.substr(2) << "__patch__" << name << "Ptr(void* __addr, ::Ice::ObjectPtr& v)";
	C << sb;
	C << nl << scope << name << "Ptr* p = static_cast< " << scope << name << "Ptr*>(__addr);";
	C << nl << "assert(p);";
	C << nl << "*p = " << scope << name << "Ptr::dynamicCast(v);";
	C << nl << "if(v && !*p)";
	C << sb;
	C << nl << "::Ice::NoObjectFactoryException e(__FILE__, __LINE__);";
	C << nl << "e.type = " << scope << fixKwd(name) << "::ice_staticId();";
	C << nl << "throw e;";
	C << eb;
	C << eb;

	C << sp;
	C << nl << "bool" << nl << scope.substr(2) << "operator==(const " << scoped
	  << "& l, const " << scoped << "& r)";
	C << sb;
	C << nl << "return static_cast<const ::Ice::Object&>(l) == static_cast<const ::Ice::Object&>(r);";
	C << eb;
	C << sp;
	C << nl << "bool" << nl << scope.substr(2) << "operator!=(const " << scoped
	  << "& l, const " << scoped << "& r)";
	C << sb;
	C << nl << "return static_cast<const ::Ice::Object&>(l) != static_cast<const ::Ice::Object&>(r);";
	C << eb;
	C << sp;
	C << nl << "bool" << nl << scope.substr(2) << "operator<(const " << scoped
	  << "& l, const " << scoped << "& r)";
	C << sb;
	C << nl << "return static_cast<const ::Ice::Object&>(l) < static_cast<const ::Ice::Object&>(r);";
	C << eb;
    }
}

bool
Slice::Gen::ObjectVisitor::visitExceptionStart(const ExceptionPtr&)
{
    return false;
}

bool
Slice::Gen::ObjectVisitor::visitStructStart(const StructPtr&)
{
    return false;
}

void
Slice::Gen::ObjectVisitor::visitOperation(const OperationPtr& p)
{
    string name = p->name();
    string scoped = fixKwd(p->scoped());
    string scope = fixKwd(p->scope());

    TypePtr ret = p->returnType();
    string retS = returnTypeToString(ret);

    string params = "(";
    string paramsDecl = "(";
    string args = "(";

    ContainerPtr container = p->container();
    ClassDefPtr cl = ClassDefPtr::dynamicCast(container);
    string classNameAMD = "AMD_" + cl->name();
    string classScope = fixKwd(cl->scope());
    string classScopedAMD = classScope + classNameAMD;

    string paramsAMD = "(const " + classScopedAMD + '_' + name + "Ptr&, ";
    string paramsDeclAMD = "(const " + classScopedAMD + '_' + name + "Ptr& __cb, ";
    string argsAMD = "(__cb, ";

    TypeStringList inParams;
    TypeStringList outParams;
    ParamDeclList paramList = p->parameters();
    for(ParamDeclList::const_iterator q = paramList.begin(); q != paramList.end(); ++q)
    {
	string paramName = fixKwd((*q)->name());
	TypePtr type = (*q)->type();
	bool isOutParam = (*q)->isOutParam();
	string typeString;
	if(isOutParam)
	{
	    outParams.push_back(make_pair(type, paramName));
	    typeString = outputTypeToString(type);
	}
	else
	{
	    inParams.push_back(make_pair(type, paramName));
	    typeString = inputTypeToString((*q)->type());
	}

	if(q != paramList.begin())
	{
	    params += ", ";
	    paramsDecl += ", ";
	    args += ", ";
	}

	params += typeString;
	paramsDecl += typeString;
	paramsDecl += ' ';
	paramsDecl += paramName;
	args += paramName;

	if(!isOutParam)
	{
	    paramsAMD += typeString;
	    paramsAMD += ", ";
	    paramsDeclAMD += typeString;
	    paramsDeclAMD += ' ';
	    paramsDeclAMD += paramName;
	    paramsDeclAMD += ", ";
	    argsAMD += paramName;
	    argsAMD += ", ";
	}
    }

    if(!cl->isLocal())
    {
	if(!paramList.empty())
	{
	    params += ", ";
	    paramsDecl += ", ";
	    args += ", ";
	}

	params += "const ::Ice::Current& = ::Ice::Current())";
	paramsDecl += "const ::Ice::Current& __current)";
	args += "__current)";
    }
    else
    {
	params += ')';
	paramsDecl += ')';
	args += ')';
    }

    paramsAMD += "const ::Ice::Current& = ::Ice::Current())";
    paramsDeclAMD += "const ::Ice::Current& __current)";
    argsAMD += "__current)";
    
    bool nonmutating = p->mode() == Operation::Nonmutating;
    bool amd = !cl->isLocal() && (cl->hasMetaData("amd") || p->hasMetaData("amd"));

    H << sp;
    if(!amd)
    {
	H << nl << "virtual " << retS << ' ' << fixKwd(name) << params
	  << (nonmutating ? " const" : "") << " = 0;";
    }
    else
    {
	H << nl << "virtual void " << name << "_async" << paramsAMD
	  << (nonmutating ? " const" : "") << " = 0;";
    }	

    if(!cl->isLocal())
    {
	H << nl << "::IceInternal::DispatchStatus ___" << name
	  << "(::IceInternal::Incoming&, const ::Ice::Current&)" << (nonmutating ? " const" : "") << ';';

	C << sp;
	C << nl << "::IceInternal::DispatchStatus" << nl << scope.substr(2) << "___" << name
	  << "(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)" << (nonmutating ? " const" : "");
	C << sb;
	if(!amd)
	{
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
	    throws.sort(derivedToBaseCompare);
#else
	    throws.sort(Slice::DerivedToBaseCompare());
#endif

	    if(!inParams.empty())
	    {
		C << nl << "::IceInternal::BasicStream* __is = __inS.is();";
	    }
	    if(ret || !outParams.empty() || !throws.empty())
	    {
		C << nl << "::IceInternal::BasicStream* __os = __inS.os();";
	    }
	    writeAllocateCode(C, inParams, 0);
	    writeUnmarshalCode(C, inParams, 0);
	    if(p->sendsClasses())
	    {
		C << nl << "__is->readPendingObjects();";
	    }
	    writeAllocateCode(C, outParams, 0);
	    if(!throws.empty())
	    {
		C << nl << "try";
		C << sb;
	    }
	    C << nl;
	    if(ret)
	    {
		C << retS << " __ret = ";
	    }
	    C << fixKwd(name) << args << ';';
	    writeMarshalCode(C, outParams, ret);
	    if(p->returnsClasses())
	    {
		C << nl << "__os->writePendingObjects();";
	    }
	    if(!throws.empty())
	    {
		C << eb;
		ExceptionList::const_iterator r;
		for(r = throws.begin(); r != throws.end(); ++r)
		{
		    C << nl << "catch(const " << fixKwd((*r)->scoped()) << "& __ex)";
		    C << sb;
		    C << nl << "__os->write(__ex);";
		    C << nl << "return ::IceInternal::DispatchUserException;";
		    C << eb;
		}
	    }
	    C << nl << "return ::IceInternal::DispatchOK;";
	}
	else
	{
	    if(!inParams.empty())
	    {
		C << nl << "::IceInternal::BasicStream* __is = __inS.is();";
	    }
	    writeAllocateCode(C, inParams, 0);
	    writeUnmarshalCode(C, inParams, 0);
	    if(p->sendsClasses())
	    {
		C << nl << "__is->readPendingObjects();";
	    }
	    C << nl << classScopedAMD << '_' << name << "Ptr __cb = new IceAsync" << classScopedAMD << '_' << name
	      << "(__inS);";
	    C << nl << "try";
	    C << sb;
	    C << nl << name << "_async" << argsAMD << ';';
	    C << eb;
	    C << nl << "catch(const ::Ice::Exception& __ex)";
	    C << sb;
	    C << nl << "__cb->ice_exception(__ex);";
	    C << eb;
	    C << nl << "catch(const ::std::exception& __ex)";
	    C << sb;
	    C << nl << "__cb->ice_exception(__ex);";
	    C << eb;
	    C << nl << "catch(...)";
	    C << sb;
	    C << nl << "__cb->ice_exception();";
	    C << eb;
	    C << nl << "return ::IceInternal::DispatchAsync;";
	}	    
	C << eb;
    }	
}

void
Slice::Gen::ObjectVisitor::visitDataMember(const DataMemberPtr& p)
{
    string name = fixKwd(p->name());
    string s = typeToString(p->type());
    H << sp;
    H << nl << s << ' ' << name << ';';
}

void
Slice::Gen::ObjectVisitor::emitGCFunctions(const ClassDefPtr& p)
{
    string scoped = fixKwd(p->scoped());
    ClassList bases = p->bases();
    DataMemberList dataMembers = p->dataMembers();

    //
    // A class can potentially be part of a cycle if it (recursively) contains class
    // members. If so, we override __incRef() and __decRef() and, hence, consider instances
    // of the class as candidates for collection by the garbage collector.
    // We override __incRef() and __decRef() only once, in the basemost potentially cyclic class
    // in an inheritance hierarchy.
    //
    bool hasBaseClass = !bases.empty() && !bases.front()->isInterface();
    bool canBeCyclic = p->canBeCyclic();
    bool override = canBeCyclic && (!hasBaseClass || !bases.front()->canBeCyclic());

    if(override)
    {
	H << nl << "virtual void __incRef();";

	C << sp << nl << "void" << nl << scoped.substr(2) << "::__incRef()";
	C << sb;
	C << nl << "IceUtil::gcRecMutex._m->lock();";
	C << nl << "assert(_ref >= 0);";
        C << nl << "if(_ref == 0)";
	C << sb;
	C.zeroIndent();
	C << nl << "#ifdef NDEBUG // To avoid annoying warnings about variables that are not used...";
	C.restoreIndent();
	C << nl << "IceUtil::gcObjects.insert(this);";
	C.zeroIndent();
	C << nl << "#else";
	C.restoreIndent();
	C << nl << "std::pair<IceUtil::GCObjectSet::iterator, bool> rc = IceUtil::gcObjects.insert(this);";
	C << nl << "assert(rc.second);";
	C.zeroIndent();
	C << nl << "#endif";
	C.restoreIndent();
	C << eb;
	C << nl << "++_ref;";
	C << nl << "IceUtil::gcRecMutex._m->unlock();";
	C << eb;

	H << nl << "virtual void __decRef();";

	C << sp << nl << "void" << nl << scoped.substr(2) << "::__decRef()";
	C << sb;
	C << nl << "IceUtil::gcRecMutex._m->lock();";
	C << nl << "bool doDelete = false;";
	C << nl << "assert(_ref > 0);";
	C << nl << "if(--_ref == 0)";
	C << sb;
	C << nl << "doDelete = !_noDelete;";
	C << nl << "_noDelete = true;";
        C.zeroIndent();
	C << nl << "#ifdef NDEBUG // To avoid annoying warnings about variables that are not used...";
	C.restoreIndent();
	C << nl << "IceUtil::gcObjects.erase(this);";
        C.zeroIndent();
	C << nl << "#else";
	C.restoreIndent();
	C << nl << "IceUtil::GCObjectSet::size_type num = IceUtil::gcObjects.erase(this);";
	C << nl << "assert(num == 1);";
        C.zeroIndent();
	C << nl << "#endif";
	C.restoreIndent();
	C << eb;
	C << nl << "IceUtil::gcRecMutex._m->unlock();";
	C << nl << "if(doDelete)";
	C << sb;
	C << nl << "delete this;";
	C << eb;
	C << eb;
    }

    //
    // __gcReachable() and __gcClear() are overridden by the basemost class that
    // can be cyclic, plus all classes derived from that class.
    //
    if(canBeCyclic)
    {
	H << nl << "virtual void __gcReachable(::IceUtil::GCObjectMultiSet&) const;";

	C << sp << nl << "void" << nl << scoped.substr(2) << "::__gcReachable(::IceUtil::GCObjectMultiSet& _c) const";
	C << sb;

	string vc6Prefix;
	string otherPrefix;

	bool hasCyclicBase = hasBaseClass && bases.front()->canBeCyclic();
	if(hasCyclicBase)
	{
	    vc6Prefix = bases.front()->name();
	    otherPrefix = bases.front()->scoped();

	    //
	    // Up-call to the base's __gcReachable() member function.
	    //
	    C.zeroIndent();
	    C << nl << "#if defined(_MSC_VER) && (MSC_VER < 1300) // VC++ 6 compiler bug";
	    C.restoreIndent();
	    C << nl << vc6Prefix << "::__gcReachable(_c);";
	    C.zeroIndent();
	    C << nl << "#else";
	    C.restoreIndent();
	    C << nl << otherPrefix << "::__gcReachable(_c);";
	    C.zeroIndent();
	    C << nl << "#endif";
	    C.restoreIndent();
	}
	for(DataMemberList::const_iterator i = dataMembers.begin(); i != dataMembers.end(); ++i)
	{
	    if((*i)->type()->usesClasses())
	    {
		emitGCInsertCode((*i)->type(), fixKwd((*i)->name()), "", 0);
	    }
	}
	C << eb;

	H << nl << "virtual void __gcClear();";

	C << sp << nl << "void" << nl << scoped.substr(2) << "::__gcClear()";
	C << sb;
	if(hasCyclicBase)
	{
	    //
	    // Up-call to the base's __gcClear() member function.
	    //
	    C.zeroIndent();
	    C << nl << "#if defined(_MSC_VER) && (MSC_VER < 1300) // VC++ 6 compiler bug";
	    C.restoreIndent();
	    C << nl << vc6Prefix<< "::__gcClear();";
	    C.zeroIndent();
	    C << nl << "#else";
	    C.restoreIndent();
	    C << nl << otherPrefix << "::__gcClear();";
	    C.zeroIndent();
	    C << nl << "#endif";
	    C.restoreIndent();
	}
	for(DataMemberList::const_iterator j = dataMembers.begin(); j != dataMembers.end(); ++j)
	{
	    if((*j)->type()->usesClasses())
	    {
		emitGCClearCode((*j)->type(), fixKwd((*j)->name()), "", 0);
	    }
	}
	C << eb;
    }
}

void
Slice::Gen::ObjectVisitor::emitGCInsertCode(const TypePtr& p, const string& prefix, const string& name, int level)
{
    if((BuiltinPtr::dynamicCast(p) && BuiltinPtr::dynamicCast(p)->kind() == Builtin::KindObject)
       || ClassDeclPtr::dynamicCast(p))
    {
	C << nl << "__addObject(_c, " << prefix << name << ".get());";
    }
    else if(StructPtr s = StructPtr::dynamicCast(p))
    {
	DataMemberList dml = s->dataMembers();
	for(DataMemberList::const_iterator i = dml.begin(); i != dml.end(); ++i)
	{
	    if((*i)->type()->usesClasses())
	    {
		emitGCInsertCode((*i)->type(), prefix + name + ".", fixKwd((*i)->name()), ++level);
	    }
	}
    }
    else if(DictionaryPtr d = DictionaryPtr::dynamicCast(p))
    {
	string scoped = fixKwd(d->scoped());
	stringstream tmp;
	tmp << "_i" << level;
	string iterName = tmp.str();
	C << sb;
	C << nl << "for(" << scoped << "::const_iterator " << iterName << " = " << prefix + name
          << ".begin(); " << iterName << " != " << prefix + name << ".end(); ++" << iterName << ")";
	C << sb;
	emitGCInsertCode(d->valueType(), "", string("(*") + iterName + ").second", ++level);
	C << eb;
	C << eb;
    }
    else if(SequencePtr s = SequencePtr::dynamicCast(p))
    {
	string scoped = fixKwd(s->scoped());
	stringstream tmp;
	tmp << "_i" << level;
	string iterName = tmp.str();
	C << sb;
	C << nl << "for(" << scoped << "::const_iterator " << iterName << " = " << prefix + name
          << ".begin(); " << iterName << " != " << prefix + name << ".end(); ++" << iterName << ")";
	C << sb;
	emitGCInsertCode(s->type(), string("(*") + iterName + ")", "", ++level);
	C << eb;
	C << eb;
    }
}

void
Slice::Gen::ObjectVisitor::emitGCClearCode(const TypePtr& p, const string& prefix, const string& name, int level)
{
    if((BuiltinPtr::dynamicCast(p) && BuiltinPtr::dynamicCast(p)->kind() == Builtin::KindObject)
       || ClassDeclPtr::dynamicCast(p))
    {
	C << nl << "if(" << prefix << name << ")";
	C << sb;
	C << nl << prefix << name << "->__decRefUnsafe();";
	C << nl << prefix << name << ".__clearHandleUnsafe();";
	C << eb;
    }
    else if(StructPtr s = StructPtr::dynamicCast(p))
    {
	DataMemberList dml = s->dataMembers();
	for(DataMemberList::const_iterator i = dml.begin(); i != dml.end(); ++i)
	{
	    if((*i)->type()->usesClasses())
	    {
		emitGCClearCode((*i)->type(), prefix + name + ".", fixKwd((*i)->name()), ++level);
	    }
	}
    }
    else if(DictionaryPtr d = DictionaryPtr::dynamicCast(p))
    {
	string scoped = fixKwd(d->scoped());
	stringstream tmp;
	tmp << "_i" << level;
	string iterName = tmp.str();
	C << sb;
	C << nl << "for(" << scoped << "::iterator " << iterName << " = " << prefix + name
          << ".begin(); " << iterName << " != " << prefix + name << ".end(); ++" << iterName << ")";
	C << sb;
	emitGCClearCode(d->valueType(), "", string("(*") + iterName + ").second", ++level);
	C << eb;
	C << eb;
    }
    else if(SequencePtr s = SequencePtr::dynamicCast(p))
    {
	string scoped = fixKwd(s->scoped());
	stringstream tmp;
	tmp << "_i" << level;
	string iterName = tmp.str();
	C << sb;
	C << nl << "for(" << scoped << "::iterator " << iterName << " = " << prefix + name
          << ".begin(); " << iterName << " != " << prefix + name << ".end(); ++" << iterName << ")";
	C << sb;
	emitGCClearCode(s->type(), "", string("(*") + iterName + ")", ++level);
	C << eb;
	C << eb;;
    }
}

Slice::Gen::IceInternalVisitor::IceInternalVisitor(Output& h, Output& c, const string& dllExport) :
    H(h), C(c), _dllExport(dllExport)
{
}

bool
Slice::Gen::IceInternalVisitor::visitUnitStart(const UnitPtr& p)
{
    if(!p->hasClassDecls())
    {
	return false;
    }

    H << sp;
    H << nl << "namespace IceInternal" << nl << '{';

    return true;
}

void
Slice::Gen::IceInternalVisitor::visitUnitEnd(const UnitPtr& p)
{
    H << sp;
    H << nl << '}';
}
    
void
Slice::Gen::IceInternalVisitor::visitClassDecl(const ClassDeclPtr& p)
{
    string scoped = fixKwd(p->scoped());
    
    H << sp;
    H << nl << _dllExport << "void incRef(" << scoped << "*);";
    H << nl << _dllExport << "void decRef(" << scoped << "*);";
    if(!p->isLocal())
    {
	H << sp;
	H << nl << _dllExport << "void incRef(::IceProxy" << scoped << "*);";
	H << nl << _dllExport << "void decRef(::IceProxy" << scoped << "*);";
    }
}

bool
Slice::Gen::IceInternalVisitor::visitClassDefStart(const ClassDefPtr& p)
{
    string scoped = fixKwd(p->scoped());
    
    C << sp;
    C << nl << "void" << nl << "IceInternal::incRef(" << scoped << "* p)";
    C << sb;
    C << nl << "p->__incRef();";
    C << eb;

    C << sp;
    C << nl << "void" << nl << "IceInternal::decRef(" << scoped << "* p)";
    C << sb;
    C << nl << "p->__decRef();";
    C << eb;

    if(!p->isLocal())
    {
	C << sp;
	C << nl << "void" << nl << "IceInternal::incRef(::IceProxy" << scoped << "* p)";
	C << sb;
	C << nl << "p->__incRef();";
	C << eb;

	C << sp;
	C << nl << "void" << nl << "IceInternal::decRef(::IceProxy" << scoped << "* p)";
	C << sb;
	C << nl << "p->__decRef();";
	C << eb;
    }

    return true;
}

Slice::Gen::HandleVisitor::HandleVisitor(Output& h, Output& c, const string& dllExport, bool stream) :
    H(h), C(c), _dllExport(dllExport), _stream(stream)
{
}

bool
Slice::Gen::HandleVisitor::visitModuleStart(const ModulePtr& p)
{
    if(!p->hasClassDecls())
    {
	return false;
    }

    string name = fixKwd(p->name());
    
    H << sp;
    H << nl << "namespace " << name << nl << '{';

    return true;
}

void
Slice::Gen::HandleVisitor::visitModuleEnd(const ModulePtr& p)
{
    H << sp;
    H << nl << '}';
}

void
Slice::Gen::HandleVisitor::visitClassDecl(const ClassDeclPtr& p)
{
    string name = p->name();
    string scoped = fixKwd(p->scoped());
    
    H << sp;
    H << nl << "typedef ::IceInternal::Handle< " << scoped << "> " << name << "Ptr;";
    if(!p->isLocal())
    {
	H << nl << "typedef ::IceInternal::ProxyHandle< ::IceProxy" << scoped << "> " << name << "Prx;";

	H << sp;
	H << nl << _dllExport << "void __write(::IceInternal::BasicStream*, const " << name << "Prx&);";
	H << nl << _dllExport << "void __read(::IceInternal::BasicStream*, " << name << "Prx&);";
	H << nl << _dllExport << "void __write(::IceInternal::BasicStream*, const " << name << "Ptr&);";
	H << nl << _dllExport << "void __patch__" << name << "Ptr(void*, ::Ice::ObjectPtr&);";
	if(_stream)
	{
            H << sp;
	    H << nl << _dllExport << "void ice_write" << name << "Prx(const ::Ice::OutputStreamPtr&, const " << name
              << "Prx&);";
	    H << nl << _dllExport << "void ice_read" << name << "Prx(const ::Ice::InputStreamPtr&, " << name
              << "Prx&);";

            H << nl << _dllExport << "void ice_write" << name << "(const ::Ice::OutputStreamPtr&, const "
              << name << "Ptr&);";
            H << nl << _dllExport << "void ice_read" << name << "(const ::Ice::InputStreamPtr&, " << name << "Ptr&);";
	}
    }
}

bool
Slice::Gen::HandleVisitor::visitClassDefStart(const ClassDefPtr& p)
{
    if(!p->isLocal())
    {
        string name = p->name();
	string scoped = fixKwd(p->scoped());
	string scope = fixKwd(p->scope());

	string factory;
	string type;
	if(!p->isAbstract())
	{
	    type = scoped + "::ice_staticId()";
	    factory = scoped + "::ice_factory()";
	}
	else
	{
	    type = "\"\"";
	    factory = "0";
	}

	C << sp;
	C << nl << "void" << nl << scope.substr(2) << "__write(::IceInternal::BasicStream* __os, const "
	  << scope << name << "Prx& v)";
	C << sb;
	C << nl << "__os->write(::Ice::ObjectPrx(v));";
	C << eb;

	C << sp;
	C << nl << "void" << nl << scope.substr(2) << "__read(::IceInternal::BasicStream* __is, "
	  << scope << name << "Prx& v)";
	C << sb;
	C << nl << "::Ice::ObjectPrx proxy;";
	C << nl << "__is->read(proxy);";
	C << nl << "if(!proxy)";
	C << sb;
	C << nl << "v = 0;";
	C << eb;
	C << nl << "else";
	C << sb;
	C << nl << "v = new ::IceProxy" << scoped << ';';
	C << nl << "v->__copyFrom(proxy);";
	C << eb;
	C << eb;

	C << sp;
	C << nl << "void" << nl << scope.substr(2) << "__write(::IceInternal::BasicStream* __os, const "
	   << scope << name << "Ptr& v)";
	C << sb;
	C << nl << "__os->write(::Ice::ObjectPtr(v));";
	C << eb;

	if(_stream)
	{
	    C << sp;
	    C << nl << "void" << nl << scope.substr(2) << "ice_write" << name
              << "Prx(const ::Ice::OutputStreamPtr& __outS, const " << scope << name << "Prx& v)";
	    C << sb;
	    C << nl << "__outS->writeProxy(v);";
	    C << eb;

	    C << sp;
	    C << nl << "void" << nl << scope.substr(2) << "ice_read" << name
              << "Prx(const ::Ice::InputStreamPtr& __inS, " << scope << name << "Prx& v)";
	    C << sb;
	    C << nl << "::Ice::ObjectPrx proxy = __inS->readProxy();";
	    C << nl << "if(!proxy)";
	    C << sb;
	    C << nl << "v = 0;";
	    C << eb;
	    C << nl << "else";
	    C << sb;
	    C << nl << "v = new ::IceProxy" << scoped << ';';
	    C << nl << "v->__copyFrom(proxy);";
	    C << eb;
	    C << eb;

	    C << sp;
	    C << nl << "void" << nl << scope.substr(2) << "ice_write" << name
              << "(const ::Ice::OutputStreamPtr& __outS, const " << scope << name << "Ptr& v)";
	    C << sb;
	    C << nl << "__outS->writeObject(v);";
	    C << eb;

            C << sp;
            C << nl << "void" << nl << scope.substr(2) << "ice_read" << name << "(const ::Ice::InputStreamPtr& __inS, "
              << scoped << "Ptr& __v)";
            C << sb;
            C << nl << "::Ice::ReadObjectCallbackPtr __cb = new ::Ice::ReadObjectCallbackI(" << scope << "__patch__"
              << name << "Ptr, &__v);";
            C << nl << "__inS->readObject(__cb);";
            C << eb;
	}
    }

    return true;
}

Slice::Gen::ImplVisitor::ImplVisitor(Output& h, Output& c,
                                     const string& dllExport) :
    H(h), C(c), _dllExport(dllExport)
{
}

void
Slice::Gen::ImplVisitor::writeDecl(Output& out, const string& name, const TypePtr& type)
{
    out << nl << typeToString(type) << ' ' << name;

    BuiltinPtr builtin = BuiltinPtr::dynamicCast(type);
    if(builtin)
    {
        switch(builtin->kind())
        {
            case Builtin::KindBool:
            {
                out << " = false";
                break;
            }
            case Builtin::KindByte:
            case Builtin::KindShort:
            case Builtin::KindInt:
            case Builtin::KindLong:
            {
                out << " = 0";
                break;
            }
            case Builtin::KindFloat:
            case Builtin::KindDouble:
            {
                out << " = 0.0";
                break;
            }
            case Builtin::KindString:
            case Builtin::KindObject:
            case Builtin::KindObjectProxy:
            case Builtin::KindLocalObject:
            {
                break;
            }
        }
    }

    EnumPtr en = EnumPtr::dynamicCast(type);
    if(en)
    {
        EnumeratorList enumerators = en->getEnumerators();
        out << " = " << fixKwd(en->scope()) << fixKwd(enumerators.front()->name());
    }

    out << ';';
}

void
Slice::Gen::ImplVisitor::writeReturn(Output& out, const TypePtr& type)
{
    BuiltinPtr builtin = BuiltinPtr::dynamicCast(type);
    if(builtin)
    {
        switch(builtin->kind())
        {
            case Builtin::KindBool:
            {
                out << nl << "return false;";
                break;
            }
            case Builtin::KindByte:
            case Builtin::KindShort:
            case Builtin::KindInt:
            case Builtin::KindLong:
            {
                out << nl << "return 0;";
                break;
            }
            case Builtin::KindFloat:
            case Builtin::KindDouble:
            {
                out << nl << "return 0.0;";
                break;
            }
            case Builtin::KindString:
            {
                out << nl << "return ::std::string();";
                break;
            }
            case Builtin::KindObject:
            case Builtin::KindObjectProxy:
            case Builtin::KindLocalObject:
            {
                out << nl << "return 0;";
                break;
            }
        }
    }
    else
    {
        ProxyPtr prx = ProxyPtr::dynamicCast(type);
        if(prx)
	{
	    out << nl << "return 0;";
	}
	else
	{
	    ClassDeclPtr cl = ClassDeclPtr::dynamicCast(type);
	    if(cl)
	    {
		out << nl << "return 0;";
	    }
	    else
	    {
		StructPtr st = StructPtr::dynamicCast(type);
		if(st)
		{
		    out << nl << "return " << fixKwd(st->scoped()) << "();";
		}
		else
		{
		    EnumPtr en = EnumPtr::dynamicCast(type);
		    if(en)
		    {
			EnumeratorList enumerators = en->getEnumerators();
			out << nl << "return " << fixKwd(en->scope()) << fixKwd(enumerators.front()->name()) << ';';
		    }
		    else
		    {
			SequencePtr seq = SequencePtr::dynamicCast(type);
			if(seq)
			{
			    out << nl << "return " << fixKwd(seq->scoped()) << "();";
			}
			else
			{
			    DictionaryPtr dict = DictionaryPtr::dynamicCast(type);
			    assert(dict);
			    out << nl << "return " << fixKwd(dict->scoped()) << "();";
			}
		    }
		}
	    }
	}
    }
}

bool
Slice::Gen::ImplVisitor::visitModuleStart(const ModulePtr& p)
{
    if(!p->hasClassDefs())
    {
        return false;
    }

    string name = fixKwd(p->name());
    
    H << sp << nl << "namespace " << name << nl << '{';

    return true;
}

void
Slice::Gen::ImplVisitor::visitModuleEnd(const ModulePtr& p)
{
    H << sp;
    H << nl << '}';
}

bool
Slice::Gen::ImplVisitor::visitClassDefStart(const ClassDefPtr& p)
{
    if(!p->isAbstract())
    {
        return false;
    }

    string name = p->name();
    string scope = fixKwd(p->scope());
    string cls = scope.substr(2) + name + "I";
    string classScopedAMD = scope + "AMD_" + name;

    ClassList bases = p->bases();
    ClassDefPtr base;
    if(!bases.empty() && !bases.front()->isInterface())
    {
        base = bases.front();
    }

    H << sp;
    H << nl << "class " << name << "I : ";
    H.useCurrentPosAsIndent();
    H << "virtual public " << fixKwd(name);
    for(ClassList::const_iterator q = bases.begin(); q != bases.end(); ++q)
    {
        H << ',' << nl << "virtual public " << fixKwd((*q)->scope());
	if((*q)->isAbstract())
	{
	    H << (*q)->name() << "I";
	}
	else
	{
	    H << fixKwd((*q)->name());
	}
    }
    H.restoreIndent();

    H << sb;
    H.dec();
    H << nl << "public:";
    H.inc();

    OperationList ops = p->operations();
    OperationList::const_iterator r;

    for(r = ops.begin(); r != ops.end(); ++r)
    {
        OperationPtr op = (*r);
        string opName = op->name();

        TypePtr ret = op->returnType();
        string retS = returnTypeToString(ret);

        if(!p->isLocal() && (p->hasMetaData("amd") || op->hasMetaData("amd")))
        {
            H << sp << nl << "virtual void " << opName << "_async(";
            H.useCurrentPosAsIndent();
            H << "const " << classScopedAMD << '_' << opName << "Ptr&";
            ParamDeclList paramList = op->parameters();
            ParamDeclList::const_iterator q;
            for(q = paramList.begin(); q != paramList.end(); ++q)
            {
                if(!(*q)->isOutParam())
                {
                    H << ',' << nl << inputTypeToString((*q)->type());
                }
            }
            H << ',' << nl << "const Ice::Current&";
            H.restoreIndent();

            bool nonmutating = op->mode() == Operation::Nonmutating;

            H << ")" << (nonmutating ? " const" : "") << ';';

            C << sp << nl << "void" << nl << scope << name << "I::" << opName << "_async(";
            C.useCurrentPosAsIndent();
            C << "const " << classScopedAMD << '_' << opName << "Ptr& " << opName << "CB";
            for(q = paramList.begin(); q != paramList.end(); ++q)
            {
                if(!(*q)->isOutParam())
                {
                    C << ',' << nl << inputTypeToString((*q)->type()) << ' ' << fixKwd((*q)->name());
                }
            }
            C << ',' << nl << "const Ice::Current& current";
            C.restoreIndent();
            C << ")" << (nonmutating ? " const" : "");
            C << sb;

            string result = "r";
            for(q = paramList.begin(); q != paramList.end(); ++q)
            {
                if((*q)->name() == result)
                {
                    result = "_" + result;
                    break;
                }
            }
            if(ret)
            {
                writeDecl(C, result, ret);
            }
            for(q = paramList.begin(); q != paramList.end(); ++q)
            {
                if((*q)->isOutParam())
                {
                    writeDecl(C, fixKwd((*q)->name()), (*q)->type());
                }
            }

            C << nl << opName << "CB->ice_response(";
            if(ret)
            {
                C << result;
            }
            for(q = paramList.begin(); q != paramList.end(); ++q)
            {
                if((*q)->isOutParam())
                {
                    if(ret || q != paramList.begin())
                    {
                        C << ", ";
                    }
                    C << fixKwd((*q)->name());
                }
            }
            C << ");";

            C << eb;
        }
        else
        {
            H << sp << nl << "virtual " << retS << ' ' << fixKwd(opName) << '(';
            H.useCurrentPosAsIndent();
            ParamDeclList paramList = op->parameters();
            ParamDeclList::const_iterator q;
            for(q = paramList.begin(); q != paramList.end(); ++q)
            {
                if(q != paramList.begin())
                {
                    H << ',' << nl;
                }
#if defined(__SUNPRO_CC) && (__SUNPRO_CC==0x550)
		//
		// Work around for Sun CC 5.5 bug #4853566
		//
		string typeString;
		if((*q)->isOutParam())
		{
		    typeString = outputTypeToString((*q)->type());
		}
		else
		{
		    typeString = inputTypeToString((*q)->type());
		}
#else
                string typeString = (*q)->isOutParam() ?
                    outputTypeToString((*q)->type()) : inputTypeToString((*q)->type());
#endif
                H << typeString;
            }
            if(!p->isLocal())
            {
                if(!paramList.empty())
                {
                    H << ',' << nl;
                }
                H << "const Ice::Current&";
            }
            H.restoreIndent();

            bool nonmutating = op->mode() == Operation::Nonmutating;

            H << ")" << (nonmutating ? " const" : "") << ';';

            C << sp << nl << retS << nl;
	    C << scope.substr(2) << name << "I::" << fixKwd(opName) << '(';
            C.useCurrentPosAsIndent();
            for(q = paramList.begin(); q != paramList.end(); ++q)
            {
                if(q != paramList.begin())
                {
                    C << ',' << nl;
                }
#if defined(__SUNPRO_CC) && (__SUNPRO_CC==0x550)
		//
		// Work around for Sun CC 5.5 bug #4853566
		//
		string typeString;
		if((*q)->isOutParam())
		{
		    typeString = outputTypeToString((*q)->type());
		}
		else
		{
		    typeString = inputTypeToString((*q)->type());
		}
#else
                string typeString = (*q)->isOutParam() ?
                    outputTypeToString((*q)->type()) : inputTypeToString((*q)->type());
#endif
                C << typeString << ' ' << fixKwd((*q)->name());
            }
            if(!p->isLocal())
            {
                if(!paramList.empty())
                {
                    C << ',' << nl;
                }
                C << "const Ice::Current& current";
            }
            C.restoreIndent();
            C << ')';
	    C << (nonmutating ? " const" : "");
            C << sb;

            if(ret)
            {
                writeReturn(C, ret);
            }

            C << eb;
        }
    }

    H << eb << ';';

    return true;
}

Slice::Gen::AsyncVisitor::AsyncVisitor(Output& h, Output& c, const string& dllExport) :
    H(h), C(c), _dllExport(dllExport)
{
}

bool
Slice::Gen::AsyncVisitor::visitModuleStart(const ModulePtr& p)
{
    if(!p->hasNonLocalClassDecls() || (!p->hasContentsWithMetaData("ami") && !p->hasContentsWithMetaData("amd")))
    {
	return false;
    }

    string name = fixKwd(p->name());
    
    H << sp << nl << "namespace " << name << nl << '{';

    return true;
}

void
Slice::Gen::AsyncVisitor::visitModuleEnd(const ModulePtr& p)
{
    H << sp << nl << '}';
}

void
Slice::Gen::AsyncVisitor::visitOperation(const OperationPtr& p)
{
    ContainerPtr container = p->container();
    ClassDefPtr cl = ClassDefPtr::dynamicCast(container);
    
    if(cl->isLocal() ||
       (!cl->hasMetaData("ami") && !p->hasMetaData("ami") && !cl->hasMetaData("amd") && !p->hasMetaData("amd")))
    {
	return;
    }

    string name = p->name();
    
    string className = cl->name();
    string classNameAMI = "AMI_" + className;
    string classNameAMD = "AMD_" + className;
    string classScope = fixKwd(cl->scope());
    string classScopedAMI = classScope + classNameAMI;
    string classScopedAMD = classScope + classNameAMD;
    string proxyName = classScope + className + "Prx";
    
    vector<string> params;
    vector<string> paramsDecl;
    vector<string> args;
    
    vector<string> paramsInvoke;
    vector<string> paramsDeclInvoke;

    paramsInvoke.push_back("const " + proxyName + "&");
    paramsDeclInvoke.push_back("const " + proxyName + "& __prx");

    TypePtr ret = p->returnType();
    string retS = inputTypeToString(ret);
    
    if(ret)
    {
	params.push_back(retS);
	paramsDecl.push_back(retS + " __ret");
	args.push_back("__ret");
    }
    
    TypeStringList inParams;
    TypeStringList outParams;
    ParamDeclList paramList = p->parameters();
    for(ParamDeclList::const_iterator q = paramList.begin(); q != paramList.end(); ++q)
    {
	string paramName = fixKwd((*q)->name());
	TypePtr type = (*q)->type();
	string typeString = inputTypeToString(type);

	if((*q)->isOutParam())
	{
	    params.push_back(typeString);
	    paramsDecl.push_back(typeString + ' ' + paramName);
	    args.push_back(paramName);

	    outParams.push_back(make_pair(type, paramName));
	}
	else
	{
	    paramsInvoke.push_back(typeString);
	    paramsDeclInvoke.push_back(typeString + ' ' + paramName);

	    inParams.push_back(make_pair(type, paramName));
	}
    }

    paramsInvoke.push_back("const ::Ice::Context&");
    paramsDeclInvoke.push_back("const ::Ice::Context& __ctx");

    if(cl->hasMetaData("ami") || p->hasMetaData("ami"))
    {
	H << sp << nl << "class " << _dllExport << classNameAMI << '_' << name
	  << " : public ::IceInternal::OutgoingAsync";
	H << sb;
	H.dec();
	H << nl << "public:";
	H.inc();
	H << sp;
	H << nl << "virtual void ice_response" << spar << params << epar << " = 0;";
	H << nl << "virtual void ice_exception(const ::Ice::Exception&) = 0;";
	H << sp;
	H << nl << "void __invoke" << spar << paramsInvoke << epar << ';';
	H << sp;
	H.dec();
	H << nl << "protected:";
	H.inc();
	H << sp;
	H << nl << "virtual void __response(bool);";
	H << eb << ';';
	H << sp << nl << "typedef ::IceUtil::Handle< " << classScopedAMI << '_' << name << "> " << classNameAMI
	  << '_' << name  << "Ptr;";
	
	C << sp << nl << "void" << nl << classScopedAMI.substr(2) << '_' << name << "::__invoke" << spar
	  << paramsDeclInvoke << epar;
	C << sb;
	C << nl << "try";
	C << sb;
	C << nl << "static const ::std::string __operation(\"" << name << "\");";
	C << nl << "__prepare(__prx, __operation, static_cast< ::Ice::OperationMode>(" << p->mode() << "), __ctx);";
	writeMarshalCode(C, inParams, 0);
	if(p->sendsClasses())
	{
	    C << nl << "__os->writePendingObjects();";
	}
	C << nl << "__os->endWriteEncaps();";
	C << eb;
	C << nl << "catch(const ::Ice::LocalException& __ex)";
	C << sb;
	C << nl << "__finished(__ex);";
	C << nl << "return;";
	C << eb;
	C << nl << "__send();";
	C << eb;

	C << sp << nl << "void" << nl << classScopedAMI.substr(2) << '_' << name << "::__response(bool __ok)";
	C << sb;
	writeAllocateCode(C, outParams, ret);
	C << nl << "try";
	C << sb;
	C << nl << "if(!__ok)";
	C << sb;
	C << nl << "__is->throwException();";
	C << eb;
	writeUnmarshalCode(C, outParams, ret);
	if(p->returnsClasses())
	{
	    C << nl << "__is->readPendingObjects();";
	}
	C << eb;
	C << nl << "catch(const ::Ice::LocalException& __ex)";
	C << sb;
	C << nl << "__finished(__ex);";
	C << nl << "return;";
	C << eb;

	//
	// Generate a catch block for each legal user exception.
	// (See comment in DelegateMVisitor::visitOperation() for details.)
	//
	ExceptionList throws = p->throws();
	throws.sort();
	throws.unique();
#if defined(__SUNPRO_CC)
	throws.sort(derivedToBaseCompare);
#else
	throws.sort(Slice::DerivedToBaseCompare());
#endif
	for(ExceptionList::const_iterator i = throws.begin(); i != throws.end(); ++i)
	{
	    string scoped = (*i)->scoped();
	    C << nl << "catch(const " << fixKwd((*i)->scoped()) << "& __ex)";
	    C << sb;
	    C << nl << "ice_exception(__ex);";
	    C << nl << "return;";
	    C << eb;
	}
	C << nl << "catch(const ::Ice::UserException&)";
	C << sb;
	C << nl << "ice_exception(::Ice::UnknownUserException(__FILE__, __LINE__));";
	C << nl << "return;";
	C << eb;
	C << nl << "ice_response" << spar << args << epar << ';';
	C << eb;
    }

    if(cl->hasMetaData("amd") || p->hasMetaData("amd"))
    {
	H << sp << nl << "class " << _dllExport << classNameAMD << '_' << name
	  << " : virtual public ::IceUtil::Shared";
	H << sb;
	H.dec();
	H << nl << "public:";
	H.inc();
	H << sp;
	H << nl << "virtual void ice_response" << spar << params << epar << " = 0;";
	H << nl << "virtual void ice_exception(const ::Ice::Exception&) = 0;";
	H << nl << "virtual void ice_exception(const ::std::exception&) = 0;";
	H << nl << "virtual void ice_exception() = 0;";
	H << eb << ';';
	H << sp << nl << "typedef ::IceUtil::Handle< " << classScopedAMD << '_' << name << "> "
	  << classNameAMD << '_' << name  << "Ptr;";
    }
}

Slice::Gen::AsyncImplVisitor::AsyncImplVisitor(Output& h, Output& c, const string& dllExport) :
    H(h), C(c), _dllExport(dllExport)
{
}

bool
Slice::Gen::AsyncImplVisitor::visitUnitStart(const UnitPtr& p)
{
    if(!p->hasNonLocalClassDecls() || !p->hasContentsWithMetaData("amd"))
    {
	return false;
    }

    H << sp << nl << "namespace IceAsync" << nl << '{';

    return true;
}

void
Slice::Gen::AsyncImplVisitor::visitUnitEnd(const UnitPtr& p)
{
    H << sp << nl << '}';
}
    
bool
Slice::Gen::AsyncImplVisitor::visitModuleStart(const ModulePtr& p)
{
    if(!p->hasNonLocalClassDecls() || !p->hasContentsWithMetaData("amd"))
    {
	return false;
    }

    string name = fixKwd(p->name());
    
    H << sp << nl << "namespace " << name << nl << '{';

    return true;
}

void
Slice::Gen::AsyncImplVisitor::visitModuleEnd(const ModulePtr& p)
{
    H << sp << nl << '}';
}

void
Slice::Gen::AsyncImplVisitor::visitOperation(const OperationPtr& p)
{
    ContainerPtr container = p->container();
    ClassDefPtr cl = ClassDefPtr::dynamicCast(container);
    
    if(cl->isLocal() || (!cl->hasMetaData("amd") && !p->hasMetaData("amd")))
    {
	return;
    }

    string name = p->name();
    
    string classNameAMD = "AMD_" + cl->name();
    string classScope = fixKwd(cl->scope());
    string classScopedAMD = classScope + classNameAMD;
    
    string params;
    string paramsDecl;
    string args;
    
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
    throws.sort(derivedToBaseCompare);
#else
    throws.sort(Slice::DerivedToBaseCompare());
#endif

    TypePtr ret = p->returnType();
    string retS = inputTypeToString(ret);
    
    if(ret)
    {
	params += retS;
	paramsDecl += retS;
	paramsDecl += ' ';
	paramsDecl += "__ret";
	args += "__ret";
    }
    
    TypeStringList outParams;
    ParamDeclList paramList = p->parameters();
    for(ParamDeclList::const_iterator q = paramList.begin(); q != paramList.end(); ++q)
    {
	if((*q)->isOutParam())
	{
	    string paramName = fixKwd((*q)->name());
	    TypePtr type = (*q)->type();
	    string typeString = inputTypeToString(type);
	    
	    if(ret || !outParams.empty())
	    {
		params += ", ";
		paramsDecl += ", ";
		args += ", ";
	    }
	    
	    params += typeString;
	    paramsDecl += typeString;
	    paramsDecl += ' ';
	    paramsDecl += paramName;
	    args += paramName;
	    
	    outParams.push_back(make_pair(type, paramName));
	}
    }

    H << sp << nl << "class " << _dllExport << classNameAMD << '_' << name
      << " : public " << classScopedAMD  << '_' << name << ", public ::IceInternal::IncomingAsync";
    H << sb;
    H.dec();
    H << nl << "public:";
    H.inc();
    
    H << sp;
    H << nl << classNameAMD << '_' << name << "(::IceInternal::Incoming&);";
    
    H << sp;
    H << nl << "virtual void ice_response(" << params << ");";
    H << nl << "virtual void ice_exception(const ::Ice::Exception&);";
    H << nl << "virtual void ice_exception(const ::std::exception&);";
    H << nl << "virtual void ice_exception();";
    H << eb << ';';
    
    C << sp << nl << "IceAsync" << classScopedAMD << '_' << name << "::" << classNameAMD << '_' << name
      << "(::IceInternal::Incoming& in) :";
    C.inc();
    C << nl << "IncomingAsync(in)";
    C.dec();
    C << sb;
    C << eb;
    
    C << sp << nl << "void" << nl << "IceAsync" << classScopedAMD << '_' << name << "::ice_response("
      << paramsDecl << ')';
    C << sb;
    if(ret || !outParams.empty())
    {
	C << nl << "try";
	C << sb;
	C << nl << "::IceInternal::BasicStream* __os = this->__os();";
	writeMarshalCode(C, outParams, ret);
	if(p->returnsClasses())
	{
	    C << nl << "__os->writePendingObjects();";
	}
	C << eb;
	C << nl << "catch(const ::Ice::Exception& __ex)";
	C << sb;
	C << nl << "__exception(__ex);";
	C << nl << "return;";
	C << eb;
    }
    C << nl << "__response(true);";
    C << eb;
    
    C << sp << nl << "void" << nl << "IceAsync" << classScopedAMD << '_' << name
      << "::ice_exception(const ::Ice::Exception& ex)";
    C << sb;
    if(throws.empty())
    {
	C << nl << "__exception(ex);";
    }
    else
    {
	C << nl << "try";
	C << sb;
	C << nl << "ex.ice_throw();";
	C << eb;
	ExceptionList::const_iterator r;
	for(r = throws.begin(); r != throws.end(); ++r)
	{
	    C << nl << "catch(const " << fixKwd((*r)->scoped()) << "& __ex)";
	    C << sb;
	    C << nl << "__os()->write(__ex);";
	    if((*r)->usesClasses())
	    {
		C << nl << "__os()->writePendingObjects();";
	    }
	    C << nl << "__response(false);";
	    C << eb;
	}
	C << nl << "catch(const ::Ice::Exception& __ex)";
	C << sb;
	C << nl << "__exception(__ex);";
	C << eb;
    }
    C << eb;
    
    C << sp << nl << "void" << nl << "IceAsync" << classScopedAMD << '_' << name
      << "::ice_exception(const ::std::exception& ex)";
    C << sb;
    C << nl << "__exception(ex);";
    C << eb;
    
    C << sp << nl << "void" << nl << "IceAsync" << classScopedAMD << '_' << name
      << "::ice_exception()";
    C << sb;
    C << nl << "__exception();";
    C << eb;
}

void
Slice::Gen::validateMetaData(const UnitPtr& unit)
{
    MetaDataVisitor visitor;
    unit->visit(&visitor, false);
}

bool
Slice::Gen::MetaDataVisitor::visitModuleStart(const ModulePtr& p)
{
    validate(p);
    return true;
}

void
Slice::Gen::MetaDataVisitor::visitModuleEnd(const ModulePtr&)
{
}

void
Slice::Gen::MetaDataVisitor::visitClassDecl(const ClassDeclPtr& p)
{
    validate(p);
}

bool
Slice::Gen::MetaDataVisitor::visitClassDefStart(const ClassDefPtr& p)
{
    validate(p);
    return true;
}

void
Slice::Gen::MetaDataVisitor::visitClassDefEnd(const ClassDefPtr&)
{
}

bool
Slice::Gen::MetaDataVisitor::visitExceptionStart(const ExceptionPtr& p)
{
    validate(p);
    return true;
}

void
Slice::Gen::MetaDataVisitor::visitExceptionEnd(const ExceptionPtr&)
{
}

bool
Slice::Gen::MetaDataVisitor::visitStructStart(const StructPtr& p)
{
    validate(p);
    return true;
}

void
Slice::Gen::MetaDataVisitor::visitStructEnd(const StructPtr&)
{
}

void
Slice::Gen::MetaDataVisitor::visitOperation(const OperationPtr& p)
{
    validate(p);
}

void
Slice::Gen::MetaDataVisitor::visitParamDecl(const ParamDeclPtr& p)
{
    validate(p);
}

void
Slice::Gen::MetaDataVisitor::visitDataMember(const DataMemberPtr& p)
{
    validate(p);
}

void
Slice::Gen::MetaDataVisitor::visitSequence(const SequencePtr& p)
{
    validate(p);
}

void
Slice::Gen::MetaDataVisitor::visitDictionary(const DictionaryPtr& p)
{
    validate(p);
}

void
Slice::Gen::MetaDataVisitor::visitEnum(const EnumPtr& p)
{
    validate(p);
}

void
Slice::Gen::MetaDataVisitor::visitConst(const ConstPtr& p)
{
    validate(p);
}

void
Slice::Gen::MetaDataVisitor::validate(const ContainedPtr& cont)
{
    DefinitionContextPtr dc = cont->definitionContext();
    assert(dc);
    StringList globalMetaData = dc->getMetaData();
    string file = dc->filename();

    StringList localMetaData = cont->getMetaData();

    StringList::const_iterator p;
    static const string prefix = "cpp:";

    for(p = globalMetaData.begin(); p != globalMetaData.end(); ++p)
    {
        string s = *p;
        if(_history.count(s) == 0)
        {
            if(s.find(prefix) == 0)
            {
		cout << file << ": warning: ignoring invalid global metadata `" << s << "'" << endl;
            }
            _history.insert(s);
        }
    }

    for(p = localMetaData.begin(); p != localMetaData.end(); ++p)
    {
	string s = *p;
        if(_history.count(s) == 0)
        {
            if(s.find(prefix) == 0)
            {
	    	if(SequencePtr::dynamicCast(cont))
		{
		    if(s.substr(prefix.size()) == "collection")
		    {
			continue;
		    }
		}
		if(StructPtr::dynamicCast(cont))
		{
		    if(s.substr(prefix.size()) == "class")
		    {
		        continue;
		    }
		}
		cout << file << ": warning: ignoring invalid metadata `" << s << "'" << endl;
            }
            _history.insert(s);
        }
    }
}
