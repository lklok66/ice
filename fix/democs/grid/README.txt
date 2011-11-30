This demo shows a more complex IceFIX deployment using IceGrid. It
configures IceFIX bridges for two trading partners named TP1 and TP2.

TP1 uses the QuickFIX configuration in "config.fix-tp1". This
configuration creates a file system store in the directory "store".
The associated Freeze databases are contained in "db-tp1".

TP2 is likewise configured, except the file suffixes are "tp2".

To run the demo, first start the IceGrid service:

> icegridnode --Ice.Config=config.grid

In a separate window:

> icegridadmin --Ice.Config=config.grid -e \
    "application add application.xml"

Messages will be displayed in the IceGrid service window.

Next start a FIX server, such as the demo FIX server in
fix\demo\server:

> ..\..\demo\server\server config.tp1

In a separate window, start the FIX server:

> ..\..\demo\server\server config.tp2

Now activate the bridges:

> icefixadmin --Ice.Config=config.admin -e activate

Finally, run the client:

> client --Ice.Config=config.client
