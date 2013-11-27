// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <IceUtil/DisableWarnings.h>
#include <IceUtil/Functional.h>
#include <IceUtil/StringUtil.h>
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
#include <Slice/FileTracker.h>
#include <Slice/Util.h>
#include <string.h>

using namespace std;
using namespace Slice;
using namespace IceUtil;
using namespace IceUtilInternal;

namespace
{

string
sliceModeToIceMode(Operation::Mode opMode)
{
    string mode;
    switch(opMode)
    {
        case Operation::Normal:
        {
            mode = "Ice.OperationMode.Normal";
            break;
        }
        case Operation::Nonmutating:
        {
            mode = "Ice.OperationMode.Nonmutating";
            break;
        }
        case Operation::Idempotent:
        {
            mode = "Ice.OperationMode.Idempotent";
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

string
opFormatTypeToString(const OperationPtr& op)
{
    switch(op->format())
    {
    case DefaultFormat:
        return "Ice.FormatType.DefaultFormat";
    case CompactFormat:
        return "Ice.FormatType.CompactFormat";
    case SlicedFormat:
        return "Ice.FormatType.SlicedFormat";
    default:
        assert(false);
    }

    return "???";
}

bool
isClassType(const TypePtr type)
{
    BuiltinPtr builtin = BuiltinPtr::dynamicCast(type);
    return (builtin && builtin->kind() == Builtin::KindObject) || ClassDeclPtr::dynamicCast(type);
}

string
getDeprecateReason(const ContainedPtr& p1, const ContainedPtr& p2, const string& type)
{
    string deprecateMetadata, deprecateReason;
    if(p1->findMetaData("deprecate", deprecateMetadata) ||
       (p2 != 0 && p2->findMetaData("deprecate", deprecateMetadata)))
    {
        deprecateReason = "This " + type + " has been deprecated.";
        const string prefix = "deprecate:";
        if(deprecateMetadata.find(prefix) == 0 && deprecateMetadata.size() > prefix.size())
        {
            deprecateReason = deprecateMetadata.substr(prefix.size());
        }
    }
    return deprecateReason;
}

}

Slice::JsVisitor::JsVisitor(Output& out) : _out(out)
{
}

Slice::JsVisitor::~JsVisitor()
{
}

void
Slice::JsVisitor::writeMarshalUnmarshalParams(const ParamDeclList& params, const OperationPtr& op, bool marshal)
{
    // TODO
    ParamDeclList optionals;

    for(ParamDeclList::const_iterator pli = params.begin(); pli != params.end(); ++pli)
    {
        string param = fixId((*pli)->name());
        TypePtr type = (*pli)->type();
        if(!marshal && isClassType(type))
        {
            param = (*pli)->name() + "__PP";
            string typeS = typeToString(type);
            if((*pli)->optional())
            {
                _out << nl << "Ice.OptionalPatcher<" << typeS << "> " << param
                     << " = new Ice.OptionalPatcher<" << typeS << ">(" << getStaticId(type) << ");";
            }
            else
            {
                _out << nl << "IceInternal.ParamPatcher<" << typeS << "> " << param
                     << " = new IceInternal.ParamPatcher<" << typeS << ">(" << getStaticId(type) << ");";
            }
        }

        if((*pli)->optional())
        {
            optionals.push_back(*pli);
        }
        else
        {
            writeMarshalUnmarshalCode(_out, type, param, marshal);
        }
    }

    TypePtr ret;

    if(op && op->returnType())
    {
        ret = op->returnType();

        string param = "ret__";
        if(!marshal && isClassType(ret))
        {
            param += "PP";
            string typeS = typeToString(ret);
            if(op->returnIsOptional())
            {
                _out << nl << "Ice.OptionalPatcher<" << typeS << "> " << param
                     << " = new Ice.OptionalPatcher<" << typeS << ">(" << getStaticId(ret) << ");";
            }
            else
            {
                _out << nl << "IceInternal.ParamPatcher<" << typeS << "> " << param
                     << " = new IceInternal.ParamPatcher<" << typeS << ">(" << getStaticId(ret) << ");";
            }
        }

        if(!op->returnIsOptional())
        {
            writeMarshalUnmarshalCode(_out, ret, param, marshal);
        }
    }

    //
    // Sort optional parameters by tag.
    //
    class SortFn
    {
    public:
        static bool compare(const ParamDeclPtr& lhs, const ParamDeclPtr& rhs)
        {
            return lhs->tag() < rhs->tag();
        }
    };
    optionals.sort(SortFn::compare);

    //
    // Handle optional parameters.
    //
    bool checkReturnType = op && op->returnIsOptional();

    for(ParamDeclList::const_iterator pli = optionals.begin(); pli != optionals.end(); ++pli)
    {
        if(checkReturnType && op->returnTag() < (*pli)->tag())
        {
            const string param = !marshal && isClassType(ret) ? "ret__PP" : "ret__";
            writeOptionalMarshalUnmarshalCode(_out, ret, param, op->returnTag(), marshal);
            checkReturnType = false;
        }

        string param = fixId((*pli)->name());
        TypePtr type = (*pli)->type();

        if(!marshal && isClassType(type))
        {
            param = (*pli)->name() + "__PP";
        }

        writeOptionalMarshalUnmarshalCode(_out, type, param, (*pli)->tag(), marshal);
    }

    if(checkReturnType)
    {
        const string param = !marshal && isClassType(ret) ? "ret__PP" : "ret__";
        writeOptionalMarshalUnmarshalCode(_out, ret, param, op->returnTag(), marshal);
    }
}

void
Slice::JsVisitor::writePostUnmarshalParams(const ParamDeclList& params, const OperationPtr& op)
{
    // TODO
    for(ParamDeclList::const_iterator pli = params.begin(); pli != params.end(); ++pli)
    {
        if(isClassType((*pli)->type()))
        {
            const string tmp = (*pli)->name() + "__PP";
            _out << nl << fixId((*pli)->name()) << " = " << tmp << ".value;";
        }
    }

    if(op && op->returnType() && isClassType(op->returnType()))
    {
        _out << nl << "ret__ = ret__PP.value;";
    }
}

void
Slice::JsVisitor::writeMarshalDataMember(const DataMemberPtr& member, const string& name)
{
    // TODO
    if(member->optional())
    {
        writeOptionalMarshalUnmarshalCode(_out, member->type(), name, member->tag(), true);
    }
    else
    {
        writeMarshalUnmarshalCode(_out, member->type(), name, true);
    }
}

void
Slice::JsVisitor::writeUnmarshalDataMember(const DataMemberPtr& member, const string& name, bool needPatcher,
                                           int& patchIter)
{
    const bool classType = isClassType(member->type());

    string patcher;
    if(classType)
    {
        patcher = "new Patcher__(" + getStaticId(member->type()) + ", this";
        if(needPatcher)
        {
            ostringstream ostr;
            ostr << ", " << patchIter++;
            patcher += ostr.str();
        }
        patcher += ")";
    }

    if(member->optional())
    {
        writeOptionalMarshalUnmarshalCode(_out, member->type(), classType ? patcher : name, member->tag(), false);
    }
    else
    {
        writeMarshalUnmarshalCode(_out, member->type(), classType ? patcher : name, false);
    }
}

void
Slice::JsVisitor::writeDispatchAndMarshalling(const ClassDefPtr& p)
{
}

vector<string>
Slice::JsVisitor::getParams(const OperationPtr& op)
{
    vector<string> params;
    ParamDeclList paramList = op->parameters();
    for(ParamDeclList::const_iterator q = paramList.begin(); q != paramList.end(); ++q)
    {
        if(!(*q)->isOutParam())
        {
            params.push_back(fixId((*q)->name()));
        }
    }
    return params;
}

vector<string>
Slice::JsVisitor::getParamsAsync(const OperationPtr& op, bool amd, bool newAMI)
{
    vector<string> params;

    string name = fixId(op->name());
    ContainerPtr container = op->container();
    ClassDefPtr cl = ClassDefPtr::dynamicCast(container); // Get the class containing the op.
    string scope = fixId(cl->scope());
    if(!newAMI)
    {
        params.push_back(scope + (amd ? "AMD_" : "AMI_") + cl->name() + '_' + op->name() + " cb__");
    }

    ParamDeclList paramList = op->parameters();
    for(ParamDeclList::const_iterator q = paramList.begin(); q != paramList.end(); ++q)
    {
        if(!(*q)->isOutParam())
        {
            params.push_back(typeToString((*q)->type(), (*q)->optional()) + " " + fixId((*q)->name()));
        }
    }
    return params;
}

vector<string>
Slice::JsVisitor::getParamsAsyncCB(const OperationPtr& op, bool newAMI, bool outKeyword)
{
    vector<string> params;

    if(!newAMI)
    {
        TypePtr ret = op->returnType();
        if(ret)
        {
            params.push_back(typeToString(ret, op->returnIsOptional()) + " ret__");
        }
    }

    ParamDeclList paramList = op->parameters();
    for(ParamDeclList::const_iterator q = paramList.begin(); q != paramList.end(); ++q)
    {
        if((*q)->isOutParam())
        {
            if(!newAMI)
            {
                params.push_back(typeToString((*q)->type(), (*q)->optional()) + ' ' + fixId((*q)->name()));
            }
            else
            {
                string s;
                if(outKeyword)
                {
                    s += "out ";
                }
                s += typeToString((*q)->type(), (*q)->optional()) + ' ' + fixId((*q)->name());
                params.push_back(s);
            }
        }
    }

    return params;
}

vector<string>
Slice::JsVisitor::getArgs(const OperationPtr& op)
{
    vector<string> args;
    ParamDeclList paramList = op->parameters();
    for(ParamDeclList::const_iterator q = paramList.begin(); q != paramList.end(); ++q)
    {
        string arg = fixId((*q)->name());
        if((*q)->isOutParam())
        {
            arg = "out " + arg;
        }
        args.push_back(arg);
    }
    return args;
}

vector<string>
Slice::JsVisitor::getArgsAsync(const OperationPtr& op, bool newAMI)
{
    vector<string> args;

    if(!newAMI)
    {
        args.push_back("cb__");
    }

    ParamDeclList paramList = op->parameters();
    for(ParamDeclList::const_iterator q = paramList.begin(); q != paramList.end(); ++q)
    {
        if(!(*q)->isOutParam())
        {
            args.push_back(fixId((*q)->name()));
        }
    }
    return args;
}

vector<string>
Slice::JsVisitor::getArgsAsyncCB(const OperationPtr& op, bool newAMI, bool outKeyword)
{
    vector<string> args;

    if(!newAMI)
    {
        TypePtr ret = op->returnType();
        if(ret)
        {
            args.push_back("ret__");
        }
    }

    ParamDeclList paramList = op->parameters();
    for(ParamDeclList::const_iterator q = paramList.begin(); q != paramList.end(); ++q)
    {
        if((*q)->isOutParam())
        {
            string s;
            if(outKeyword)
            {
                s = "out ";
            }
            s += fixId((*q)->name());
            args.push_back(s);
        }
    }

    return args;
}

string
Slice::JsVisitor::getValue(const string& scope, const TypePtr& type)
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
        return getReference(scope, (*en->getEnumerators().begin())->scoped());
    }

    return "null";
}

void
Slice::JsVisitor::writeConstantValue(const string& scope, const TypePtr& type, const SyntaxTreeBasePtr& valueType,
                                     const string& value)
{
    ConstPtr constant = ConstPtr::dynamicCast(valueType);
    if(constant)
    {
        _out << getReference(scope, constant->scoped());
    }
    else
    {
        BuiltinPtr bp = BuiltinPtr::dynamicCast(type);
        EnumPtr ep;
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

            _out << "\"";                                    // Opening "

            for(string::const_iterator c = value.begin(); c != value.end(); ++c)
            {
                if(charSet.find(*c) == charSet.end())
                {
                    unsigned char uc = *c;                   // char may be signed, so make it positive
                    ostringstream s;
                    s << "\\u";                      // Print as unicode if not in basic source character set
                    s << hex;
                    s.width(4);
                    s.fill('0');
                    s << static_cast<unsigned>(uc);
                    _out << s.str();
                }
                else
                {
                   switch(*c)
                    {
                        case '\\':
                        case '"':
                        {
                            _out << "\\";
                            break;
                        }
                    }
                    _out << *c;                              // Print normally if in basic source character set
                }
            }

            _out << "\"";                                    // Closing "
        }
        else if((ep = EnumPtr::dynamicCast(type)))
        {
            string::size_type colon = value.rfind(':');
            string enumerator;
            if(colon != string::npos)
            {
                enumerator = fixId(value.substr(colon + 1));
            }
            else
            {
                enumerator = fixId(value);
            }
            _out << getReference(scope, ep->scoped()) << '.' << enumerator;
        }
        else
        {
            _out << value;
        }
    }
}

