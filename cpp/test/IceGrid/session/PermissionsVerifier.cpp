// **********************************************************************
//
// Copyright (c) 2003-2007 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Ice/Ice.h>
#include <Glacier2/PermissionsVerifier.h>
#include <IceSSL/Plugin.h>

using namespace std;

class AdminPermissionsVerifierI : public Glacier2::PermissionsVerifier
{
public:

    virtual bool
    checkPermissions(const string& userId, const string& passwd, string&, const Ice::Current& c) const
    {
        if(userId == "shutdown")
        {
            c.adapter->getCommunicator()->shutdown();
            return true;
        }
        return userId == "admin1" && passwd == "test1" || userId == "admin2" && passwd == "test2" ||
                userId == "admin3" && passwd == "test3";
    }
};

class PermissionsVerifierServer : public Ice::Application
{
public:

    virtual int run(int, char*[])
    {
        Ice::ObjectAdapterPtr adapter = communicator()->createObjectAdapterWithEndpoints(
            "PermissionsVerifier", "tcp -p 12002");
        adapter->add(new AdminPermissionsVerifierI, communicator()->stringToIdentity("AdminPermissionsVerifier"));
        adapter->activate();
        communicator()->waitForShutdown();
        return EXIT_SUCCESS;
    }
};

int
main(int argc, char* argv[])
{
    PermissionsVerifierServer app;
    return app.main(argc, argv);
}
