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

import org.eclipse.core.runtime.IPath;
import org.eclipse.jdt.core.IClasspathContainer;
import org.eclipse.jdt.core.IClasspathEntry;

public class IceClasspathContainer implements IClasspathContainer
{
    private IClasspathEntry[] _cpEntry;
    private IPath _path;

    IceClasspathContainer(IClasspathEntry entry, IPath path)
    {
        _cpEntry = new IClasspathEntry[] { entry };
        _path = path;
    }

    public IClasspathEntry[] getClasspathEntries()
    {
        return _cpEntry;
    }

    public String getDescription()
    {
        return "Ice Library";
    }

    public int getKind()
    {
        return IClasspathContainer.K_DEFAULT_SYSTEM;
    }

    public IPath getPath()
    {
        return _path;
    }
}
