TODO: XXX: Explain how to install the binary distribution


Ice for Silverlight is a port of Ice for C# to Silverlight with some
limitations:

- it only supports the TCP socket transport. SSL or UDP transports are
  not supported.

- there's no support for server side connections: an Ice for
  Silverlight application can't accept connections. However, it does
  support bi-directional connections and can therefore dispatch
  requests received over a connection to a server.

- it doesn't support protocol compression.

- Ice.Application and Glacier2.Application classes are not supported.

- Dynamic loading of Slice-generated class and exception factories are
  not supported, the Ice.FactoryAssemblies property must be used
  instead.

- the ICE_CONFIG environment variable is not supported.

- Dynamic loading of Slice checksums.

- Thread priorities are not supported.

- the Ice.StdOut, Ice.StdErr, Ice.PrintProcessId and Ice.LogFile
  properties are not supported.

- Loading properties from the Windows registry is not supported.

This Ice for Silverlight release also no longer supports the HTTP
transport supported in previous Ice for Silverlight versions and
doesn't restrict anymore Ice connections to Glacier2 routers, it can
connect to any Ice servers.

Features specific to Ice for Silverlight are described in more details
below.

TCP Socket transport
====================

To use the socket transport in Ice for Silverlight, you must run a
Silverlight policy server or use a web server as a policy server to
control the IP addresses to which a Silverlight client is allowed to
connect.

The policy server must run on the host from which the Silverlight
client is downloaded. Ice for Silverlight includes a simple
implementation of a policy server that you can use in your own
applications. Refer to the link below for more information on policy
servers:

    http://msdn.microsoft.com/en-us/library/cc645032(VS.95).aspx

If you don't want to deploy a policy server you can also use the web
server as a policy server by adding a clientaccesspolicy.xml file at
the document root directory of your web server. Refer to the link
above for moer information on this.

Silverlight limits the range of ports to which a client can
connect. As a result, you must configure your Ice server to listen on
a TCP port within the range 4502 to 4534.

TODO: talk about OOB silverlight applications

Callbacks
=========

Although Ice for Silverlight does not offer a complete server-side run
time, it does provide support for callback requests when using the
socket transport. The process is consistent with other Ice language
mappings: you create an object adapter and associate it to an Ice
connection, which causes callback requests from the server to your
Silverlight client to traverse the outgoing connection from the client
to the server. In other words, the Silverlight client establishes a
bi-directional connection with the server in which requests can flow
in either direction.


User Exceptions and Object-by-Value
===================================

Ice for .NET uses reflection during the unmarshaling of user
exceptions and objects by value. Since Silverlight does not support
retrieving the list of assemblies loaded with the application, it is
necessary to explicitly configure the list of assemblies where the
user exception or object by value types are located. This is done
using the Ice.FactoryAssemblies property, whose value represents a
list of qualified assembly names separated by whitespace. For example:

  Ice.FactoryAssemblies=UserAssembly1,version=1.0.0 UserAssembly2,version=1.0.0.0

If you do not set this property, your client will throw
Ice.UnmarshalOutOfBoundsException or Ice.NoObjectFactoryException if
it receives an unknown user exception or object by value respectively.
