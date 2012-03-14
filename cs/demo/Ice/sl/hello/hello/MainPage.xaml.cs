// **********************************************************************
//
// Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using System.Collections.ObjectModel;
using Demo;

namespace hello
{
    public partial class MainPage : UserControl
    {
        public MainPage()
        {
            InitializeComponent();
            ObservableCollection<string> modes = new ObservableCollection<string>();
            modes.Add("Twoway");
            modes.Add("Oneway");
            modes.Add("Batch Oneway");
            cmbModes.DataContext = modes;
            cmbModes.SelectedIndex = 0;
            _communicator = Ice.Util.initialize();
        }

        private void timeoutValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            _timeout = Math.Round(e.NewValue * 1000, 0);
            txtTimeout.Text = _timeout.ToString() + " ms";
        }

        private void delayValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            _delay = Math.Round(e.NewValue * 1000, 0);
            txtDelay.Text = _delay.ToString() + " ms";
        }

        private void btnSayHelloClick(object sender, RoutedEventArgs e)
        {
            int mode = cmbModes.SelectedIndex;
            string host = txtHost.Text;
            txtOutput.Text = "";
            System.Threading.Thread t = new System.Threading.Thread(() =>
                {
                    try
                    {
                        Ice.ObjectPrx proxy = _communicator.stringToProxy("hello:tcp -h " + host + " -p 4502");

                        switch (mode)
                        {
                            case 0:
                                {
                                    proxy = proxy.ice_twoway();
                                    break;
                                }
                            case 1:
                                {
                                    proxy = proxy.ice_oneway();
                                    break;
                                }
                            case 2:
                                {
                                    proxy = proxy.ice_batchOneway();
                                    btnFlush.Dispatcher.BeginInvoke(delegate()
                                        {
                                            btnFlush.IsEnabled = true;
                                        });
                                    break;
                                }
                            default:
                                {
                                    throw new ArgumentException("Invalid Mode index " + mode.ToString());
                                }
                        }

                        if (_timeout == 0)
                        {
                            proxy = proxy.ice_timeout(-1);
                        }
                        else
                        {
                            proxy = proxy.ice_timeout((int)_timeout);
                        }
                        HelloPrx hello = HelloPrxHelper.uncheckedCast(proxy);
                        hello.sayHello((int)_delay);
                        txtOutput.Dispatcher.BeginInvoke(delegate()
                        {
                            appendText("OK" + Environment.NewLine);
                        });
                    }
                    catch(System.Exception ex)
                    {
                        appendText(ex.ToString());
                    }
                });
            t.Start();
        }

        private void btnFlushClick(object sender, RoutedEventArgs e)
        {
            System.Threading.Thread t = new System.Threading.Thread(() =>
            {
                try
                {
                    _communicator.flushBatchRequests();
                    btnFlush.Dispatcher.BeginInvoke(delegate()
                        {
                            btnFlush.IsEnabled = false;
                        });
                }
                catch (System.Exception ex)
                {
                    appendText(ex.ToString());
                }
            });
            t.Start();
        }

        private void btnShutdownClick(object sender, RoutedEventArgs e)
        {
            string host = txtHost.Text;
            txtOutput.Text = "";
            System.Threading.Thread t = new System.Threading.Thread(() =>
            {
                try
                {
                    Ice.ObjectPrx proxy = _communicator.stringToProxy("hello:tcp -h " + host + " -p 4502");
                    HelloPrx hello = HelloPrxHelper.uncheckedCast(proxy);
                    hello.shutdown();
                    appendText("Ok" + Environment.NewLine);
                }
                catch (System.Exception ex)
                {
                    appendText(ex.ToString());
                }
            });
            t.Start();
        }

        private void appendText(string text)
        {
            txtOutput.Dispatcher.BeginInvoke(delegate()
                {
                    txtOutput.Text += text;
                    txtOutput.SelectionLength = 0;
                    txtOutput.SelectionStart = txtOutput.Text.Length;
                });
        }

        private double _timeout = 0;
        private double _delay = 0;
        private Ice.Communicator _communicator;
    }
}
