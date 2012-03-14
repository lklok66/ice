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

namespace defaultValue
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
