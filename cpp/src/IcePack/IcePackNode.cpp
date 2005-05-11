// **********************************************************************
//
// Copyright (c) 2003-2005 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <IceUtil/UUID.h>
#include <Ice/Ice.h>
#include <Ice/Service.h>
#include <IcePack/Activator.h>
#include <IcePack/WaitQueue.h>
#include <IcePack/Registry.h>
#include <IcePack/ServerFactory.h>
#include <IcePack/AdapterFactory.h>
#include <IcePack/AdapterI.h>
#include <IcePack/NodeI.h>
#include <IcePack/TraceLevels.h>
#include <IcePack/DescriptorParser.h>

#ifdef _WIN32
#   include <direct.h>
#   include <sys/types.h>
#   include <sys/stat.h>
#   define S_ISDIR(mode) ((mode) & _S_IFDIR)
#   define S_ISREG(mode) ((mode) & _S_IFREG)
#else
#   include <csignal>
#   include <signal.h>
#   include <sys/wait.h>
#   include <sys/types.h>
#   include <sys/stat.h>
#   include <unistd.h>
#endif

using namespace std;
using namespace Ice;
using namespace IcePack;

namespace IcePack
{

class NodeService : public Service
{
public:

    NodeService();

    virtual bool shutdown();

protected:

    virtual bool start(int, char*[]);
    virtual void waitForShutdown();
    virtual bool stop();
    virtual CommunicatorPtr initializeCommunicator(int&, char*[]);

private:

    void usage(const std::string&);

    ActivatorPtr _activator;
    WaitQueuePtr _waitQueue;
    RegistryPtr _registry;
};

class CollocatedRegistry : public Registry
{
public:

    CollocatedRegistry(const Ice::CommunicatorPtr&, const ActivatorPtr&);

    virtual void shutdown();

private:

    ActivatorPtr _activator;
};

} // End of namespace IcePack

#ifndef _WIN32
extern "C"
{

static void
childHandler(int)
{
    //
    // Call wait to de-allocate any resources allocated for the child
    // process and avoid zombie processes. See man wait or waitpid for
    // more information.
    //
    int olderrno = errno;

    pid_t pid;
    do
    {
	pid = waitpid(-1, 0, WNOHANG);
    }
    while(pid > 0);

    assert(pid != -1 || errno == ECHILD);

    errno = olderrno;
}

}
#endif

CollocatedRegistry::CollocatedRegistry(const Ice::CommunicatorPtr& communicator, const ActivatorPtr& activator) :
    Registry(communicator), 
    _activator(activator)
{
}

void
CollocatedRegistry::shutdown()
{
    _activator->shutdown();
}

IcePack::NodeService::NodeService()
{
}

bool
IcePack::NodeService::shutdown()
{
    assert(_activator);
    _activator->shutdown();
    return true;
}