string
Slice::JsVisitor::toJsIdent(const string& s)
{
    string::size_type pos = s.find('#');
    if(pos == string::npos)
    {
        return s;
    }

    string result = s;
    if(pos == 0)
    {
        return result.erase(0, 1);
    }

    result[pos] = '.';
    return result;
}

string
Slice::JsVisitor::editMarkup(const string& s)
{
    // TODO

    //
    // Strip HTML markup and javadoc links--VS doesn't display them.
    //
    string result = s;
    string::size_type pos = 0;
    do
    {
        pos = result.find('<', pos);
        if(pos != string::npos)
        {
            string::size_type endpos = result.find('>', pos);
            if(endpos == string::npos)
            {
                break;
            }
            result.erase(pos, endpos - pos + 1);
        }
    }
    while(pos != string::npos);

    const string link = "{@link";
    pos = 0;
    do
    {
        pos = result.find(link, pos);
        if(pos != string::npos)
        {
            result.erase(pos, link.size() + 1); // erase following white space too
            string::size_type endpos = result.find('}', pos);
            if(endpos != string::npos)
            {
                string ident = result.substr(pos, endpos - pos);
                result.erase(pos, endpos - pos + 1);
                result.insert(pos, toJsIdent(ident));
            }
        }
    }
    while(pos != string::npos);

    //
    // Strip @see sections because VS does not display them.
    //
    static const string seeTag = "@see";
    pos = 0;
    do
    {
        //
        // Look for the next @ and delete up to that, or
        // to the end of the string, if not found.
        //
        pos = result.find(seeTag, pos);
        if(pos != string::npos)
        {
            string::size_type next = result.find('@', pos + seeTag.size());
            if(next != string::npos)
            {
                result.erase(pos, next - pos);
            }
            else
            {
                result.erase(pos, string::npos);
            }
        }
    } while(pos != string::npos);

    //
    // Replace @param, @return, and @throws with corresponding <param>, <returns>, and <exception> tags.
    //
    static const string paramTag = "@param";
    pos = 0;
    do
    {
        pos = result.find(paramTag, pos);
        if(pos != string::npos)
        {
            result.erase(pos, paramTag.size() + 1);

            string::size_type startIdent = result.find_first_not_of(" \t", pos);
            if(startIdent != string::npos)
            {
                string::size_type endIdent = result.find_first_of(" \t", startIdent);
                if(endIdent != string::npos)
                {
                    string ident = result.substr(startIdent, endIdent - startIdent);
                    string::size_type endComment = result.find_first_of("@<", endIdent);
                    string comment = result.substr(endIdent + 1,
                                                   endComment == string::npos ? endComment : endComment - endIdent - 1);
                    result.erase(startIdent, endComment == string::npos ? string::npos : endComment - startIdent);
                    string newComment = "<param name=\"" + ident + "\">" + comment + "</param>\n";
                    result.insert(startIdent, newComment);
                    pos = startIdent + newComment.size();
                }
            }
            else
            {
               pos += paramTag.size();
            }
        }
    } while(pos != string::npos);

    static const string returnTag = "@return";
    pos = result.find(returnTag);
    if(pos != string::npos)
    {
        result.erase(pos, returnTag.size() + 1);
        string::size_type endComment = result.find_first_of("@<", pos);
        string comment = result.substr(pos, endComment == string::npos ? endComment : endComment - pos);
        result.erase(pos, endComment == string::npos ? string::npos : endComment - pos);
        string newComment = "<returns>" + comment + "</returns>\n";
        result.insert(pos, newComment);
        pos = pos + newComment.size();
    }

    static const string throwsTag = "@throws";
    pos = 0;
    do
    {
        pos = result.find(throwsTag, pos);
        if(pos != string::npos)
        {
            result.erase(pos, throwsTag.size() + 1);

            string::size_type startIdent = result.find_first_not_of(" \t", pos);
            if(startIdent != string::npos)
            {
                string::size_type endIdent = result.find_first_of(" \t", startIdent);
                if(endIdent != string::npos)
                {
                    string ident = result.substr(startIdent, endIdent - startIdent);
                    string::size_type endComment = result.find_first_of("@<", endIdent);
                    string comment = result.substr(endIdent + 1,
                                                   endComment == string::npos ? endComment : endComment - endIdent - 1);
                    result.erase(startIdent, endComment == string::npos ? string::npos : endComment - startIdent);
                    string newComment = "<exception name=\"" + ident + "\">" + comment + "</exception>\n";
                    result.insert(startIdent, newComment);
                    pos = startIdent + newComment.size();
                }
            }
            else
            {
               pos += throwsTag.size();
            }
        }
    } while(pos != string::npos);

    return result;
}

