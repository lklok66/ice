// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef GEN_H
#define GEN_H

#include <JsUtil.h>

namespace Slice
{

class JsVisitor : public JsGenerator, public ParserVisitor
{
public:

    JsVisitor(::IceUtilInternal::Output&);
    virtual ~JsVisitor();

protected:

    void writeMarshalUnmarshalParams(const ParamDeclList&, const OperationPtr&, bool);
    void writePostUnmarshalParams(const ParamDeclList&, const OperationPtr&);
    void writeMarshalDataMember(const DataMemberPtr&);
    void writeUnmarshalDataMember(const DataMemberPtr&);

    virtual void writeDispatchAndMarshalling(const ClassDefPtr&);
    virtual std::vector<std::string> getParams(const OperationPtr&);
    virtual std::vector<std::string> getParamsAsync(const OperationPtr&, bool, bool = false);
    virtual std::vector<std::string> getParamsAsyncCB(const OperationPtr&, bool = false, bool = true);
    virtual std::vector<std::string> getArgs(const OperationPtr&);
    virtual std::vector<std::string> getArgsAsync(const OperationPtr&, bool = false);
    virtual std::vector<std::string> getArgsAsyncCB(const OperationPtr&, bool = false, bool = false);

    std::string getValue(const std::string&, const TypePtr&);

    void writeConstantValue(const std::string&, const TypePtr&, const SyntaxTreeBasePtr&, const std::string&);

    std::string toJsIdent(const std::string&);
    std::string editMarkup(const std::string&);
    StringList splitIntoLines(const std::string&);
    void splitComment(const ContainedPtr&, StringList&, StringList&);
    StringList getSummary(const ContainedPtr&);
    void writeDocComment(const ContainedPtr&, const std::string&, const std::string& = "");
    void writeDocCommentOp(const OperationPtr&);

    enum ParamDir { InParam, OutParam };
    void writeDocCommentAsync(const OperationPtr&, ParamDir, const std::string& = "", bool = false);
    void writeDocCommentParam(const OperationPtr&, ParamDir, bool = false);

    ::IceUtilInternal::Output& _out;
};

class Gen : public JsGenerator
{
public:

    Gen(const std::string&,
        const std::vector<std::string>&,
        const std::string&);
    ~Gen();

    void generate(const UnitPtr&);
    void generateChecksums(const UnitPtr&);
    void closeOutput();

private:

    IceUtilInternal::Output _out;

    std::vector<std::string> _includePaths;

    void printHeader();

    class RequireVisitor : public JsVisitor
    {
    public:

        RequireVisitor(::IceUtilInternal::Output&);

        virtual bool visitClassDefStart(const ClassDefPtr&);
        virtual bool visitExceptionStart(const ExceptionPtr&);
        virtual void visitEnum(const EnumPtr&);

        void writeRequires();

    private:

        bool _seenClass;
        bool _seenUserException;
        bool _seenLocalException;
        bool _seenEnum;
    };

    class CompactIdVisitor : public JsVisitor
    {
    public:

        CompactIdVisitor(IceUtilInternal::Output&);

        virtual bool visitUnitStart(const UnitPtr&);
        virtual void visitUnitEnd(const UnitPtr&);
        virtual bool visitClassDefStart(const ClassDefPtr&);
    };

    class TypesVisitor : public JsVisitor
    {
    public:

        TypesVisitor(::IceUtilInternal::Output&);

        virtual bool visitModuleStart(const ModulePtr&);
        virtual void visitModuleEnd(const ModulePtr&);
        virtual bool visitClassDefStart(const ClassDefPtr&);
        virtual void visitOperation(const OperationPtr&);
        virtual bool visitExceptionStart(const ExceptionPtr&);
        virtual bool visitStructStart(const StructPtr&);
        virtual void visitSequence(const SequencePtr&);
        virtual void visitDictionary(const DictionaryPtr&);
        virtual void visitEnum(const EnumPtr&);
        virtual void visitConst(const ConstPtr&);

    private:

        void writeMemberHashCode(const std::string&, const TypePtr&, int);
        void writeMemberEquals(const std::string&, const std::string&, const TypePtr&, int);
        void writeMemberClone(const std::string&, const std::string&, const TypePtr&, int);
    };

