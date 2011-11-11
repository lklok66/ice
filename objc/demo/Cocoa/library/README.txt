This demo is a Cocoa client for the Java library demo located at
demoj/Database/library in the Ice distribution. This application
provides a graphical interface to the library server. It permits you
to:

  - Create and delete books
  - Query for books by isbn, author or title
  - Rent and return books

ZeroC hosts a server for this demo on demo2.zeroc.com. The iPhone
client connects to this host by default. The deployment on
demo2.zeroc.com accepts both secure and non-secure connections using
Glacier2. Direct connections to the library session manager are not
permitted on demo2.zeroc.com.


SSL Notes
---------

If you intend to run this demo using a local server instead of the
public one provided by ZeroC, you will need to modify the SSL
configurations for Glacier2 and the server to use new certificates
that are included in your Ice Touch distribution.

In the file demoj/Database/library/config.glacier2, replace the
following property definitions:

#
# SSL Configuration
#
Ice.Plugin.IceSSL=IceSSL:createIceSSL
IceSSL.DefaultDir=/opt/IceTouch-1.2.0/certs
IceSSL.CertAuthFile=cacert.pem
IceSSL.CertFile=s_rsa1024_pub.pem
IceSSL.KeyFile=s_rsa1024_priv.pem
IceSSL.VerifyPeer=0

In the file demoj/Database/library/config.server, replace the
following property definitions:

#
# SSL Configuration
#
Ice.Plugin.IceSSL=IceSSL.PluginFactory
IceSSL.VerifyPeer=0
IceSSL.DefaultDir=/opt/IceTouch-1.2.0/certs
IceSSL.Keystore=server.jks
IceSSL.Password=password
