// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Ice/Buffer.h>
#include <Ice/LocalException.h>
#include <Ice/MemoryPool.h>

using namespace std;
using namespace Ice;
using namespace IceInternal;

static IceInternal::Buffer::Container::pointer
MallocWrapper(MemoryPool*, IceInternal::Buffer::Container::size_type n)
{
    return reinterpret_cast<IceInternal::Buffer::Container::pointer>(malloc(n));
}

static IceInternal::Buffer::Container::pointer
ReallocWrapper(MemoryPool*, IceInternal::Buffer::Container::pointer buf, IceInternal::Buffer::Container::size_type n)
{
    return reinterpret_cast<IceInternal::Buffer::Container::pointer>(realloc(buf, n));
}

static void
FreeWrapper(MemoryPool*, IceInternal::Buffer::Container::pointer buf)
{
    ::free(buf);
}

static IceInternal::Buffer::Container::pointer
PoolMallocWrapper(MemoryPool* p, IceInternal::Buffer::Container::size_type n)
{
    return p->alloc(n); 
}

static IceInternal::Buffer::Container::pointer
PoolReallocWrapper(MemoryPool* p, IceInternal::Buffer::Container::pointer buf, IceInternal::Buffer::Container::size_type n)
{
    return p->realloc(buf, n);
}

static void
PoolFreeWrapper(MemoryPool* p, IceInternal::Buffer::Container::pointer buf)
{
    p->free(buf);
}

void
IceInternal::Buffer::swap(Buffer& other)
{
#ifdef ICE_SMALL_MESSAGE_BUFFER_OPTIMIZATION
    Container::difference_type pos = i - b.begin();
    Container::difference_type otherPos = other.i - other.b.begin();
    b.swap(other.b);
    i = b.begin() + otherPos;
    other.i = other.b.begin() + pos;
#else
    b.swap(other.b);
    std::swap(i, other.i); 
#endif
}

IceInternal::Buffer::Container::Container(IceInternal::MemoryPool* pool) :
#ifdef ICE_SMALL_MESSAGE_BUFFER_OPTIMIZATION
    _buf(_fixed),
    _size(0),
    _capacity(ICE_BUFFER_FIXED_SIZE),
    _pool(pool)
#else
    _buf(0),
    _size(0),
    _capacity(0),
    _pool(pool)
#endif
{
    if(pool)
    {
	_alloc= PoolMallocWrapper; 
	_realloc = PoolReallocWrapper; 
	_free = PoolFreeWrapper; 
    }
    else
    {
	_alloc= MallocWrapper;
	_realloc = ReallocWrapper;
	_free = FreeWrapper;
    }
}

IceInternal::Buffer::Container::~Container()
{
#ifdef ICE_SMALL_MESSAGE_BUFFER_OPTIMIZATION
    if(_buf != _fixed)
    {
	_free(_pool, _buf);
    }
#else
    _free(_pool, _buf);
#endif
}

void
IceInternal::Buffer::Container::swap(Container& other)
{
#ifdef ICE_SMALL_MESSAGE_BUFFER_OPTIMIZATION
    if(_buf == _fixed)
    {
	if(other._buf == other._fixed)
	{
	    value_type tmp[ICE_BUFFER_FIXED_SIZE];
	    memcpy(tmp, _fixed, _size);
	    memcpy(_fixed, other._fixed, other._size);
	    memcpy(other._fixed, tmp, _size);
	}
	else
	{
	    _buf = other._buf;
	    memcpy(other._fixed, _fixed, _size);
	    other._buf = other._fixed;
	}
    }
    else
    {
	if(other._buf == other._fixed)
	{
	    other._buf = _buf;
	    memcpy(_fixed, other._fixed, other._size);
	    _buf = _fixed;
	}
	else
	{
	    std::swap(_buf, other._buf);
	}
    }
#else
    std::swap(_buf, other._buf);
#endif

    std::swap(_size, other._size);
    std::swap(_capacity, other._capacity);
    std::swap(_shrinkCounter, other._shrinkCounter);
}

void
IceInternal::Buffer::Container::clear()
{
#ifdef ICE_SMALL_MESSAGE_BUFFER_OPTIMIZATION
    if(_buf != _fixed)
    {
	_free(_pool, _buf);
	_buf = _fixed;
    }
    _size = 0;
    _capacity = ICE_BUFFER_FIXED_SIZE;
#else
    _free(_pool, _buf);
    _buf = 0;
    _size = 0;
    _capacity = 0;
#endif
}

void
IceInternal::Buffer::Container::reserve(size_type n)
{
    if(n > _capacity)
    {
	_capacity = std::max<size_type>(n, 2 * _capacity);
	_capacity = std::max<size_type>(static_cast<size_type>(240), _capacity);
    }
    else if(n < _capacity)
    {
	_capacity = n;
    }
    else
    {
	return;
    }
    
#ifdef ICE_SMALL_MESSAGE_BUFFER_OPTIMIZATION
    if(_buf != _fixed)
    {
	_buf = _realloc(_pool, _buf, _capacity);
    }
    else if(_capacity > ICE_BUFFER_FIXED_SIZE)
    {
	_buf = _alloc(_pool, _capacity);
	memcpy(_buf, _fixed, _size);
    }
#else
    if(_buf)
    {
	_buf = _realloc(_pool, _buf, _capacity);
    }
    else
    {
	_buf = _alloc(_pool, _capacity);
    }
#endif
	
    if(!_buf)
    {
	SyscallException ex(__FILE__, __LINE__);
	ex.error = getSystemErrno();
	throw ex;
    }
}
