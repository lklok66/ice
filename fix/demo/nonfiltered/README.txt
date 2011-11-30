This demo shows a simple nonfiltered IceFIX client.

To run the demo, first start a FIX server such as the demo FIX server
in fix\demo\server:

> ..\server\server config.cfg

In a separate window, start the IceFIX server:

> icebox --Ice.Config=config.icebox

In a separate window, activate the IceFIX server:

> icefixadmin --Ice.Config=config.admin -e activate

Finally, run the client:

> client --Ice.Config=config.client

The client will print any received messages. Press ^C to terminate the
client. To see some messages, run the simple client (in ..\simple) and
submit some orders.
