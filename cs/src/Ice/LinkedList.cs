// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

namespace IceUtil
{
    using System;
    using System.Collections;
    using System.Diagnostics;

    public class LinkedList : ICollection, ICloneable
    {
	public LinkedList()
	{
	    _head = null;
	    _tail = null;
	    _count = 0;
	}

	public int Count
	{
	    get
	    {
		return _count;
	    }
	}

	public bool IsSynchronized
	{
	    get
	    {
		return false;
	    }
	}

	public object SyncRoot
	{
	    get
	    {
		return this;
	    }
	}

	public void CopyTo(Array array, int index)
	{
	    //
	    // Check preconditions.
	    //
	    if(array == null)
	    {
		throw new ArgumentNullException("array", "array parameter must not be null");
	    }
	    if(index < 0)
	    {
		throw new ArgumentOutOfRangeException("index", _count, "index must not be less than zero");
	    }
	    if(index >= array.Length)
	    {
		throw new ArgumentException("index out of bounds for array", "index");
	    }
	    if(array.Length - index > _count)
	    {
		throw new ArgumentException("insufficient room in array", "array");
	    }
	    if(array.Rank != 1)
	    {
		throw new ArgumentException("array must be one-dimensional", "array");
	    }

	    //
	    // Copy the elements.
	    //
	    Node n = _head;
	    while(n != null)
	    {
		array.SetValue(n.val, index++);
		n = (Node)n.next;
	    }
	}

	public IEnumerator GetEnumerator()
	{
	    return new Enumerator(this);
	}

	public object Clone()
	{
	    LinkedList l = new LinkedList();
	    Node cursor = _head;
	    while(cursor != null)
	    {
		l.Add(cursor.val);
		cursor = cursor.next;
	    }
	    return l;
	}

	public void Add(object value)
	{
	    Node n = new Node();
	    n.val = value;
	    if(_tail == null)
	    {
		n.prev = null;
		n.next = null;
		_head = n;
		_tail = n;
	    }
	    else
	    {
		n.prev = _tail;
		n.next = null;
		_tail.next = n;
		_tail = n;	  
	    }
	    _count++;
	}

	public void AddFirst(object value)
	{
	    Node n = new Node();
	    n.val = value;
	    if(_head == null)
	    {
		n.prev = null;
		n.next = null;
		_head = n;
		_tail = n;
	    }
	    else
	    {
		n.prev = null;
		n.next = _head;
		_head.prev = n;
		_head = n;
	    }
	    _count++;
	}

	private void Remove(Node n)
	{
	    Debug.Assert(n != null);
	    Debug.Assert(_count != 0);
	    
	    if(n.prev != null)
	    {
		n.prev.next = n.next;
	    }
	    else
	    {
		_head = n.next;
	    }
	    if(n.next != null)
	    {
		n.next.prev = n.prev;
	    }
	    else
	    {
		_tail = n.prev;
	    }
	    _count--;
	}

	internal class Node
	{
	    internal Node next;
	    internal Node prev;
	    internal object val;
	}

	private Node _head;
	private Node _tail;
	private int _count;

	public class Enumerator : IEnumerator
	{
	    internal Enumerator(LinkedList list)
	    {
		_list = list;
		_current = null;
		_movePrev = null;
		_moveNext = null;
		_removed = false;
	    }

	    public void Reset()
	    {
		_current = null;
		_movePrev = null;
		_moveNext = null;
		_removed = false;
	    }

	    public object Current
	    {
		get
		{
		    if(_current == null)
		    {
			throw new InvalidOperationException("iterator not positioned on an element");
		    }
		    return _current.val;
		}
	    }

	    public bool MoveNext()
	    {
		if(_removed)
		{
		    _current = _moveNext;
		    _moveNext = null;
		    _movePrev = null;
		    _removed = false;
		}
		else
		{
		    if(_current == _list._tail) // Make sure the iterator "sticks" if on last element.
		    {
			return false;
		    }
		    _current = _current == null ? _list._head : _current.next;
		}
		return _current != null;
	    }

	    public bool MovePrev()
	    {
		if(_removed)
		{
		    _current = _movePrev;
		    _movePrev = null;
		    _moveNext = null;
		    _removed = false;
		}
		else
		{
		    if(_current == _list._head) // Make sure the iterator "sticks" if on first element.
		    {
			return false;
		    }
		    _current = _current == null ? _list._tail : _current.prev;
		}
		return _current != null;
	    }

	    public void Remove()
	    {
		if(_current == null)
		{
		    throw new InvalidOperationException("iterator is not positioned on an element");
		}
		_removed = true;
		_moveNext = _current.next; // Remember where to move next for call to MoveNext().
		_movePrev = _current.prev; // Remember where to move next for call to MovePrev().
		_list.Remove(_current);
		_current = null;
	    }

	    private LinkedList _list; // The list we are iterating over.
	    private Node _current; // Current iterator position.
	    private Node _moveNext; // Remembers node that preceded a removed element.
	    private Node _movePrev; // Remembers node that followed a removed element.
	    private bool _removed; // True after a call to Remove(), false otherwise.
	}
    }
}

