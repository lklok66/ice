This demo illustrates how to invoke ordinary (twoway) operations, as
well as how to make oneway, datagram, secure, and batched invocations.

To test timeouts you can use the timeout slider to set a timeout on
the client proxy and the delay slider to set a delayed response in the
server to cause a timeout. In the case of a timeout, you will notice
that the server prints two "Hello World!" messages. This happens
because the Slice operation sayHello is marked as idempotent, meaning
that Ice does not need to preserve the at-most-once retry semantics.
See the manual for more information about retry behavior.

This demo can run against a hello server, such as the server in
objc/demo/Ice/hello. Note that to run this particular server you must
have previously built this demo with the Mac OS X version of Ice Touch
in a separate build tree.
