This demo shows how to backup an IceGrid-deployed IceFIX database.

The demo configures a single IceFIX bridge for a trading partner
named TP1. TP1 uses the QuickFIX configuration in "config.fix". This
configuration creates a file system store in the directory "store".
The associated Freeze database is contained in "db-tp1".

To run the demo, first start the IceGrid service:

> icegridnode --Ice.Config=config.grid

In a separate window:

> icegridadmin --Ice.Config=config.grid -e \
    "application add application.xml"

Messages will be displayed in the IceGrid service window.

Next start a FIX server, such as the demo FIX server in
fix\demo\server:

> ..\server\server config.tp1

Now activate the IceFIX bridge:

> icefixadmin --Ice.Config=config.admin -e activate

Finally, run the grid client:

$ ..\grid\client --Ice.Config=config.client


Backup
------

While the IceFIX bridge is running, you can perform a hot backup
using the batch file named "backup". This file illustrates how to use
the Berkeley DB db_hotbackup utility. For a full description of
db_hotbackup, please refer to:

  http://www.oracle.com/technology/documentation/berkeley-db/db/utility/db_hotbackup.html

The following command creates a full backup (both data and log files)
in the 'hotbackup' directory:

> backup full

Alternatively, you can perform an incremental backup, which only moves
and copies log files to the 'hotbackup' directory:

> backup incremental

Note that the IceFIX bridge configuration disables the automatic
deletion of old log files:

Freeze.DbEnv.backup.OldLogsAutoDelete=0

The configuration also sets DbPrivate to 0 to allow db_archive to
access the database environment while the client is running:

Freeze.DbEnv.backup.DbPrivate=0

The file db-tp1/DB_CONFIG contains directives that put the data files
and log files in different subdirectories, which simplifies archiving.


Recovery
--------

After a successful backup, the hotbackup directory always contains a
full backup that is ready to use. To switch over to this backup, you
only need to do the following:

 1. Make sure the IceFIX bridge is not running.

    > icegridadmin --Ice.Config=config.grid -e "server stop TP1"

 2. Move the old 'db-tp1' directory out of the way:

    > move db-tp1 db-tp1.old

 3. Move or copy the hotbackup directory to db-tp1:

    > xcopy /s hotbackup db-tp1\

If you have recent uncorrupted log files in your original db-tp1
directory, you can also copy these log files over your restored log
files and perform a catastrophic recovery to recover these updates:

> xcopy /y db-tp1.old\logs\log.* db-tp1\logs
> db_recover -c -h db-tp1

Finally, restart the IceFIX bridge.

> icegridadmin --Ice.Config=config.grid -e "server start TP1"
