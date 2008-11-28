package com.zeroc.library.controller;

interface SessionAdapter
{
    void destroy();
    void refresh();
    Demo.LibraryPrx getLibrary();
}