StringList
Slice::JsVisitor::splitIntoLines(const string& comment)
{
    string s = editMarkup(comment);
    StringList result;
    string::size_type pos = 0;
    string::size_type nextPos;
    while((nextPos = s.find_first_of('\n', pos)) != string::npos)
    {
        result.push_back(string(s, pos, nextPos - pos));
        pos = nextPos + 1;
    }
    string lastLine = string(s, pos);
    if(lastLine.find_first_not_of(" \t\n\r") != string::npos)
    {
        result.push_back(lastLine);
    }
    return result;
}

void
Slice::JsVisitor::splitComment(const ContainedPtr& p, StringList& summaryLines, StringList& remarksLines)
{
    string s = p->comment();
    string summary;
    unsigned int i;
    for(i = 0; i < s.size(); ++i)
    {
        if(s[i] == '.' && (i + 1 >= s.size() || isspace(static_cast<unsigned char>(s[i + 1]))))
        {
            summary += '.';
            ++i;
            break;
        }
        else
        {
            summary += s[i];
        }
    }
    summaryLines = splitIntoLines(summary);

    if(!summaryLines.empty())
    {
        remarksLines = splitIntoLines(trim(s.substr(i)));
    }
}

void
Slice::JsVisitor::writeDocComment(const ContainedPtr& p, const string& deprecateReason, const string& extraParam)
{
    // TODO

    StringList summaryLines;
    StringList remarksLines;
    splitComment(p, summaryLines, remarksLines);

    if(summaryLines.empty())
    {
        if(!deprecateReason.empty())
        {
            _out << nl << "///";
            _out << nl << "/// <summary>" << deprecateReason << "</summary>";
            _out << nl << "///";
        }
        return;
    }

    _out << nl << "/// <summary>";

    for(StringList::const_iterator i = summaryLines.begin(); i != summaryLines.end(); ++i)
    {
        _out << nl << "/// " << *i;
    }

    //
    // We generate everything into the summary tag (despite what the MSDN doc says) because
    // Visual Studio only shows the <summary> text and omits the <remarks> text.
    //

    if(!deprecateReason.empty())
    {
        _out << nl << "///";
        _out << nl << "/// <para>" << deprecateReason << "</para>";
        _out << nl << "///";
    }

    bool summaryClosed = false;

    if(!remarksLines.empty())
    {
        for(StringList::const_iterator i = remarksLines.begin(); i != remarksLines.end(); ++i)
        {
            //
            // The first param, returns, or exception tag ends the description.
            //
            static const string paramTag = "<param";
            static const string returnsTag = "<returns";
            static const string exceptionTag = "<exception";

            if(!summaryClosed &&
               (i->find(paramTag) != string::npos ||
                i->find(returnsTag) != string::npos ||
                i->find(exceptionTag) != string::npos))
            {
                _out << nl << "/// </summary>";
                _out << nl << "/// " << *i;
                summaryClosed = true;
            }
            else
            {
                _out << nl << "/// " << *i;
            }
        }
    }

    if(!summaryClosed)
    {
        _out << nl << "/// </summary>";
    }

    if(!extraParam.empty())
    {
        _out << nl << "/// " << extraParam;
    }

    _out << nl;
}

void
Slice::JsVisitor::writeDocCommentOp(const OperationPtr& p)
{
    ContainerPtr container = p->container();
    ContainedPtr contained = ContainedPtr::dynamicCast(container);
    string deprecateReason = getDeprecateReason(p, contained, "operation");

    StringList summaryLines;
    StringList remarksLines;
    splitComment(p, summaryLines, remarksLines);

    if(summaryLines.empty())
    {
        if(!deprecateReason.empty())
        {
            _out << nl << "///";
            _out << nl << "/// <summary>" << deprecateReason << "</summary>";
            _out << nl << "///";
        }
        return;
    }

    _out << nl << "/// <summary>";

    //
    // Output the leading comment block up until the first <param>, <returns>, or <exception> tag.
    //
    for(StringList::const_iterator i = summaryLines.begin(); i != summaryLines.end(); ++i)
    {
        _out << nl << "/// " << *i;
    }

    bool done = false;
    for(StringList::const_iterator i = remarksLines.begin(); i != remarksLines.end() && !done; ++i)
    {
        if(i->find("<param") != string::npos ||
           i->find("<returns") != string::npos ||
           i->find("<exception") != string::npos)
        {
            done = true;
        }
        else
        {
            _out << nl << "/// " << *i;
        }
    }

    if(!deprecateReason.empty())
    {
        _out << nl << "/// <para>" << deprecateReason << "</para>";
    }

    _out << nl << "/// </summary>";
}

