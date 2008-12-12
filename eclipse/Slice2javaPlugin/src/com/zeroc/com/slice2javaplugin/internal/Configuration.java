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

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

import org.eclipse.core.resources.IFolder;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.ProjectScope;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.preferences.InstanceScope;
import org.eclipse.jdt.core.IClasspathEntry;
import org.eclipse.jdt.core.IJavaProject;
import org.eclipse.jdt.core.JavaCore;
import org.eclipse.jdt.core.JavaModelException;
import org.eclipse.jface.dialogs.ErrorDialog;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.preferences.ScopedPreferenceStore;

import com.zeroc.slice2javaplugin.Activator;

public class Configuration
{
    public Configuration(IProject project)
    {
        _project = project;

        _instanceStore = new ScopedPreferenceStore(new InstanceScope(), Activator.PLUGIN_ID + "." + _project.getName());

        _store = new ScopedPreferenceStore(new ProjectScope(project), Activator.PLUGIN_ID);

        _instanceStore.setDefault(ICE_HOME_KEY, getDefaultHome());

        _store.setDefault(GENERATED_KEY, GENERATED_KEY);
        _store.setDefault(DEFINES_KEY, "");
        _store.setDefault(TIE_KEY, false);
        _store.setDefault(ICE_KEY, false);
        _store.setDefault(STREAM_KEY, false);
        _store.setDefault(META_KEY, "");
        _store.setDefault(CONSOLE_KEY, true); // XXX: Default should be false.
        _store.setDefault(SLICE_SOURCE_DIRS_KEY, "slice");
        _store.setDefault(INCLUDES_KEY, "");
    }

    /**
     * Turns list of strings into a single ';' delimited string. ';' in the
     * string values are escaped with a leading '\'. '\' are turned into '\\'.
     * 
     * @param l
     *            List of strings.
     * @return Semicolon delimited string.
     */
    static public String fromList(List<String> l)
    {
        StringBuffer sb = new StringBuffer();
        for(Iterator<String> p = l.iterator(); p.hasNext();)
        {
            if(sb.length() > 0)
            {
                sb.append(";");
            }
            sb.append(escape(p.next()));
        }
        return sb.toString();
    }

    /**
     * Turn a semicolon delimited string into a list of strings. Escaped values
     * are preserved (characters prefixed with a '\').
     * 
     * @param s
     *            Semicolon delimited string.
     * @return List of strings.
     */
    static public List<String> toList(String s)
    {
        java.util.List<String> l = new ArrayList<String>();
        int curr = 0;
        int end = s.length();
        boolean escape = false;
        StringBuffer sb = new StringBuffer();
        for(curr = 0; curr < end; ++curr)
        {
            char ch = s.charAt(curr);
            if(escape)
            {
                sb.append(ch);
                escape = false;
            }
            else
            {
                if(ch == ';')
                {
                    String tok = sb.toString().trim();
                    sb.setLength(0);
                    if(tok.length() > 0)
                    {
                        l.add(tok);
                    }
                }
                else if(ch == '\\')
                {
                    escape = true;
                }
                else
                {
                    sb.append(ch);
                }
            }
        }
        String tok = sb.toString().trim();
        if(tok.length() > 0)
        {
            l.add(tok);
        }
        return l;
    }

    public boolean write()
        throws CoreException, IOException
    {
        boolean rc = false;
        if(_store.needsSaving())
        {
            _store.save();
            rc = true;
        }
        if(_instanceStore.needsSaving())
        {
            _instanceStore.save();
            rc = true;
        }

        return rc;
    }
    
    public String getIceJar(String iceHome, BooleanValue exists)
    {
        exists.value = true;
        String os = System.getProperty("os.name");
        if(os.equals("Linux") && iceHome.equals("/usr"))
        {
            File f = new File(iceHome + File.separator + "share" + File.separator + "java" + File.separator + "Ice.jar");
            if(f.exists())
            {
                return f.toString();
            }
        }

        File f = new File(iceHome + File.separator + "lib" + File.separator + "Ice.jar");
        if(!f.exists())
        {
            File f2 = new File(iceHome + File.separator + "java" + File.separator + "lib" + File.separator + "Ice.jar");
            if(f2.exists())
            {
                return f2.toString();
            }
            exists.value = false;
        }
        // Add the platform default even if it cannot be found.
        return f.toString();
    }

    public String getIceHome()
    {
        return _instanceStore.getString(ICE_HOME_KEY);
    }