bool
IcePack::NodeService::start(int argc, char* argv[])
{
#ifndef _WIN32
    //
    // This application forks, so we need a signal handler for child termination.
    //
    struct sigaction action;
    action.sa_handler = childHandler;
    sigemptyset(&action.sa_mask);
    sigaddset(&action.sa_mask, SIGCHLD);
    action.sa_flags = 0;
    sigaction(SIGCHLD, &action, 0);
#endif

    bool nowarn = false;
    bool checkdb = false;
    string descriptor;
    vector<string> targets;
    for(int i = 1; i < argc; ++i)
    {
        if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
        {
            usage(argv[0]);
            return false;
        }
        else if(strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0)
        {
            print(ICE_STRING_VERSION);
            return false;
        }
        else if(strcmp(argv[i], "--nowarn") == 0)
        {
            nowarn = true;
        }
        else if(strcmp(argv[i], "--deploy") == 0)
        {
            if(i + 1 >= argc)
            {
                error("missing descriptor argument for option `" + string(argv[i]) + "'");
                usage(argv[0]);
                return false;
            }

            descriptor = argv[++i];

            while(i + 1 < argc && argv[++i][0] != '-')
            {
                targets.push_back(argv[i]);
            }
        }
        else if(strcmp(argv[i], "--checkdb") == 0)
        {
	    checkdb = true;
	}
    }

    PropertiesPtr properties = communicator()->getProperties();

    //
    // Disable server idle time. Otherwise, the adapter would be
    // shutdown prematurely and the deactivation would fail.
    // Deactivation of the node relies on the object adapter
    // to be active since it needs to terminate servers.
    //
    // TODO: implement Ice.ServerIdleTime in the activator
    // termination listener instead?
    //
    properties->setProperty("Ice.ServerIdleTime", "0");
    if(properties->getPropertyAsIntWithDefault("Ice.ThreadPool.Server.Size", 5) <= 5)
    {
	properties->setProperty("Ice.ThreadPool.Server.Size", "5");
    }

    //
    // Create the activator.
    //
    TraceLevelsPtr traceLevels = new TraceLevels(properties, communicator()->getLogger());
    _activator = new Activator(traceLevels, properties);

    //
    // Collocate the IcePack registry if we need to.
    //
    if(properties->getPropertyAsInt("IcePack.Node.CollocateRegistry") > 0)
    {
        //
        // The node needs a different thread pool.
        //
        if(properties->getPropertyAsInt("IcePack.Node.ThreadPool.Size") == 0)
        {
            int size = properties->getPropertyAsInt("Ice.ThreadPool.Server.Size");

            ostringstream os1;
            os1 << static_cast<int>(size / 3);
            properties->setProperty("IcePack.Node.ThreadPool.Size", os1.str());

            ostringstream os2;
            os2 << size - static_cast<int>(size / 3);
            properties->setProperty("Ice.ThreadPool.Server.Size", os2.str());
        }

        _registry = new CollocatedRegistry(communicator(), _activator);
        if(!_registry->start(nowarn))
        {
            return false;
        }

        //
        // Set the Ice.Default.Locator property to point to the
        // collocated locator (this property is passed by the
        // activator to each activated server).
        //
        string locatorPrx = "IcePack/Locator:" + properties->getProperty("IcePack.Registry.Client.Endpoints");
        properties->setProperty("Ice.Default.Locator", locatorPrx);
    }
    else if(properties->getProperty("Ice.Default.Locator").empty())
    {
        error("property `Ice.Default.Locator' is not set");
        return false;
    }

    //
    // Initialize the database environment (first setup the directory structure if needed).
    //
    string dataPath = properties->getProperty("IcePack.Node.Data");
    string dbPath;
    if(dataPath.empty())
    {
        error("property `IcePack.Node.Data' is not set");
        return false;
    }
    else
    {
#ifdef _WIN32
        struct _stat filestat;
        if(::_stat(dataPath.c_str(), &filestat) != 0 || !S_ISDIR(filestat.st_mode))
        {
	    ostringstream os;
	    SyscallException ex(__FILE__, __LINE__);
	    ex.error = getSystemErrno();
	    os << ex;
            error("property `IcePack.Node.Data' is set to an invalid path:\n" + os.str());
	    return false;
        }            
#else
        struct stat filestat;
        if(::stat(dataPath.c_str(), &filestat) != 0 || !S_ISDIR(filestat.st_mode))
        {
	    ostringstream os;
	    SyscallException ex(__FILE__, __LINE__);
	    ex.error = getSystemErrno();
	    os << ex;
            error("property `IcePack.Node.Data' is set to an invalid path:\n" + os.str());
	    return false;
        }            
#endif

        //
        // Creates subdirectories db and servers in the IcePack.Node.Data directory if they don't already exist.
        //
        if(dataPath[dataPath.length() - 1] != '/')
        {
            dataPath += "/"; 
        }

	dbPath = dataPath + "db";
        string serversPath = dataPath + "servers";
	string tmpPath = dataPath + "tmp";

#ifdef _WIN32
        if(::_stat(dbPath.c_str(), &filestat) != 0)
        {
            _mkdir(dbPath.c_str());
        }
        if(::_stat(serversPath.c_str(), &filestat) != 0)
        {
            _mkdir(serversPath.c_str());
        }
        if(::_stat(tmpPath.c_str(), &filestat) != 0)
        {
            _mkdir(tmpPath.c_str());
        }
#else
        if(::stat(dbPath.c_str(), &filestat) != 0)
        {
            mkdir(dbPath.c_str(), 0755);
        }
        if(::stat(serversPath.c_str(), &filestat) != 0)
        {
            mkdir(serversPath.c_str(), 0755);
        }
        if(::stat(tmpPath.c_str(), &filestat) != 0)
        {
            mkdir(tmpPath.c_str(), 0755);
        }
#endif
    }

    //
    // Check that required properties are set and valid.
    //
    if(properties->getProperty("IcePack.Node.Endpoints").empty())
    {
        error("property `IcePack.Node.Endpoints' is not set");
        return false;
    }

    string name = properties->getProperty("IcePack.Node.Name");
    if(name.empty())
    {
        char host[1024 + 1];
        if(gethostname(host, 1024) != 0)
        {
            syserror("property `IcePack.Node.Name' is not set and couldn't get the hostname:");
            return false;
        }
        else if(!nowarn)
        {
            warning("property `IcePack.Node.Name' is not set, using hostname: " + string(host));
        }
	properties->setProperty("IcePack.Node.Name", string(host));
    }

    //
    // Setup the Freeze database environment home directory. The name of the database
    // environment for the IcePack node is the name of the node.
    //
    properties->setProperty("Freeze.DbEnv." + name + ".DbHome", dbPath);

    //
    // Set the adapter id for this node and create the node object adapter.
    //
    properties->setProperty("IcePack.Node.AdapterId", "IcePack.Node." + name);
    ObjectAdapterPtr adapter = communicator()->createObjectAdapter("IcePack.Node");

    //
    // Create the wait queue.
    //
    _waitQueue = new WaitQueue();
    _waitQueue->start();

    //
    // Create the server factory. The server factory creates persistent objects
    // for the server and server adapter. It also takes care of installing the
    // evictors and object factories necessary to store these objects.
    //
    ServerFactoryPtr serverFactory = new ServerFactory(adapter, traceLevels, name, _activator, _waitQueue);
    NodePtr node = new NodeI(_activator, name, serverFactory, communicator(), properties);
    Identity id = stringToIdentity(IceUtil::generateUUID());
    adapter->add(node, id);
    NodePrx nodeProxy = NodePrx::uncheckedCast(adapter->createDirectProxy(id));
 
    //
    // Register this node with the node registry.
    //
    try
    {
	ObjectPrx obj = communicator()->stringToProxy("IcePack/NodeRegistry@IcePack.Registry.Internal");
	NodeRegistryPrx nodeRegistry = NodeRegistryPrx::uncheckedCast(obj);
	if(properties->getPropertyAsInt("IcePack.Node.CollocateRegistry") > 0)
	{
	    try
	    {
		nodeRegistry->remove(name);
	    }
	    catch(const IcePack::NodeNotExistException&)
	    {
		// Ignore.
	    }
	}
	nodeRegistry->add(name, nodeProxy);
    }
    catch(const NodeActiveException&)
    {
        error("a node with the same name is already registered and active");
        return false;
    }
    catch(const LocalException&)
    {
        error("couldn't contact the IcePack registry");
        return false;
    }

    //
    // Check the consistency of the databases.
    //
    if(checkdb)
    {
	serverFactory->checkConsistency();
    }

    //
    // Start the activator.
    //
    _activator->start();

    //
    // We are ready to go! Activate the object adapter. NOTE: we don't want the activate call to 
    // set the direct proxy of the object adapter with the locator registry. This was already 
    // taken care of by the node registry. Furthermore, this wouldn't work anyway because the 
    // locator registry proxy would have collocation optimization enabled.
    //
    adapter->setLocator(0);
    adapter->activate();
    adapter->setLocator(communicator()->getDefaultLocator());

    //
    // Deploy application if a descriptor is passed as a command-line option.
    //
    if(!descriptor.empty())
    {
        AdminPrx admin;
        try
        {
            admin = AdminPrx::checkedCast(communicator()->stringToProxy("IcePack/Admin"));
        }
        catch(const LocalException& ex)
        {
            ostringstream ostr;
            ostr << "couldn't contact IcePack admin interface to deploy application `" << descriptor << "':" << endl
                 << ex;
            warning(ostr.str());
        }

        if(admin)
        {
            try
            {
		map<string, string> vars;
		admin->addApplication(
		    DescriptorParser::parseApplicationDescriptor(descriptor, targets, vars, communicator()));
            }
            catch(const DeploymentException& ex)
            {
                ostringstream ostr;
                ostr << "failed to deploy application `" << descriptor << "':" << endl << ex << ": " << ex.reason;
                warning(ostr.str());
            }
            catch(const LocalException& ex)
            {
                ostringstream ostr;
                ostr << "failed to deploy application `" << descriptor << "':" << endl << ex;
                warning(ostr.str());
            }
        }
    }

    string bundleName = properties->getProperty("IcePack.Node.PrintServersReady");
    if(!bundleName.empty())
    {
	print(bundleName + " ready");
    }

    return true;
}

