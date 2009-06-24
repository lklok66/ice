This demo illustrates how to route Ice requests from computers on an
isolated (not connected to the Internet) Wifi network to the Edge/3G
network of the iPhone.

The demo shows:

  - How to implement an Ice router. The Ice router implementation runs
    on the iPhone. Clients on the Wifi network establish a session
    with this router in order to be able to send Ice requests on the
    Internet through the iPhone Edge/3G network.
    
  - How to implement a custom Ice logger.

To try the router provided with this demo, you can use the Cocoa
library or chat clients. These clients can optionally connect to the
libray or chat servers using the iPhone router provided with this
demo.

You also need to configure the Wifi network appropriately on the
iPhone and computers that wish to route Ice requests through the
iPhone Edge/3G connection. The instructions below provide steps to
configure the iPhone and a Mac OS X computer whose Wifi device is
setup to create the Wifi network (no Wifi router is required).

On the Mac OS X computer, start the System Preferences and choose
"Network". Select the Airport interface and in the Network Name drop
down list select "Create Network...". Enter for example "iPhone
Network". This creates a new Wifi network named "iPhone
Network". Next, assign a fixed IP address for the Airport interface,
"192.168.1.2" for example.

On the iPhone, you need to setup the network to join the "iPhone
Network" Wifi network in Settings->Wi-Fi. Since no DHCP server is
setup for this network, you also need to manually assign an IP address
to the Wifi interface, we'll use "192.168.1.3".

The Mac OS X computer and the iPhone should now be able to
communicate. You can try pinging the iPhone from you Mac OS X
computer to verify it.

On the iPhone, run router built with the project from this
directory. The router application listens on port 12000 and on all
network interfaces by defaut.

You're now ready to try the Cocoa library or chat demo located in the
demo/Cocoa directory of your Ice Touch distribution. To use the iPhone
router, you need to check the "iPhone Router" checkbox and specify its
IP address (192.168.1.3) in the "Advanced" panel of the library or
chat client login dialog. The client will connect to the library or
chat server on the Internet through the iPhone 3G/Edge connection.

