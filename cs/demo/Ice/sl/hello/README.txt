This demo illustrates how to invoke ordinary (twoway) operations, 
as well as how to make oneway, and batched invocations.

1) You can use a server from any Ice language mapping. Before starting
   the server, you must edit the server's endpoints to use a port in
   the range allowed by Silverlight.

   Edit the config.server file in the server's demo directory and
   update the property Hello.Endpoints as follows:

     Hello.Endpoints=tcp -p 4502

2) Start the hello server according to the instructions in the demo
   README file.

3) In a command window, start the policy server:

   > cd <Ice installation directory>\bin
   > policyserver 127.0.0.1 ..\config\clientaccesspolicy.xml

4) In Visual Studio, open the `hello.Web' project and start the
   Silverlight client using the "Debug > Start new instance"
   command.

5) In the browser window, open helloTestPage.html


==========================================================================
Using a web server as a policy server
==========================================================================

If you do not want to deploy a policy server, you can use a web server
instead.

1) Copy clientaccesspolicy.xml to your web server document root directory.

   > cd <Ice installation directory>\cs
   > copy config\clientaccesspolicy.xml C:\inetpub\wwwroot\

2) In MainPage.xaml.cs, uncomment the line that sets the policy
   protocol:

   //initData.properties.setProperty("Ice.ClientAccessPolicyProtocol", "Http");

3) Rebuild the demo and deploy it in the web server.

Note that the web server should serve the policy file on standard
port 80.
