#!/usr/bin/env python
# **********************************************************************
#
# Copyright (c) 2003-2007 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

import os, sys, shutil, re, signal, time, string, pprint

from xml.sax import make_parser
from xml.sax.handler import feature_namespaces
from xml.sax.handler import ContentHandler
from xml.sax import saxutils
from xml.sax import SAXException

from xml.dom.minidom import parse

progname = os.path.basename(sys.argv[0])
contentHandler = None
propertyClasses = {}

commonPreamble = """// **********************************************************************
//
// Copyright (c) 2003-2007 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************
//
""" 
commonPreamble = commonPreamble + "// Generated by " + progname + " from file %(inputfile)s, " + time.ctime()
commonPreamble = commonPreamble + """

// IMPORTANT: Do not edit this file -- any edits made here will be lost!
"""

cppHeaderPreamble = commonPreamble + """
#ifndef ICE_INTERNAL_%(classname)s_H
#define ICE_INTERNAL_%(classname)s_H

#include <Ice/Config.h>

namespace IceInternal
{

struct Property
{
    const char* pattern;
    bool deprecated;
    const char* deprecatedBy;

    Property(const char* n, bool d, const char* b) :
	pattern(n),
	deprecated(d),
	deprecatedBy(b)
    {
    }

    Property() :
        pattern(0),
        deprecated(false),
        deprecatedBy(0)
    {
    }

};

struct PropertyArray
{
    const Property* properties;
    const int length;

    PropertyArray(const Property* p, int len) :
        properties(p),
        length(len)
    {
    }
};

class %(classname)s
{
public:

"""

cppHeaderPostamble = """
    static const PropertyArray validProps[];
    static const char * clPropNames[];
};

}

#endif
"""

cppSrcPreamble = commonPreamble + """
#include <Ice/%(classname)s.h>

"""

javaPropertyClass = commonPreamble + """
package IceInternal;

public class Property
{
    public Property(String pattern, boolean deprecated, String deprecatedBy)
    {
        _pattern = pattern;
        _deprecated = deprecated;
        _deprecatedBy = deprecatedBy;
    }

    public String
    pattern()
    {
        return _pattern;
    }

    public boolean
    deprecated()
    {
        return _deprecated;
    }

    public String
    deprecatedBy()
    {
        return _deprecatedBy;
    }

    private String _pattern;
    private boolean _deprecated;
    private String _deprecatedBy;
}
"""

javaPreamble = commonPreamble + """
package IceInternal;

public final class %(classname)s
{
"""

csPropertyClass = commonPreamble + """
namespace IceInternal
{
    public sealed class Property
    {
        public Property(string pattern, bool deprecated, string deprecatedBy)
        {
            _pattern = pattern;
            _deprecated = deprecated;
            _deprecatedBy = deprecatedBy;
        }

        public string
        pattern()
        {
            return _pattern;
        }

        public bool
        deprecated()
        {
            return _deprecated;
        }

        public string
        deprecatedBy()
        {
            return _deprecatedBy;
        }

        private string _pattern;
        private bool _deprecated;
        private string _deprecatedBy;
    }
}
"""

csPreamble = commonPreamble + """
namespace IceInternal
{
    public sealed class %(classname)s
    {
"""

def usage():
    global progname
    print >> sys.stderr, "Usage: " + progname + " [--{cpp|java|cs} file]"

def progError(msg):
    global progname
    print >> sys.stderr, progname + ": " + msg

#
# Currently the processing of PropertyNames.def is going to take place
# in two parts. One is using DOM to extract the property 'classes' such
# as 'proxy', 'objectadapter', etc. The other part uses SAX to create
# the language mapping source code.
# 

class PropertyClass:
    def __init__(self, type, suffixes, nestedClasses):
        self.type = type
        self.suffixes = suffixes
        self.nestedClasses = nestedClasses

    def __repr__(self):
        return repr((repr(self.type), repr(self.suffixes),
            repr(self.nestedClasses)))

def expandClassLists(classTree, list):
    expansion = []
    for nc in list:
        if nc[0] == None:
            expansion.extend(classTree[nc[1]].suffixes)
        else:
            for s in classTree[nc[1]].suffixes:
                expansion.append("%s.%s" % (nc[0], s))
        expansion.extend(expandClassLists(classTree, classTree[nc[1]].nestedClasses))
    return expansion