    public void setIceHome(Shell shell, String iceHome)
        throws CoreException
    {
        if(setValue(_instanceStore, ICE_HOME_KEY, iceHome))
        {
            BooleanValue exists = new BooleanValue();
            String newIceJar = getIceJar(iceHome, exists);
            IceClasspathContainerIntializer.reinitialize(_project, newIceJar);
            if(!exists.value)
            {
                ErrorDialog
                        .openError(
                                shell,
                                "Missing Resource",
                                "Cannot find Ice.jar. You must either fix the Ice installation location, or manually add Ice.jar to the java build path.",
                                new Status(Status.WARNING, Activator.PLUGIN_ID, 0, "Missing 'Ice.jar' resource in "
                                        + iceHome, null));
            }
        }
    }

    public void initialize()
        throws CoreException
    {
        // Create the slice source directories, if necessary.
        for(Iterator<String> p = getSliceSourceDirs().iterator(); p.hasNext();)
        {
            IFolder slice = _project.getFolder(p.next());
            if(!slice.exists())
            {
                slice.create(false, true, null);
            }
        }

        // Create the generated directory, if necessary.
        IFolder generated = _project.getFolder(getGeneratedDir());
        if(!generated.exists())
        {
            generated.create(false, true, null);
        }

        BooleanValue exists = new BooleanValue();
        getIceJar(getIceHome(), exists);
        if(!exists.value)
        {
            ErrorDialog
                    .openError(
                            null,
                            "Missing Resource",
                            "Cannot find Ice.jar. You must either fix the Ice installation location, or manually add Ice.jar to the java build path.",
                            new Status(Status.WARNING, Activator.PLUGIN_ID, 0, "Missing 'Ice.jar' resource in "
                                    + getIceHome(), null));
        }

        fixGeneratedCP(null, getGeneratedDir());

        IJavaProject javaProject = JavaCore.create(_project);

        IClasspathEntry cpEntry = IceClasspathContainerIntializer.getContainerEntry();
        IClasspathEntry[] entries = javaProject.getRawClasspath();

        boolean found = false;
        for(int i = 0; i < entries.length; ++i)
        {
            if(entries[i].equals(cpEntry))
            {
                found = true;
                break;
            }
        }

        if(!found)
        {
            IClasspathEntry[] newEntries = new IClasspathEntry[entries.length + 1];
            System.arraycopy(entries, 0, newEntries, 0, entries.length);
            newEntries[entries.length] = cpEntry;

            try
            {
                javaProject.setRawClasspath(newEntries, null);
            }
            catch(JavaModelException e)
            {
                throw new CoreException(new Status(IStatus.ERROR, Activator.PLUGIN_ID, e.toString(), null));
            }
        }
    }

    public void deinstall()
        throws CoreException
    {
        IJavaProject javaProject = JavaCore.create(_project);

        IClasspathEntry cpEntry = IceClasspathContainerIntializer.getContainerEntry();
        IClasspathEntry[] entries = javaProject.getRawClasspath();

        for(int i = 0; i < entries.length; ++i)
        {
            if(entries[i].equals(cpEntry))
            {
                IClasspathEntry[] newEntries = new IClasspathEntry[entries.length - 1];
                System.arraycopy(entries, 0, newEntries, 0, i);
                System.arraycopy(entries, i + 1, newEntries, i, entries.length - i - 1);

                try
                {
                    javaProject.setRawClasspath(newEntries, null);
                }
                catch(JavaModelException e)
                {
                    throw new CoreException(new Status(IStatus.ERROR, Activator.PLUGIN_ID, e.toString(), null));
                }
                break;
            }
        }
    }

    public List<String> getSliceSourceDirs()
    {
        return toList(_store.getString(SLICE_SOURCE_DIRS_KEY));
    }

    public void setSliceSourceDirs(List<String> sliceSourceDirs)
    {
        setValue(SLICE_SOURCE_DIRS_KEY, fromList(sliceSourceDirs));
    }

    public String getGeneratedDir()
    {
        return _store.getString(GENERATED_KEY);
    }

    public void fixGeneratedCP(String oldG, String newG)
        throws CoreException
    {
        IJavaProject javaProject = JavaCore.create(_project);

        IFolder newGenerated = _project.getFolder(newG);

        IClasspathEntry[] entries = javaProject.getRawClasspath();
        IClasspathEntry newEntry = JavaCore.newSourceEntry(newGenerated.getFullPath());

        if(oldG != null)
        {
            IFolder oldGenerated = _project.getFolder(oldG);
            IClasspathEntry oldEntry = JavaCore.newSourceEntry(oldGenerated.getFullPath());
            for(int i = 0; i < entries.length; ++i)
            {
                if(entries[i].equals(oldEntry))
                {
                    entries[i] = newEntry;
                    javaProject.setRawClasspath(entries, null);
                    return;
                }
            }
        }

        IClasspathEntry[] newEntries = new IClasspathEntry[entries.length + 1];
        System.arraycopy(entries, 0, newEntries, 1, entries.length);
        newEntries[0] = newEntry;

        try
        {
            javaProject.setRawClasspath(newEntries, null);
        }
        catch(JavaModelException e)
        {
            // This can occur if a duplicate CLASSPATH entry is made.
            //
            // throw new CoreException(new Status(IStatus.ERROR,
            // Activator.PLUGIN_ID, e.toString(), null));
        }
    }

