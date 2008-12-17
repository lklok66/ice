// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This plug-in is provided to you under the terms and conditions
// of the Eclipse Public License Version 1.0 ("EPL"). A copy of
// the EPL is available at http://www.eclipse.org/legal/epl-v10.html.
//
// **********************************************************************

package com.zeroc.com.slice2javaplugin.internal;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.StringReader;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;

import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.transform.OutputKeys;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerConfigurationException;
import javax.xml.transform.TransformerException;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;

import org.eclipse.core.filesystem.EFS;
import org.eclipse.core.filesystem.IFileStore;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Status;
import org.eclipse.ui.console.MessageConsoleStream;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.w3c.dom.Text;
import org.xml.sax.SAXException;

import com.zeroc.slice2javaplugin.Activator;

public class Dependencies
{
    public Dependencies(IProject project, Set<IFile> LprojectResources, MessageConsoleStream err)
    {
        _project = project;
        _projectResources = LprojectResources;
        _err = err;
        
        // Build a map of location to project resource.
        
        for(Iterator<IFile> p = _projectResources.iterator(); p.hasNext();)
        {
            IFile f = p.next();
            _locationToResource.put(f.getLocation(), f);
        }
    }

    /**
     *
     * @param allDependencies The string of all dependencies.
     * @param _projectResources a set of all slice file project resources.
     * @throws CoreException
     */
    public void updateDependencies(String allDependencies)
        throws CoreException
    {
        try
        {
            BufferedReader in = new BufferedReader(new StringReader(allDependencies));
            StringBuffer depline = new StringBuffer();
            String line;
            boolean warning = false;

            while((line = in.readLine()) != null)
            {
                if(line.length() == 0)
                {
                    continue;
                }
                // Warnings start with a line contain ": warning:" and end when
                // a new line is encountered starting with non-whitespace.
                if(warning)
                {
                    if(Character.isWhitespace(line.charAt(0)))
                    {
                        continue;
                    }
                    warning = false;
                }
                if(line.contains(": warning:"))
                {
                    warning = true;
                    continue;
                }
                
                if(line.endsWith("\\"))
                {
                    depline.append(line.substring(0, line.length() - 1));
                }
                else
                {
                    depline.append(line);

                    //
                    // It's easier to split up the filenames if we first convert
                    // Windows path separators into Unix path separators.
                    //
                    char[] chars = depline.toString().toCharArray();
                    int pos = 0;
                    while(pos < chars.length)
                    {
                        if(chars[pos] == '\\')
                        {
                            if(pos + 1 < chars.length)
                            {
                                //
                                // Only convert the backslash if it's not an
                                // escape.
                                //
                                if(chars[pos + 1] != ' ' && chars[pos + 1] != ':' && chars[pos + 1] != '\r'
                                        && chars[pos + 1] != '\n')
                                {
                                    chars[pos] = '/';
                                }
                            }
                        }
                        ++pos;
                    }

                    //
                    // Split the dependencies up into filenames. Note that
                    // filenames containing spaces are escaped and the initial
                    // file may have escaped colons (e.g., "C\:/Program\
                    // Files/...").
                    //
                    java.util.ArrayList<String> l = new java.util.ArrayList<String>();
                    StringBuffer file = new StringBuffer();
                    pos = 0;
                    while(pos < chars.length)
                    {
                        if(Character.isWhitespace(chars[pos]))
                        {
                            if(file.length() > 0)
                            {
                                l.add(file.toString());
                                file = new StringBuffer();
                            }
                        }
                        else if(chars[pos] != '\\') // Skip backslash of an
                        // escaped character.
                        {
                            file.append(chars[pos]);
                        }
                        ++pos;
                    }
                    if(file.length() > 0)
                    {
                        l.add(file.toString());
                    }

                    // Remove the trailing ':'
                    String source = l.remove(0);

                    pos = source.lastIndexOf(':');
                    assert (pos == source.length() - 1);
                    Path sourcePath = new Path(source.substring(0, pos));
                    assert sourcePath.isAbsolute();

                    IFile sourceFile = _locationToResource.get(sourcePath);
                    if(sourceFile == null)
                    {
                        if(_err != null)
                        {
                            _err.println("Dependencies: ignoring non-project resource " + sourcePath.toString());
                        }
                        // This should not occur.
                        continue;
                    }

                    Iterator<String> p = l.iterator();
                    while(p.hasNext())
                    {
                        IFile f = getProjectResource(new Path(p.next()));
                        // Ignore any resources not in the project.
                        if(f != null)
                        {
                            Set<IFile> dependents = reverseSliceSliceDependencies.get(f);
                            if(dependents == null)
                            {
                                dependents = new HashSet<IFile>();
                                reverseSliceSliceDependencies.put(f, dependents);
                            }
                            dependents.add(sourceFile);
                        }
                    }

                    Set<IFile> dependents = new HashSet<IFile>();
                    sliceSliceDependencies.put(sourceFile, dependents);
                    p = l.iterator();
                    while(p.hasNext())
                    {
                        IFile f = getProjectResource(new Path(p.next()));
                        // Ignore any resources not in the project.
                        if(f != null)
                        {
                            dependents.add(f);
                        }
                    }

                    depline = new StringBuffer();
                }
            }
        }
        catch(java.io.IOException ex)
        {
            throw new CoreException(new Status(IStatus.ERROR, Activator.PLUGIN_ID,
                    "Unable to read dependencies from slice translator: " + ex, null));
        }
    }

