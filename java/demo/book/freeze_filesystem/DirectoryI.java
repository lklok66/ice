// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

import Filesystem.*;

public final class DirectoryI extends PersistentDirectory
{
    public
    DirectoryI()
    {
        _destroyed = false;
    }

    public
    DirectoryI(Ice.Identity id)
    {
        _id = id;
        nodes = new java.util.HashMap();
        _destroyed = false;
    }

    public synchronized String
    name(Ice.Current current)
    {
        if(_destroyed)
        {
            throw new Ice.ObjectNotExistException(current.id, current.facet, current.operation);
        }

        return nodeName;
    }

    public void
    destroy(Ice.Current current)
        throws PermissionDenied
    {
        if(parent == null)
        {
            throw new PermissionDenied("cannot remove root directory");
        }

        java.util.Map children = null;

        synchronized(this)
        {
            if(_destroyed)
            {
                throw new Ice.ObjectNotExistException(current.id, current.facet, current.operation);
            }

            children = (java.util.Map)((java.util.HashMap)nodes).clone();
            _destroyed = true;
        }

        //
        // For consistency with C++, we iterate over the children outside of synchronization.
        //
        java.util.Iterator p = children.values().iterator();
        while(p.hasNext())
        {
            NodeDesc desc = (NodeDesc)p.next();
            desc.proxy.destroy();
        }

        assert(nodes.isEmpty());

        parent.removeNode(nodeName);
        _evictor.remove(_id);
    }

    public synchronized NodeDesc[]
    list(Ice.Current current)
    {
        if(_destroyed)
        {
            throw new Ice.ObjectNotExistException(current.id, current.facet, current.operation);
        }

        NodeDesc[] result = new NodeDesc[nodes.size()];
        int i = 0;
        java.util.Iterator p = nodes.values().iterator();
        while(p.hasNext())
        {
            result[i++] = (NodeDesc)p.next();
        }
        return result;
    }

    public synchronized NodeDesc
    find(String name, Ice.Current current)
        throws NoSuchName
    {
        if(_destroyed)
        {
            throw new Ice.ObjectNotExistException(current.id, current.facet, current.operation);
        }

        if(!nodes.containsKey(name))
        {
            throw new NoSuchName(name);
        }

        return (NodeDesc)nodes.get(name);
    }

    public synchronized DirectoryPrx
    createDirectory(String name, Ice.Current current)
        throws NameInUse
    {
        if(_destroyed)
        {
            throw new Ice.ObjectNotExistException(current.id, current.facet, current.operation);
        }

        if(name.length() == 0 || nodes.containsKey(name))
        {
            throw new NameInUse(name);
        }

        Ice.Identity id = current.adapter.getCommunicator().stringToIdentity(java.util.UUID.randomUUID().toString());
        PersistentDirectory dir = new DirectoryI(id);
        dir.nodeName = name;
        dir.parent = PersistentDirectoryPrxHelper.uncheckedCast(current.adapter.createProxy(current.id));
        DirectoryPrx proxy = DirectoryPrxHelper.uncheckedCast(_evictor.add(dir, id));

        NodeDesc nd = new NodeDesc();
        nd.name = name;
        nd.type = NodeType.DirType;
        nd.proxy = proxy;
        nodes.put(name, nd);

        return proxy;
    }

    public synchronized FilePrx
    createFile(String name, Ice.Current current)
        throws NameInUse
    {
        if(_destroyed)
        {
            throw new Ice.ObjectNotExistException(current.id, current.facet, current.operation);
        }

        if(name.length() == 0 || nodes.containsKey(name))
        {
            throw new NameInUse(name);
        }

        Ice.Identity id = current.adapter.getCommunicator().stringToIdentity(java.util.UUID.randomUUID().toString());
        PersistentFile file = new FileI(id);
        file.nodeName = name;
        file.parent = PersistentDirectoryPrxHelper.uncheckedCast(current.adapter.createProxy(current.id));
        FilePrx proxy = FilePrxHelper.uncheckedCast(_evictor.add(file, id));

        NodeDesc nd = new NodeDesc();
        nd.name = name;
        nd.type = NodeType.FileType;
        nd.proxy = proxy;
        nodes.put(name, nd);

        return proxy;
    }

    public synchronized void
    removeNode(String name, Ice.Current current)
    {
        assert(nodes.containsKey(name));
        nodes.remove(name);
    }

    public static Freeze.Evictor _evictor;
    public Ice.Identity _id;
    private boolean _destroyed;
}
