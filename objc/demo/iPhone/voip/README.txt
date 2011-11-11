This demo illustrates how to write an iPhone background application.
The demo only simulates how Ice might be be used for the control
channel of a VoIP application.

ZeroC hosts a server for this demo on demo2.zeroc.com. The iPhone
client connects to this host by default. The deployment on
demo2.zeroc.com accepts both secure and non-secure connections using
Glacier2.

After pressing the "Call Back" button the server will call
"incomingCall" on the Voip::Control callback proxy after the specified
delay. A local notification is pushed if the application is in the
background, otherwise an alert box is displayed.

Note that background applications are not supported in devices before
3G, and background notifications do not work in the simulator.

If you want to deploy your own VoIP server, the source code for the
server is included with the Ice distribution in the directory
java/demo/Glacier2/voip.


SSL Notes
---------

If you intend to run this demo using a local server instead of the
public one provided by ZeroC, you will need to modify the Glacier2
configuration to use new certificates. iOS 5 no longer supports X.509
certificates that use MD5 hashes. Since the certificates that are
included in the Ice distribution use MD5 hashes, the iPhone client 
will not be able to connect to a local Glacier2 server unless you 
replace the Glacier2 certificates with new ones that are included
in your Ice Touch distribution.

In the file java/demo/Glacier2/voip/config.glacier2, replace the
SSL configuration with the following property definitions:

#
# SSL Configuration
#
Ice.Plugin.IceSSL=IceSSL:createIceSSL
IceSSL.DefaultDir=/opt/IceTouch-1.2.0/certs
IceSSL.CertAuthFile=cacert.pem
IceSSL.CertFile=s_rsa1024_pub.pem
IceSSL.KeyFile=s_rsa1024_priv.pem
IceSSL.VerifyPeer=0
