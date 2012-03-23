This example demonstrates the use of the Glacier2 session helper
classes in a Silverlight client for the simple chat server.

1) You must use the server from the C++ demo directory. Before
   starting the Glacier2 router, you must modify the router's
   client endpoints to use a port in the range allowed by Silverlight.

   Edit the config.glacier2 file in the C++ chat demo directory and
   update the property Glacier2.Client.Endpoints as follows:

     Glacier2.Client.Endpoints=tcp -p 4502 -h 127.0.0.1

2) Follow the instructions in the C++ demo README to start the chat
   server and the Glacier2 router.

3) In a command window, start the policy server.

   > cd <Ice installation directory>\bin
   > policyserver 127.0.0.1 ..\config\PolicyResponse.xml

4) In Visual Studio, open the `chat.Web' project and start the
   Silverlight chat client using the "Debug > Start new instance"
   command.

5) In the browser window, open chatTestPage.html


==========================================================================
Using a web server as a policy server
==========================================================================

If you do not want to deploy a policy server, you can use a web server
instead.

1) Copy PolicyResponse.xml to your web server document root directory
   and name it clientaccesspolicy.xml.

   > cd <Ice installation directory>\cs
   > copy config\PolicyResponse.xml C:\inetpub\wwwroot\clientaccesspolicy.xml

2) In Coordinator.cs, uncomment the line that sets the policy
   protocol:

   //initData.properties.setProperty("Ice.ClientAccessPolicyProtocol", "Http");

3) Rebuild the demo and deploy it in the web server.

Note that the web server should serve the policy file on standard
port 80.
