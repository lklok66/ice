// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef JS_UTIL_H
#define JS_UTIL_H

#include <Slice/Parser.h>
#include <IceUtil/OutputUtil.h>

namespace Slice
{

class SLICE_API JsGenerator : private ::IceUtil::noncopyable
{
public:

    virtual ~JsGenerator() {};

    //
    // Validate all metadata in the unit with a "js:" prefix.
    //
    static void validateMetaData(const UnitPtr&);

protected:

    static bool isClassType(const TypePtr&);
    static std::string localProxyHelper(const TypePtr&);
    static std::string fixId(const std::string&, bool = false);
    static std::string fixId(const ContainedPtr&, bool = false);
    static std::string getOptionalFormat(const TypePtr&);
    static std::string getStaticId(const TypePtr&);
    static std::string typeToString(const TypePtr&, bool = false);
    static std::string getLocalScope(const std::string&);
    static std::string getReference(const std::string&, const std::string&);

    static std::string getHelper(const TypePtr&);
    //
    // Generate code to marshal or unmarshal a type
    //
    void writeMarshalUnmarshalCode(::IceUtilInternal::Output&, const TypePtr&, const std::string&, bool);
    void writeOptionalMarshalUnmarshalCode(::IceUtilInternal::Output&, const TypePtr&, const std::string&, int, bool);
    void writeSequenceMarshalUnmarshalCode(::IceUtilInternal::Output&, const SequencePtr&, const std::string&, bool);
    void writeDictionaryMarshalUnmarshalCode(::IceUtilInternal::Output&, const DictionaryPtr&, const std::string&, bool);
    void writeOptionalSequenceMarshalUnmarshalCode(::IceUtilInternal::Output&, const SequencePtr&, const std::string&,
                                                   int, bool);
    void writeOptionalDictionaryMarshalUnmarshalCode(::IceUtilInternal::Output&, const DictionaryPtr&, const std::string&, 
                                                     int, bool);

private:

    class MetaDataVisitor : public ParserVisitor
    {
    public:

        virtual bool visitUnitStart(const UnitPtr&);
        virtual bool visitModuleStart(const ModulePtr&);
        virtual void visitModuleEnd(const ModulePtr&);
        virtual void visitClassDecl(const ClassDeclPtr&);
        virtual bool visitClassDefStart(const ClassDefPtr&);
        virtual void visitClassDefEnd(const ClassDefPtr&);
        virtual bool visitExceptionStart(const ExceptionPtr&);
        virtual void visitExceptionEnd(const ExceptionPtr&);
        virtual bool visitStructStart(const StructPtr&);
        virtual void visitStructEnd(const StructPtr&);
        virtual void visitOperation(const OperationPtr&);
        virtual void visitParamDecl(const ParamDeclPtr&);
        virtual void visitDataMember(const DataMemberPtr&);
        virtual void visitSequence(const SequencePtr&);
        virtual void visitDictionary(const DictionaryPtr&);
        virtual void visitEnum(const EnumPtr&);
        virtual void visitConst(const ConstPtr&);

    private:

        void validate(const ContainedPtr&);

        std::string _fileName;
        StringSet _history;
    };
    
    std::vector< std::string> _seenProxy;
};

}

#endif
