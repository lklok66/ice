This demo is a Cocoa client for the ZeroC Chat Demo, which is fully
described at http://www.zeroc.com/chat/index.html. This demo connects
to a single chat room, allowing you to chat with other chat room
participants.

By default, the demo connects to a chat server hosted on
demo.zeroc.com. The server accepts both secure and non-secure
connections through Glacier2. You may use any user name and password
when connecting.

If you would like to run your own server, or are otherwise interested
in the Chat Demo, the complete source is available at

http://www.zeroc.com/chat/download.html


SSL Notes
---------

If you intend to run this demo using a local chat server instead of
the public one provided by ZeroC, you will need to modify the Glacier2
configuration to use new certificates that are included in your Ice
Touch distribution.

In the file config/config.glacier2router from the Chat Demo package,
replace the following property definitions:

#
# SSL Configuration
#
Ice.Plugin.IceSSL=IceSSL:createIceSSL
IceSSL.DefaultDir=/opt/IceTouch-1.2.0/certs
IceSSL.CertAuthFile=cacert.pem
IceSSL.CertFile=s_rsa1024_pub.pem
IceSSL.KeyFile=s_rsa1024_priv.pem
IceSSL.VerifyPeer=0
