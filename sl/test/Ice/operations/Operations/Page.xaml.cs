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

namespace Operations
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

                //
                // In this test, we need at least two threads in the
                // client side thread pool for nested AMI.
                //
                initData.properties.setProperty("Ice.ThreadPool.Client.Size", "2");
                initData.properties.setProperty("Ice.ThreadPool.Client.SizeWarn", "0");

                //
                // We must set MessageSizeMax to an explicit values,
                // because we run tests to check whether
                // Ice.MemoryLimitException is raised as expected.
                //
                initData.properties.setProperty("Ice.MessageSizeMax", "100");

                _comm = Ice.Util.initialize(initData);
            }
            catch (Exception ex)
            {
                _tb.Text = "Initialization failed with exception:\n" + ex.ToString();
            }
        
            var myThread = new System.Threading.Thread(() =>
            {
                try
                {
                    Button1.Dispatcher.BeginInvoke(delegate() { _tb.Text = "Testing..."; });
                    Test.MyClassPrx myClass = AllTests.allTests(_comm);
                    myClass.shutdown();
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
