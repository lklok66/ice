This demo is an iPhone client for the java library demo located at
demoj/Database/library in the Ice distribution. This application
provides a graphical interface to the library server. It permits you to:

  - Create and delete books
  - Query for books by isbn, author or title
  - Rent and return books

ZeroC hosts a server for this demo on demo2.zeroc.com. The iPhone
client connects to this host by default. The deployment on
demo2.zeroc.com accepts both secure and non-secure connections using
Glacier2. Direct connections to the library session manager are not
permitted on demo2.zeroc.com.

Note that SSL is not avaible when running the demo in the iPhone simulator.
