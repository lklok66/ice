This demo shows how to use bidirectional connections for callbacks.
This is typically used if the server cannot open a connection to the
client to send callbacks, for example, because firewalls block
incoming connections to the client.

1) You can use a server from any Ice language mapping, befofe start
   the server, you must edit the server endpoints to use a port
   in the range allowed for silverlight.

   Edit config.server in demo directory and update the property
   Callback.Server.Endpoints, update it to contain the following:

   Callback.Server.Endpoints=tcp -p 4502
   
2) start the bidir server, following instructions in the demo REAMDE

3) In a command window, start the policy server.

   > cd <IceHome>\bin
   > policyserver 127.0.0.1 ..\config\PolicyResponse.xml

4) From visual studio start bidir.web project, using 
   "Debug > Start new instance" command.