    private IFile getProjectResource(Path path)
    {
        IFile f;
        if(path.isAbsolute())
        {
            f = _locationToResource.get(path);
        }
        else
        {                            
            f = _project.getFile(path);
        }
        if(_projectResources.contains(f))
        {
            return f;
        }
        return null;
    }

    public void read()
        throws CoreException
    {
        IFileStore dependencies = getDependenciesStore();
        if(!dependencies.fetchInfo(EFS.NONE, null).exists())
        {
            return;
        }
        InputStream in = dependencies.openInputStream(EFS.NONE, null);

        try
        {
            Document doc = DocumentBuilderFactory.newInstance().newDocumentBuilder().parse(new BufferedInputStream(in));
            DependenciesParser parser = new DependenciesParser(_project);
            parser.visit(doc);
            sliceSliceDependencies = parser.sliceSliceDependencies;
            reverseSliceSliceDependencies = parser.reverseSliceSliceDependencies;
            sliceJavaDependencies = parser.sliceJavaDependencies;
            errorSliceFiles = parser.errorSliceFiles;
        }
        catch(SAXException e)
        {
            throw new CoreException(new Status(IStatus.ERROR, Activator.PLUGIN_ID,
                    "internal error reading dependencies", e));
        }
        catch(ParserConfigurationException e)
        {
            throw new CoreException(new Status(IStatus.ERROR, Activator.PLUGIN_ID,
                    "internal error reading dependencies", e));
        }
        catch(IOException e)
        {
            throw new CoreException(new Status(IStatus.ERROR, Activator.PLUGIN_ID,
                    "internal error reading dependencies", e));
        }
    }

