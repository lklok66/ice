// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

import Demo.*;
import Freeze.*;

class Create extends Ice.Application
{
    public int
    run(String[] args)
    {
        if(args.length > 0)
        {
            System.err.println(appName() + ": too many arguments");
            return 1;
        }

        final String[] names = { "don", "ed", "frank", "gary", "arnold", "bob", "carlos" };
        final String[] phoneNumbers = { "(777)777-7777", "(666)666-6666", "(555)555-5555 x123", 
                                        "(444)444-4444", "(333)333-3333 x1234", "(222)222-2222", "(111)111-1111" };

        Connection connection = Util.createConnection(communicator(), "db");

        final java.util.Comparator less =
        new java.util.Comparator()
        {
            public int compare(Object o1, Object o2)
            {
                if(o1 == o2)
                {
                    return 0;
                }
                else if(o1 == null)
                {
                    return -((Comparable)o2).compareTo(o1);
                }
                else
                {
                    return ((Comparable)o1).compareTo(o2);
                }
            }
        };

        java.util.Map indexComparators = new java.util.HashMap();
        indexComparators.put("phoneNumber", less);
        Contacts contacts = new Contacts(connection, "contacts", true, less, indexComparators);
            

        //
        // Create a bunch of contacts within one transaction, and commit it
        //
        try
        {
            Transaction tx = connection.beginTransaction();

            for(int i = 0; i < names.length; ++i)
            {
                ContactData data = new ContactData(phoneNumbers[i]);
                contacts.put(names[i], data);
            }

            tx.commit();
        }
        finally
        {
            connection.close();
        }

        System.out.println(names.length + " contacts were successfully created or updated");
    
        return 0;
    }

    static public void
    main(String[] args)
    {
        Create app = new Create();
        app.main("demo.Freeze.transform.Create", args);
    }
}
