// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

using System.Collections;
using System.Collections.Generic;
using System.Diagnostics;
using System.Text.RegularExpressions;

namespace Ice
{
    sealed class PropertiesI :  Properties
    {
        class PropertyValue
        {
            public PropertyValue(string v, bool u)
            {
                val = v;
                used = u;
            }

            public string val;
            public bool used;
        }

        public string getProperty(string key)
        {
            lock(this)
            {
                string result = "";
                if(_properties.ContainsKey(key))
                {
                    PropertyValue pv = _properties[key];
                    pv.used = true;
                    result = pv.val;
                }
                return result;
            }
        }
        
        public string getPropertyWithDefault(string key, string val)
        {
            lock(this)
            {
                string result = val;
                if(_properties.ContainsKey(key))
                {
                    PropertyValue pv = _properties[key];
                    pv.used = true;
                    result = pv.val;
                }
                return result;
            }
        }
        
        public int getPropertyAsInt(string key)
        {
            return getPropertyAsIntWithDefault(key, 0);
        }
        
        public int getPropertyAsIntWithDefault(string key, int val)
        {
            lock(this)
            {
               
                if(_properties.ContainsKey(key))
                {
                    PropertyValue pv = _properties[key];
                    pv.used = true;
                    try
                    {
                        return System.Int32.Parse(pv.val);
                    }
                    catch(System.FormatException)
                    {
                        Ice.Util.getProcessLogger().warning("numeric property " + key + 
                                                            " set to non-numeric value, defaulting to " + val);
                        return val;
                    }
                }
                else
                {
                    return val;
                }
            }
        }
        
        public Dictionary<string, string> getPropertiesForPrefix(string prefix)
        {
            lock(this)
            {
                Dictionary<string, string> result = new Dictionary<string, string>();

                foreach(string s in _properties.Keys)
                {
                    if(prefix.Length == 0 || s.StartsWith(prefix))
                    {
                        PropertyValue pv = _properties[s];
                        pv.used = true;
                        result[s] = pv.val;
                    }
                }
                return result;
            }
        }
        
        public void setProperty(string key, string val)
        {
            if(key == null || key.Length == 0)
            {
                return;
            }

            //
            // Check if the property is legal.
            //
            Logger logger = Ice.Util.getProcessLogger();
            int dotPos = key.IndexOf('.');
            if(dotPos != -1)
            {
                string prefix = key.Substring(0, dotPos);
                for(int i = 0; IceInternal.PropertyNames.validProps[i] != null; ++i)
                {
                    string pattern = IceInternal.PropertyNames.validProps[i][0].pattern();
                    dotPos = pattern.IndexOf('.');
                    Debug.Assert(dotPos != -1);
                    string propPrefix = pattern.Substring(1, dotPos - 2);
                    if(!propPrefix.Equals(prefix))
                    {
                        continue;
                    }

                    bool found = false;
                    for(int j = 0; IceInternal.PropertyNames.validProps[i][j] != null && !found; ++j)
                    {
                        Regex r = new Regex(IceInternal.PropertyNames.validProps[i][j].pattern());
                        Match m = r.Match(key);
                        found = m.Success;
                        if(found && IceInternal.PropertyNames.validProps[i][j].deprecated())
                        {
                            logger.warning("deprecated property: " + key);
                            if(IceInternal.PropertyNames.validProps[i][j].deprecatedBy() != null)
                            {
                                key = IceInternal.PropertyNames.validProps[i][j].deprecatedBy();
                            }
                        }
                    }
                    if(!found)
                    {
                        logger.warning("unknown property: " + key);
                    }
                }
            }

            lock(this)
            {
                //
                //
                // Set or clear the property.
                //
                if(val != null && val.Length > 0)
                {
                    PropertyValue pv;
                    if(_properties.ContainsKey(key))
                    {
                        pv = _properties[key];
                        pv.val = val;
                    }
                    else
                    {
                        pv = new PropertyValue(val, false);
                    }
                    _properties[key] = pv;
                }
                else
                {
                    _properties.Remove(key);
                }
            }
        }
        
        public string[] getCommandLineOptions()
        {
            lock(this)
            {
                string[] result = new string[_properties.Count];
                int i = 0;
                foreach(KeyValuePair<string, PropertyValue> entry in _properties)
                {
                    result[i++] = "--" + entry.Key + "=" + entry.Value.val;
                }
                return result;
            }
        }

        public string[] parseCommandLineOptions(string pfx, string[] options)
        {
            if(pfx.Length > 0 && pfx[pfx.Length - 1] != '.')
            {
                pfx += '.';
            }
            pfx = "--" + pfx;

            List<string> result = new List<string>();
            for(int i = 0; i < options.Length; i++)
            {
                string opt = options[i];
                if(opt.StartsWith(pfx))
                {
                    if(opt.IndexOf('=') == -1)
                    {
                        opt += "=1";
                    }

                    parseLine(opt.Substring(2));
                }
                else
                {
                    result.Add(opt);
                }
            }
            string[] arr = new string[result.Count];
            if(arr.Length != 0)
            {
                result.CopyTo(arr);
            }
            return arr;
        }
        