    public void write()
        throws CoreException
    {
        // Create a DOM of the map.
        Document doc = null;
        try
        {
            doc = DocumentBuilderFactory.newInstance().newDocumentBuilder().newDocument();
        }
        catch(ParserConfigurationException e)
        {
            throw new CoreException(new Status(IStatus.ERROR, Activator.PLUGIN_ID,
                    "internal error writing dependencies", e));
        }

        Element root = doc.createElement("dependencies");
        doc.appendChild(root);

        writeDependencies(sliceSliceDependencies, doc, "sliceSliceDependencies", root);
        writeDependencies(reverseSliceSliceDependencies, doc, "reverseSliceSliceDependencies", root);
        writeDependencies(sliceJavaDependencies, doc, "sliceJavaDependencies", root);
        writeErrorSliceFiles(errorSliceFiles, doc, "errorSliceFiles", root);

        // Write the DOM to the dependencies.xml file.
        TransformerFactory transfac = TransformerFactory.newInstance();
        Transformer trans = null;
        try
        {
            trans = transfac.newTransformer();
        }
        catch(TransformerConfigurationException e)
        {
            throw new CoreException(new Status(IStatus.ERROR, Activator.PLUGIN_ID,
                    "internal error writing dependencies", e));
        }
        // tf.setAttribute("indent-number", 4);

        // trans.setOutputProperty(OutputKeys.OMIT_XML_DECLARATION, "yes");
        trans.setOutputProperty(OutputKeys.INDENT, "yes");
        trans.setOutputProperty(OutputKeys.ENCODING, "UTF8");
        trans.setOutputProperty(OutputKeys.INDENT, "yes");
        trans.setOutputProperty(OutputKeys.METHOD, "XML");

        IFileStore dependencies = getDependenciesStore();
        OutputStream out = dependencies.openOutputStream(EFS.NONE, null);
        StreamResult result = new StreamResult(new BufferedOutputStream(out));
        DOMSource source = new DOMSource(doc);
        try
        {
            trans.transform(source, result);
        }
        catch(TransformerException e)
        {
            throw new CoreException(new Status(IStatus.ERROR, Activator.PLUGIN_ID,
                    "internal error writing dependencies", e));
        }
        try
        {
            out.close();
        }
        catch(IOException e)
        {
            throw new CoreException(new Status(IStatus.ERROR, Activator.PLUGIN_ID,
                    "internal error writing dependencies", e));
        }
    }

    private void writeErrorSliceFiles(Set<IFile> s, Document doc, String name, Element root)
    {
        Element jsd = doc.createElement(name);
        root.appendChild(jsd);

        for(IFile f : s)
        {
            Element elem = doc.createElement("file");
            jsd.appendChild(elem);
            Text text = doc.createTextNode(f.getProjectRelativePath().toString());
            elem.appendChild(text);
        }
    }

    private void writeDependencies(Map<IFile, Set<IFile>> map, Document doc, String name, Element root)
    {
        Element jsd = doc.createElement(name);
        root.appendChild(jsd);

        Iterator<Map.Entry<IFile, Set<IFile>>> p = map.entrySet().iterator();
        while(p.hasNext())
        {
            Map.Entry<IFile, Set<IFile>> e = p.next();
            Element entry = doc.createElement("entry");
            jsd.appendChild(entry);

            Element key = doc.createElement("key");
            entry.appendChild(key);
            Text text = doc.createTextNode(e.getKey().getProjectRelativePath().toString());
            key.appendChild(text);

            Element value = doc.createElement("value");
            entry.appendChild(value);

            Iterator<IFile> q = e.getValue().iterator();
            while(q.hasNext())
            {
                IFile f = q.next();
                Element elem = doc.createElement("file");
                value.appendChild(elem);
                text = doc.createTextNode(f.getProjectRelativePath().toString());
                elem.appendChild(text);
            }
        }
    }

    private IFileStore getDependenciesStore()
        throws CoreException
    {
        IPath name = new Path(_project.getName());
        IFileStore store = EFS.getLocalFileSystem().getStore(Activator.getDefault().getStateLocation()).getFileStore(
                name);
        if(!store.fetchInfo(EFS.NONE, null).exists())
        {
            store.mkdir(EFS.NONE, null);
        }
        return store.getFileStore(new Path("dependencies.xml"));
    }

    static class DependenciesParser
    {
        private IProject _project;

        Map<IFile, Set<IFile>> sliceSliceDependencies = new java.util.HashMap<IFile, Set<IFile>>();
        Map<IFile, Set<IFile>> reverseSliceSliceDependencies = new java.util.HashMap<IFile, Set<IFile>>();
        Map<IFile, Set<IFile>> sliceJavaDependencies = new java.util.HashMap<IFile, Set<IFile>>();
        Set<IFile> errorSliceFiles = new java.util.HashSet<IFile>();

