// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef GEN_H
#define GEN_H

#include <Slice/ObjCUtil.h>

namespace Slice
{


class ObjCVisitor : public ObjCGenerator, public ParserVisitor
{
public:

    ObjCVisitor(::IceUtilInternal::Output&, ::IceUtilInternal::Output&);
    virtual ~ObjCVisitor();

protected:

    virtual void writeDispatchAndMarshalling(const ClassDefPtr&);
    virtual std::string getName(const OperationPtr&) const;
    virtual std::string getSelector(const OperationPtr&) const;
    virtual std::string getParams(const OperationPtr&) const;
    virtual std::string getServerParams(const OperationPtr&) const;
    virtual std::string getParamsAsync(const OperationPtr&, bool);
    virtual std::string getParamsAsyncCB(const OperationPtr&);
    virtual std::string getArgs(const OperationPtr&) const;
    virtual std::string getServerArgs(const OperationPtr&) const;
    virtual std::string getArgsAsync(const OperationPtr&, bool);
    virtual std::string getArgsAsyncCB(const OperationPtr&);
    virtual std::string getSigAsyncCB(const OperationPtr&);

    ::IceUtilInternal::Output& _H;
    ::IceUtilInternal::Output& _M;
};

class Gen : private ::IceUtil::noncopyable
{
public:

    Gen(const std::string&,
        const std::string&,
        const std::string&,
        const std::vector<std::string>&,
        const std::string&);
    ~Gen();

    bool operator!() const; // Returns true if there was a constructor error

    void generate(const UnitPtr&);
    void closeOutput();

private:

    IceUtilInternal::Output _H;
    IceUtilInternal::Output _M;

    std::string _base;
    std::string _include;
    std::vector<std::string> _includePaths;

    void printHeader(::IceUtilInternal::Output&);

    class UnitVisitor : public ObjCVisitor
    {
    public:

        UnitVisitor(::IceUtilInternal::Output&, ::IceUtilInternal::Output&);

        virtual bool visitModuleStart(const ModulePtr&);
        virtual void visitUnitEnd(const UnitPtr&);

    private:

        bool _globalMetaDataDone;
        std::vector<Slice::ObjCGenerator::ModulePrefix> _prefixes;
    };

    class ObjectDeclVisitor : public ObjCVisitor
    {
    public:

        ObjectDeclVisitor(::IceUtilInternal::Output&, ::IceUtilInternal::Output&);

        virtual void visitClassDecl(const ClassDeclPtr&);
    };

    class ProxyDeclVisitor : public ObjCVisitor
    {
    public:

        ProxyDeclVisitor(::IceUtilInternal::Output&, ::IceUtilInternal::Output&);

        virtual void visitClassDecl(const ClassDeclPtr&);
    };

    class TypesVisitor : public ObjCVisitor
    {
    public:

        TypesVisitor(::IceUtilInternal::Output&, ::IceUtilInternal::Output&);

        virtual bool visitModuleStart(const ModulePtr&);
        virtual void visitModuleEnd(const ModulePtr&);
        virtual bool visitClassDefStart(const ClassDefPtr&);
        virtual void visitOperation(const OperationPtr&);
        virtual void visitClassDefEnd(const ClassDefPtr&);
        virtual bool visitExceptionStart(const ExceptionPtr&);
        virtual void visitExceptionEnd(const ExceptionPtr&);
        virtual bool visitStructStart(const StructPtr&);
        virtual void visitStructEnd(const StructPtr&);
        virtual void visitSequence(const SequencePtr&);
        virtual void visitDictionary(const DictionaryPtr&);
        virtual void visitEnum(const EnumPtr&);
        virtual void visitConst(const ConstPtr&);

    private:

        enum Escape { NoEscape, WithEscape };
	enum ContainerType { LocalException, Other };

	void writeMembers(const DataMemberList&, int) const;
	void writeMemberSignature(const DataMemberList&, int, ContainerType) const;
	void writeMemberCall(const DataMemberList&, int, ContainerType, Escape) const;
	void writeMemberInit(const DataMemberList&, int) const;
	void writeProperties(const DataMemberList&, int) const;
	void writeSynthesize(const DataMemberList&, int) const;
        void writeMemberHashCode(const DataMemberList&, int) const;
        void writeMemberEquals(const DataMemberList&, int) const;
        void writeMemberDealloc(const DataMemberList&, int) const;
        void writeMemberMarshal(const ::std::string&, const DataMemberList&, int) const;
        void writeMemberUnmarshal(const ::std::string&, const DataMemberList&, int) const;
    };

    class ProxyVisitor : public ObjCVisitor
    {
    public:

        ProxyVisitor(::IceUtilInternal::Output&, ::IceUtilInternal::Output&);

        virtual bool visitClassDefStart(const ClassDefPtr&);
        virtual void visitClassDefEnd(const ClassDefPtr&);
        virtual void visitOperation(const OperationPtr&);
    };

    class HelperVisitor : public ObjCVisitor
    {
    public:

        HelperVisitor(::IceUtilInternal::Output&, ::IceUtilInternal::Output&);

	virtual void visitEnum(const EnumPtr&);
        virtual void visitSequence(const SequencePtr&);
        virtual void visitDictionary(const DictionaryPtr&);
    };

    class DelegateMVisitor : public ObjCVisitor
    {
    public:

        DelegateMVisitor(::IceUtilInternal::Output&, ::IceUtilInternal::Output&);

        virtual bool visitModuleStart(const ModulePtr&);
        virtual void visitModuleEnd(const ModulePtr&);
        virtual bool visitClassDefStart(const ClassDefPtr&);
        virtual void visitClassDefEnd(const ClassDefPtr&);
        virtual void visitOperation(const OperationPtr&);
    };
};

}

#endif