void
IcePack::NodeService::waitForShutdown()
{
    //
    // Wait for the activator shutdown. Once the run method returns
    // all the servers have been deactivated.
    //
    enableInterrupt();
    _activator->waitForShutdown();
    disableInterrupt();
}

bool
IcePack::NodeService::stop()
{
    try
    {
        _activator->destroy();
    }
    catch(...)
    {
    }

    //
    // The wait queue must be destroyed after the activator and before
    // the communicator is shutdown.
    //
    try
    {
        _waitQueue->destroy();
    }
    catch(...)
    {
    }

    //
    // We can now safely shutdown the communicator.
    //
    try
    {
        communicator()->shutdown();
        communicator()->waitForShutdown();
    }
    catch(...)
    {
    }

    _activator = 0;

    return true;
}

CommunicatorPtr
IcePack::NodeService::initializeCommunicator(int& argc, char* argv[])
{
    PropertiesPtr defaultProperties = getDefaultProperties(argc, argv);
    
    //
    // Make sure that IcePackNode doesn't use thread-per-connection.
    //
    defaultProperties->setProperty("Ice.ThreadPerConnection", "");

    return Service::initializeCommunicator(argc, argv);
}

void
IcePack::NodeService::usage(const string& appName)
{
    string options =
	"Options:\n"
	"-h, --help           Show this message.\n"
	"-v, --version        Display the Ice version.\n"
	"--nowarn             Don't print any security warnings.\n"
	"\n"
	"--deploy DESCRIPTOR [TARGET1 [TARGET2 ...]]\n"
	"                     Deploy descriptor in file DESCRIPTOR, with\n"
	"                     optional targets.\n"
        "--checkdb            Do a consistency check of the node database.";
#ifdef _WIN32
    if(checkSystem())
    {
        options.append(
	"\n"
	"\n"
	"--service NAME       Run as the Windows service NAME.\n"
	"\n"
	"--install NAME [--display DISP] [--executable EXEC] [args]\n"
	"                     Install as Windows service NAME. If DISP is\n"
	"                     provided, use it as the display name,\n"
	"                     otherwise NAME is used. If EXEC is provided,\n"
	"                     use it as the service executable, otherwise\n"
	"                     this executable is used. Any additional\n"
	"                     arguments are passed unchanged to the\n"
	"                     service at startup.\n"
	"--uninstall NAME     Uninstall Windows service NAME.\n"
	"--start NAME [args]  Start Windows service NAME. Any additional\n"
	"                     arguments are passed unchanged to the\n"
	"                     service.\n"
	"--stop NAME          Stop Windows service NAME."
        );
    }
#else
    options.append(
        "\n"
        "\n"
        "--daemon             Run as a daemon.\n"
        "--noclose            Do not close open file descriptors.\n"
        "--nochdir            Do not change the current working directory."
    );
#endif
    print("Usage: " + appName + " [options]\n" + options);
}

int
main(int argc, char* argv[])
{
    NodeService svc;
    return svc.main(argc, argv);
}