void
Slice::JsVisitor::writeDocCommentAsync(const OperationPtr& p, ParamDir paramType, const string& extraParam, bool newAMI)
{
    // TODO: this needs fixing for newAMI == true
    ContainerPtr container = p->container();
    ClassDefPtr contained = ClassDefPtr::dynamicCast(container);
    string deprecateReason = getDeprecateReason(p, contained, "operation");

    StringList summaryLines;
    StringList remarksLines;
    splitComment(p, summaryLines, remarksLines);

    if(summaryLines.empty() && deprecateReason.empty())
    {
        return;
    }

    if(paramType == OutParam)
    {
        if(!newAMI)
        {
            _out << nl << "/// <summary>";
            _out << nl << "/// ice_response indicates that";
            _out << nl << "/// the operation completed successfully.";
            _out << nl << "/// </summary>";
        }

        //
        // Find the comment for the return value (if any) and rewrite that as a <param> comment.
        //
        static const string returnsTag = "<returns>";
        bool doneReturn = false;
        bool foundReturn = false;
        for(StringList::const_iterator i = remarksLines.begin(); i != remarksLines.end() && !doneReturn; ++i)
        {
            if(!foundReturn)
            {
                string::size_type pos = i->find(returnsTag);
                if(pos != string::npos)
                {
                    pos += returnsTag.size();
                    foundReturn = true;
                    string::size_type endpos = i->find('<', pos);
                    if(endpos != string::npos)
                    {
                        _out << nl << "/// <param name=\"ret__\">(return value) " << i->substr(pos, endpos - pos);
                    }
                    else
                    {
                        _out << nl << "/// <param name=\"ret__\">(return value) " << i->substr(pos);
                    }
                }
            }
            else
            {
                string::size_type pos = i->find('<');
                if(pos != string::npos)
                {
                    _out << nl << "/// " << i->substr(0, pos) << "</param>";
                    doneReturn = true;
                }
                else
                {
                    _out << nl << "/// " << *i;
                }
            }
        }
        if(foundReturn && !doneReturn)
        {
            _out << "</param>";
        }
    }
    else
    {
        //
        // Output the leading comment block up until the first tag.
        //
        _out << nl << "/// <summary>";
        for(StringList::const_iterator i = summaryLines.begin(); i != summaryLines.end(); ++i)
        {
            _out << nl << "/// " << *i;
        }

        bool done = false;
        for(StringList::const_iterator i = remarksLines.begin(); i != remarksLines.end() && !done; ++i)
        {
            string::size_type pos = i->find('<');
            done = true;
            if(pos != string::npos)
            {
                if(pos != 0)
                {
                    _out << nl << "/// " << i->substr(0, pos);
                }
            }
            else
            {
                _out << nl << "/// " << *i;
            }
        }
        _out << nl << "/// </summary>";
    }

    //
    // Write the comments for the parameters.
    //
    writeDocCommentParam(p, paramType, newAMI);

    if(!extraParam.empty())
    {
        _out << nl << "/// " << extraParam;
    }

    if(paramType == InParam)
    {
        if(!deprecateReason.empty())
        {
            _out << nl << "/// <para>" << deprecateReason << "</para>";
        }
    }
}

void
Slice::JsVisitor::writeDocCommentParam(const OperationPtr& p, ParamDir paramType, bool newAMI)
{
    //
    // Collect the names of the in- or -out parameters to be documented.
    //
    ParamDeclList tmp = p->parameters();
    vector<string> params;
    for(ParamDeclList::const_iterator q = tmp.begin(); q != tmp.end(); ++q)
    {
        if((*q)->isOutParam() && paramType == OutParam)
        {
            params.push_back((*q)->name());
        }
        else if(!(*q)->isOutParam() && paramType == InParam)
        {
            params.push_back((*q)->name());
        }
    }

    //
    // Print a comment for the callback parameter.
    //
    if(paramType == InParam && !newAMI)
    {
        _out << nl << "/// <param name=\"cb__\">The callback object for the operation.</param>";
    }

    //
    // Print the comments for all the parameters that appear in the parameter list.
    //
    StringList summaryLines;
    StringList remarksLines;
    splitComment(p, summaryLines, remarksLines);

    const string paramTag = "<param";
    StringList::const_iterator i = remarksLines.begin();
    while(i != remarksLines.end())
    {
        string line = *i++;
        if(line.find(paramTag) != string::npos)
        {
            string::size_type paramNamePos = line.find('"', paramTag.length());
            if(paramNamePos != string::npos)
            {
                string::size_type paramNameEndPos = line.find('"', paramNamePos + 1);
                string paramName = line.substr(paramNamePos + 1, paramNameEndPos - paramNamePos - 1);
                if(std::find(params.begin(), params.end(), paramName) != params.end())
                {
                    _out << nl << "/// " << line;
                    StringList::const_iterator j;
                    if (i == remarksLines.end())
                    {
                        break;
                    }
                    j = i++;
                    while(j != remarksLines.end())
                    {
                        string::size_type endpos = j->find('>');
                        if(endpos == string::npos)
                        {
                            i = j;
                            _out << nl << "/// " << *j++;
                        }
                        else
                        {
                            _out << nl << "/// " << *j++;
                            break;
                        }
                    }
                }
            }
        }
    }
}

Slice::Gen::Gen(const string& base, const vector<string>& includePaths, const string& dir)
    : _includePaths(includePaths)
{
    string fileBase = base;
    string::size_type pos = base.find_last_of("/\\");
    if(pos != string::npos)
    {
        fileBase = base.substr(pos + 1);
    }
    string file = fileBase + ".js";

    if(!dir.empty())
    {
        file = dir + '/' + file;
    }

    _out.open(file.c_str());
    if(!_out)
    {
        ostringstream os;
        os << "cannot open `" << file << "': " << strerror(errno);
        throw FileException(__FILE__, __LINE__, os.str());
    }
    FileTracker::instance()->addFile(file);
    printHeader();

    printGeneratedHeader(_out, fileBase + ".ice");
}

Slice::Gen::~Gen()
{
    if(_out.isOpen())
    {
        _out << '\n';
    }
}

void
Slice::Gen::generate(const UnitPtr& p)
{
    JsGenerator::validateMetaData(p);

    RequireVisitor requireVisitor(_out);
    p->visit(&requireVisitor, false);
    requireVisitor.writeRequires();

    //CompactIdVisitor compactIdVisitor(_out);
    //p->visit(&compactIdVisitor, false);

    TypesVisitor typesVisitor(_out);
    p->visit(&typesVisitor, false);

    //
    // Export the top-level modules.
    //
    ExportVisitor exportVisitor(_out);
    p->visit(&exportVisitor, false);
    exportVisitor.writeExports();
}

void
Slice::Gen::generateChecksums(const UnitPtr& u)
{
    // TODO
#if 0
    ChecksumMap map = createChecksums(u);
    if(!map.empty())
    {
        string className = "X" + generateUUID();
        for(string::size_type pos = 1; pos < className.size(); ++pos)
        {
            if(!isalnum(static_cast<unsigned char>(className[pos])))
            {
                className[pos] = '_';
            }
        }

        _out << sp << nl << "namespace IceInternal";
        _out << sb;
        _out << nl << "namespace SliceChecksums";
        _out << sb;
        _out << nl << "[_System.CodeDom.Compiler.GeneratedCodeAttribute(\"slice2cs\", \"" << ICE_STRING_VERSION
             << "\")]";
        _out << nl << "public sealed class " << className;
        _out << sb;
        _out << nl << "public static _System.Collections.Hashtable map = new _System.Collections.Hashtable();";
        _out << sp << nl << "static " << className << "()";
        _out << sb;
        for(ChecksumMap::const_iterator p = map.begin(); p != map.end(); ++p)
        {
            _out << nl << "map.Add(\"" << p->first << "\", \"";
            ostringstream str;
            str.flags(ios_base::hex);
            str.fill('0');
            for(vector<unsigned char>::const_iterator q = p->second.begin(); q != p->second.end(); ++q)
            {
                str << (int)(*q);
            }
            _out << str.str() << "\");";
        }
        _out << eb;
        _out << eb << ';';
        _out << eb;
        _out << eb;
    }
#endif
}

void
Slice::Gen::closeOutput()
{
    _out.close();
}

void
Slice::Gen::printHeader()
{
    static const char* header =
"// **********************************************************************\n"
"//\n"
"// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.\n"
"//\n"
"// This copy of Ice is licensed to you under the terms described in the\n"
"// ICE_LICENSE file included in this distribution.\n"
"//\n"
"// **********************************************************************\n"
        ;

    _out << header;
    _out << "//\n";
    _out << "// Ice version " << ICE_STRING_VERSION << "\n";
    _out << "//\n";
}

Slice::Gen::RequireVisitor::RequireVisitor(IceUtilInternal::Output& out)
    : JsVisitor(out), _seenClass(false), _seenUserException(false), _seenLocalException(false), _seenEnum(false)
{
}

bool
Slice::Gen::RequireVisitor::visitClassDefStart(const ClassDefPtr& p)
{
    if(!p->isLocal())
    {
        _seenClass = true;
    }

    return false;
}

bool
Slice::Gen::RequireVisitor::visitExceptionStart(const ExceptionPtr& p)
{
    if(p->isLocal())
    {
        _seenLocalException = true;
    }
    else
    {
        _seenUserException = true;
    }

    return false;
}