    class ExportVisitor : public JsVisitor
    {
    public:

        ExportVisitor(::IceUtilInternal::Output&);

        virtual bool visitModuleStart(const ModulePtr&);

        void writeExports();

    private:

        StringList _modules;
    };

#if 0
    class AsyncDelegateVisitor : public JsVisitor
    {
    public:

        AsyncDelegateVisitor(::IceUtilInternal::Output&);

        virtual bool visitModuleStart(const ModulePtr&);
        virtual void visitModuleEnd(const ModulePtr&);
        virtual bool visitClassDefStart(const ClassDefPtr&);
        virtual void visitClassDefEnd(const ClassDefPtr&);
        virtual void visitOperation(const OperationPtr&);
    };

    class ProxyVisitor : public JsVisitor
    {
    public:

        ProxyVisitor(::IceUtilInternal::Output&);

        virtual bool visitModuleStart(const ModulePtr&);
        virtual void visitModuleEnd(const ModulePtr&);
        virtual bool visitClassDefStart(const ClassDefPtr&);
        virtual void visitClassDefEnd(const ClassDefPtr&);
        virtual void visitOperation(const OperationPtr&);
    };

    class OpsVisitor : public JsVisitor
    {
    public:

        OpsVisitor(::IceUtilInternal::Output&);

        virtual bool visitModuleStart(const ModulePtr&);
        virtual void visitModuleEnd(const ModulePtr&);
        virtual bool visitClassDefStart(const ClassDefPtr&);

    private:
        void writeOperations(const ClassDefPtr&, bool);
    };

    class HelperVisitor : public JsVisitor
    {
    public:

        HelperVisitor(::IceUtilInternal::Output&);

        virtual bool visitModuleStart(const ModulePtr&);
        virtual void visitModuleEnd(const ModulePtr&);
        virtual bool visitClassDefStart(const ClassDefPtr&);
        virtual void visitClassDefEnd(const ClassDefPtr&);
        virtual void visitSequence(const SequencePtr&);
        virtual void visitDictionary(const DictionaryPtr&);
    };

    class DelegateVisitor : public JsVisitor
    {
    public:

        DelegateVisitor(::IceUtilInternal::Output&);

        virtual bool visitModuleStart(const ModulePtr&);
        virtual void visitModuleEnd(const ModulePtr&);
        virtual bool visitClassDefStart(const ClassDefPtr&);
        virtual void visitClassDefEnd(const ClassDefPtr&);
    };

    class DelegateMVisitor : public JsVisitor
    {
    public:

        DelegateMVisitor(::IceUtilInternal::Output&);

        virtual bool visitModuleStart(const ModulePtr&);
        virtual void visitModuleEnd(const ModulePtr&);
        virtual bool visitClassDefStart(const ClassDefPtr&);
        virtual void visitClassDefEnd(const ClassDefPtr&);
    };

    class DelegateDVisitor : public JsVisitor
    {
    public:

        DelegateDVisitor(::IceUtilInternal::Output&);

        virtual bool visitModuleStart(const ModulePtr&);
        virtual void visitModuleEnd(const ModulePtr&);
        virtual bool visitClassDefStart(const ClassDefPtr&);
        virtual void visitClassDefEnd(const ClassDefPtr&);
    };

    class DispatcherVisitor : public JsVisitor
    {
    public:

        DispatcherVisitor(::IceUtilInternal::Output&);

        virtual bool visitModuleStart(const ModulePtr&);
        virtual void visitModuleEnd(const ModulePtr&);
        virtual bool visitClassDefStart(const ClassDefPtr&);
    };

    class AsyncVisitor : public JsVisitor
    {
    public:

        AsyncVisitor(::IceUtilInternal::Output&);

        virtual bool visitModuleStart(const ModulePtr&);
        virtual void visitModuleEnd(const ModulePtr&);
        virtual bool visitClassDefStart(const ClassDefPtr&);
        virtual void visitClassDefEnd(const ClassDefPtr&);
        virtual void visitOperation(const OperationPtr&);
    };
#endif
};

}

#endif
