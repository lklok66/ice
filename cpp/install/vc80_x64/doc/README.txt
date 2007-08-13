The Internet Communications Engine
----------------------------------

Ice is a modern alternative to object middleware such as CORBA or
COM/DCOM/COM+. It is easy to learn, yet provides a powerful network
infrastructure for demanding technical applications. It features an
object-oriented specification language, easy to use C++, C#, Java,
Python, Ruby, PHP, and Visual Basic mappings, a highly efficient
protocol, asynchronous method invocation and dispatch, dynamic
transport plug-ins, TCP/IP and UDP/IP support, SSL-based security, a
firewall solution, and much more.

Ice is available under the terms of the GNU General Public License
(GPL) (see LICENSE file). Commercial licenses are available for
customers who wish to use Ice in proprietary products. Please contact
sales@zeroc.com for more information on licensing Ice.


About this distribution
-----------------------

This binary distribution provides all Ice run time services and 
development tools to build Ice applications in C++ using Visual
Studio 2005 for the x64 architecture.

If you want to develop Ice applications in another programming
language, or with another C++ compiler, please download the 
appropriate Ice binary distribution from the ZeroC web site at

  http://www.zeroc.com/download.html

This file describes how to setup Visual Studio for Ice and provides 
instructions for building and running the sample programs.


Supported Windows versions
--------------------------

This distribution is only supported on Windows Server 2003 x64 
Standard.


Setting up Visual Studio 2005 to build Ice applications in C++
--------------------------------------------------------------

Before you can use Ice in your C++ applications, you first need to
configure Visual Studio with the locations of the Ice header files,
libraries, and executables.

- In the IDE, choose Tools->Options->Projects and Solutions->VC++ Directories

- Select "Include files"

- Add <Ice installation root directory>\include

- Select "Library files"

- Add <Ice installation root directory>\lib

- Select "Executable files"

- Add <Ice installation root directory>\bin


Running IceGrid and Glacier2 components as services
---------------------------------------------------

The WINDOWS_SERVICES.txt file included in this distribution contains
information on how to install and run the IceGrid registry, IceGrid
node, and Glacier2 router as Windows services.


About ICE_HOME
--------------

To avoid conflicts with the Ice installers for other Windows
compilers, the default installation path for the Visual 2005 for x64
installer is c:\Ice-@ver@-VC80-x64. While most parts of this
installation are designed to work without changing your environment
other than as described later in this document, you will need to set
the ICE_HOME environment variable if you wish to run the iceca.bat
utility script found in Ice-@ver@-VC80-x64\bin.
 
To set the ICE_HOME environment variable from the command prompt,
simply run:

set ICE_HOME=<Ice installation root directory>

You can also set ICE_HOME through My Computer|Advanced System
Settings|Environment Variables.


Building and running the C++ demos
----------------------------------

The C++ demos are in the demo directory.

To build a C++ demo, start Visual Studio 2005 and open the solution
demo\demo.sln. Right click on the desired demo in the Solution
Explorer window and select "Build".

If you are using a multi-processor machine, parallel builds must be
disabled. Under Tools->Options->Projects and Solutions->Build and Run 
in the IDE set "maximum number of parallel project builds" to 1.

To run these demos, you need to add the Ice bin directory to your
PATH, as shown below:

set PATH=<Ice installation root directory>\bin;%PATH%

Next, open a Command Prompt, change to the desired demo subdirectory,
and type 'server' to start the server. In a separate Command Prompt
window, type 'client' to start the client.

Some demo directories contain README files if additional requirements
are necessary.

The Ice demo source code archive include examples for integrating Ice
with databases other than BerkeleyDB. These demos are not supported for
Visual Studio 2005 for 64 bit platforms and are not included in this
distribution.


Binary compatibility
--------------------

Ice patch releases are binary compatible. For example, Ice version 
<x>.<y>.1 is compatible with <x>.<y>.0: you can run an application 
built against Ice <x>.<y>.0 with Ice <x>.<y>.1 (or later) without 
having to recompile or relink this application.

Please refer to the RELEASE_NOTES.txt file included in this
distribution for detailed upgrade instructions.


Acknowledgments
---------------

This product includes software developed by the OpenSSL Project for
use in the OpenSSL Toolkit (http://www.openssl.org/).

This product includes cryptographic software written by Eric Young 
(eay@cryptsoft.com).