void
Slice::Gen::RequireVisitor::visitEnum(const EnumPtr& p)
{
    _seenEnum = true;
}

void
Slice::Gen::RequireVisitor::writeRequires()
{
    //
    // Generate require() statements for all of the run-time code needed by the generated code.
    //

    if(_seenClass)
    {
        _out << nl << "var __ice_Object = require(\"./Object\");";
        _out << nl << "var __ice_ObjectPrx = require(\"./ObjectPrx\");";
    }

    if(_seenLocalException || _seenUserException)
    {
        _out << nl << "var __ice_Ex = require(\"./Exception\");";
    }
    if(_seenLocalException)
    {
        _out << nl << "var __ice_LocalException = __ice_Ex.LocalException;";
    }
    if(_seenUserException)
    {
        _out << nl << "var __ice_UserException = __ice_Ex.UserException;";
    }

    if(_seenEnum)
    {
        _out << nl << "var __ice_EnumBase = require(\"./EnumBase\");";
    }

    _out << nl << "var __ice_HashMap = require(\"./HashMap\");";
    _out << nl << "var __ice_HashUtil = require(\"./HashUtil\");";
    _out << nl << "var __ice_ArrayUtil = require(\"./ArrayUtil\");";
}

Slice::Gen::CompactIdVisitor::CompactIdVisitor(IceUtilInternal::Output& out) :
    JsVisitor(out)
{
}

bool
Slice::Gen::CompactIdVisitor::visitUnitStart(const UnitPtr&)
{
    // TODO
    //_out << sp << nl << "namespace IceCompactId";
    //_out << sb;
    return true;
}

void
Slice::Gen::CompactIdVisitor::visitUnitEnd(const UnitPtr&)
{
    //_out << eb;
}

bool
Slice::Gen::CompactIdVisitor::visitClassDefStart(const ClassDefPtr& p)
{
    if(p->compactId() >= 0)
    {
#if 0 // TODO
        _out << sp;
        _out << nl << "public sealed class TypeId_" << p->compactId();
        _out << sb;
        _out << nl << "public readonly static string typeId = \"" << p->scoped() << "\";";
        _out << eb;
#endif
    }
    return false;
}

Slice::Gen::TypesVisitor::TypesVisitor(IceUtilInternal::Output& out)
    : JsVisitor(out)
{
}

bool
Slice::Gen::TypesVisitor::visitModuleStart(const ModulePtr& p)
{
    const bool topLevel = UnitPtr::dynamicCast(p->container());

    //
    // For a top-level module we write the following:
    //
    // var Foo = (function(_mod_Foo, undefined)
    // {
    //     // ...
    //     return _mod_Foo;
    // }(Foo || {}));
    //
    // A nested module is generated like this:
    //
    // var Foo = (function(_mod_Foo, undefined)
    // {
    //     _mod_Foo.SubModule = function(_mod_Foo_SubModule, undefined)
    //     {
    //         // ...
    //     }(_mod_Foo.SubModule || {}));
    //
    //     return _mod_Foo;
    // }(Foo || {}));
    //
    // The trailing "undefined" argument to the function is NOT passed a value, so by
    // default it takes on the native value of undefined. This avoids situations where
    // user code redefines the meaning of undefined.
    //
    // The argument to the inline invocation of the function either passes the existing
    // value of Foo (if present) or a new object ({}). This allows a module to be reopened.
    //

    _out << sp << nl;
    if(topLevel)
    {
        _out << "var ";
    }
    else
    {
        _out << getLocalScope(p->scope()) << '.';
    }
    _out << fixId(p->name()) << " = (function(" << getLocalScope(p->scoped()) << ", undefined)";
    _out << nl << "{";
    _out.inc();

    return true;
}

void
Slice::Gen::TypesVisitor::visitModuleEnd(const ModulePtr& p)
{
    const bool topLevel = UnitPtr::dynamicCast(p->container());

    _out << sp << nl << "return " << getLocalScope(p->scoped()) << ';';
    _out.dec();
    _out << nl << "}(";
    if(!topLevel)
    {
        _out << getLocalScope(p->scope()) << '.';
    }
    _out << fixId(p->name()) << " || {}));";
}

bool
Slice::Gen::TypesVisitor::visitClassDefStart(const ClassDefPtr& p)
{
    const string scope = p->scope();
    const string localScope = getLocalScope(scope);
    const string name = fixId(p->name());
    const string prxName = p->name() + "Prx";
    const string objectRef = "__ice_Object";
    const string prxRef = "__ice_ObjectPrx";

    ClassList bases = p->bases();
    ClassDefPtr base;
    string baseRef;
    string basePrxRef;
    const bool hasBaseClass = !bases.empty() && !bases.front()->isInterface();
    if(hasBaseClass)
    {
        base = bases.front();
        bases.erase(bases.begin());
        baseRef = getReference(scope, base->scoped());
        basePrxRef = getReference(scope, base->scoped() + "Prx");
    }
    else
    {
        baseRef = objectRef;
        basePrxRef = prxRef;
    }

    const DataMemberList allDataMembers = p->allDataMembers();
    const DataMemberList dataMembers = p->dataMembers();

    vector<string> allParamNames;
    for(DataMemberList::const_iterator q = allDataMembers.begin(); q != allDataMembers.end(); ++q)
    {
        allParamNames.push_back(fixId((*q)->name()));
    }

    vector<string> paramNames;
    for(DataMemberList::const_iterator q = dataMembers.begin(); q != dataMembers.end(); ++q)
    {
        paramNames.push_back(fixId((*q)->name()));
    }

    vector<string> baseParamNames;
    DataMemberList baseDataMembers;

    if(base)
    {
        baseDataMembers = base->allDataMembers();
        for(DataMemberList::const_iterator q = baseDataMembers.begin(); q != baseDataMembers.end(); ++q)
        {
            baseParamNames.push_back(fixId((*q)->name()));
        }
    }

    if(!p->isLocal())
    {
        _out << sp;
        _out << nl << localScope << '.' << prxName << " = function" << spar << epar;
        _out << sb;
        _out << nl << basePrxRef << ".call" << spar << "this" << epar << ';';
        _out << eb;
        _out << nl << localScope << '.' << prxName << ".prototype = new " << basePrxRef << "();";
        _out << nl << localScope << '.' << prxName << ".prototype.constructor = " << localScope << '.' << prxName
             << ';';
        _out << nl << localScope << '.' << prxName << ".checkedCast = function(__prx, __facet, __ctx)";
        _out << sb;
        // TODO
        _out << eb;
        _out << nl << localScope << '.' << prxName << ".uncheckedCast = function(__prx, __facet)";
        _out << sb;
        // TODO
        _out << eb;
    }

    _out << sp;
    writeDocComment(p, getDeprecateReason(p, 0, "type"));
    _out << nl << localScope << '.' << name << " = function" << spar << allParamNames << epar;
    _out << sb;
    _out << nl << baseRef << ".call" << spar << "this" << baseParamNames << epar << ';';
    for(DataMemberList::const_iterator q = dataMembers.begin(); q != dataMembers.end(); ++q)
    {
        const bool isProtected = p->hasMetaData("protected") || (*q)->hasMetaData("protected");
        const string m = fixId((*q)->name());
        const string memberName = isProtected ? "_" + (*q)->name() : m;
        if((*q)->optional())
        {
            if((*q)->defaultValueType())
            {
                _out << nl << "this." << memberName << " = " << m << " !== undefined ? " << m << " : ";
                writeConstantValue(scope, (*q)->type(), (*q)->defaultValueType(), (*q)->defaultValue());
                _out << ';';
            }
            else
            {
                _out << nl << "this." << memberName << " = " << m << ';';
            }
        }
        else
        {
            _out << nl << "this." << memberName << " = " << m << " !== undefined ? " << m << " : ";
            if((*q)->defaultValueType())
            {
                writeConstantValue(scope, (*q)->type(), (*q)->defaultValueType(), (*q)->defaultValue());
            }
            else
            {
                _out << getValue(scope, (*q)->type());
            }
            _out << ';';
        }
    }
    _out << eb;
    _out << nl << localScope << '.' << name << ".prototype = new " << baseRef << "();";
    _out << nl << localScope << '.' << name << ".prototype.constructor = " << localScope << '.' << name << ';';

    if(!p->isLocal())
    {
        string scoped = p->scoped();
        ClassList allBases = p->allBases();
        StringList ids;
#if defined(__IBMCPP__) && defined(NDEBUG)
        //
        // VisualAge C++ 6.0 does not see that ClassDef is a Contained,
        // when inlining is on. The code below issues a warning: better
        // than an error!
        //
        transform(allBases.begin(), allBases.end(), back_inserter(ids),
                  ::IceUtil::constMemFun<string,ClassDef>(&Contained::scoped));
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
        StringList::const_iterator scopedIter = find(ids.begin(), ids.end(), scoped);
        assert(scopedIter != ids.end());
        StringList::difference_type scopedPos = IceUtilInternal::distance(firstIter, scopedIter);

        _out << nl << localScope << '.' << name << ".__ids = [";
        _out.inc();
        for(StringList::const_iterator q = ids.begin(); q != ids.end(); ++q)
        {
            if(q != ids.begin())
            {
                _out << ',';
            }
            _out << nl << '"' << *q << '"';
        }
        _out.dec();
        _out << nl << "];";

        _out << nl << localScope << '.' << name << ".prototype.ice_ids = function(current)";
        _out << sb;
        _out << nl << "return " << localScope << '.' << name << ".__ids;";
        _out << eb;

        _out << sp;
        _out << nl << localScope << '.' << name << ".ice_staticId = function()";
        _out << sb;
        _out << nl << "return " << localScope << '.' << name << ".__ids[" << scopedPos << "];";
        _out << eb;
    }

    const OperationList ops = p->operations();
    for(OperationList::const_iterator q = ops.begin(); q != ops.end(); ++q)
    {
        //
        // TODO: This is compact but doesn't show the function's signature. A more verbose alternative
        // would be to generate a dummy function:
        //
        // Foo.prototype.doSomething = function(arg1, arg2)
        // {
        //     this.__notImplemented();
        // }
        //
        _out << nl << localScope << '.' << name << ".prototype." << fixId((*q)->name()) << " = " << objectRef
             << ".prototype.__notImplemented;";
    }

    _out << sp;
    _out << nl << localScope << '.' << name << ".prototype.toString = function()";
    _out << sb;
    _out << nl << "return \"[object " << p->scoped().substr(2) << "]\";";
    _out << eb;

    // TODO: equals?

    if(!p->isLocal())
    {
        for(OperationList::const_iterator q = ops.begin(); q != ops.end(); ++q)
        {
            _out << sp << nl << localScope << '.' << prxName << ".prototype." << fixId((*q)->name()) << " = function"
                 << spar << getParams(*q) << "__ctx" << epar;
            _out << sb;
            // TODO
            _out << eb;
        }
    }

    return false;
}

