// **********************************************************************
//
// Copyright (c) 2003-2010 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef FREEZE_OBJECT_STORE_H
#define FREEZE_OBJECT_STORE_H

#include <Ice/Ice.h>
#include <Ice/Identity.h>
#include <Freeze/DB.h>
#include <Freeze/EvictorStorage.h>
#include <Freeze/Index.h>
#include <Freeze/TransactionI.h>
#include <IceUtil/Cache.h>

#include <vector>
#include <list>
#include <db_cxx.h>

namespace Freeze
{

template<class T> class EvictorI;

class EvictorIBase;

class ObjectStoreBase
{
public:

    ObjectStoreBase(const std::string&, const std::string&, bool, EvictorIBase*, 
                    const std::vector<IndexPtr>&, bool);

    virtual ~ObjectStoreBase();

    const Ice::ObjectPtr& sampleServant() const;

    bool dbHasObject(const Ice::Identity&, const TransactionIPtr&) const;
    void save(Key& key, Value& value, Ice::Byte status, DbTxn* tx);

    static void marshal(const Ice::Identity&, Key&, const Ice::CommunicatorPtr&);
    static void unmarshal(Ice::Identity&, const Key&, const Ice::CommunicatorPtr&);
    static void marshal(const ObjectRecord&, Value&, const Ice::CommunicatorPtr&);
    static void unmarshal(ObjectRecord&, const Value&, const Ice::CommunicatorPtr&);

    bool load(const Ice::Identity&, const TransactionIPtr&, ObjectRecord&);
    void update(const Ice::Identity&, const ObjectRecord&, const TransactionIPtr&);

    bool insert(const Ice::Identity&, const ObjectRecord&, const TransactionIPtr&);
    bool remove(const Ice::Identity&, const TransactionIPtr&);
    
    EvictorIBase* evictor() const;

    //
    // For IndexI and Iterator
    //
    Db* db() const;
    const std::string& dbName() const;

    const Ice::CommunicatorPtr& communicator() const;
    const std::string& facet() const;

protected:

    bool loadImpl(const Ice::Identity&, ObjectRecord&);

private:
    
    std::auto_ptr<Db> _db;
    std::string _facet;
    std::string _dbName;
    EvictorIBase* _evictor;
    std::vector<IndexPtr> _indices;
    Ice::CommunicatorPtr _communicator;
    Ice::ObjectPtr _sampleServant;
};


template<class T>
class ObjectStore : public ObjectStoreBase, public IceUtil::Cache<Ice::Identity, T>
{
 public:

    ObjectStore(const std::string& facet, const std::string facetType,
                bool createDb, EvictorIBase* evictor, 
                const std::vector<IndexPtr>& indices = std::vector<IndexPtr>(),
                bool populateEmptyIndices = false) :
        ObjectStoreBase(facet, facetType, createDb, evictor, indices, populateEmptyIndices)
    {
    }

#ifdef __BCPLUSPLUS__
    bool load(const Ice::Identity& ident, const TransactionIPtr& trans, ObjectRecord& rec)
    {
        return ObjectStoreBase::load(ident, trans, rec);
    }
#else
    using ObjectStoreBase::load;
#endif

    typedef IceUtil::Cache<Ice::Identity, T> Cache;

protected:

    virtual IceUtil::Handle<T> 
    load(const Ice::Identity& ident)
    {
        ObjectRecord rec;
        if(loadImpl(ident, rec))
        {
            return new T(rec, *this);
        }
        else
        {
            return 0;
        }
    }

    virtual void 
    pinned(const IceUtil::Handle<T>& element, typename Cache::Position p)
    {
        element->init(p);
    }
};

//
// Inline member function definitions
//

inline Db* 
ObjectStoreBase::db() const
{
    return _db.get();
}

inline const Ice::CommunicatorPtr& 
ObjectStoreBase::communicator() const
{
    return _communicator;
}

inline EvictorIBase*
ObjectStoreBase::evictor() const
{
    return _evictor;
}

inline const std::string&
ObjectStoreBase::facet() const
{
    return _facet;
}

inline const Ice::ObjectPtr&
ObjectStoreBase::sampleServant() const
{
    return _sampleServant;
}

}

#endif

