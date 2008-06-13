Please see the file INSTALL.txt for installation instructions.


Ice for Silverlight Bridge
--------------------------

The bridge is an Ice for .NET application implemented as an ASP.NET 
web handler. It retrieves Ice requests encoded in HTTP POST requests,
forwards each request to the appropriate Ice server using the Ice
protocol, and returns the server's reply as an HTTP response to the
web client. The bridge requires an ASP.NET capable server, such as
Microsoft IIS or Apache with mod_mono; it also requires the Ice for
.NET run time (which must be accessible to ASP.NET applications).

The Ice for Silverlight bridge is generic; in other words, the bridge
is not specific to a particular Ice application and does not require
type information for the requests and replies that it processes. A
single bridge deployment can be used for multiple Ice applications
without requiring application-specific configuration. New applications
may also be added without additional configuration, and removing
applications does not require "clean-up".

A generic bridge offers additional benefits as well:

 - Only one instance per web server host needs to be maintained or
   updated.

 - Changes to running applications have no effect on the bridge or 
   existing applications.

 - It is easier to control the resources used for processing Ice
   applications.

While a single bridge can be used for any number of Ice applications,
it is also possible to deploy multiple bridges on the same web server
host. For example, this can be used control resource consumption and
access permissions for different groups of applications, with each
group using a different bridge.  


Configuring the Ice for Silverlight Bridge on Microsoft IIS
-----------------------------------------------------------

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


Configuring the Ice for Silverlight Bridge on Apache and mod_mono
-----------------------------------------------------------------

Apache does not include support for ASP.NET applications. However, the
Mono project develops and maintains an ASP.NET implementation that can
be configured as an Apache module. (For information on how to install
mod_mono and configure Apache, please consult the mod_mono
documentation.) Once you have a working Apache and mod_mono
installation, setting up the Ice for Silverlight bridge is very similar
to the procedure for IIS. There are two important points: Ice for .NET
is a .NET 2.0 application, so the Apache configuration should contain:

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
 
   application/manifest .manifest
   application/xaml+xml .xaml
   application/x-ms-application .application
   application/octet-stream .deploy
   application/x-ms-xbap .xbap
   application/vnd.ms-xpsdocument .xps


Threading
---------

Using Ice for Silverlight, invocations can be made from the UI thread
as well as from non-UI threads. However, only asynchronous invocations
can be made from the UI thread, while non-UI threads allow both
synchronous and asynchronous invocations.


Asynchronous Message Invocation (AMI)
-------------------------------------

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
-----------------------------------

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
