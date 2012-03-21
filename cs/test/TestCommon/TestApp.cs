using System;

#if SILVERLIGHT
using System.Collections.Generic;
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

            int args = Application.Current.Host.Source.OriginalString.IndexOf("?");
            Dictionary<string, string> properties = new Dictionary<string, string>();
            if(args > 0 && args + 1 < Application.Current.Host.Source.OriginalString.Length)
            {
                string[] props = Application.Current.Host.Source.OriginalString.Substring(args + 1).Split(';');
                foreach (string prop in props)
                {
                    int pos = prop.IndexOf('=');
                    if(pos > 0)
                    {
                        properties[prop.Substring(0, pos)] = prop.Substring(pos + 1);
                    }
                }
            }
           
            System.Threading.Thread t = new System.Threading.Thread(() =>
                {
                    Ice.Communicator communicator = null;
                    try
                    {
                        Ice.InitializationData initializationData = initData();
                        if (initializationData.properties == null)
                        {
                            initializationData.properties = Ice.Util.createProperties();
                            
                        }
                        
                        foreach(KeyValuePair<String,String> entry in properties)
                        {
                            initializationData.properties.setProperty(entry.Key, entry.Value);
                        }

                        communicator = Ice.Util.initialize(initializationData);
                        run(communicator);
                        completed();

                        // Exit the application if success
                        _btnRun.Dispatcher.BeginInvoke(delegate()
                                                       {
                                                           Application.Current.MainWindow.Close();
                                                       });
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
