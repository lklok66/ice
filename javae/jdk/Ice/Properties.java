// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice-E is licensed to you under the terms described in the
// ICEE_LICENSE file included in this distribution.
//
// **********************************************************************

package Ice;

public final class Properties
{
    public synchronized String
    getProperty(String key)
    {
        String result = (String)_properties.get(key);
        if(result == null)
        {
            result = System.getProperty(key);
        }
        if(result == null)
        {
            result = "";
        }
        return result;
    }

    public synchronized String
    getPropertyWithDefault(String key, String value)
    {
        String result = (String)_properties.get(key);
        if(result == null)
        {
            result = System.getProperty(key);
        }
        if(result == null)
        {
            result = value;
        }
        return result;
    }

    public int
    getPropertyAsInt(String key)
    {
        return getPropertyAsIntWithDefault(key, 0);
    }

    public synchronized int
    getPropertyAsIntWithDefault(String key, int value)
    {
        String result = (String)_properties.get(key);
        if(result == null)
        {
            result = System.getProperty(key);
        }
        if(result == null)
        {
            return value;
        }

        try
        {
            return Integer.parseInt(result);
        }
        catch(NumberFormatException ex)
        {
            return value;
        }
    }

    public synchronized java.util.Hashtable
    getPropertiesForPrefix(String prefix)
    {
        java.util.Hashtable result = new java.util.Hashtable();
        java.util.Enumeration p = _properties.keys();
        while(p.hasMoreElements())
        {
            String key = (String)p.nextElement();
            String value = (String)_properties.get(key);
            if(prefix.length() == 0 || key.startsWith(prefix))
            {
                result.put(key, value);
            }
        }
        return result;
    }

    public synchronized String[]
    getCommandLineOptions()
    {
        String[] result = new String[_properties.size()];
        java.util.Enumeration p = _properties.keys();
        int i = 0;
        while(p.hasMoreElements())
        {
            java.lang.Object key = p.nextElement();
            java.lang.Object value = _properties.get(key);
            result[i++] = "--" + key + "=" + value;
        }
        if(IceUtilInternal.Debug.ASSERT)
        {
            IceUtilInternal.Debug.Assert(i == result.length);
        }
        return result;
    }

    public synchronized String[]
    parseCommandLineOptions(String pfx, String[] options)
    {
        String prefix = pfx;
        if(pfx.length() > 0 && pfx.charAt(pfx.length() - 1) != '.')
        {
            pfx += '.';
        }
        pfx = "--" + pfx;

        java.util.Vector result  = new java.util.Vector();
        for(int i = 0; i < options.length; i++)
        {
            String opt = options[i];
            if(opt.startsWith(pfx))
            {
                if(opt.indexOf('=') == -1)
                {
                    opt += "=1";
                }

                parseLine(opt.substring(2));
            }
            else
            {
                result.addElement(opt);
            }
        }
        String[] arr = new String[result.size()];
        result.copyInto(arr);
        return arr;
    }

    public synchronized String[]
    parseIceCommandLineOptions(String[] options)
    {
        return parseCommandLineOptions("Ice", options);
    }

    public synchronized Properties
    _clone()
    {
        return new Properties(this);
    }

    Properties(Properties p)
    {
        java.util.Enumeration e = p._properties.keys();
        while(e.hasMoreElements())
        {
            java.lang.Object key = e.nextElement();
            java.lang.Object value = p._properties.get(key);
            _properties.put(key, value);
        }
    }

    Properties()
    {
    }

    Properties(StringSeqHolder args, Properties defaults)
    {
        if(defaults != null)
        {
            java.util.Hashtable m = defaults.getPropertiesForPrefix("");
            java.util.Enumeration e = m.keys();
            while(e.hasMoreElements())
            {
                java.lang.Object key = e.nextElement();
                java.lang.Object value = m.get(key);
                _properties.put(key, value);
            }
        }

        boolean loadConfigFiles = false;

        for(int i = 0; i < args.value.length; i++)
        {
            if(args.value[i].startsWith("--Ice.Config"))
            {
                String line = args.value[i];
                if(line.indexOf('=') == -1)
                {
                    line += "=1";
                }
                parseLine(line.substring(2));
                loadConfigFiles = true;
                String[] arr = new String[args.value.length - 1];
                System.arraycopy(args.value, 0, arr, 0, i);
                if(i < args.value.length - 1)
                {
                    System.arraycopy(args.value, i + 1, arr, i, args.value.length - i - 1);
                }
                args.value = arr;
            }
        }

        if(loadConfigFiles)
        {
            loadConfig();
        }

        args.value = parseIceCommandLineOptions(args.value);
    }

