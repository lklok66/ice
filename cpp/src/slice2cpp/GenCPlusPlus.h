// **********************************************************************
//
// Copyright (c) 2001
// MutableRealms, Inc.
// Huntsville, AL, USA
//
// All Rights Reserved
//
// **********************************************************************

#ifndef GEN_C_PLUS_PLUS_H
#define GEN_C_PLUS_PLUS_H

#include <Parser.h>
#include <OutputUtil.h>

namespace IceLang
{

class GenCPlusPlus : ::__Ice::noncopyable
{
public:

    GenCPlusPlus(const std::string&,
		 const std::string&,
		 const std::string&,
		 const std::vector<std::string>&,
		 const std::string&);
    ~GenCPlusPlus();

    bool operator!() const; // Returns true if there was a constructor error

    void generate(const Parser_ptr&);

private:

    std::string changeInclude(const std::string&);
    void printHeader(Output&);

    Output FH;
    Output H;
    Output C;

    std::string base_;
    std::string include_;
    std::vector<std::string> includePaths_;
    std::string dllExport_;

    class TypesVisitor : ::__Ice::noncopyable, public ParserVisitor
    {
    public:

	TypesVisitor(Output&, Output&, const std::string&);

	virtual void visitModuleStart(const Module_ptr&);
	virtual void visitModuleEnd(const Module_ptr&);
	virtual void visitVector(const Vector_ptr&);

    private:

	Output& H;
	Output& C;

	std::string dllExport_;
    };

    class ProxyDeclVisitor : ::__Ice::noncopyable, public ParserVisitor
    {
    public:

	ProxyDeclVisitor(Output&, Output&, const std::string&);

	virtual void visitUnitStart(const Parser_ptr&);
	virtual void visitUnitEnd(const Parser_ptr&);
	virtual void visitModuleStart(const Module_ptr&);
	virtual void visitModuleEnd(const Module_ptr&);
	virtual void visitClassDecl(const ClassDecl_ptr&);

    private:

	Output& H;
	Output& C;

	std::string dllExport_;
    };

    class ProxyVisitor : ::__Ice::noncopyable, public ParserVisitor
    {
    public:

	ProxyVisitor(Output&, Output&, const std::string&);

	virtual void visitUnitStart(const Parser_ptr&);
	virtual void visitUnitEnd(const Parser_ptr&);
	virtual void visitModuleStart(const Module_ptr&);
	virtual void visitModuleEnd(const Module_ptr&);
	virtual void visitClassDefStart(const ClassDef_ptr&);
	virtual void visitClassDefEnd(const ClassDef_ptr&);
	virtual void visitOperation(const Operation_ptr&);

    private:

	Output& H;
	Output& C;

	std::string dllExport_;
    };

    class DelegateVisitor : ::__Ice::noncopyable, public ParserVisitor
    {
    public:

	DelegateVisitor(Output&, Output&, const std::string&);

	virtual void visitUnitStart(const Parser_ptr&);
	virtual void visitUnitEnd(const Parser_ptr&);
	virtual void visitModuleStart(const Module_ptr&);
	virtual void visitModuleEnd(const Module_ptr&);
	virtual void visitClassDefStart(const ClassDef_ptr&);
	virtual void visitClassDefEnd(const ClassDef_ptr&);
	virtual void visitOperation(const Operation_ptr&);

    private:

	Output& H;
	Output& C;

	std::string dllExport_;
    };

    class DelegateMVisitor : ::__Ice::noncopyable, public ParserVisitor
    {
    public:

	DelegateMVisitor(Output&, Output&, const std::string&);

	virtual void visitUnitStart(const Parser_ptr&);
	virtual void visitUnitEnd(const Parser_ptr&);
	virtual void visitModuleStart(const Module_ptr&);
	virtual void visitModuleEnd(const Module_ptr&);
	virtual void visitClassDefStart(const ClassDef_ptr&);
	virtual void visitClassDefEnd(const ClassDef_ptr&);
	virtual void visitOperation(const Operation_ptr&);

    private:

	Output& H;
	Output& C;

	std::string dllExport_;
    };

    class ObjectDeclVisitor : ::__Ice::noncopyable, public ParserVisitor
    {
    public:

	ObjectDeclVisitor(Output&, Output&, const std::string&);

	virtual void visitModuleStart(const Module_ptr&);
	virtual void visitModuleEnd(const Module_ptr&);
	virtual void visitClassDecl(const ClassDecl_ptr&);

    private:

	Output& H;
	Output& C;

	std::string dllExport_;
    };

    class ObjectVisitor : ::__Ice::noncopyable, public ParserVisitor
    {
    public:

	ObjectVisitor(Output&, Output&, const std::string&);

	virtual void visitModuleStart(const Module_ptr&);
	virtual void visitModuleEnd(const Module_ptr&);
	virtual void visitClassDefStart(const ClassDef_ptr&);
	virtual void visitClassDefEnd(const ClassDef_ptr&);
	virtual void visitOperation(const Operation_ptr&);
	virtual void visitDataMember(const DataMember_ptr&);

    private:

	Output& H;
	Output& C;

	std::string dllExport_;
    };

    class IceVisitor : ::__Ice::noncopyable, public ParserVisitor
    {
    public:

	IceVisitor(Output&, Output&, const std::string&);

	virtual void visitUnitStart(const Parser_ptr&);
	virtual void visitUnitEnd(const Parser_ptr&);
	virtual void visitClassDecl(const ClassDecl_ptr&);
	virtual void visitClassDefStart(const ClassDef_ptr&);

    private:

	Output& H;
	Output& C;

	std::string dllExport_;
    };

    class HandleVisitor : ::__Ice::noncopyable, public ParserVisitor
    {
    public:

	HandleVisitor(Output&, Output&, const std::string&);

	virtual void visitModuleStart(const Module_ptr&);
	virtual void visitModuleEnd(const Module_ptr&);
	virtual void visitClassDecl(const ClassDecl_ptr&);
	virtual void visitClassDefStart(const ClassDef_ptr&);

    private:

	Output& H;
	Output& C;

	std::string dllExport_;
    };
};

}

#endif
