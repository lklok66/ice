This demo demonstrates the use of a Glacier2 session helpers and
Silverlight to create a graphical client for the simple chat server.

First follow the instructions in the C++ Glacier2 chat demo README
to start the server and the Glacier2 router.

1) You must use the server from the C++ demo directory, before
   starting the Glacier2 router, you must edit the client endpoints to
   use a port in the range allowed by Silverlight.

   Edit the config.glacier2 file from the C++ chat demo directory and
   update the property Glacier2.Client.Endpoints as follow:

      Glacier2.Client.Endpoints=tcp -p 4502 -h 127.0.0.1
   
2) Follow the instructions in the C++ demo README to start the server
   and the Glacier2 router.

3) In a command window, start the policy server.

   > cd <Ice installation directory>\bin
   > policyserver 127.0.0.1 ..\config\PolicyResponse.xml

4) From Visual Studio open the `chat.Web' project and start the chat
   Silverlight application using the "Debug > Start new instance"
   command.

5) In the browser window click chatTestPage.html


==========================================================================
Using a web server as a policy server
==========================================================================

If you don't want to deploy a policy server you can use the web server
as a policy server.

1) copy PolicyResponse.xml to your web server document root directory
   and name it clientaccesspolicy.xml.

   > cd <Ice installation directory>\cs
   > copy config\PolicyResponse.xml C:\inetpub\wwwroot\clientaccesspolicy.xml

2) in Coordinator.cs ucomment the line that set the policy protocol
   //initData.properties.setProperty("Ice.ClientAccessPolicyProtocol", "Http");

3) Rebuild the demo and deploy it in the webserver.

* Note that the web server should serve the policy file in standard port 80