def initPropertyClasses(filename):
    doc = parse(filename)
    propertyClassNodes = doc.getElementsByTagName('propertyClass')
    propertyClassTree = {}
    for n in propertyClassNodes:
        className = n.attributes["name"].nodeValue
        classType = n.attributes["type"].nodeValue
        classValues = []
        nestedClasses = []
        for a in n.childNodes:
            if a.localName == "suffix":
                if a.attributes.has_key("value"):
                    if a.attributes.has_key("class"):
                        nestedClasses.append((a.attributes["value"].nodeValue, a.attributes["class"].nodeValue))
                    else:
                        classValues.append(a.attributes["value"].nodeValue)
        propertyClassTree[className] = PropertyClass(classType, classValues, nestedClasses)

    #
    # resolve and expand nested property classes.
    #
    global propertyClasses
    for k in propertyClassTree.keys():
        pc = propertyClassTree[k]
        pc.suffixes.extend(expandClassLists(propertyClassTree, pc.nestedClasses))
        propertyClasses[k] = (pc.type, pc.suffixes)

#
# SAX part.
#

def handler(signum, frame):
    """Installed as signal handler. Should cause an files that are in
    use to be closed and removed"""
    global contentHandler
    contentHandler.cleanup()
    sys.exit(128 + signum)

class UnknownElementException(Exception):
    def __init__(self, value):
        self.value = value

    def __str__(self):
        return repr(self.value)

class PropertyHandler(ContentHandler):

    def __init__(self, inputfile, className):
        self.start = False
        self.properties = {}
        self.inputfile = inputfile
        self.className = className
        self.currentSection = None
        self.sectionPropertyCount = 0
        self.sections = []
        self.cmdLineOptions = []

    def cleanup(self):
        """Needs to be overridden in derived class"""
        pass

    def startFiles(self):
        """Needs to be overridden in derived class"""
        pass

    def closeFiles(self):
        """Needs to be overridden in derived class"""
        pass

    def deprecatedImpl(self, propertyName):
        """Needs to be overridden in derived class"""
        pass

    def deprecatedImplWithReplacementImpl(self, propertyName, deprecatedBy):
        """Needs to be overridden in derived class"""
        pass

    def propertyImpl(self, propertyName):
        """Needs to be overridden in derived class"""
        pass

    def newSection(self, sectionName):
        """Needs to be overridden in derived class"""
        pass

    def moveFiles(self, location):
        """Needs to be overridden in derived class"""
        pass

    def handleNewSection(self, sectionName, cmdLine):
        self.currentSection = sectionName
        self.sectionPropertyCount = 0
        if cmdLine == "true":
            self.cmdLineOptions.append(sectionName)
        self.sections.append(sectionName)
        self.newSection()
        
    def handleDeprecated(self, propertyName):
        self.properties[propertyName] = None
        self.deprecatedImpl(propertyName)
        
    def handleDeprecatedWithReplacement(self, propertyName, deprecatedBy):
        self.properties[propertyName] = deprecatedBy
        self.deprecatedImplWithReplacementImpl(propertyName, deprecatedBy)

    def handleProperty(self, propertyName):
        self.properties[propertyName] = ""
        self.propertyImpl(propertyName)

    def startElement(self, name, attrs):
        if name == "properties":
            self.start = True
            self.startFiles()
            return

        if not self.start:
            return
        
        if name == "section":
            noCmdLine = attrs.get("noCmdLine", None)
            self.handleNewSection(attrs.get("name"), noCmdLine)
        
        elif name == "property":
            propertyName = attrs.get("name", None)
            if attrs.has_key("propertyClass"):
                c = propertyClasses[attrs["propertyClass"]]
                for p in c[1]:
                    if propertyName == None:
                        self.startElement(name, { 'name': "%s" %  p})
                    else:
                        self.startElement(name, { 'name': "%s.%s" % (propertyName, p)})
                if c[0] == "placeholder":
                    return

            #
            # != None implies deprecated == true
            #
            deprecatedBy = attrs.get("deprecatedBy", None)
            if deprecatedBy != None:
                self.handleDeprecatedWithReplacement(propertyName, deprecatedBy)
                pass
            elif attrs.get("deprecated", "false").lower() == "true" :
                self.handleDeprecated(propertyName)
            else:
                self.handleProperty(propertyName)

    def endElement(self, name):
        if name == "properties":
            self.closeFiles()
        elif name == "section":
            self.closeSection()

