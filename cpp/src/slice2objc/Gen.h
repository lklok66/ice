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

    virtual void writeInheritedOperations(const ClassDefPtr&);
    virtual void writeDispatchAndMarshalling(const ClassDefPtr&, bool);
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

    void emitAttributes(const ContainedPtr&);
    ::std::string getParamAttributes(const ParamDeclPtr&);

    ::std::string writeValue(const TypePtr&);

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
        const std::string&,
        bool,
        bool,
        bool);
    ~Gen();

    bool operator!() const; // Returns true if there was a constructor error

    void generate(const UnitPtr&);
    void generateTie(const UnitPtr&);
    void generateImpl(const UnitPtr&);
    void generateImplTie(const UnitPtr&);
    void generateChecksums(const UnitPtr&);
    void closeOutput();

private:

    IceUtilInternal::Output _H;
    IceUtilInternal::Output _M;
    IceUtilInternal::Output _implH;
    IceUtilInternal::Output _implM;

    std::string _base;
    std::string _include;
    std::vector<std::string> _includePaths;
    bool _impl;
    bool _stream;

    void printHeader(::IceUtilInternal::Output&);

    class UnitVisitor : public ObjCVisitor
    {
    public:

        UnitVisitor(::IceUtilInternal::Output&, ::IceUtilInternal::Output&, bool);

        virtual bool visitModuleStart(const ModulePtr&);

    private:

        bool _stream;
        bool _globalMetaDataDone;
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

        TypesVisitor(::IceUtilInternal::Output&, ::IceUtilInternal::Output&, bool);

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
        virtual void visitDataMember(const DataMemberPtr&);

    private:

        enum Escape { NoEscape, WithEscape };
	enum ContainerType { LocalException, Other };
	enum Destination { MOnly , HAndM};

	void writeMembers(const DataMemberList&, int) const;
	void writeMemberSignature(const DataMemberList&, int, ContainerType, Destination) const;
	void writeMemberCall(const DataMemberList&, int, ContainerType, Escape) const;
	void writeMemberInit(const DataMemberList&, int) const;
	void writeProperties(const DataMemberList&, int) const;
	void writeSynthesize(const DataMemberList&, int) const;
        void writeMemberCopy(const SyntaxTreeBasePtr&, const DataMemberList&, int) const;
        void writeMemberHashCode(const DataMemberList&, int) const;
        void writeMemberEquals(const DataMemberList&, int) const;
        void writeMemberDealloc(const DataMemberList&, int) const;
        void writeMemberMarshal(const ::std::string&, const DataMemberList&, int) const;
        void writeMemberUnmarshal(const ::std::string&, const DataMemberList&, int) const;

        bool _stream;
    };

    class ProxyVisitor : public ObjCVisitor
    {
    public:

        ProxyVisitor(::IceUtilInternal::Output&, ::IceUtilInternal::Output&);

        virtual bool visitClassDefStart(const ClassDefPtr&);
        virtual void visitClassDefEnd(const ClassDefPtr&);
        virtual void visitOperation(const OperationPtr&);
    };

    class OpsVisitor : public ObjCVisitor
    {
    public:

        OpsVisitor(::IceUtilInternal::Output&);

        virtual bool visitModuleStart(const ModulePtr&);
        virtual void visitModuleEnd(const ModulePtr&);
        virtual bool visitClassDefStart(const ClassDefPtr&);

    private:
        void writeOperations(const ClassDefPtr&, bool);
    };

    class ProxyHelperVisitor : public ObjCVisitor
    {
    public:

        ProxyHelperVisitor(::IceUtilInternal::Output&, ::IceUtilInternal::Output&, bool);

        virtual bool visitClassDefStart(const ClassDefPtr&);
        virtual void visitClassDefEnd(const ClassDefPtr&);
        virtual void visitDictionary(const DictionaryPtr&);

    private:

        bool _stream;
    };

    class HelperVisitor : public ObjCVisitor
    {
    public:

        HelperVisitor(::IceUtilInternal::Output&, ::IceUtilInternal::Output&, bool);

	virtual void visitEnum(const EnumPtr&);
        virtual void visitSequence(const SequencePtr&);
        virtual void visitDictionary(const DictionaryPtr&);

    private:

        bool _stream;
    };

    class DelegateVisitor : public ObjCVisitor
    {
    public:

        DelegateVisitor(::IceUtilInternal::Output&);

        virtual bool visitModuleStart(const ModulePtr&);
        virtual void visitModuleEnd(const ModulePtr&);
        virtual bool visitClassDefStart(const ClassDefPtr&);
        virtual void visitClassDefEnd(const ClassDefPtr&);
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

    class DelegateDVisitor : public ObjCVisitor
    {
    public:

        DelegateDVisitor(::IceUtilInternal::Output&);

        virtual bool visitModuleStart(const ModulePtr&);
        virtual void visitModuleEnd(const ModulePtr&);
        virtual bool visitClassDefStart(const ClassDefPtr&);
        virtual void visitClassDefEnd(const ClassDefPtr&);
    };

    class DispatcherVisitor : public ObjCVisitor
    {
    public:

        DispatcherVisitor(::IceUtilInternal::Output&, bool);

        virtual bool visitModuleStart(const ModulePtr&);
        virtual void visitModuleEnd(const ModulePtr&);
        virtual bool visitClassDefStart(const ClassDefPtr&);

    private:

        bool _stream;
    };

    class AsyncVisitor : public ObjCVisitor
    {
    public:

        AsyncVisitor(::IceUtilInternal::Output&);

        virtual bool visitModuleStart(const ModulePtr&);
        virtual void visitModuleEnd(const ModulePtr&);
        virtual bool visitClassDefStart(const ClassDefPtr&);
        virtual void visitClassDefEnd(const ClassDefPtr&);
        virtual void visitOperation(const OperationPtr&);
    };

    class TieVisitor : public ObjCVisitor
    {
    public:

        TieVisitor(::IceUtilInternal::Output&);

        virtual bool visitModuleStart(const ModulePtr&);
        virtual void visitModuleEnd(const ModulePtr&);
        virtual bool visitClassDefStart(const ClassDefPtr&);
        virtual void visitClassDefEnd(const ClassDefPtr&);

    private:

        typedef ::std::set< ::std::string> NameSet;
        void writeInheritedOperationsWithOpNames(const ClassDefPtr&, NameSet&);
    };

    class BaseImplVisitor : public ObjCVisitor
    {
    public:

        BaseImplVisitor(::IceUtilInternal::Output&);

    protected:

        void writeOperation(const OperationPtr&, bool, bool);
    };

    class ImplVisitor : public BaseImplVisitor
    {
    public:

        ImplVisitor(::IceUtilInternal::Output&);

        virtual bool visitModuleStart(const ModulePtr&);
        virtual void visitModuleEnd(const ModulePtr&);
        virtual bool visitClassDefStart(const ClassDefPtr&);
        virtual void visitClassDefEnd(const ClassDefPtr&);
    };

    class ImplTieVisitor : public BaseImplVisitor
    {
    public:

        ImplTieVisitor(::IceUtilInternal::Output&);

        virtual bool visitModuleStart(const ModulePtr&);
        virtual void visitModuleEnd(const ModulePtr&);
        virtual bool visitClassDefStart(const ClassDefPtr&);
    };
};

}

#endif
