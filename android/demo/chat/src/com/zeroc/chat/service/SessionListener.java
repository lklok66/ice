package com.zeroc.chat.service;

public interface SessionListener
{
    /** Called when a new session has been established. */
    void onLogin();

    /** Called to confirm a connection to a server. */
    void onConnectConfirm();

    /**
     * Called if the establishment of a session failed.
     * 
     * @param ex
     *            The error.
     */
    void onException(String ex);
}
