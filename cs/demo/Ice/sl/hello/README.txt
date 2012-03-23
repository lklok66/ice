This demo illustrates how to invoke ordinary (twoway) operations, 
as well as how to make oneway, and batched invocations.

1) You can use a server from any Ice language mapping, before starting
   the server, you must edit the server endpoints to use a port in the
   range allowed by Silverlight.

   Edit the config.server file from the server hello demo directory
   and update the property Hello.Endpoints as follow:

      Hello.Endpoints=tcp -p 4502

2) start the hello server following the instructions in the demo
   README file.

3) In a command window, start the policy server.

   > cd <Ice installation directory>\bin
   > policyserver 127.0.0.1 ..\config\PolicyResponse.xml

4) From Visual Studio open the `hello.Web' project and start the hello
   Silverlight application using the "Debug > Start new instance"
   command.

5) In the browser window click helloTestPage.html

==========================================================================
Using a web server as a policy server
==========================================================================

If you don't want to deploy a policy server you can use the web server
as a policy server.

1) copy PolicyResponse.xml to your web server document root directory
   and name it clientaccesspolicy.xml.

   > cd <Ice installation directory>\cs
   > copy config\PolicyResponse.xml C:\inetpub\wwwroot\clientaccesspolicy.xml

2) in MainPage.xaml.cs ucomment the line that set the policy protocol
   //initData.properties.setProperty("Ice.ClientAccessPolicyProtocol", "Http");

3) Rebuild the demo and deploy it in the webserver.

* Note that the web server should serve the policy file in standard port 80