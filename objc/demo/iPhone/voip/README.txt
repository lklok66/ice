This demo illustrates how to write an iPhone background application.
The demo only simulates how Ice might be be used for the control
channel of a VoIP application.

ZeroC hosts a server for this demo on demo2.zeroc.com. The iPhone
client connects to this host by default. The deployment on
demo2.zeroc.com accepts both secure and non-secure connections using
Glacier2.

If you want to deploy your own VoIP server, the source code for the 
server is included in Ice in the directory "java/demo/Glacier2/voip"

After pressing the "Call Back" button the server will call
"incomingCall" on the Voip::Control callback proxy after the specified
delay. A local notification is pushed if the application is in the
background, otherwise an alert box is displayed.

Note that background notifications do not work in the simulator.
