// **********************************************************************
//
// Copyright (c) 2003-2017 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef ICE_GRID_ADMIN_ROUTER_H
#define ICE_GRID_ADMIN_ROUTER_H

#include <Ice/Ice.h>
#include <IceGrid/TraceLevels.h>

namespace IceGrid
{

//
// An Admin Router routes requests to an admin object
//
class AdminRouter : public Ice::BlobjectArrayAsync
{
protected:

    AdminRouter(const TraceLevelsPtr&);

    void invokeOnTarget(const Ice::ObjectPrx&,
                        const Ice::AMD_Object_ice_invokePtr&,
                        const std::pair<const Ice::Byte*, const Ice::Byte*>&,
                        const Ice::Current&);

    const TraceLevelsPtr _traceLevels;
};

}
#endif