        private Node findNode(Node n, String qName)
            throws SAXException
        {
            NodeList children = n.getChildNodes();
            for(int i = 0; i < children.getLength(); ++i)
            {
                Node child = children.item(i);
                if(child.getNodeType() == Node.ELEMENT_NODE && child.getNodeName().equals(qName))
                {
                    return child;
                }
            }
            throw new SAXException("no such node: " + qName);
        }

        private String getText(Node n)
            throws SAXException
        {
            NodeList children = n.getChildNodes();
            if(children.getLength() == 1 && children.item(0).getNodeType() == Node.TEXT_NODE)
            {
                return children.item(0).getNodeValue();
            }
            throw new SAXException("no text element");
        }

        private List<String> processFiles(Node n)
            throws SAXException
        {
            List<String> files = new ArrayList<String>();
            NodeList children = n.getChildNodes();
            for(int i = 0; i < children.getLength(); ++i)
            {
                Node child = children.item(i);
                if(child.getNodeType() == Node.ELEMENT_NODE && child.getNodeName().equals("file"))
                {
                    files.add(getText(child));
                }
            }
            return files;
        }

        public void visitDependencies(Map<IFile, Set<IFile>> map, Node n)
            throws SAXException
        {
            NodeList children = n.getChildNodes();
            for(int i = 0; i < children.getLength(); ++i)
            {
                Node child = children.item(i);
                if(child.getNodeType() == Node.ELEMENT_NODE && child.getNodeName().equals("entry"))
                {
                    IFile key = _project.getFile(new Path(getText(findNode(child, "key"))));

                    Node value = findNode(child, "value");
                    List<String> files = processFiles(value);
                    Set<IFile> f = new HashSet<IFile>();
                    Iterator<String> p = files.iterator();
                    while(p.hasNext())
                    {
                        f.add(_project.getFile(new Path(p.next())));
                    }

                    map.put(key, f);
                }
            }
        }
        
        public void visitErrorList(Set<IFile> s, Node n) throws SAXException
        {
            NodeList children = n.getChildNodes();
            for(int i = 0; i < children.getLength(); ++i)
            {
                Node child = children.item(i);
                if(child.getNodeType() == Node.ELEMENT_NODE && child.getNodeName().equals("file"))
                {
                    s.add(_project.getFile(new Path(getText(child))));
                }
            }
        }

        public void visit(Node doc)
            throws SAXException
        {
            Node dependencies = findNode(doc, "dependencies");
            visitDependencies(sliceSliceDependencies, findNode(dependencies, "sliceSliceDependencies"));
            visitDependencies(reverseSliceSliceDependencies, findNode(dependencies, "reverseSliceSliceDependencies"));
            visitDependencies(sliceJavaDependencies, findNode(dependencies, "sliceJavaDependencies"));
            try
            {
                visitErrorList(errorSliceFiles, findNode(dependencies, "errorSliceFiles"));
            }
            catch(SAXException e)
            {
                // Optional.
            }
        }

        DependenciesParser(IProject project)
        {
            _project = project;
        }
    }

    // A map of slice to dependencies.
    //
    // sliceSliceDependencies is the set of slice files that depend on the IFile
    // (the output of slice2java --depend).
    //
    // _reverseSliceSliceDependencies is the reverse.
    public Map<IFile, Set<IFile>> sliceSliceDependencies = new java.util.HashMap<IFile, Set<IFile>>();
    public Map<IFile, Set<IFile>> reverseSliceSliceDependencies = new java.util.HashMap<IFile, Set<IFile>>();

    // A map of slice file to java source files.
    public Map<IFile, Set<IFile>> sliceJavaDependencies = new java.util.HashMap<IFile, Set<IFile>>();
    
    // A set of slice files that have not, or cannot be built.
    public Set<IFile> errorSliceFiles = new java.util.HashSet<IFile>();

    private IProject _project;
    private MessageConsoleStream _err;
    private Set<IFile> _projectResources;
    private Map<IPath, IFile> _locationToResource = new HashMap<IPath, IFile>();
}