void
Slice::Gen::TypesVisitor::visitOperation(const OperationPtr& p)
{
#if 0 // TODO
    ClassDefPtr classDef = ClassDefPtr::dynamicCast(p->container());
    bool isLocal = classDef->isLocal();
    bool isInterface = classDef->isInterface();

    //
    // Non-local classes and interfaces get the operations from their
    // Operations base interfaces.
    //
    if(isInterface && !isLocal)
    {
        return;
    }

    bool amd = !isLocal && (classDef->hasMetaData("amd") || p->hasMetaData("amd"));

    string name = p->name();
    ParamDeclList paramList = p->parameters();
    vector<string> params;
    vector<string> args;
    string retS;

    if(!amd)
    {
        params = getParams(p);
        args = getArgs(p);
        name = fixId(name, DotNet::ICloneable, true);
        retS = typeToString(p->returnType(), p->returnIsOptional());
    }
    else
    {
        params = getParamsAsync(p, true);
        args = getArgsAsync(p);
        retS = "void";
        name = name + "_async";
    }

    _out << sp;
    if(isInterface && isLocal)
    {
        _out << nl;
    }

    writeDocComment(p, getDeprecateReason(p, classDef, "operation"));
    if(!isInterface)
    {
        _out << nl << "public ";
        if(isLocal)
        {
            _out << "abstract ";
        }
    }
    _out << retS << " " << name << spar << params << epar;
    if(isLocal)
    {
        _out << ";";
    }
    else
    {
        _out << sb;
        _out << nl;
        if(!amd && p->returnType())
        {
            _out << "return ";
        }
        _out << name << spar << args << "Ice.ObjectImpl.defaultCurrent" << epar << ';';
        _out << eb;
    }

    if(!isLocal)
    {
        _out << nl << "public abstract " << retS << " " << name
             << spar << params << "Ice.Current current__" << epar << ';';
    }

    if(isLocal && (classDef->hasMetaData("async") || p->hasMetaData("async")))
    {
        vector<string> paramsNewAsync = getParamsAsync(p, false, true);

        _out << sp;
        _out << nl;
        if(!isInterface)
        {
            _out << "public abstract ";
        }
        _out << "Ice.AsyncResult";
        if(p->returnsData())
        {
            string clScope = fixId(classDef->scope());
            string cbType = clScope + "Callback_" + classDef->name() + "_" + name;
            _out << '<' << cbType << '>';
        }
        _out << " begin_" << name << spar << paramsNewAsync << epar << ';';

        _out << sp;
        _out << nl;
        if(!isInterface)
        {
            _out << "public abstract ";
        }
        _out << "Ice.AsyncResult begin_" << name << spar << paramsNewAsync << "Ice.AsyncCallback cb__"
             << "object cookie__" << epar << ';';

        _out << sp;
        _out << nl;
        if(!isInterface)
        {
            _out << "public abstract ";
        }
        _out << typeToString(p->returnType(), p->returnIsOptional()) << " end_" << name << spar
             << getParamsAsyncCB(p, true) << "Ice.AsyncResult r__" << epar << ';';
    }
#endif
}

void
Slice::Gen::TypesVisitor::visitSequence(const SequencePtr& p)
{
    // TODO
}

bool
Slice::Gen::TypesVisitor::visitExceptionStart(const ExceptionPtr& p)
{
    const string scope = p->scope();
    const string localScope = getLocalScope(scope);
    const string name = fixId(p->name());
    const ExceptionPtr base = p->base();
    string baseRef;
    if(base)
    {
        baseRef = getReference(scope, base->scoped());
    }
    else
    {
        baseRef = p->isLocal() ? "__ice_LocalException" : "__ice_UserException";
    }

    const DataMemberList allDataMembers = p->allDataMembers();
    const DataMemberList dataMembers = p->dataMembers();

    vector<string> allParamNames;
    for(DataMemberList::const_iterator q = allDataMembers.begin(); q != allDataMembers.end(); ++q)
    {
        allParamNames.push_back(fixId((*q)->name()));
    }

    vector<string> paramNames;
    for(DataMemberList::const_iterator q = dataMembers.begin(); q != dataMembers.end(); ++q)
    {
        paramNames.push_back(fixId((*q)->name()));
    }

    vector<string> baseParamNames;
    DataMemberList baseDataMembers;

    if(p->base())
    {
        baseDataMembers = p->base()->allDataMembers();
        for(DataMemberList::const_iterator q = baseDataMembers.begin(); q != baseDataMembers.end(); ++q)
        {
            baseParamNames.push_back(fixId((*q)->name()));
        }
    }

    _out << sp;
    writeDocComment(p, getDeprecateReason(p, 0, "type"));
    _out << nl << localScope << '.' << name << " = function" << spar << allParamNames << "_cause" << epar;
    _out << sb;
    _out << nl << baseRef << ".call" << spar << "this" << baseParamNames << "_cause" << epar << ';';
    for(DataMemberList::const_iterator q = dataMembers.begin(); q != dataMembers.end(); ++q)
    {
        const string m = fixId((*q)->name());
        if((*q)->optional())
        {
            if((*q)->defaultValueType())
            {
                _out << nl << "this." << m << " = " << m << " !== undefined ? " << m << " : ";
                writeConstantValue(scope, (*q)->type(), (*q)->defaultValueType(), (*q)->defaultValue());
                _out << ';';
            }
            else
            {
                _out << nl << "this." << m << " = " << m << ';';
            }
        }
        else
        {
            _out << nl << "this." << m << " = " << m << " !== undefined ? " << m << " : ";
            if((*q)->defaultValueType())
            {
                writeConstantValue(scope, (*q)->type(), (*q)->defaultValueType(), (*q)->defaultValue());
            }
            else
            {
                _out << getValue(scope, (*q)->type());
            }
            _out << ';';
        }
    }
    _out << eb;
    _out << nl << localScope << '.' << name << ".prototype = new " << baseRef << "();";
    _out << nl << localScope << '.' << name << ".prototype.constructor = " << localScope << '.' << name << ';';

    _out << sp;
    _out << nl << localScope << '.' << name << ".prototype.ice_name = function()";
    _out << sb;
    _out << nl << "return \"" << p->scoped().substr(2) << "\";";
    _out << eb;

    _out << sp;
    _out << nl << localScope << '.' << name << ".prototype.toString = function()";
    _out << sb;
    // TODO
    _out << nl << "return this.ice_name();";
    _out << eb;

    // TODO: equals?

    return false;
}

