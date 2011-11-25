// **********************************************************************
//
// Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef ICE_DB_H
#define ICE_DB_H

#include <IceUtil/Exception.h>
#include <IceUtil/Shared.h>
#include <IceUtil/Handle.h>

#include <map>

#ifndef ICE_DB_API
#   ifdef ICE_DB_API_EXPORTS
#       define ICE_DB_API ICE_DECLSPEC_EXPORT
#    else
#       define ICE_DB_API ICE_DECLSPEC_IMPORT
#    endif
#endif

namespace IceDB
{

class ICE_DB_API DatabaseException : public IceUtil::Exception
{
protected:

    DatabaseException(const char* file, int line) : IceUtil::Exception(file, line)
    {
    }

    virtual std::string ice_name() const;
};

class ICE_DB_API DeadlockException : public DatabaseException
{
protected:

    DeadlockException(const char* file, int line) : DatabaseException(file, line)
    {
    }

    virtual std::string ice_name() const;
};

class ICE_DB_API NotFoundException : public DatabaseException
{
protected:

    NotFoundException(const char* file, int line) : DatabaseException(file, line)
    {
    }

    virtual std::string ice_name() const;
};

class ICE_DB_API DatabaseConnection : virtual public IceUtil::Shared
{
public:

    virtual void beginTransaction() = 0;
    virtual void commitTransaction() = 0;
    virtual void rollbackTransaction() = 0;
};
typedef IceUtil::Handle<DatabaseConnection> DatabaseConnectionPtr;

class ICE_DB_API TransactionHolder
{
public:
    
    TransactionHolder(const DatabaseConnectionPtr&);
    ~TransactionHolder();

    void commit();
    void rollback();

private:
    
    //
    // Not implemented
    //
    TransactionHolder(const TransactionHolder&);

    TransactionHolder& 
    operator=(const TransactionHolder&);

    DatabaseConnectionPtr _connection;
};

class ICE_DB_API DatabaseCache : public IceUtil::Shared
{
public:
    
    virtual DatabaseConnectionPtr getConnection() = 0;
    virtual DatabaseConnectionPtr newConnection() = 0;
};
typedef IceUtil::Handle<DatabaseCache> DatabaseCachePtr;

template<class Key, class Value> class Wrapper : virtual public IceUtil::Shared
{
public:

    virtual std::map<Key, Value> getMap() = 0;
    virtual void put(const Key&, const Value&) = 0;
    virtual Value find(const Key&) = 0;
    virtual void erase(const Key&) = 0;
    virtual void clear() = 0;
};

};

#endif