class CppPropertyHandler(PropertyHandler):

    def __init__(self, inputfile, c):
        PropertyHandler.__init__(self, inputfile, c)
        self.hFile = None
        self.cppFile = None

    def cleanup(self):
        if self.hFile != None:
            self.hFile.close()
            if os.path.exists(self.className + ".h"):
                os.remove(self.className + ".h")
        if self.cppFile != None:
            self.cppFile.close()
            if os.path.exists(self.className + ".cpp"):
                os.remove(self.className + ".cpp")

    def startFiles(self):
        self.hFile = open(self.className + ".h", "w")
        self.cppFile = open(self.className + ".cpp", "w")
        self.hFile.write(cppHeaderPreamble % {'inputfile' : self.inputfile, 'classname' : self.className})
        self.cppFile.write(cppSrcPreamble % {'inputfile' : self.inputfile, 'classname' : self.className})

    def closeFiles(self):
        self.hFile.write(cppHeaderPostamble % {'classname' : self.className})
        self.cppFile.write("\nconst IceInternal::PropertyArray "\
                        "IceInternal::%(classname)s::validProps[] =\n" % \
                {'classname' : self.className})

        self.cppFile.write("{\n")
        for s in self.sections:
            self.cppFile.write("    %sProps,\n" % s)
        self.cppFile.write("    IceInternal::PropertyArray(0,0)\n");
        self.cppFile.write("};\n\n")

        self.cppFile.write("\nconst char* IceInternal::%(classname)s::clPropNames[] =\n" % \
                {'classname' : self.className})
        self.cppFile.write("{\n")
        for s in self.sections:
            self.cppFile.write("    \"%s\",\n" % s)
        self.cppFile.write("    0\n")
        self.cppFile.write("};\n\n")
        self.hFile.close()
        self.cppFile.close()

    def fix(self, propertyName):
        return string.replace(propertyName, "[any]", "*")

    def deprecatedImpl(self, propertyName):
        self.cppFile.write("    IceInternal::Property(\"%s.%s\", true, 0),\n" % (self.currentSection, \
                self.fix(propertyName)))

    def deprecatedImplWithReplacementImpl(self, propertyName, deprecatedBy):
        self.cppFile.write("    IceInternal::Property(\"%s.%s\", true, \"%s\"),\n" % (self.currentSection, \
                self.fix(propertyName), deprecatedBy))

    def propertyImpl(self, propertyName):
        self.cppFile.write("    IceInternal::Property(\"%s.%s\", false, 0),\n" % \
                (self.currentSection, self.fix(propertyName)))

    def newSection(self):
        self.hFile.write("    static const PropertyArray %sProps;\n" % self.currentSection)
        self.cppFile.write("const IceInternal::Property %sPropsData[] = \n" % self.currentSection)
        self.cppFile.write("{\n")
        
    def closeSection(self):
        self.cppFile.write("};\n")
        self.cppFile.write("""
const IceInternal::PropertyArray
    IceInternal::%(className)s::%(section)sProps(%(section)sPropsData,
                                                sizeof(%(section)sPropsData)/sizeof(%(section)sPropsData[0]));

""" % { 'className' : self.className, 'section': self.currentSection })

    def moveFiles(self, location):
        shutil.move(self.className + ".h", os.path.join(location, "src", "Ice"))
        shutil.move(self.className + ".cpp", os.path.join(location, "src", "Ice"))