    public void setGeneratedDir(String generated)
        throws CoreException
    {
        String oldGenerated = getGeneratedDir();
        if(setValue(GENERATED_KEY, generated))
        {
            fixGeneratedCP(oldGenerated, generated);
        }
    }

    public List<String> getCommandLine()
    {
        List<String> cmds = new ArrayList<String>();
        for(Iterator<String> p = getIncludes().iterator(); p.hasNext();)
        {
            cmds.add("-I" + p.next());
        }
        for(Iterator<String> p = getDefines().iterator(); p.hasNext();)
        {
            cmds.add("-D" + p.next());
        }
        for(Iterator<String> p = getMeta().iterator(); p.hasNext();)
        {
            cmds.add("--meta");
            cmds.add(p.next());
        }
        if(getStream())
        {
            cmds.add("--stream");
        }
        if(getTie())
        {
            cmds.add("--tie");
        }
        if(getIce())
        {
            cmds.add("--ice");
        }

        return cmds;
    }

    public List<String> getIncludes()
    {
        return toList(_store.getString(INCLUDES_KEY));
    }

    public void setIncludes(List<String> includes)
    {
        setValue(INCLUDES_KEY, fromList(includes));
    }

    public List<String> getDefines()
    {
        return toList(_store.getString(DEFINES_KEY));
    }

    public void setDefines(List<String> defines)
    {
        setValue(DEFINES_KEY, fromList(defines));
    }

    public boolean getStream()
    {
        return _store.getBoolean(STREAM_KEY);
    }

    public void setStream(boolean stream)
    {
        _store.setValue(STREAM_KEY, stream);
    }

    public boolean getTie()
    {
        return _store.getBoolean(TIE_KEY);
    }

    public void setTie(boolean tie)
    {
        _store.setValue(TIE_KEY, tie);
    }

    public boolean getIce()
    {
        return _store.getBoolean(ICE_KEY);
    }

    public void setIce(boolean ice)
    {
        _store.setValue(ICE_KEY, ice);
    }

    public boolean getConsole()
    {
        return _store.getBoolean(CONSOLE_KEY);
    }

    public void setConsole(boolean console)
    {
        _store.setValue(CONSOLE_KEY, console);
    }

    public List<String> getMeta()
    {
        return toList(_store.getString(META_KEY));
    }

    public void setMeta(List<String> meta)
    {
        setValue(META_KEY, fromList(meta));
    }

