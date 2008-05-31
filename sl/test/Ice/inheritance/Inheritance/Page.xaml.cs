using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;

namespace Inheritance
{
    public partial class Page : UserControl
    {
        public Page()
        {
            InitializeComponent();
            Button1.MouseLeftButtonUp += new MouseButtonEventHandler(OnClick);

            _tb = Button2.Children[0] as TextBlock;
        }

        ~Page()
        {
            destroyCommunicator();
        }

        private void destroyCommunicator()
        {
            if(_comm != null)
            {
                try
                {
                    _comm.destroy();
                }
                catch (Exception ex)
                {
                    _tb.Text = ex.ToString();
                }
            }
        }

        private void OnClick(object sender, MouseEventArgs e)
        {
            destroyCommunicator();
            try
            {
                Ice.InitializationData initData = new Ice.InitializationData();
                initData.properties = Ice.Util.createProperties();
                initData.properties.setProperty("Ice.BridgeUri", "http://localhost:1287/IceBridge.ashx");
                _comm = Ice.Util.initialize(Button1.Dispatcher, initData);
            }
            catch (Exception ex)
            {
                _tb.Text = "Initialization failed with exception:\n" + ex.ToString();
            }
        
            var myThread = new System.Threading.Thread(() =>
            {
                try
                {
                    Test.InitialPrx initial = AllTests.allTests(_comm);
                    initial.shutdown();
                }
                catch (Exception ex)
                {
                    Button1.Dispatcher.BeginInvoke(delegate() { _tb.Text = "Test failed:\n" + ex.ToString(); });
                    return;
                }
                Button1.Dispatcher.BeginInvoke(delegate() { _tb.Text = "Test passed"; });
            });
            myThread.Start();
        }

        private Ice.Communicator _comm = null;
        private static TextBlock _tb;
    }
}
