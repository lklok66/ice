// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

module IceFIX
{

/** Raised if execute fails. */
exception ExecuteException
{
    string reason;
};

/** Used by clients to send FIX messages to their trading partner. */
interface Executor
{
    /**
     *
     * Send the given FIX message to the trading partner.
     *
     * @param data The FIX encoded message.
     *
     * @return The sequence number of this message.
     *
     * @raise ExecuteException if the message send failed.
     */
    int execute(string data)
        throws ExecuteException;

    /**
     *
     * Destroy the executor. This should be called before a client
     * disconnects.
     *
     */
    void destroy();
};

/** Used by the bridge to forward incoming messages from the acceptor to a client. */
interface Reporter
{
    /**
     *
     * Forwards a message to a client.
     *
     * @param data The FIX encoded message.
     *
     */
    ["ami"] void message(string data);
};

/** Raised if register/unregister fails. */
exception RegistrationException
{
    string reason;
};

/**  Information about a client. */
struct ClientInfo
{
    string id; /** The client id. */
    bool isConnected; /** Is the client active? */
};
/** A sequence of ClientInfo structures. */
sequence<ClientInfo> ClientInfoSeq;

/** Requested quality of service. */
dictionary<string, string> QoS;

/** The bridge status. */
enum BridgeStatus
{
    /** The bridge is inactive. */
    BridgeStatusNotActive,
    /** The bridge is active, and trying to login with the trading
     * partner. */
    BridgeStatusActive,
    /** The bridge is active, and currently logged into the trading
     * partner. */
    BridgeStatusLoggedOn
};

/** Used by administrators to manage the bridge. */
interface BridgeAdmin
{
    /**
     *
     * Activate the bridge. Once this has been called the bridge will
     * start trying to login to the trading partner.
     *
     **/
    void activate();

    /**
     *
     * Deactivate the bridge.
     *
     **/
    void deactivate();

    /**
     *
     * Remove any routing records older than timeout seconds.
     *
     * @param timeout The timeout in seconds.
     *
     * @param commit true if the records should be erased, false
     * otherwise.
     *
     * @return The number of records erased, or if commit is false the
     * number of records that would be erased.
     *
     */
    long clean(long timeout, bool commit);

    /**
     *
     * Return some statistics about the bridge database.
     *
     */
    string dbstat();

    /**
     *
     * Get the bridge status.
     *
     * @return the status.
     *
     **/
    BridgeStatus getStatus();    

    /**
     *
     * Unregister the given client id. If the client is active, or the
     * client has associated unrouted messages and force is false,
     * then unregister fails.
     *
     * @param id The id of the client to unregister.
     *
     * @param force If true erase the client even if there are queued
     * messages.
     *
     * @throws RegistrationException if the request cannot be fulfilled.
     *
     */
    void unregister(string id, bool force)
        throws RegistrationException;

    /**
     *
     * Register a client and return a new id.
     *
     * @param clientQoS The requested quality of service.
     *
     * @return The new client id.
     *
     * @throws RegistrationException if the client cannot register.
     *
     **/
    string register(QoS clientQoS)
        throws RegistrationException;
        
    /**
     *
     * Register a client with the given id.
     *
     * @param id The requested client id.
     *
     * @param clientQoS The requested quality of service.
     *
     * @throws RegistrationException if the client cannot register.
     *
     **/
    void registerWithId(string id, QoS clientQoS)
        throws RegistrationException;

    /**
     * 
     * Get list of clients.
     *
     * @return A sequence of clients.
     *
     **/
    ClientInfoSeq getClients();
};

/** Used by clients to register with the bridge. */
interface Bridge
{
    /**
     *
     * Retrieve the bridge admin.
     *
     * @return A proxy to the bridge admin.
     *
     **/
    BridgeAdmin* getAdmin();

    /**
     *
     * Connect a client with the given id.
     *
     * @param id The client id.
     *
     * @param cb The callback the client uses for receiving incoming
     * messages from the acceptor.
     *
     * @return The executor on which the client should call.
     *
     * @throws RegistrationException if the client cannot connect.
     *
     **/
    Executor* connect(string id, Reporter* cb)
        throws RegistrationException;
};

};
