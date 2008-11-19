// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

module Chat
{

/** Types used by the android chat service. */
module Android
{

/** A sequence of users. */
["java:type:java.util.ArrayList<String>:java.util.List<String>"] sequence<string> UserSeq;

/** Notifications of changes to the chat room state. */
interface ChatRoomListener
{
    /**
     *
     * Called when the listener is attached to the chat session. 
     * This provides the client with the initial list of users
     * currently in the chat room.
     *
     * @param users The names of users currently in the chat room.
     *
     **/
    void init(UserSeq users);
    
    /**
     *
     * Called when a new user joins the chat room. 
     *
     * @param name The name of the user that joined the chat room.
     *
     * @param timestamp The time at which the user joined the chat room.
     *
     **/
    void join(long timestamp, string name);

    /**
     *
     * Called when a user leaves the chat room. 
     *
     * @param name The name of the user that joined the chat room.
     *
     * @param timestamp The time at which the user joined the chat room.
     *
     **/
    void leave(long timestamp, string name);
    /**
     *
     * Called when a user sends a message. 
     *
     * @param name The name of the user that send the message.
     *
     * @param message The contents of the message.
     *
     * @param timestamp The time at which the message was sent.
     *
     **/
    void send(long timestamp, string name, string message);
    
    /**
     * 
     * Called upon an error when communicating with the chat room. Once this
     * method is called the chat session has been destroyed.
     *
     * @param ex A description of the error.
     *
     */
    void error(string ex);
    
    /**
     * 
     * Called if the chat room has been inactive for too long. Once this
     * method is called the chat session has been destroyed.
     */ 
    void inactivity();
};

/** Notifications of changes when establishing a chat session. */
interface SessionListener
{
	/** Called when a new session has been established. */
	void onLogin();
	
	/** Called to confirm a connection to a server. */
	void onConnectConfirm();
	
	/** Called if the establishment of a session failed.
	 *
	 * @param ex The error.
	 */
	void onException(string ex);
};

/** The interface to the chat service. */
interface Service
{
    /**
     * 
     * Set a new session listener.
     *
     * @param listener The new session listener.
     *
     * @return true if a login is already in progress, false otherwise.
     *
     */
	bool setSessionListener(SessionListener* listener);
	
	/**
     * 
     * Establish a session with the chat server.
     *
     * @param hostname The chat server host.
     *
     * @param username The username.
     *
     * @param password The password.
     *
     * @param secure True if a secure session should be established, false otherwise.
     *
     */
	void login(string hostname, string username, string password, bool secure);
	
	/**
     * 
     * Add a chat room listener.
     *
     * @param listener The listener.
     *
     * @param replay True if all received events should be replayed.
     *
     */
	bool addChatRoomListener(ChatRoomListener* listener, bool replay);

	/**
     * 
     * Remove a chat room listener.
     *
     * @param listener The listener.
     *
     */	
	void removeChatRoomListener(ChatRoomListener* listener);
	
	/**
     * 
     * Send a message to the chat room.
     *
     */	
	void send(string message);
	
	/**
     * 
     * Called to confirm a connection establishment.
     *
     * @param confirm True if the connection should be established, false otherwise.
     *
     */	
	void confirmConnection(bool confirm);
	
	/**
     * 
     * Called to logout from the current session.
     *
     */
	void logout();
};

};

};
