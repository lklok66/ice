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

namespace timeout
{
    public partial class MainPage : UserControl
    {
        public MainPage()
        {
            InitializeComponent();
            _app = new AllTests(txtOutput, btnRun);
        }

        private void run(object sender, RoutedEventArgs e)
        {
            _app.main();
        }

        private TestCommon.TestApp _app;
    }
}