bool
Slice::Gen::TypesVisitor::visitStructStart(const StructPtr& p)
{
    const string scope = p->scope();
    const string localScope = getLocalScope(scope);
    const string name = fixId(p->name());

    const DataMemberList dataMembers = p->dataMembers();

    vector<string> paramNames;
    for(DataMemberList::const_iterator q = dataMembers.begin(); q != dataMembers.end(); ++q)
    {
        paramNames.push_back(fixId((*q)->name()));
    }

    _out << sp;
    writeDocComment(p, getDeprecateReason(p, 0, "type"));
    _out << nl << localScope << '.' << name << " = function" << spar << paramNames << epar;
    _out << sb;
    for(DataMemberList::const_iterator q = dataMembers.begin(); q != dataMembers.end(); ++q)
    {
        string memberName = fixId((*q)->name());
        _out << nl << "this." << memberName << " = " << memberName << " !== undefined ? " << memberName << " : ";
        if((*q)->defaultValueType())
        {
            writeConstantValue(scope, (*q)->type(), (*q)->defaultValueType(), (*q)->defaultValue());
        }
        else
        {
            _out << getValue(scope, (*q)->type());
        }
        _out << ';';
    }
    _out << eb;

    _out << sp;
    _out << nl << localScope << '.' << name << ".prototype.toString = function()";
    _out << sb;
    // TODO
    _out << nl << "return \"\";";
    _out << eb;

    _out << sp;
    _out << nl << localScope << '.' << name << ".prototype.clone = function()";
    _out << sb;
    _out << nl << "var __r = new " << localScope << '.' << name << "();";
    for(DataMemberList::const_iterator q = dataMembers.begin(); q != dataMembers.end(); ++q)
    {
        const string memberName = fixId((*q)->name());
        writeMemberClone("__r." + memberName, "this." + memberName, (*q)->type(), 0);
    }
    _out << nl << "return __r;";
    _out << eb;

    _out << sp;
    _out << nl << localScope << '.' << name << ".prototype.equals = function(rhs)";
    _out << sb;
    _out << nl << "if(this === rhs)";
    _out << sb;
    _out << nl << "return true;";
    _out << eb;
    _out << nl << "if(!(rhs instanceof " << localScope << '.' << name << "))";
    _out << sb;
    _out << nl << "return false;";
    _out << eb;
    for(DataMemberList::const_iterator q = dataMembers.begin(); q != dataMembers.end(); ++q)
    {
        const string memberName = fixId((*q)->name());
        writeMemberEquals("this." + memberName, "rhs." + memberName, (*q)->type(), 0);
    }
    _out << sp << nl << "return true;";
    _out << eb;

    _out << sp;
    _out << nl << localScope << '.' << name << ".prototype.hashCode = function()";
    _out << sb;
    _out << nl << "var __h = 5381;";
    for(DataMemberList::const_iterator q = dataMembers.begin(); q != dataMembers.end(); ++q)
    {
        const string memberName = fixId((*q)->name());
        writeMemberHashCode("this." + memberName, (*q)->type(), 0);
    }
    _out << nl << "return __h;";
    _out << eb;

#if 0
    _out << sp << nl << "public java.lang.Object" << nl << "clone()";
    _out << sb;
    _out << nl << "java.lang.Object o = null;";
    _out << nl << "try";
    _out << sb;
    _out << nl << "o = super.clone();";
    _out << eb;
    _out << nl << "catch(CloneNotSupportedException ex)";
    _out << sb;
    _out << nl << "assert false; // impossible";
    _out << eb;
    _out << nl << "return o;";
    _out << eb;
#endif

    return false;
}

void
Slice::Gen::TypesVisitor::visitDictionary(const DictionaryPtr& p)
{
#if 0
    if(!p->hasMetaData("clr:collection"))
    {
        return;
    }

    string name = fixId(p->name());
    string ks = typeToString(p->keyType());
    string vs = typeToString(p->valueType());

    _out << sp;
    _out << nl << "public class " << name
         << " : Ice.DictionaryBase<" << ks << ", " << vs << ">, _System.ICloneable";
    _out << sb;

    _out << sp << nl << "#region " << name << " members";

    _out << sp << nl << "public void AddRange(" << name << " d__)";
    _out << sb;
    _out << nl << "foreach(_System.Collections.Generic.KeyValuePair<" << ks << ", " << vs << "> e in d__.dict_)";
    _out << sb;
    _out << nl << "try";
    _out << sb;
    _out << nl << "dict_.Add(e.Key, e.Value);";
    _out << eb;
    _out << nl << "catch(_System.ArgumentException)";
    _out << sb;
    _out << nl << "// ignore";
    _out << eb;
    _out << eb;
    _out << eb;

    _out << sp << nl << "#endregion"; // <name> members

    _out << sp << nl << "#region ICloneable members";

    _out << sp << nl << "public object Clone()";
    _out << sb;
    _out << nl << name << " d = new " << name << "();";
    _out << nl << "foreach(_System.Collections.Generic.KeyValuePair<" << ks << ", " << vs <<"> e in dict_)";
    _out << sb;
    _out << nl << "d.dict_.Add(e.Key, e.Value);";
    _out << eb;
    _out << nl << "return d;";
    _out << eb;

    _out << sp << nl << "#endregion"; // ICloneable members

    _out << eb;
#endif
}

void
Slice::Gen::TypesVisitor::visitEnum(const EnumPtr& p)
{
    const string scope = p->scope();
    const string localScope = getLocalScope(scope);
    const string name = fixId(p->name());
    const string baseRef = "__ice_EnumBase";

    _out << sp;
    writeDocComment(p, getDeprecateReason(p, 0, "type"));
    _out << nl << localScope << '.' << name << " = function(_n, _v)";
    _out << sb;
    _out << nl << baseRef << ".call" << spar << "this" << "_n" << "_v" << epar << ';';
    _out << eb;
    _out << nl << localScope << '.' << name << ".prototype = new " << baseRef << "();";
    _out << nl << localScope << '.' << name << ".prototype.constructor = " << localScope << '.' << name << ';';

    const EnumeratorList enumerators = p->getEnumerators();
    for(EnumeratorList::const_iterator en = enumerators.begin(); en != enumerators.end(); ++en)
    {
        _out << nl << "Object.defineProperty(" << localScope << '.' << name << ", '" << fixId((*en)->name())
             << "', {";
        _out.inc();
        _out << nl << "enumerable: true,"
             << nl << "value: new " << localScope << '.'
             << name << "(\"" << (*en)->name() << "\", " << (*en)->value() << ')';
        _out.dec();
        _out << nl << "});";
    }

    //
    // EnumBase provides an equals() method
    //
}

