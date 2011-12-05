This demo shows a simple IceFIX client.

To run the demo, first start a FIX server such as the demo FIX server
in fix\demo\server:

> ..\..\demo\server\server config.cfg

In a separate window, start the IceFIX server:

> icebox --Ice.Config=config.icebox

In a separate window, activate the IceFIX server:

> icefixadmin --Ice.Config=config.admin -e activate

Finally, run the client:

> client --Ice.Config=config.client
