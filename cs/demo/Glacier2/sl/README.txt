This demo demonstrates the use of a Glacier2 session helpers and 
Silverlight Framework to create a graphical client for the simple
chat server.

1) You must use the server from C++ demo direcotry, befofe start
   the Glacier2 router, you must edit the client endpoints to use 
   a port in the range allowed for silverlight.

   Edit config.glacier2 in C++ demo directory and update the property
   Glacier2.Client.Endpoints, update it to contain the following:

   Glacier2.Client.Endpoints=tcp -p 4502 -h 127.0.0.1
   
2) follow instructions in the C++ demo REAMDE to start the server
   and the Glacier2 router.

3) In a command window, start the policy server.

   > cd <IceHome>\bin
   > policyserver 127.0.0.1 ..\config\PolicyResponse.xml

4) From visual studio start chat.web project, using 
   "Debug > Start new instance" command.