class JavaPropertyHandler(PropertyHandler):
    def __init__(self, inputfile, c):
        PropertyHandler.__init__(self, inputfile, c)
        self.srcFile = None

    def cleanup(self):
        if self.srcFile != None:
            self.srcFile.close()
            if os.path.exists(self.className + ".java"):
                os.remove(self.className + ".java")
        if os.path.exists("Property.java"):
            os.remove("Property.java")

    def startFiles(self):
        self.srcFile = file(self.className + ".java", "w")
        self.srcFile.write(javaPreamble % {'inputfile' : self.inputfile, 'classname' : self.className})
        propertyClassFile = file("Property.java", "w")
        propertyClassFile.write(javaPropertyClass)
        propertyClassFile.close()

    def closeFiles(self):
        self.srcFile.write("\n   public static final Property[] validProps[] = \n" % \
                {'classname' : self.className})

        self.srcFile.write("    {\n")
        for s in self.sections:
            self.srcFile.write("        %sProps,\n" % s)
        self.srcFile.write("        null\n")
        self.srcFile.write("    };\n\n")

        self.srcFile.write("\n   public static final String clPropNames[] =\n" % \
                {'classname' : self.className})
        self.srcFile.write("    {\n")
        for s in self.sections:
            self.srcFile.write("        \"%s\",\n" % s)
        self.srcFile.write("        null\n")
        self.srcFile.write("    };\n\n")
        self.srcFile.write("};\n\n")
        self.srcFile.close()

    def fix(self, propertyName):
        #
        # The Java property strings are actually regexp's that will be passed to Java's regexp facitlity.
        #
        propertyName = string.replace(propertyName, ".", "\\\\.")
        return string.replace(propertyName, "[any]", "[^\\\\s]+")

    def deprecatedImpl(self, propertyName):
        self.srcFile.write("        new Property(\"%(section)s\\\\.%(pattern)s\", " \
                "true, null),\n" % \
                {"section" : self.currentSection, "pattern": self.fix(propertyName)})

    def deprecatedImplWithReplacementImpl(self, propertyName, deprecatedBy):
        self.srcFile.write("        new Property(\"%(section)s\\\\.%(pattern)s\", "\
                "true, \"%(deprecatedBy)s\"),\n"  % \
                {"section" : self.currentSection, "pattern": self.fix(propertyName),
                    "deprecatedBy" : deprecatedBy})

    def propertyImpl(self, propertyName):
        self.srcFile.write("        new Property(\"%(section)s\\\\.%(pattern)s\", " \
                "false, null),\n" % \
                {"section" : self.currentSection, "pattern": self.fix(propertyName)} )

    def newSection(self):
        self.srcFile.write("    public static final Property %sProps[] = \n" % self.currentSection)
        self.srcFile.write("    {\n")
            
    def closeSection(self):
        self.srcFile.write("        null\n")
        self.srcFile.write("    };\n\n")

    def moveFiles(self, location):
        shutil.move(self.className + ".java", os.path.join(location, "..", "icej", "src", "IceInternal"))
        shutil.move("Property.java", os.path.join(location, "..", "icej", "src", "IceInternal"))

class CSPropertyHandler(PropertyHandler):
    def __init__(self, inputfile, c):
        PropertyHandler.__init__(self, inputfile, c)
        self.srcFile = None

    def cleanup(self):
        if self.srcFile != None:
            self.srcFile.close()
            if os.path.exists(self.className + ".cs"):
                os.remove(self.className + ".cs")
        if os.path.exists("Property.cs"):
            os.remove("Property.cs")

    def startFiles(self):
        self.srcFile = file(self.className + ".cs", "w")
        self.srcFile.write(csPreamble % {'inputfile' : self.inputfile, 'classname' : self.className})
        propertyClassFile = file("Property.cs", "w")
        propertyClassFile.write(csPropertyClass)
        propertyClassFile.close()

    def closeFiles(self):
        self.srcFile.write("        public static Property[][] validProps = \n" % \
                {'classname' : self.className})

        self.srcFile.write("        {\n")
        for s in self.sections:
            self.srcFile.write("            %sProps,\n" % s)
        self.srcFile.write("            null\n")
        self.srcFile.write("        };\n\n")

        self.srcFile.write("        public static string[] clPropNames =\n" % \
                {'classname' : self.className})
        self.srcFile.write("        {\n")
        for s in self.sections:
            self.srcFile.write("            \"%s\",\n" % s)
        self.srcFile.write("            null\n")
        self.srcFile.write("        };\n")
        self.srcFile.write("    }\n")
        self.srcFile.write("}\n")
        self.srcFile.close()

    def fix(self, propertyName):
        propertyName = string.replace(propertyName, ".", "\\.")
        return string.replace(propertyName, "[any]", "[^\\s]+")

    def deprecatedImpl(self, propertyName):
        self.srcFile.write("             new Property(@\"^%s\.%s$\", true, null),\n" % (self.currentSection, \
                self.fix(propertyName)))

    def deprecatedImplWithReplacementImpl(self, propertyName, deprecatedBy):
        self.srcFile.write("             new Property(@\"^%s\.%s$\", true, @\"%s\"),\n" % \
                (self.currentSection, self.fix(propertyName), deprecatedBy))

    def propertyImpl(self, propertyName):
        self.srcFile.write("             new Property(@\"^%s\.%s$\", false, null),\n" % (self.currentSection, \
                self.fix(propertyName)))

    def newSection(self):
        self.srcFile.write("        public static Property[] %sProps =\n" % self.currentSection);
        self.srcFile.write("        {\n")
            
    def closeSection(self):
        self.srcFile.write("             null\n")
        self.srcFile.write("        };\n")
        self.srcFile.write("\n")

    def moveFiles(self, location):
        shutil.move(self.className + ".cs", os.path.join(location, "..", "icecs", "src", "Ice"))
        shutil.move("Property.cs", os.path.join(location, "..", "icecs", "src", "Ice"))