    public void setupSharedLibraryPath(Map<String, String> env)
    {
        String iceHome = getIceHome();

        String libPath;
        boolean srcdist = false;
        if(new File(iceHome + File.separator + "cpp" + File.separator + "bin").exists())
        {
            // iceHome points at a source distribution.
            libPath = new File(iceHome + File.separator + "cpp" + File.separator + "lib").toString();
            srcdist = true;
        }
        else
        {
            libPath = new File(iceHome + File.separator + "lib").toString();
        }

        String ldLibPathEnv = null;
        String ldLib64PathEnv = null;
        String lib64Path = null;

        String os = System.getProperty("os.name");
        if(os.equals("Mac OS X"))
        {
            ldLibPathEnv = "DYLD_LIBRARY_PATH";
        }
        else if(os.equals("AIX"))
        {
            ldLibPathEnv = "LIBPATH";
        }
        else if(os.equals("HP-UX"))
        {
            ldLibPathEnv = "SHLIB_PATH";
            ldLib64PathEnv = "LD_LIBRARY_PATH";
            if(srcdist)
            {
                lib64Path = libPath;
            }
            else
            {
                lib64Path = new File(iceHome + File.separator + "lib" + File.separator + "pa20_64").toString();
            }
        }
        else if(os.startsWith("Windows"))
        {
            //
            // No need to change the PATH environment variable on Windows, the
            // DLLs should be found
            // in the translator local directory.
            //
            // ldLibPathEnv = "PATH";
        }
        else if(os.equals("SunOS"))
        {
            ldLibPathEnv = "LD_LIBRARY_PATH";
            ldLib64PathEnv = "LD_LIBRARY_PATH_64";
            String arch = System.getProperty("os.arch");
            if(srcdist)
            {
                lib64Path = libPath;
            }
            else if(arch.equals("x86"))
            {
                lib64Path = new File(iceHome + File.separator + "lib" + File.separator + "amd64").toString();
            }
            else
            // Sparc
            {
                lib64Path = new File(iceHome + File.separator + "lib" + File.separator + "sparcv9").toString();
            }
        }
        else
        {
            ldLibPathEnv = "LD_LIBRARY_PATH";
            ldLib64PathEnv = "LD_LIBRARY_PATH";
            if(srcdist)
            {
                lib64Path = libPath;
            }
            else
            {
                lib64Path = new File(iceHome + File.separator + "lib64").toString();
            }
        }

        if(ldLibPathEnv != null)
        {
            if(ldLibPathEnv.equals(ldLib64PathEnv))
            {
                libPath = libPath + File.pathSeparator + lib64Path;
            }

            String envLibPath = env.get(ldLibPathEnv);
            if(envLibPath != null)
            {
                libPath = libPath + File.pathSeparator + envLibPath;
            }

            env.put(ldLibPathEnv, libPath);
        }

        if(ldLib64PathEnv != null && !ldLib64PathEnv.equals(ldLibPathEnv))
        {
            String envLib64Path = env.get(ldLib64PathEnv);
            if(envLib64Path != null)
            {
                lib64Path = lib64Path + File.pathSeparator + envLib64Path;
            }
            env.put(ldLib64PathEnv, lib64Path);
        }
    }

    public String getTranslator()
    {
        return getTranslatorForHome(getIceHome());
    }

    static public boolean verifyIceHome(String dir)
    {
        return getTranslatorForHome(dir) != null;
    }

    // For some reason ScopedPreferenceStore.setValue(String, String)
    // doesn't check to see whether the stored value is the same as
    // the new value.
    private boolean setValue(String key, String value)
    {
        return setValue(_store, key, value);
    }

    private boolean setValue(ScopedPreferenceStore store, String key, String value)
    {
        if(!store.getString(key).equals(value))
        {
            store.setValue(key, value);
            return true;
        }
        return false;
    }

    static private String escape(String s)
    {
        int curr = 0;
        int end = s.length();
        StringBuffer sb = new StringBuffer();
        for(curr = 0; curr < end; ++curr)
        {
            char ch = s.charAt(curr);
            if(ch == '\\' || ch == ';')
            {
                sb.append('\\');
            }
            sb.append(ch);
        }
        return sb.toString();
    }

    private static String getTranslatorForHome(String dir)
    {
        String suffix = "";
        String os = System.getProperty("os.name");
        if(os.startsWith("Windows"))
        {
            suffix = ".exe";
        }
        File f = new File(dir + File.separator + "bin" + File.separator + "slice2java" + suffix);
        if(f.exists())
        {
            return f.toString();
        }
        f = new File(dir + File.separator + "cpp" + File.separator + "bin" + File.separator + "slice2java" + suffix);
        if(f.exists())
        {
            return f.toString();
        }
        return null;
    }

    private String getDefaultHome()
    {
        String os = System.getProperty("os.name");
        if(os.equals("Linux"))
        {
            return "/usr";
        }
        else if(os.startsWith("Windows"))
        {
            File f = new File("C:\\Ice-3.3.0");
            if(!f.exists())
            {
                File f2 = new File("C:\\Ice-3.3.0-VC90");
                if(f2.exists())
                {
                    return f2.toString();
                }
            }
            return f.toString();
        }
        return "/opt/Ice-3.3.0";
    }

    static class BooleanValue
    {
        boolean value;
    }

    private static final String INCLUDES_KEY = "includes";
    private static final String SLICE_SOURCE_DIRS_KEY = "sliceSourceDirs";
    private static final String CONSOLE_KEY = "console";
    private static final String META_KEY = "meta";
    private static final String STREAM_KEY = "stream";
    private static final String ICE_KEY = "ice";
    private static final String TIE_KEY = "tie";
    private static final String DEFINES_KEY = "defines";
    private static final String GENERATED_KEY = "generated";
    private static final String ICE_HOME_KEY = "icehome";

    // Preferences store for items which should go in SCM. This includes things
    // like build flags.
    private ScopedPreferenceStore _store;

    // Preferences store per project items which should not go in SCM, such as
    // the location of the Ice installation.
    private ScopedPreferenceStore _instanceStore;

    private IProject _project;

}
