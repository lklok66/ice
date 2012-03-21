using System;

#if SILVERLIGHT
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
#endif

namespace TestCommon
{
    public abstract class TestApp
    {
        protected static void test(bool b)
        {
            if(!b)
            {
                throw new System.Exception();
            }
        }

#if !SILVERLIGHT
        static
#endif
        public void Write(string msg)
        {
#if SILVERLIGHT
            _output.Dispatcher.BeginInvoke(delegate()
            {
                _output.Text += msg;
            });
#else
            Console.Out.Write(msg);
#endif
        }

#if !SILVERLIGHT
        static
#endif
        public void WriteLine(string msg)
        {
#if SILVERLIGHT
            Write(msg + Environment.NewLine);
#else
            Console.Out.WriteLine(msg);
#endif
        }

#if !SILVERLIGHT
        static
#endif
        public void Flush()
        { 
#if !SILVERLIGHT
            Console.Out.Flush();
#endif
        }

#if SILVERLIGHT
        public TestApp(TextBox output, Button btnRun)
        {
            _output = output;
            _btnRun = btnRun;
        }
        
        public abstract void run(Ice.Communicator communicator);

        public virtual Ice.InitializationData
        initData()
        {
            return new Ice.InitializationData();
        }

        public void main()
        {
            _output.Text = "";
            _btnRun.IsEnabled = false;
            System.Threading.Thread t = new System.Threading.Thread(() =>
                {
                    Ice.Communicator communicator = null;
                    try
                    {
                        communicator = Ice.Util.initialize(initData());
                        run(communicator);
                        completed();
                    }
                    catch(System.Exception ex)
                    {
                        failed(ex);
                    }
                    finally
                    {
                        if(communicator != null)
                        {
                            communicator.destroy();
                        }
                    }
                });
            t.Start();
        }

        public void completed()
        {
            _btnRun.Dispatcher.BeginInvoke(delegate()
            {
                _btnRun.IsEnabled = true;
            });
        }

        public void failed(System.Exception ex)
        {
            WriteLine(Environment.NewLine + "Test Failed:");
            WriteLine("Exception: " + ex.ToString());
            completed();
        }

        private TextBox _output;
        private Button _btnRun;
#endif
    }
}
