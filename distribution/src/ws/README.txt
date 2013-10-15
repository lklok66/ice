======================================================================
The Internet Communications Engine
======================================================================

Ice is a modern object-oriented toolkit that enables you to build
distributed applications with minimal effort. Ice allows you to focus
your efforts on your application logic while it takes care of all
interactions with low-level network programming interfaces. With Ice,
there is no need to worry about details such as opening network
connections, serializing and deserializing data for network
transmission, or retrying failed connection attempts (to name but a
few of dozens of such low-level details).

In addition to fully-featured and high-performance RPC, Ice also
provides a number of services. These services supply functionality
that most distributed applications require, such as event distribution
or server management.

Ice is available under the terms and conditions of the GNU General
Public License v2 (see the ICE_LICENSE file). Commercial licenses are
available for customers who wish to use Ice in proprietary products.
Please contact sales@zeroc.com for more information on licensing Ice.


Ice WebSocket transport
-----------------------

This distribution includes patched Ice and IceSSL libraries as well as
the Ice WebSocket transport for Visual Studio 2012. The Ice and IceSSL
libraries are binary compatible with Ice 3.5.1.

The WebSocket transport is intended to be used in environments where
only connections to the HTTP or HTTPs ports are allowed.

It can also be used to simplify the firewall setup and deployment on
the server side where a web infrastructure already exists. For
example, a reversed HTTP proxy can be used to forward either the HTTP
traffic or web socket traffic to the appropriate back-end servers.

More information on the WebSocket transport from

  http://www.zeroc.com/labs/icews/index.html

Using this distribution
-----------------------

Follow the intructions from the Ice release notes to build and run
your application:

  http://doc.zeroc.com/display/Ice/Release+Notes

Since the patched Ice and IceSSL libraries from this distribution are
binary compatible with Ice 3.5.1, you can continue building your
application as usual and only update your runtime configuration to
look fo the patched libraries from the IceWS distribution.

Note that this distribution only includes binaries for Visual Studio
2012.

To use the IceWS transport, you will need to update your environment
to first look for libraries from the bin directory of your IceWS
installation.

You can set the PATH environment variable:

  > set PATH=<IceWS installation directory>\bin;%PATH%

If your application is a 64bits application:

  > set PATH=<IceWS installation directory>\bin\x64;%PATH%