void
Slice::Gen::TypesVisitor::visitConst(const ConstPtr& p)
{
    const string scope = p->scope();
    const string localScope = getLocalScope(scope);
    const string name = fixId(p->name());

    _out << sp;
    _out << nl << "Object.defineProperty(" << localScope << ", '" << name << "', {";
    _out.inc();
    _out << nl << "value: ";
    writeConstantValue(scope, p->type(), p->valueType(), p->value());
    _out.dec();
    _out << nl << "});";
}

void
Slice::Gen::TypesVisitor::writeMemberHashCode(const string& m, const TypePtr& type, int iter)
{
    // TODO: Check for optional?

    const BuiltinPtr b = BuiltinPtr::dynamicCast(type);

    if(b)
    {
        switch(b->kind())
        {
            case Builtin::KindBool:
            {
                _out << nl << "__h = __ice_HashUtil.addBoolean(__h, " << m << ");";
                break;
            }
            case Builtin::KindByte:
            case Builtin::KindShort:
            case Builtin::KindInt:
            case Builtin::KindLong:
            case Builtin::KindFloat:
            case Builtin::KindDouble:
            {
                _out << nl << "__h = __ice_HashUtil.addNumber(__h, " << m << ");";
                break;
            }
            case Builtin::KindString:
            {
                _out << nl << "__h = __ice_HashUtil.addString(__h, " << m << ");";
                break;
            }
            case Builtin::KindObjectProxy:
            {
                _out << nl << "__h = __ice_HashUtil.addHashable(__h, " << m << ");";
                break;
            }
            case Builtin::KindObject:
            case Builtin::KindLocalObject:
            default:
            {
                // TODO: Hash objects?
                break;
            }
        }
        return;
    }

    if(ClassDeclPtr::dynamicCast(type))
    {
        // TODO: Hash objects?
        return;
    }

    if(ProxyPtr::dynamicCast(type) || EnumPtr::dynamicCast(type) || StructPtr::dynamicCast(type) ||
       DictionaryPtr::dynamicCast(type))
    {
        _out << nl << "__h = __ice_HashUtil.addHashable(__h, " << m << ");";
        return;
    }

    const SequencePtr seq = SequencePtr::dynamicCast(type);
    assert(seq);

    const TypePtr elemType = seq->type();

    const BuiltinPtr eb = BuiltinPtr::dynamicCast(elemType);
    if(eb)
    {
        switch(eb->kind())
        {
            case Builtin::KindBool:
            {
                _out << nl << "__h = __ice_HashUtil.addBooleanArray(__h, " << m << ");";
                break;
            }
            case Builtin::KindByte:
            case Builtin::KindShort:
            case Builtin::KindInt:
            case Builtin::KindLong:
            case Builtin::KindFloat:
            case Builtin::KindDouble:
            {
                _out << nl << "__h = __ice_HashUtil.addNumberArray(__h, " << m << ");";
                break;
            }
            case Builtin::KindString:
            {
                _out << nl << "__h = __ice_HashUtil.addStringArray(__h, " << m << ");";
                break;
            }
            case Builtin::KindObjectProxy:
            {
                _out << nl << "__h = __ice_HashUtil.addHashableArray(__h, " << m << ");";
                break;
            }
            case Builtin::KindObject:
            case Builtin::KindLocalObject:
            default:
            {
                // TODO: Hash objects?
                break;
            }
        }
        return;
    }

    if(ProxyPtr::dynamicCast(elemType) || EnumPtr::dynamicCast(elemType) || StructPtr::dynamicCast(elemType) ||
       DictionaryPtr::dynamicCast(elemType))
    {
        _out << nl << "__h = __ice_HashUtil.addHashableArray(__h, " << m << ");";
        return;
    }

    if(ClassDeclPtr::dynamicCast(elemType))
    {
        // TODO: Hash objects?
        return;
    }

    _out << nl << "if(" << m << " !== null)";
    _out << sb;
    ostringstream ostr;
    ostr << "__i" << iter;
    string i = ostr.str();
    _out << nl << "for(var " << i << " = 0; " << i << " < " << m << ".length; ++" << i << ')';
    _out << sb;
    writeMemberHashCode(m + "[" + i + "]", elemType, iter + 1);
    _out << eb;
}

void
Slice::Gen::TypesVisitor::writeMemberEquals(const string& m1, const string& m2, const TypePtr& type, int iter)
{
    const BuiltinPtr b = BuiltinPtr::dynamicCast(type);

    if((b && b->kind() != Builtin::KindObjectProxy) || ClassDeclPtr::dynamicCast(type))
    {
        _out << nl << "if(" << m1 << " !== " << m2 << ')';
        _out << sb;
        _out << nl << "return false;";
        _out << eb;
        return;
    }

    if((b && b->kind() == Builtin::KindObjectProxy) || ProxyPtr::dynamicCast(type) ||
       EnumPtr::dynamicCast(type) || StructPtr::dynamicCast(type) ||
       DictionaryPtr::dynamicCast(type))
    {
        _out << nl << "if(" << m1 << " !== " << m2 << " && !" << m1 << ".equals(" << m2 << "))";
        _out << sb;
        _out << nl << "return false;";
        _out << eb;
        return;
    }

    const SequencePtr seq = SequencePtr::dynamicCast(type);
    assert(seq);

    const TypePtr elemType = seq->type();

    _out << nl << "if(" << m1 << " !== " << m2 << " && " << m1 << " !== null && " << m2 << " !== null)";
    _out << sb;
    _out << nl << "if(" << m1 << ".length !== " << m2 << ".length)";
    _out << sb;
    _out << "return false;";
    _out << eb;
    ostringstream ostr;
    ostr << "__i" << iter;
    string i = ostr.str();
    _out << nl << "for(var " << i << " = 0; " << i << " < " << m1 << ".length; ++" << i << ')';
    _out << sb;
    writeMemberEquals(m1 + "[" + i + "]", m2 + "[" + i + "]", elemType, iter + 1);
    _out << eb;
    _out << eb;
}

void
Slice::Gen::TypesVisitor::writeMemberClone(const string& dest, const string& src, const TypePtr& type, int iter)
{
    if(SequencePtr::dynamicCast(type))
    {
        _out << nl << dest << " = __ice_ArrayUtil.clone(" << src << ");";
        return;
    }

    if(DictionaryPtr::dynamicCast(type))
    {
        _out << nl << "if(" << src << " !== undefined && " << src << " !== null)";
        _out << sb;
        _out << nl << dest << " = " << src << ".clone();";
        _out << eb;
        _out << nl << "else";
        _out << sb;
        _out << nl << dest << " = " << src << ';';
        _out << eb;
        return;
    }

    _out << nl << dest << " = " << src << ';';
}

Slice::Gen::ExportVisitor::ExportVisitor(IceUtilInternal::Output& out)
    : JsVisitor(out)
{
}

bool
Slice::Gen::ExportVisitor::visitModuleStart(const ModulePtr& p)
{
    const string name = fixId(p->name());

    //
    // Only consider top-level modules (i.e., a module whose container is a Unit, not another Module).
    //
    if(UnitPtr::dynamicCast(p->container()))
    {
        if(find(_modules.begin(), _modules.end(), name) == _modules.end())
        {
            _modules.push_back(name);
        }
    }

    return false;
}

void
Slice::Gen::ExportVisitor::writeExports()
{
    if(!_modules.empty())
    {
        _out << sp;
        for(StringList::const_iterator q = _modules.begin(); q != _modules.end(); ++q)
        {
            _out << nl << "module.exports." << *q << " = " << *q << ';';
        }
    }
}
