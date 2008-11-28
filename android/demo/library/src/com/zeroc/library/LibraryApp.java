package com.zeroc.library;

import com.zeroc.library.controller.LoginController;
import com.zeroc.library.controller.SessionController;

import android.app.Application;

public class LibraryApp extends Application
{
    private LoginController _loginController;

    @Override
    public void onCreate()
    {
    }

    @Override
    public void onTerminate()
    {
        if(_loginController != null)
        {
            _loginController.destroy();
            _loginController = null;
        }
    }
    
    public LoginController getLoginController()
    {
        return _loginController;
    }
    
    public void setLoginController(LoginController controller)
    {
        if(_loginController != null)
        {
            _loginController.destroy();
        }
        _loginController = controller;
    }

    public SessionController getSessionController()
    {
        if(_loginController != null)
        {
            return _loginController.getSessionController();
        }
        return null;
    }
    
    public void logout()
    {
        if(_loginController != null)
        {
            _loginController.destroy();
            _loginController = null;
        }
    }
}