package com.zeroc.chat;

import java.util.List;

public interface ChatRoomListener
{
    public void init(List<String> users);

    public void join(long timestamp, String name);

    public void leave(long timestamp, String name);

    public void send(long timestamp, String name, String message);
    
    public void exception(Ice.LocalException ex);
    
    public void exception(Ice.UserException ex);

    public void inactivity();
}