    public synchronized void
    setProperty(String key, String value)
    {
        //
        // Check if the property is legal. (We write to System.err instead of using
        // a logger because no logger may be established at the time the property
        // is parsed.)
        //
        if(key == null || key.length() == 0)
        {
            return;
        }

        //
        // Set or clear the property.
        //
        if(value != null && value.length() > 0)
        {
            _properties.put(key, value);
        }
        else
        {
            _properties.remove(key);
        }
    }

    private static final int ParseStateKey = 0;
    private static final int ParseStateValue = 1;

    private void
    parseLine(String line)
    {
        String key = "";
        String value = "";

        int state = ParseStateKey;

        String whitespace = "";
        String escapedspace = "";
        boolean finished = false;
        for(int i = 0; i < line.length(); ++i)
        {
            char c = line.charAt(i);
            switch(state)
            {
              case ParseStateKey:
              {
                  switch(c)
                  {
                    case '\\':
                      if(i < line.length() - 1)
                      {
                          c = line.charAt(++i);
                          switch(c)
                          {
                            case '\\':
                            case '#':
                            case '=':
                              key += whitespace;
                              whitespace = "";
                              key += c;
                              break;

                            case ' ':
                              if(key.length() != 0)
                              {
                                  whitespace += c;
                              }
                              break;

                            default:
                              key += whitespace;
                              whitespace = "";
                              key += '\\';
                              key += c;
                              break;
                          }
                      }
                      else
                      {
                          key += whitespace;
                          key += c;
                      }
                      break;

                    case ' ':
                    case '\t':
                    case '\r':
                    case '\n':
                        if(key.length() != 0)
                        {
                            whitespace += c;
                        }
                        break;

                    case '=':
                        whitespace = "";
                        state = ParseStateValue;
                        break;

                    case '#':
                        finished = true;
                        break;

                    default:
                        key += whitespace;
                        whitespace = "";
                        key += c;
                        break;
                  }
                  break;
              }

              case ParseStateValue:
              {
                  switch(c)
                  {
                    case '\\':
                      if(i < line.length() - 1)
                      {
                          c = line.charAt(++i);
                          switch(c)
                          {
                            case '\\':
                            case '#':
                            case '=':
                              value += value.length() == 0 ? escapedspace : whitespace;
                              whitespace = "";
                              escapedspace = "";
                              value += c;
                              break;

                            case ' ':
                              whitespace += c;
                              escapedspace += c;
                              break;

                            default:
                              value += value.length() == 0 ? escapedspace : whitespace;
                              whitespace = "";
                              escapedspace = "";
                              value += '\\';
                              value += c;
                              break;
                          }
                      }
                      else
                      {
                          value += value.length() == 0 ? escapedspace : whitespace;
                          value += c;
                      }
                      break;

                    case ' ':
                    case '\t':
                    case '\r':
                    case '\n':
                        if(value.length() != 0)
                        {
                            whitespace += c;
                        }
                        break;

                    case '#':
                        value += escapedspace;
                        finished = true;
                        break;

                    default:
                        value += value.length() == 0 ? escapedspace : whitespace;
                        whitespace = "";
                        escapedspace = "";
                        value += c;
                        break;
                  }
                  break;
              }
            }
            if(finished)
            {
                break;
            }
        }
        value += escapedspace;

        if((state == ParseStateKey && key.length() != 0) || (state == ParseStateValue && key.length() == 0) ||
           (key.length() == 0))
        {
            return;
        }

        setProperty(key, value);
    }

    private void
    loadConfig()
    {
        String value = getProperty("Ice.Config");

        if(value.equals("1"))
        {
            value = "";
        }

        if(value.length() > 0)
        {
            String[] files = IceUtilInternal.StringUtil.split(value, ",");
            for(int i = 0; i < files.length; i++)
            {
                load(files[i]);
            }
        }

        setProperty("Ice.Config", value);
    }

    public synchronized void
    load(String file)
    {
        try
        {
            java.io.FileReader fr = new java.io.FileReader(file);
            java.io.BufferedReader br = new java.io.BufferedReader(fr);
            String line;
            while((line = br.readLine()) != null)
            {
                parseLine(line);
            }
        }
        catch(java.io.IOException ex)
        {
            FileException se = new FileException();
            se.path = file;
            se.initCause(ex); // Exception chaining
            throw se;
        }
    }

    public synchronized void
    load(java.io.InputStream is)
    {
        try
        {
            java.io.BufferedReader reader = new java.io.BufferedReader(new java.io.InputStreamReader(is));
            String line;
            while((line = reader.readLine()) != null)
            {
                parseLine(line);
            }
        }
        catch(java.io.IOException ex)
        {
            FileException se = new FileException();
            se.path = is.toString();
            se.initCause(ex); // Exception chaining
            throw se;
        }
    }

    public java.lang.Object
    ice_clone()
    {
        return new Properties(this);
    }

    private java.util.Hashtable _properties = new java.util.Hashtable();
}
