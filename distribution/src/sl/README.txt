Please see the file INSTALL.txt for installation instructions.

Transports
==========

Ice for Silverlight supports two transports:

- Ice requests over regular sockets forwarded by a Glacier2 router.
- HTTP (or HTTPS) POST requests forwarded by an ASP.NET bridge.

These transports are described in more detail below.


Socket transport
----------------

Several steps are necessary for using the socket transport in Ice for
Silverlight:

- You must run a Glacier2 router. This is a special Ice server that
  efficiently and securely forwards requests from clients to servers.

- You must run a Silverlight policy server, which controls the IP
  addresses to which a Silverlight client is allowed to connect. The
  policy server must run on the host from which the Silverlight client
  is downloaded. Ice for Silverlight includes a simple implementation
  of a policy server that you can use in your own applications. Refer
  to the link below for more information on policy servers:

    http://msdn.microsoft.com/en-us/library/cc645032(VS.95).aspx

- Your Silverlight client must configure its communicator with a proxy
  for the Glacier2 router. By establishing a default router in the
  communicator, the Ice run time transparently forwards every
  invocation on a proxy to the specified router.

- Your Silverlight client must also establish a session with the
  Glacier2 router and keep that session alive by sending messages at
  regular intervals. If your application can have long periods of
  inactivity (at least as far as Ice requests are concerned), the
  application should use a dedicated thread to "ping" the session.

Silverlight 2 Beta 2 has two limitations that affect the configuration
of your Glacier2 router. First, SSL is not supported, therefore your
router must listen on a TCP port. Second, Silverlight limits the range
of ports to which a client can connect. As a result, you must
configure your Glacier2 router to listen on a TCP port within the
range 4502 to 4534.

The Ice for Silverlight run time does not establish a connection to
the router until an operation is invoked that requires network
activity. If the connection is lost due to an exception or network
error, all subsequent attempts to invoke Ice operations will fail with
an appropriate exception. If a client wishes to reestablish the
connection, it must first invoke the resetConnection method on the
communicator. Note that a Glacier2 session becomes invalid as soon as
the connection is lost, therefore the client must also create a new
session after resetting the connection.

This distribution includes an example that demonstrates the use of the
socket transport, which you can find in the demo/Glacier2/callback
subdirectory. The README.txt file in this subdirectory provides
instructions for running the example.

For more information on configuring Glacier2 and using it in an Ice
application, please refer to the Glacier2 chapter of the Ice manual.


Callbacks
---------

Although Ice for Silverlight does not offer a complete server-side
run time, it does provide support for callback requests when using the
socket transport. The process is consistent with other Ice language
mappings: you create an object adapter and configure it with a router,
which causes callback requests from the server to your Silverlight
client to traverse the outgoing connection from the client to the
router. In other words, the Silverlight client establishes a
bi-directional connection with the router in which requests can flow
in either direction.

When registering servants with this object adapter, the client must
use an identity category that is supplied by the router. This category
is assigned to a session and is valid only while the session remains
active. If the client loses its connection to the router for any
reason, not only must it reset the connection and create a new session
as described in the previous section, but also register servants with
the identity category assigned to the new session and remove servants
that used the old identity category.

The example in demo/Glacier2/callback shows how to use callbacks in a
Silverlight application. Refer to the Glacier2 chapter of the Ice
manual for further information on bi-directional connections.


HTTP transport
--------------

The HTTP transport relies on a bridge that extracts Ice requests
from POST requests, forwards them to the appropriate Ice servers
using the Ice protocol, and returns the replies as HTTP responses.
The bridge is an Ice for .NET application implemented as an ASP.NET 
web handler. It requires an ASP.NET capable server, such as Microsoft
IIS or Apache with mod_mono; it also requires the Ice for .NET run
time (which must be accessible to ASP.NET applications).


Intermediaries
--------------

Both transports depend on an intermediary that forwards requests on
behalf of a Silverlight client. The intermediaries (the Glacier2
router and the HTTP bridge) are generic; in other words, they are not
specific to a particular Ice application and do not require type
information for the requests and replies that they process.

A single router or bridge deployment can be used for multiple Ice
applications without requiring application-specific configuration. New
applications may also be added without additional configuration, and
removing applications does not require "clean up". It is also possible
to deploy multiple routers or bridges, which is useful for controlling
resource consumption and access permissions for different groups of
applications, with each group using a different router or bridge.  


Comparing transports
--------------------

The requirements of an application typically dictate which transport
should be used. For example, the socket transport is the only option
if callbacks are necessary. For an application that does not need to
use callbacks, the choice of a transport might depend on other
environmental issues: the socket transport may be the better choice
if your organization is already using Glacier2 or if the prospect of
using the ASP.NET HTTP bridge is infeasible. On the other hand, the
need to run a policy server when using the socket transport may cause
you to prefer the HTTP transport instead.

As far as performance is concerned, we have not performed an
exhaustive analysis but anecdotal evidence and our own experience tell
us that the socket option is likely to provide lower latency because
it avoids the additional overhead of encapsulating Ice messages inside
POST requests and benefits from Glacier2's natively-compiled C++ code
during request forwarding.

Also note that an application is able to use both transports if
necessary, but not within the same communicator. A communicator can
only be configured for one transport, therefore an application needing
to use both transports would have to initialize two communicators and
configure them appropriately.


