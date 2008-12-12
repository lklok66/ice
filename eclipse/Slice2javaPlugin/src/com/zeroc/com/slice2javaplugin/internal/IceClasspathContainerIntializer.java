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

import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.NullProgressMonitor;
import org.eclipse.core.runtime.Path;
import org.eclipse.jdt.core.ClasspathContainerInitializer;
import org.eclipse.jdt.core.IAccessRule;
import org.eclipse.jdt.core.IClasspathAttribute;
import org.eclipse.jdt.core.IClasspathContainer;
import org.eclipse.jdt.core.IClasspathEntry;
import org.eclipse.jdt.core.IJavaProject;
import org.eclipse.jdt.core.JavaCore;

import com.zeroc.com.slice2javaplugin.internal.Configuration.BooleanValue;

public class IceClasspathContainerIntializer extends ClasspathContainerInitializer
{
    private final static String CONTAINER_ID = "com.zeroc.Slice2javaPlugin.ICE_FRAMEWORK";

    @Override
    public void initialize(IPath containerPath, IJavaProject project)
        throws CoreException
    {
        if(containerPath.toString().equals(CONTAINER_ID))
        {
            Configuration c = new Configuration(project.getProject());
            IPath iceLib = new Path(c.getIceJar(c.getIceHome(), new BooleanValue()));
            IClasspathEntry classpathEntry = JavaCore.newLibraryEntry(iceLib, null, null, new IAccessRule[0], new IClasspathAttribute[0], false);
            IClasspathContainer container = new IceClasspathContainer(classpathEntry, new Path(CONTAINER_ID));
            JavaCore.setClasspathContainer(containerPath, new IJavaProject[] { project },
                    new IClasspathContainer[] { container }, new NullProgressMonitor());
        }
    }

    public static IClasspathEntry getContainerEntry()
    {
        return JavaCore.newContainerEntry(new Path(CONTAINER_ID));
    }

    public static void reinitialize(IProject _project, String newIceJar) throws CoreException
    {
        IJavaProject javaProject = JavaCore.create(_project);
        IPath iceLib = new Path(newIceJar);
        IClasspathEntry classpathEntry = JavaCore.newLibraryEntry(iceLib, null, null, new IAccessRule[0], new IClasspathAttribute[0], false);
        IClasspathContainer container = new IceClasspathContainer(classpathEntry, new Path(CONTAINER_ID));
        JavaCore.setClasspathContainer(new Path(CONTAINER_ID), new IJavaProject[] { javaProject },
                new IClasspathContainer[] { container }, new NullProgressMonitor());
    }
}
