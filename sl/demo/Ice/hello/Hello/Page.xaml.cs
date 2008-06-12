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

namespace Hello
{
    public partial class Page : UserControl
    {
        public Page()
        {
            InitializeComponent();
            Button1.MouseLeftButtonUp += new MouseButtonEventHandler(OnClick);
            Button2.MouseLeftButtonUp += new MouseButtonEventHandler(OnClickOneway);
            Button3.MouseLeftButtonUp += new MouseButtonEventHandler(OnClickAMI);

            Button1.MouseLeftButtonDown += new MouseButtonEventHandler(OnPress);
            Button2.MouseLeftButtonDown += new MouseButtonEventHandler(OnPress);
            Button3.MouseLeftButtonDown += new MouseButtonEventHandler(OnPress);

            _tb = Button4.Children[0] as TextBlock;

            try
            {
                Ice.InitializationData initData = new Ice.InitializationData();
                initData.properties = Ice.Util.createProperties();
                initData.properties.setProperty("Ice.BridgeUri", "http://localhost:1287/IceBridge.ashx");
                _comm = Ice.Util.initialize(initData);
                _hello = Demo.HelloPrxHelper.uncheckedCast(_comm.stringToProxy("hello:tcp -p 10000"));
                _helloOneway = _hello.ice_oneway();
            }
            catch (Exception ex)
            {
                _tb.Text = "Initialization failed with exception:\n" + ex.ToString();
            }
        }

        ~Page()
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

        private void OnPress(object sender, MouseEventArgs e)
        {
            _tb.Text = "";
        }

        private void OnClick(object sender, MouseEventArgs e)
        {
            //
            // Send regular twoway call. Synchronous calls cannot be sent
            // from the UI thread.
            //
            var myThread = new System.Threading.Thread(() =>
            {
                try
                {
                    _hello.sayHello(0);
                    int id = System.Threading.Thread.CurrentThread.ManagedThreadId;
                    sayHelloResponse();
                }
                catch (Exception ex)
                {
                    sayHelloException(ex);
                }
            });
            myThread.Start();
        }

        void OnClickOneway(object sender, MouseEventArgs e)
        {
            //
            // Send regular twoway call. Synchronous calls cannot be sent
            // from the UI thread. This will return as soon as the request
            // as been sent and unblock the UI thread. However there is no
            // guarantee that the send has succeeded.
            //
            var myThread = new System.Threading.Thread(() =>
            {
                try
                {
                    _helloOneway.sayHello(0);
                    sayHelloResponse();
                }
                catch (Exception ex)
                {
                    sayHelloException(ex);
                }
            });
            myThread.Start();
        }

        void sayHelloResponse()
        {
            Button1.Dispatcher.BeginInvoke(delegate() { _tb.Text = "Call succeeded"; });
        }

        void sayHelloException(Ice.Exception ex)
        {
            Button1.Dispatcher.BeginInvoke(delegate() { _tb.Text = "Call failed with exception:\n" + ex.ToString(); });
        }

        void OnClickAMI(object sender, MouseEventArgs e)
        {
            try
            {
                _hello.sayHello_async(sayHelloResponse, sayHelloException, 0);
                _tb.Text = "Calling sayHello()...";
            }
            catch (Exception ex)
            {
                _tb.Text = "Call failed with exception:\n" + ex.ToString();
            }
        }

        private Ice.Communicator _comm = null;
        private Demo.HelloPrx _hello;
        private Demo.HelloPrx _helloOneway;
        private static TextBlock _tb;
    }
}
