This demo illustrates how to write an iPhone background application. The demo
only simulates how Ice might be be used for the control channel of a VoIP application.

ZeroC hosts a server for this demo on demo2.zeroc.com. The iPhone
client connects to this host by default. The deployment on
demo2.zeroc.com accepts both secure and non-secure connections using
Glacier2.

After pressing the "Call Back" button the server will call "incomingCall" on
the Voip::Control callback proxy after the specified delay. If the application
is in the background a local notification is pushed, otherwise an alert box
is displayed.

Note that background notifications do not work in the simulator.