class MultiHandler(PropertyHandler):
    def __init__(self, inputfile, c):
        self.handlers = []
        PropertyHandler.__init__(self, inputfile, c)

    def cleanup(self):
        for f in self.handlers:
            f.cleanup()

    def addHandlers(self, handlers):
        self.handlers.extend(handlers)

    def startFiles(self):
        for f in self.handlers:
            f.startFiles()

    def closeFiles(self):
        for f in self.handlers:
            f.closeFiles()

    def newSection(self):
        for f in self.handlers:
            f.newSection()
            
    def closeSection(self):
        for f in self.handlers:
            f.closeSection()

    def handleNewSection(self, sectionName, cmdLine):
        for f in self.handlers:
            f.handleNewSection(sectionName, cmdLine)
        
    def handleDeprecated(self, propertyName):
        for f in self.handlers:
            f.handleDeprecated(sectionName, cmdLine)
        
    def handleDeprecatedWithReplacement(self, propertyName, deprecatedBy):
        for f in self.handlers:
            f.handleDeprecatedWithReplacement(propertyName, deprecatedBy)

    def handleProperty(self, propertyName):
        for f in self.handlers:
            f.handleProperty(propertyName)

    def moveFiles(self, location):
        for f in self.handlers:
            f.moveFiles(location)

def main():
    if len(sys.argv) != 1 and len(sys.argv) != 3:
        usage()
        sys.exit(1)

    infile = None
    lang = None
    toplevel = '..'

    #
    # TODO: Why does determining the top level directory depend on which
    # arguments are used?
    #
    if len(sys.argv) == 1:
        #
        # Find where the root of the tree is.
        #
        for toplevel in [".", "..", "../..", "../../..", "../../../.."]:
            toplevel = os.path.normpath(toplevel)
            if os.path.exists(os.path.join(toplevel, "config", "makeprops.py")):
                break
        else:
            progError("cannot find top-level directory")
            sys.exit(1)

        infile = os.path.join(toplevel, "config", "PropertyNames.xml")
    else:
        option = sys.argv[1]
        if option == "--cpp":
            lang = "cpp"
        elif option == "--java":
            lang = "java"
        elif option == "--cs":
            lang = "cs"
        elif option in ["-h", "--help", "-?"]:
            usage()
            sys.exit(0)
        else:
            usage()
            sys.exit(1)
        infile = sys.argv[2]

    className, ext = os.path.splitext(os.path.basename(infile))
    global contentHandler
    if lang == None:
        contentHandler = MultiHandler(infile, "")
        contentHandler.addHandlers([CppPropertyHandler(infile, className), 
            JavaPropertyHandler(infile, className),
            CSPropertyHandler(infile, className)])
    else:
        if lang == "cpp":
            contentHandler = CppPropertyHandler(infile, className)
        elif lang == "java":
            contentHandler = JavaPropertyHandler(infile, className)
        elif lang == "cs":
            contentHandler = CSPropertyHandler(infile, className)

    #
    # Install signal handler so we can remove the output files if we are interrupted.
    #
    signal.signal(signal.SIGINT, handler)
    # signal.signal(signal.SIGHUP, handler)
    signal.signal(signal.SIGTERM, handler)
    initPropertyClasses(infile)

    parser = make_parser()
    parser.setFeature(feature_namespaces, 0)
    parser.setContentHandler(contentHandler)
    pf = file(infile)
    try:
        parser.parse(pf)
        contentHandler.moveFiles(toplevel)
    except IOError, ex:
        progError(str(ex))
        contentHandler.cleanup()
    except SAXException, ex:
        progError(str(ex))
        contentHandler.cleanup()

if __name__ == "__main__":
    main()