        public string[] parseIceCommandLineOptions(string[] options)
        {
            string[] args = options;
            for(int i = 0; IceInternal.PropertyNames.clPropNames[i] != null; ++i)
            {
                args = parseCommandLineOptions(IceInternal.PropertyNames.clPropNames[i], args);
            }
            return args;
        }
        
        public void load(string file)
        {
            try
            {
                using(System.IO.StreamReader sr = new System.IO.StreamReader(file))
                {
                    parse(sr);
                }
            }
            catch(System.IO.IOException ex)
            {
                Ice.FileException fe = new Ice.FileException(ex);
                fe.path = file;
                throw fe;
            }
        }
        
        public Properties ice_clone_()
        {
            lock(this)
            {
                return new PropertiesI(this);
            }
        }

        public List<string> getUnusedProperties()
        {
            lock(this)
            {
                List<string> unused = new List<string>();
                foreach(KeyValuePair<string, PropertyValue> entry in _properties)
                {
                    if(!entry.Value.used)
                    {
                        unused.Add(entry.Key);
                    }
                }
                return unused;
            }
        }
        
        internal PropertiesI(PropertiesI p)
        {
            _properties = new Dictionary<string, PropertyValue>(p._properties);
        }

        internal PropertiesI()
        {
            _properties = new Dictionary<string, PropertyValue>();
        }
        
        internal PropertiesI(ref string[] args, Properties defaults)
        {
            if(defaults == null)
            {
                _properties = new Dictionary<string, PropertyValue>();
            }
            else
            {
                _properties = ((PropertiesI)defaults)._properties;
            }
            
            if(_properties.ContainsKey("Ice.ProgramName"))
            {
                PropertyValue pv = _properties["Ice.ProgramName"];
                pv.used = true;
            }
            else
            {
                _properties["Ice.ProgramName"] = new PropertyValue(System.AppDomain.CurrentDomain.FriendlyName, true);
            }

            bool loadConfigFiles = false;

            for(int i = 0; i < args.Length; i++)
            {
                if(args[i].StartsWith("--Ice.Config"))
                {
                    string line = args[i];
                    if(line.IndexOf('=') == -1)
                    {
                        line += "=1";
                    }
                    parseLine(line.Substring(2));
                    loadConfigFiles = true;

                    string[] arr = new string[args.Length - 1];
                    System.Array.Copy(args, 0, arr, 0, i);
                    if(i < args.Length - 1)
                    {
                        System.Array.Copy(args, i + 1, arr, i, args.Length - i - 1);
                    }
                    args = arr;
                }
            }

            if(!loadConfigFiles)
            {
                //
                // If Ice.Config is not set, load from ICE_CONFIG (if set)
                //
                loadConfigFiles = !_properties.ContainsKey("Ice.Config");
            }
            
            if(loadConfigFiles)
            {
                loadConfig();
            }
            
            args = parseIceCommandLineOptions(args); 
        }
        
        private void parse(System.IO.StreamReader input)
        {
            try
            {
                string line;
                while((line = input.ReadLine()) != null)
                {
                    parseLine(line);
                }
            }
            catch(System.IO.IOException ex)
            {
                SyscallException se = new SyscallException(ex);
                throw se;
            }
        }
        
        private void parseLine(string line)
        {
            string s = line;
            
            int hash = s.IndexOf('#');
            if(hash == 0)
            {
                return; // ignore comment lines
            }
            else if(hash != - 1)
            {
                s = s.Substring(0, (hash) - (0));
            }
            
            s = s.Trim();
            
            char[] arr = s.ToCharArray();
            int end = -1;
            for(int i = 0; i < arr.Length; i++)
            {
                if(arr[i] == ' ' || arr[i] == '\t' || arr[i] == '\r' || arr[i] == '\n' || arr[i] == '=')
                {
                    end = i;
                    break;
                }
            }
            if(end == -1)
            {
                return;
            }
            
            string key = s.Substring(0, end);
            
            end = s.IndexOf('=', end);
            if(end == -1)
            {
                return;
            }
            ++end;
            
            string val = "";
            if(end < s.Length)
            {
                val = s.Substring(end).Trim();
            }
            
            setProperty(key, val);
        }
        
        private void loadConfig()
        {
            string val = getProperty("Ice.Config");
            
            if(val.Length > 0)
            {
                char[] separator = { ',' };
                string[] files = val.Split(separator);
                for(int i = 0; i < files.Length; i++)
                {
                    load(files[i]);
                }
            }
            
            _properties["Ice.Config"] = new PropertyValue(val, true);
        }
        
        private Dictionary<string, PropertyValue> _properties;
    }
}