Configuring the Ice for Silverlight Bridge
==========================================


Microsoft IIS
-------------

IIS is Microsoft's Internet Information Server. IIS includes ASP.NET
support and is pre-configured to run .ashx files. The configuration
steps are:

 - Decide where you will store the Ice for Silverlight bridge files,
   creating a new directory if necessary.

 - Place the Global.asax and IceBridge.ashx files included in the 
   distribution into that directory.

 - Verify that the permissions on IceBridge.ashx will allow it to be
   run by IIS.

 - If the Ice for .NET assembly, Ice.dll, is not to be installed in
   the global assembly cache on the web server host, create a bin
   sub-directory in the bridge's directory and copy the assembly there.

 - Create a web application with IIS Manager or another administration
   tool, using the directory where the bridge files were placed as the
   physical path for the application. Take note of the alias specified
   for the web application; its URI is required when configuring
   Silverlight clients.


Apache and mod_mono
-------------------

Apache does not include support for ASP.NET applications. However, the
Mono project develops and maintains an ASP.NET implementation that can
be configured as an Apache module. (For information on how to install
mod_mono and configure Apache, please consult the mod_mono
documentation.) Once you have a working Apache and mod_mono
installation, setting up the Ice for Silverlight bridge is very
similar to the procedure for IIS. There are two important points: Ice
for .NET is a .NET 2.0 application, so the Apache configuration should
contain:

  MonoServerPath /usr/bin/mod-mono-server2

Substitute the proper path for mod-mono-server2 if it is not installed
in /usr/bin.

Secondly, the bridge must be configured specifically as an ASP.NET
application. For example, the Apache configuration could contain a
line such as:

  MonoApplications "/icebridge:/var/www/htdocs/icebridge"

As with IIS, file permissions are important. For example, the 
IceBridge.ashx file must have execute permission and the Silverlight
client assemblies must not.

mod_mono adds MIME types to the Apache configuration for typical
ASP.NET file types. Some MIME types required for Silverlight
applications are incorrect or missing. Strictly speaking, this only
affects Silverlight clients, not the bridge itself. However, since Ice
for Silverlight applications will not work properly unless the
configuration is correct, the following changes are required:

 - The .dll extension must be changed from application/x-asp-net to
   application/x-msdownload. Without this change, assemblies such as
   the Ice for Silverlight assembly will not be transferred properly,
   resulting in loading errors.

 - The following MIME types must be added:

   application/x-silverlight-app .xap

Threading
=========

Using Ice for Silverlight, invocations can be made from the UI thread
as well as from non-UI threads. However, only asynchronous invocations
can be made from the UI thread, while non-UI threads allow both
synchronous and asynchronous invocations.


Asynchronous Message Invocation (AMI)
=====================================

AMI is implemented differently in Ice for Silverlight than it is in
Ice for C#. Specifically, the means by which you define the callbacks
that are called once the AMI method either succeeds or fails is 
different.

In Ice for C# it is necessary for the application to extend an AMI
callback class generated by the Slice-to-C# translator and implement
the abstract ice_response() and ice_exception() methods in the 
derived class. An instance of this class is then passed to the async
call on the proxy.

For example consider the following Slice definitions:

    module Foo
    {
        interface Bar
        {
            ["ami"] void method(int param1, out string param2);
        };
    };

The AMI callback class generated for the operation "method" would be
named Foo.AMI_Bar_method and the user would have to implement a 
callback class similar to the following:

    class AMI_Bar_methodI : Foo.AMI_Bar_method
    {
        public override void ice_response(string param2)
        {
            // Called on success
            ...
        }

        public override void ice_exception(Ice.Exception ex)
        {
            // Called on failure
            ...
        }
    }

The method invocation would then look like this:

    public void someFunction()
    {
        ...

        Foo.BarPrx proxy = ... // Obtain proxy by regular means
        proxy.method_async(new AMI_Bar_methodI(), 0);

        ...
    }

Ice for Silverlight uses C# delegates instead of a callback class to
notify the program about the completion of an operation. The caller
must supply two delegate functions, one for the response callback and
one for the exception callback. The signatures for these methods are
the same as those for the ice_response() and ice_exception() methods
in Ice for C#.

Using the same Slice definitions as the example above, an Ice for
Silverlight program could invoke the AMI operation as follows:

    public void methodResponse(string param2)
    {
        // Called on success
        ...
    }

    public void methodException(Ice.Exception ex)
    {
        // Called on failure
        ...
    }

    public void someFunction()
    {
        ...

        Foo.BarPrx proxy = ... // Obtain proxy by regular means
        proxy.method_async(methodResponse, methodException, 0);

        ...
    }


User Exceptions and Object-by-Value
===================================

Ice for .NET uses reflection during the unmarshaling of user
exceptions and objects by value. Since Silverlight does not support
retrieving the list of assemblies loaded with the application, it is
necessary to explicitly configure the list of assemblies where the
user exception or object by value types are located. This is done
using the Ice.FactoryAssemblies property, whose value represents a
list of qualified assembly names separated by whitespace. For
example:

  Ice.FactoryAssemblies=UserAssembly1,version=1.0.0 UserAssembly2,version=1.0.0.0

If you do not set this property, your client will throw
Ice.UnmarshalOutOfBoundsException or Ice.NoObjectFactoryException if
it receives an unknown user exception or object by value respectively.
