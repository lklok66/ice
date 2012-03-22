// **********************************************************************
//
// Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

using System;
using System.Text;
using System.Collections.Generic;
using System.ComponentModel;

using EnvDTE;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System.IO;
using System.Diagnostics;
using Extensibility;
using EnvDTE80;
using Microsoft.VisualStudio;
using Microsoft.VisualStudio.CommandBars;
using Microsoft.VisualStudio.VCProjectEngine;
using Microsoft.VisualStudio.VCProject;
using Microsoft.VisualStudio.Shell;
using Microsoft.VisualStudio.Shell.Interop;
using System.Resources;
using System.Reflection;
using VSLangProj;
using System.Globalization;

using System.Collections;
using System.Runtime.InteropServices.ComTypes;
using Microsoft.CSharp;
using System.Xml;

namespace Ice.VisualStudio
{
    public interface LinkerAdapter
    {
        String AdditionalDependencies
        {
            get;
            set;
        }

        String AdditionalLibraryDirectories
        {
            get;
            set;
        }
    }

    public class DynamicLinkerAdapter : LinkerAdapter
    {
        public DynamicLinkerAdapter(VCLinkerTool linkerTool)
        {
            _linkerTool = linkerTool;
        }

        public String AdditionalDependencies
        {
            get
            {
                return _linkerTool.AdditionalDependencies;
            }

            set
            {
                _linkerTool.AdditionalDependencies = value;
            }
        }

        public String AdditionalLibraryDirectories
        {
            get
            {
                return _linkerTool.AdditionalLibraryDirectories;
            }

            set
            {
                _linkerTool.AdditionalLibraryDirectories = value;
            }
        }

        private VCLinkerTool _linkerTool;
    }

    public class StaticLinkerAdapter : LinkerAdapter
    {
        public StaticLinkerAdapter(VCLibrarianTool librarianTool)
        {
            _librarianTool = librarianTool;
        }

        public String AdditionalDependencies
        {
            get
            {
                return "";
            }

            set
            {
            }
        }

        public String AdditionalLibraryDirectories
        {
            get
            {
                return "";
            }

            set
            {
            }
        }

        private VCLibrarianTool _librarianTool;
    }

    public class ComponentList : List<string>
    {
        public ComponentList()
        {
        }

        public ComponentList(string[] values)
        {
            foreach(string s in values)
            {
                Add(s);
            }
        }

        public ComponentList(string value)
        {
            if(value == null)
            {
                value = "";
            }
            init(value, ';');
        }

        public ComponentList(string value, char separator)
        {
            init(value, separator);
        }

        public new void Add(string value)
        {
            value = value.Trim();
            if(String.IsNullOrEmpty(value))
            {
                return;
            }
            if(!base.Contains(value))
            {
                base.Add(value);
            }
        }

        public new bool Contains(string value)
        {
            string found = base.Find(delegate(string s)
                                    {
                                        return s.Equals(value.Trim(), StringComparison.CurrentCultureIgnoreCase);
                                    });
            return found != null;
        }

        public new bool Remove(string value)
        {
            value = value.Trim();
            if(Contains(value))
            {
                return base.Remove(value);
            }
            return false;
        }

        private void init(string value, char separator)
        {
            Array items = value.Split(separator);
            foreach(string s in items)
            {
                Add(s);
            }
        }

        public override string ToString()
        {
            return ToString(';');
        }

        public string ToString(char separator)
        {
            if(this.Count == 0)
            {
                return "";
            }
            StringBuilder sb = new StringBuilder();
            for(int cont = 0; cont < this.Count; ++cont)
            {
                sb.Append(this[cont]);
                if(cont < this.Count - 1)
                {
                    sb.Append(separator);
                }
            }
            return sb.ToString();
        }

        public bool Equal(ComponentList other)
        {
            if(this.Count != other.Count)
            {
                return false;
            }
            bool equal = true;
            for(int i = 0; i < this.Count; ++i)
            {
                string val1 = this[i];
                string val2 = other[i];
                if(val1 == null && val2 == null)
                {
                    continue;
                }

                if(val1 == null || val2 == null)
                {
                    equal = false;
                    break;
                }

                if(!val1.Equals(val2, StringComparison.CurrentCultureIgnoreCase))
                {
                    equal = false;
                    break;
                }
            }
            return equal;
        }
    }

    public class IncludePathList : ComponentList
    {
        public IncludePathList()
            : base()
        {
        }

        public IncludePathList(string[] values)
            : base(values)
        {
        }

        public IncludePathList(string value)
            : base(value, '|')
        {
        }

        public override string ToString()
        {
            return base.ToString('|');
        }

        public bool Contains(Project project, string value)
        {
            string path = Util.absolutePath(project, value);
            string found = base.Find(delegate(string s)
            {
                string other = Util.absolutePath(project, s);
                return path.Equals(other, StringComparison.CurrentCultureIgnoreCase);
            });
            return found != null;
        }
    }

    public static class Util
    {
        public enum msgLevel{ msgError, msgInfo, msgDebug };

        public const string slice2cs = "slice2cs.exe";
        public const string slice2cpp = "slice2cpp.exe";
        public const string slice2sl = "slice2sl.exe";

        //
        // Property names used to persist project configuration.
        //
        public const string PropertyIce = "ZerocIce_Enabled";
        public const string PropertyIceOutputDir = "ZerocIce_OutputDir";
        public const string PropertyIceHeaderExt = "ZerocIce_HeaderExt";
        public const string PropertyIceSourceExt = "ZerocIce_SourceExt";
        public const string PropertyIceComponents = "ZerocIce_Components";
        public const string PropertyIceExtraOptions = "ZerocIce_ExtraOptions";
        public const string PropertyIceIncludePath = "ZerocIce_IncludePath";
        public const string PropertyIceStreaming = "ZerocIce_Streaming";
        public const string PropertyIceChecksum = "ZerocIce_Checksum";
        public const string PropertyIceTie = "ZerocIce_Tie";
        public const string PropertyIcePrefix = "ZerocIce_Prefix";
        public const string PropertyIceDllExport = "ZerocIce_DllExport";
        public const string PropertyVerboseLevel = "ZerocIce_VerboseLevel";

        private static readonly string[] silverlightNames =
        {
            "IceSL"
        };

        public static string[] getSilverlightNames()
        {
            return (string[])silverlightNames.Clone();
        }

        private static readonly string[] cppNames =
        {
            "Freeze", "Glacier2", "Ice", "IceBox", "IceGrid", "IcePatch2", 
            "IceSSL", "IceStorm", "IceUtil" 
        };

        public static string[] getCppNames()
        {
            return (string[])cppNames.Clone();
        }

        private static readonly string[] dotNetNames =
        {
            "Glacier2", "Ice", "IceBox", "IceGrid", "IcePatch2", 
            "IceSSL", "IceStorm"
        };

        private static readonly string[] dotNetCompactNames =
        {
            "Glacier2", "Ice", "IceBox", "IceGrid", "IcePatch2", 
            "IceStorm"
        };

        public static string[] getDotNetCompactNames()
        {
            return (string[])dotNetCompactNames.Clone();
        }

        public static string[] getDotNetNames()
        {
            return (string[])dotNetNames.Clone();
        }

        public static string getIceSlHome()
        {
            //
            // For Silverlight projects Ice Home is always set to the value of ICE_SL_HOME
            // environment variable.
            //
            return Environment.ExpandEnvironmentVariables("%ICE_SL_HOME%");
        }

        public static string getIceHome()
        {
            string defaultIceHome = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);
            if(defaultIceHome.EndsWith("\\vsaddin", StringComparison.CurrentCultureIgnoreCase))
            {
                defaultIceHome = defaultIceHome.Substring(0, defaultIceHome.Length - "\\vsaddin".Length);
            }
            return defaultIceHome;
        }

        public static string getProjectOutputDirRaw(Project project)
        {
            return getProjectProperty(project, Util.PropertyIceOutputDir, "", false);
        }

        public static string getProjectOutputDir(Project project)
        {
            String path = getProjectOutputDirRaw(project);
            if(containsEnvironmentVars(path))
            {
                path = expandEnvironmentVars(path);
            }
            return path;
        }
        
        public static string getProjectAbsoluteOutputDir(Project project)
        {
            String path = getProjectOutputDir(project);
            if(!Path.IsPathRooted(path))
            {
                path = Util.absolutePath(project, path);
            }
            return path;
        }

        public static VCPropertySheet findPropertySheet(IVCCollection propertySheets, string sheetName)
        {
            VCPropertySheet value = null;
            foreach(VCPropertySheet sheet in propertySheets)
            {
                if(sheet == null || String.IsNullOrEmpty(sheet.Name))
                {
                    continue;
                }
                if(sheet.Name == sheetName)
                {
                    value = sheet;
                    break;
                }
            }
            return value;
        }

        //
        // This will add the Ice property sheet to the project.
        //
        public static void addIcePropertySheet(Project project)
        {
            VCProject vcProj = (VCProject)project.Object;

            string propSheetFileName = "$(ALLUSERSPROFILE)\\ZeroC\\ice";

#if VS2008
            propSheetFileName += ".vsprops";
#endif

#if VS2010
            propSheetFileName += ".props";
#endif


            //
            // All project configurations must include ice.vsprops (vc90) or ice.props (vc100)
            //
            IVCCollection configurations = (IVCCollection)vcProj.Configurations;
            foreach(VCConfiguration vcConfig in configurations)
            {
                VCPropertySheet newSheet = findPropertySheet(vcConfig.PropertySheets as IVCCollection, "ice");
                if(newSheet == null)
                {
#if VS2008
                    string inhertiedPropertySheets = vcConfig.InheritedPropertySheets;
                    if(String.IsNullOrEmpty(inhertiedPropertySheets) || !inhertiedPropertySheets.Contains(propSheetFileName))
                    {
                        if(!String.IsNullOrEmpty(inhertiedPropertySheets) && !inhertiedPropertySheets.EndsWith(";"))
                        {
                            inhertiedPropertySheets += " ; ";
                        }
                        inhertiedPropertySheets += propSheetFileName;
                        vcConfig.InheritedPropertySheets = inhertiedPropertySheets;
                    }
#endif

#if VS2010
                    newSheet = vcConfig.AddPropertySheet(propSheetFileName);
#endif

                }
            }
        }

        public static string getCsBinDir(Project project)
        {
            string binDir = "";
            if(isVBSmartDeviceProject(project) || isCSharpSmartDeviceProject(project))
            {
                binDir = _csCompactFrameworkBinDirs;
            }
            else
            {
                binDir = _csBinDirs;
            }
            return Path.Combine(Util.getIceHome(), binDir);
        }

        public static string getPathRelativeToProject(ProjectItem item)
        {
            StringBuilder path = new StringBuilder();
            if(item != null)
            {
                path.Append(Util.getPathRelativeToProject(item, item.ContainingProject.ProjectItems));
            }
            return path.ToString();
        }

        public static string getPathRelativeToProject(ProjectItem item, ProjectItems items)
        {
            StringBuilder path = new StringBuilder();
            foreach(ProjectItem i in items)
            {
                if(i == item)
                {
                    if(path.Length > 0)
                    {
                        path.Append("\\");
                    }
                    path.Append(i.Name);
                    break;
                }
                else if(Util.isProjectItemFilter(i) || Util.isProjectItemFolder(i))
                {
                    string token = Util.getPathRelativeToProject(item, i.ProjectItems);
                    if(!String.IsNullOrEmpty(token))
                    {
                        path.Append(i.Name);
                        path.Append("\\");
                        path.Append(token);
                        break;
                    }
                }
            }
            return path.ToString();
        }

        private static readonly string iceIncludeDir = "$(IceHome)\\include";

        public static bool addCppIncludes(VCCLCompilerTool tool, Project project)
        {
            if(tool == null || project == null)
            {
                return false;
            }


            bool changed = false;
            ComponentList includes = new ComponentList(tool.AdditionalIncludeDirectories);
            

            if(!includes.Contains(iceIncludeDir) && !includes.Contains(quote(iceIncludeDir)))
            {
                changed = true;
                includes.Add(quote(iceIncludeDir));
            }

            string outputDir = getProjectOutputDirRaw(project);
            if(outputDir.Equals(""))
            {
                outputDir = ".";
            }
            
            if(!includes.Contains(outputDir) && !includes.Contains(quote(outputDir)))
            {
                changed = true;
                includes.Add(outputDir);
            }
            
            if(changed)
            {
                tool.AdditionalIncludeDirectories = includes.ToString();
            }
            return changed;
        }

        public static void removeCppIncludes(VCCLCompilerTool tool, string iceHome, string generatedDir)
        {
            if(tool == null || String.IsNullOrEmpty(tool.AdditionalIncludeDirectories))
            {
                return;
            }

            bool changed = false;

            ComponentList includes = new ComponentList(tool.AdditionalIncludeDirectories);

            if(includes.Remove(quote(iceHome + "\\include")))
            {
                changed = true;
            }

            if(includes.Remove(iceHome + "\\include"))
            {
                changed = true;
            }

            if(!generatedDir.Equals("."))
            {
                if(includes.Remove(generatedDir))
                {
                    changed = true;
                }
            }

            if(changed)
            {
                tool.AdditionalIncludeDirectories = includes.ToString();
            }
        }

        public static void removeIcePropertySheet(VCConfiguration configuration)
        {
            if(configuration == null)
            {
                return;
            }

#if VS2008
            ComponentList sheets = new ComponentList(configuration.InheritedPropertySheets);
            if(sheets.Remove("$(ALLUSERSPROFILE)\\ZeroC\\ice.vsprops"))
            {
                configuration.InheritedPropertySheets = sheets.ToString();
            }
#endif
#if VS2010
            VCPropertySheet sheet = null;
            IVCCollection sheets = (IVCCollection)configuration.PropertySheets;
            foreach(VCPropertySheet s in sheets)
            {
                if(!s.PropertySheetFile.Equals(configuration.Evaluate("$(ALLUSERSPROFILE)\\ZeroC\\ice.props"),
                                               StringComparison.CurrentCultureIgnoreCase))
                {
                    continue;
                }
                sheet = s;
                break;
            }

            if(sheet != null)
            {
                configuration.RemovePropertySheet(sheet);
            }
#endif
        }

        private static readonly string _csBinDirs = "\\bin\\";
        private static readonly string _csCompactFrameworkBinDirs = "\\bin\\cf\\";

        public static bool addDotNetReference(Project project, string component, string iceHome, bool development)
        {
            if(project == null || String.IsNullOrEmpty(component))
            {
                return false;
            }

            string csBinDir = null;
            if(isVBSmartDeviceProject(project) || isCSharpSmartDeviceProject(project))
            {
                csBinDir = _csCompactFrameworkBinDirs;
            }
            else
            {
                csBinDir = _csBinDirs;
            }


            string reference = iceHome + csBinDir + component + ".dll";
            if (File.Exists(reference))
            {
                VSLangProj.VSProject vsProject = (VSLangProj.VSProject)project.Object;
                try
                {
                    Reference r = vsProject.References.Add(reference);
                    if (development)
                    {
                        r.CopyLocal = false;
                    }
                    return true;
                }
                catch (COMException ex)
                {
                    Console.WriteLine(ex);
                }
            }

            MessageBox.Show("Could not locate '" + component +
                            ".dll'. in '" + Path.GetDirectoryName(reference) + "'",
                            "Ice Visual Studio Add-in", MessageBoxButtons.OK,
                            MessageBoxIcon.Error,
                            MessageBoxDefaultButton.Button1,
                            (MessageBoxOptions)0);
            return false;
        }

        public static bool removeDotNetReference(Project project, string component)
        {
            if(project == null || String.IsNullOrEmpty(component))
            {
                return false;
            }

            foreach(Reference r in ((VSProject)project.Object).References)
            {
                if(r.Name.Equals(component, StringComparison.OrdinalIgnoreCase))
                {
                    r.Remove();
                    return true;
                }
            }
            return false;
        }

        public static void addCppLib(LinkerAdapter tool, string component, bool debug)
        {
            if(tool == null || String.IsNullOrEmpty(component))
            {
                return;
            }

            if(Array.BinarySearch(Util.getCppNames(), component, StringComparer.CurrentCultureIgnoreCase) < 0)
            {
                return;
            }

            string libName = component;
            if(debug)
            {
                libName += "d";
            }
            libName += ".lib";

            string additionalDependencies = tool.AdditionalDependencies;
            if(String.IsNullOrEmpty(additionalDependencies))
            {
                additionalDependencies = "";
            }

            ComponentList components = new ComponentList(additionalDependencies.Split(' '));
            if(!components.Contains(libName))
            {
                components.Add(libName);
                additionalDependencies = components.ToString(' ');
                tool.AdditionalDependencies = additionalDependencies;
            }
        }

        public static bool removeCppLib(LinkerAdapter tool, string component, bool debug)
        {
            if(tool == null || String.IsNullOrEmpty(tool.AdditionalDependencies))
            {
                return false;
            }

            string libName = component;
            if(debug)
            {
                libName += "d";
            }
            libName += ".lib";

            ComponentList components = new ComponentList(tool.AdditionalDependencies.Split(' '));
            if(components.Contains(libName))
            {
                components.Remove(libName);
                tool.AdditionalDependencies = components.ToString(' ');
                return true;
            }
            return false;
        }

        //
        // Add the Ice bin path to the debug environment.
        //
        // Note: Only the last setting in the environment has effect.
        //
        public static void addIceCppEnvironment(VCDebugSettings debugSettings, Project project,
                                                bool x64)
        {
            if(debugSettings == null || project == null)
            {
                return;
            }
            String value = "PATH=" + cppBinDir(project, x64);
            if(String.IsNullOrEmpty(debugSettings.Environment))
            {
                debugSettings.Environment = value;
                return;
            }
            if(value.Equals(debugSettings.Environment))
            {
                return;
            }

            if(String.IsNullOrEmpty(debugSettings.Environment))
            {
                debugSettings.Environment = value;
                return;
            }

            ComponentList envs = new ComponentList(debugSettings.Environment, '\n');

            string path = "";

            //
            // Find the last in the list that begins: "PATH=" accounting for case and whitespace.
            //
            int index = -1;
            for(int i = 0; i < envs.Count; ++i)
            {
                string s = envs[i].Trim();
                if(s.StartsWith("PATH", StringComparison.CurrentCultureIgnoreCase))
                {
                    if(s.Substring("PATH".Length).Trim().StartsWith("=", StringComparison.CurrentCultureIgnoreCase))
                    {
                        path = s;
                        index = i;
                    }
                }
            }

            if(index == -1)
            {
                envs.Add("PATH=" + cppBinDir(project, x64));
            }
            else
            {
                string binDir = cppBinDir(project, x64);
                ComponentList paths = new ComponentList(assignmentValue(path), ';');
                while(paths.Contains(binDir))
                {
                    paths.Remove(binDir);
                }
                path = "PATH=" + binDir + Path.PathSeparator + paths.ToString(Path.PathSeparator);
                path = path.TrimEnd(Path.PathSeparator);
                envs[index] = path;
            }

            value = envs.ToString('\n');
            if(!debugSettings.Environment.Equals(value))
            {
                debugSettings.Environment = value;
            }
            return;
        }

        private static string removeFromPath(string path, string dir)
        {
            ComponentList list = new ComponentList(path.Split(Path.PathSeparator));
            while(list.Contains(dir))
            {
                list.Remove(dir);
            }
            return list.ToString(Path.PathSeparator);
        }

        private static string assignmentValue(string expr)
        {
            int i = expr.IndexOf('=');
            if(i < 0)
            {
                return "";
            }
            try
            {
                return expr.Substring(i).Substring(1).Trim();
            }
            catch(ArgumentOutOfRangeException)
            {
                return "";
            }
        }

        private static string prependToPath(string path, string dir)
        {
            path = removeFromPath(path, dir);
            return dir + Path.PathSeparator + path;
        }

        public static string cppBinDir(Project project, bool x64)
        {
            string cppBinDir = Path.Combine("$(IceHome)", "bin");
#if VS2010
            cppBinDir = Path.Combine(cppBinDir, "vc100");
#endif
            if(x64)
            {
                cppBinDir = Path.Combine(cppBinDir, "x64");
            }
            return cppBinDir;
        }

        public static void removeIceCppEnvironment(VCDebugSettings debugSettings, string iceHome)
        {
            if(debugSettings == null || String.IsNullOrEmpty(debugSettings.Environment))
            {
                return;
            }

            string[] _cppBinDirs =
            {
                "bin",
                "bin\\x64",
                "bin\\vc100",
                "bin\\vc100\\x64",
                "cpp\\bin",
            };

            ComponentList envs = new ComponentList(debugSettings.Environment, '\n');
            /* Find the last in the list that begins: "PATH=" accounting for case and whitespace. */
            string path = "";
            int index = -1;
            for(int i = 0; i < envs.Count; ++i)
            {
                string s = envs[i];
                if(s.StartsWith("PATH", StringComparison.CurrentCultureIgnoreCase))
                {
                    if(s.Substring("PATH".Length).Trim().StartsWith("=", StringComparison.CurrentCultureIgnoreCase))
                    {
                        path = s;
                        index = i;
                    }
                }
            }

            if(index == -1)
            {
                return;
            }

            foreach(string dir in _cppBinDirs)
            {
                path = "PATH=" + removeFromPath(assignmentValue(path).Trim(), Path.Combine(iceHome, dir));
            }

            if(path.Equals("PATH="))
            {
                envs.RemoveAt(index);
            }
            else
            {
                envs[index] = path;
            }

            String value = envs.ToString();
            if(!debugSettings.Environment.Equals(value))
            {
                debugSettings.Environment = value;
            }
            return;
        }

        public static bool addIceCppLibraryDir(LinkerAdapter tool, Project project, bool x64)
        {
            if(tool == null || project == null)
            {
                return false;
            }

            string iceLibDir = "$(IceHome)\\lib";
#if VS2010
            iceLibDir += "\\vc100";
#endif
            if(x64)
            {
                iceLibDir += "\\x64";
            }

            string additionalLibraryDirectories = tool.AdditionalLibraryDirectories;
            if(String.IsNullOrEmpty(additionalLibraryDirectories))
            {
                tool.AdditionalLibraryDirectories = quote(iceLibDir);
                return true;
            }

            //
            // Remove Ice lib directories that doesn't match the current ice lib dir.
            //

            ComponentList libs = new ComponentList(additionalLibraryDirectories);
            ComponentList remove = new ComponentList();

            foreach(string lib in libs)
            {
                if(lib.Equals(iceLibDir) ||
                   lib.Equals(quote(iceLibDir)))
                {
                    continue;
                }
                if(lib.StartsWith("$(IceHome)") ||
                   lib.StartsWith("\"$(IceHome)"))
                {
                    remove.Add(lib);
                }            
            }

            bool changed = false;
            foreach(string lib in remove)
            {
                if(libs.Remove(lib))
                {
                    changed = true;
                }
            }

            if(!libs.Contains(iceLibDir) && 
               !libs.Contains(quote(iceLibDir)))
            {
                libs.Add(quote(iceLibDir));
                changed = true;
            }

            if(changed)
            {
                tool.AdditionalLibraryDirectories = libs.ToString();
            }

            return changed;
        }

        public static void removeIceCppLibraryDir(LinkerAdapter tool, string iceHome)
        {
            if(tool == null || String.IsNullOrEmpty(tool.AdditionalLibraryDirectories))
            {
                return;
            }

            string[] _cppLibDirs =
            {
                "lib",
                "lib\\x64",
                "lib\\vc100",
                "lib\\vc100\\x64"
            };

            bool changed = false;
            ComponentList libs = new ComponentList(tool.AdditionalLibraryDirectories);
            foreach(string dir in _cppLibDirs)
            {
                if(libs.Remove(quote(Path.Combine(iceHome, dir))))
                {
                    changed = true;
                }
                if(libs.Remove(Path.Combine(iceHome, dir)))
                {
                    changed = true;
                }
            }
            if(changed)
            {
                tool.AdditionalLibraryDirectories = libs.ToString();
            }
        }

        public static bool isSliceFilename(string s)
        {
            return s != null && s.EndsWith(".ice", StringComparison.CurrentCultureIgnoreCase);
        }

        public static bool equalPath(string p1, string p2, string basePath)
        {
            if(p1 == p2)
            {
                return true;
            }

            if(String.IsNullOrEmpty(p1) || String.IsNullOrEmpty(p2))
            {
                return false;
            }

            //
            // Convert both paths to absolute paths if necessary
            //
            if(!Path.IsPathRooted(p1))
            {
                p1 = Path.Combine(basePath, p1);
            }

            if(!Path.IsPathRooted(p2))
            {
                p2 = Path.Combine(basePath, p2);
            }

            try
            {
                //
                // Note that in Windows white space at the beginning or end of a file are ignored.
                // When comparing the filenames "Foo ", " Foo", and "foo", all refer to the same file.
                //
                // We also need to trim / (directory separator) from the end in case it's present.
                // 
                return 0 == String.Compare(
                                Path.GetFullPath(p1).Trim().TrimEnd(Path.DirectorySeparatorChar),
                                Path.GetFullPath(p2).Trim().TrimEnd(Path.DirectorySeparatorChar),
                                StringComparison.CurrentCultureIgnoreCase);
            }
            catch(ArgumentException)
            {
            }
            catch(NotSupportedException)
            {
            }
            catch(System.Security.SecurityException)
            {
            }
            catch(PathTooLongException)
            {
            }
            return false;
        }

        public static bool isSliceBuilderEnabled(Project project)
        {
            return Util.getProjectPropertyAsBool(project, Util.PropertyIce);
        }

        public static bool isCSharpProject(Project project)
        {
            if(project == null)
            {
                return false;
            }

            if(String.IsNullOrEmpty(project.Kind))
            {
                return false;
            }

            return project.Kind == VSLangProj.PrjKind.prjKindCSharpProject;
        }

        public static bool isCSharpSmartDeviceProject(Project project)
        {
            return hasProjecType(project, vsSmartDeviceCSharp);
        }

        public static bool isVBSmartDeviceProject(Project project)
        {
            return hasProjecType(project, vsSmartDeviceVB);
        }

        public static bool isVBProject(Project project)
        {
            if(project == null)
            {
                return false;
            }

            if(String.IsNullOrEmpty(project.Kind))
            {
                return false;
            }

            return project.Kind == VSLangProj.PrjKind.prjKindVBProject;
        }

        public static bool isSilverlightProject(Project project)
        {
            if(!Util.isCSharpProject(project))
            {
                return false;
            }

            Array extenders = (Array)project.ExtenderNames;
            foreach(string s in extenders)
            {
                if(String.IsNullOrEmpty(s))
                {
                    continue;
                }
                if(s.Equals("SilverlightProject"))
                {
                    return true;
                }
            }
            return false;
        }

        public static bool isCppProject(Project project)
        {
            if(project == null)
            {
                return false;
            }

            if(String.IsNullOrEmpty(project.Kind))
            {
                return false;
            }
            return project.Kind == vcContextGuids.vcContextGuidVCProject;
        }

        public static bool isProjectItemFolder(ProjectItem item)
        {
            if(item == null)
            {
                return false;
            }

            if(String.IsNullOrEmpty(item.Kind))
            {
                return false;
            }
            return item.Kind == "{6BB5F8EF-4483-11D3-8BCF-00C04F8EC28C}";
        }

        public static bool isProjectItemFilter(ProjectItem item)
        {
            if(item == null)
            {
                return false;
            }

            if(String.IsNullOrEmpty(item.Kind))
            {
                return false;
            }
            return item.Kind == "{6BB5F8F0-4483-11D3-8BCF-00C04F8EC28C}";
        }

        public static bool isProjectItemFile(ProjectItem item)
        {
            if(item == null)
            {
                return false;
            }

            if(String.IsNullOrEmpty(item.Kind))
            {
                return false;
            }
            return item.Kind == "{6BB5F8EE-4483-11D3-8BCF-00C04F8EC28C}";
        }

        public static bool hasItemNamed(ProjectItems items, string name)
        {
            bool found = false;
            foreach(ProjectItem item in items)
            {
                if(item == null)
                {
                    continue;
                }

                if(item.Name == null)
                {
                    continue;
                }

                if(item.Name.Equals(name, StringComparison.OrdinalIgnoreCase))
                {
                    found = true;
                    break;
                }
            }
            return found;
        }

        public static ProjectItem findItem(string path)
        {
            Builder builder = Connect.getBuilder();
            DTE dte = builder.getCurrentDTE();
            if(dte == null)
            {
                return null;
            }
            if(dte.Solution == null)
            {
                return null;
            }

            List<Project> projects = Util.getProjects(dte.Solution);
            ProjectItem item = null;
            foreach(Project project in projects)
            {
                item = findItem(path, project.ProjectItems);
                if(item != null)
                {
                    break;
                }
            }
            return item;
        }

        public static ProjectItem findItem(string path, ProjectItems items)
        {
            if(String.IsNullOrEmpty(path))
            {
                return null;
            }
            ProjectItem item = null;
            foreach(ProjectItem i in items)
            {
                if(i == null)
                {
                    continue;
                }
                else if(Util.isProjectItemFile(i))
                {
                    string fullPath = i.Properties.Item("FullPath").Value.ToString();
                    Project project = i.ContainingProject;
                    if(Util.equalPath(fullPath, path, Path.GetDirectoryName(project.FileName)))
                    {
                        item = i;
                        break;
                    }
                }
                else if(Util.isProjectItemFolder(i))
                {
                    string p = Path.GetDirectoryName(i.Properties.Item("FullPath").Value.ToString());
                    Project project = i.ContainingProject;
                    if(Util.equalPath(p, path, Path.GetDirectoryName(project.FileName)))
                    {
                        item = i;
                        break;
                    }

                    item = findItem(path, i.ProjectItems);
                    if(item != null)
                    {
                        break;
                    }
                }
                else if(Util.isProjectItemFilter(i))
                {
                    string p = Path.GetFullPath(Path.Combine(Path.GetDirectoryName(i.ContainingProject.FileName),
                                            Util.getPathRelativeToProject(i)));

                    Project project = i.ContainingProject;
                    if(Util.equalPath(p, path, Path.GetDirectoryName(project.FileName)))
                    {
                        item = i;
                        break;
                    }

                    item = findItem(path, i.ProjectItems);
                    if(item != null)
                    {
                        break;
                    }
                }
            }
            return item;
        }

        public static VCFile findVCFile(IVCCollection files, string name, string fullPath)
        {
            VCFile vcFile = null;
            foreach(VCFile file in files)
            {
                if(file.ItemName == name)
                {
                    Project project = (Project)((VCProject) file.project).Object;
                    if(!Util.equalPath(file.FullPath, fullPath, Path.GetDirectoryName(project.FileName)))
                    {
                        file.Remove();
                        break;
                    }
                    vcFile = file;
                    break;
                }
            }
            return vcFile;
        }

        public static string relativePath(Project project, string absoluteFilePath)
        {
            if(absoluteFilePath == null)
            {
                return "";
            }

            if(project == null)
            {
                return absoluteFilePath;
            }

            return relativePath(Path.GetDirectoryName(project.FileName), absoluteFilePath);
        }

        public static string relativePath(string mainDirPath, string absoluteFilePath)
        {
            if(absoluteFilePath == null)
            {
                return "";
            }

            if(mainDirPath == null)
            {
                return absoluteFilePath;
            }

            if(!Path.IsPathRooted(absoluteFilePath))
            {
                return absoluteFilePath;
            }

            mainDirPath = Path.GetFullPath(mainDirPath).Trim(Path.DirectorySeparatorChar);
            absoluteFilePath = Path.GetFullPath(absoluteFilePath).Trim(Path.DirectorySeparatorChar);

            string[] firstPathParts = mainDirPath.Split(Path.DirectorySeparatorChar);
            string[] secondPathParts = absoluteFilePath.Split(Path.DirectorySeparatorChar);

            int sameCounter = 0;
            while(sameCounter < Math.Min(firstPathParts.Length, secondPathParts.Length) &&
                String.Equals(firstPathParts[sameCounter], secondPathParts[sameCounter],
                StringComparison.CurrentCultureIgnoreCase))
            {
                ++sameCounter;
            }

            // Different volumes, relative path not possible.
            if(sameCounter == 0)
            {
                return absoluteFilePath;
            }

            // Pop back up to the common point.
            string newPath = "." + Path.DirectorySeparatorChar;
            for(int i = sameCounter; i < firstPathParts.Length; ++i)
            {
                newPath += ".." + Path.DirectorySeparatorChar;
            }

            // Descend to the target.
            for(int i = sameCounter; i < secondPathParts.Length; ++i)
            {
                newPath += secondPathParts[i] + Path.DirectorySeparatorChar;
            }
            return newPath.TrimEnd(Path.DirectorySeparatorChar);
        }

        // Relative paths are relative to project.
        // Inverse of Util.relativePath().
        public static string absolutePath(Project project, string path)
        {
            if(Path.IsPathRooted(path)) // If path is absolute return that path
            {
                return path;
            }
            return Path.GetFullPath(Path.Combine(Path.GetDirectoryName(project.FileName), path)).TrimEnd(
                                                                                         Path.DirectorySeparatorChar);
        }

        public static Project getSelectedProject(_DTE dte)
        {
            Microsoft.VisualStudio.Shell.ServiceProvider sp = new Microsoft.VisualStudio.Shell.ServiceProvider(
                getCurrentDTE() as Microsoft.VisualStudio.OLE.Interop.IServiceProvider);
            IVsMonitorSelection selectionMonitor = sp.GetService(typeof(IVsMonitorSelection)) as IVsMonitorSelection;

            //
            // There isn't an open project.
            //
            if(selectionMonitor == null)
            {
                return null;
            }

            Project project = null;
            IntPtr ppHier;
            uint pitemid;
            IVsMultiItemSelect ppMIS;
            IntPtr ppSC;
            if(ErrorHandler.Failed(selectionMonitor.GetCurrentSelection(out ppHier, out pitemid, out ppMIS, out ppSC)))
            {
                return null;
            }

            if(ppHier != IntPtr.Zero)
            {
                IVsHierarchy hier = (IVsHierarchy)Marshal.GetObjectForIUnknown(ppHier);
                Marshal.Release(ppHier);
                object obj;
                hier.GetProperty(VSConstants.VSITEMID_ROOT, (int)__VSHPROPID.VSHPROPID_ExtObject, out obj);
                if(obj != null)
                {
                    project = obj as EnvDTE.Project;
                }
            }

            if(ppSC != IntPtr.Zero)
            {
                Marshal.Release(ppSC);
            }

            return project;
        }

        public static ProjectItem getSelectedProjectItem(_DTE dte)
        {
            Microsoft.VisualStudio.Shell.ServiceProvider sp = new Microsoft.VisualStudio.Shell.ServiceProvider(
                getCurrentDTE() as Microsoft.VisualStudio.OLE.Interop.IServiceProvider);
            IVsMonitorSelection selectionMonitor = sp.GetService(typeof(IVsMonitorSelection)) as IVsMonitorSelection;

            //
            // There isn't an open project.
            //
            if(selectionMonitor == null)
            {
                return null;
            }

            ProjectItem projectItem = null;
            IntPtr ppHier;
            uint pitemid;
            IVsMultiItemSelect ppMIS;
            IntPtr ppSC;
            if(ErrorHandler.Failed(selectionMonitor.GetCurrentSelection(out ppHier, out pitemid, out ppMIS, out ppSC)))
            {
                return null;
            }

            if(ppHier != IntPtr.Zero)
            {
                IVsHierarchy hier = (IVsHierarchy)Marshal.GetObjectForIUnknown(ppHier);
                Marshal.Release(ppHier);
                object obj;
                hier.GetProperty(pitemid, (int)__VSHPROPID.VSHPROPID_ExtObject, out obj);
                if(obj != null)
                {
                    projectItem = obj as EnvDTE.ProjectItem;
                }
            }

            if(ppSC != IntPtr.Zero)
            {
                Marshal.Release(ppSC);
            }

            return projectItem;
        }

        public static bool updateOutputDir(Project project, String outputDir)
        {
            //
            // Note that ouputDir could be empty, which means the project dir will
            // be used as the output directory, and is also the default.
            //
            if(project == null || outputDir == null)
            {
                return false;
            }

            Util.cleanProject(project, true);

            string oldOutputDir = getProjectAbsoluteOutputDir(project);
            
            string projectDir = Path.GetDirectoryName(project.FileName);

            // Remove old output directory if necessary.
            if(!equalPath(oldOutputDir, projectDir, projectDir) && 
               isEmptyDir(oldOutputDir))
            {
                // In C# projects, diretories are project items, but not in C++ projects.
                if(isCSharpProject(project))
                {
                    ProjectItem item = findItem(oldOutputDir, project.ProjectItems);
                    if(item != null)
                    {
                        item.Delete();
                    }
                }
                if(Directory.Exists(oldOutputDir))
                {
                    Directory.Delete(oldOutputDir, true);
                }
            }

            if(isCppProject(project))
            {
                // We must remove old output directory from C++ project include path.
                removeCppIncludes(project);
            }

            Util.setProjectProperty(project, Util.PropertyIceOutputDir, outputDir);

            if(isCppProject(project))
            {
                // We must add the new output directory to C++ project include path.
                addCppIncludes(project);
            }

            return true;
        }

        public static void removeCppIncludes(Project project)
        {
            if(!isCppProject(project))
            {
                return;
            }

            VCProject vcProject = (VCProject)project.Object;
            IVCCollection configurations = (IVCCollection)vcProject.Configurations;
            foreach(VCConfiguration conf in configurations)
            {
                if(conf != null)
                {
                    VCCLCompilerTool compilerTool =
                        (VCCLCompilerTool)(((IVCCollection)conf.Tools).Item("VCCLCompilerTool"));
                    Util.removeCppIncludes(compilerTool, "$(IceHome)", Util.getProjectOutputDirRaw(project));
                }
            }
        }

        public static void addCppIncludes(Project project)
        {
            if(!isCppProject(project))
            {
                return;
            }

            VCProject vcProject = (VCProject)project.Object;
            IVCCollection configurations = (IVCCollection)vcProject.Configurations;
            foreach(VCConfiguration conf in configurations)
            {
                if(conf != null)
                {
                    VCCLCompilerTool compilerTool =
                        (VCCLCompilerTool)(((IVCCollection)conf.Tools).Item("VCCLCompilerTool"));
                    Util.addCppIncludes(compilerTool, project);
                }
            }
        }

        //
        // Return true if the directory doesn't contain any
        // files. We use this to check if it is safe to delete
        // the output-dir after the user changes the setting.
        //
        public static bool isEmptyDir(String path)
        {
            if(String.IsNullOrEmpty(path))
            {
                return false;
            }

            if(!Directory.Exists(path))
            {
                return false;
            }

            DirectoryInfo dir = new DirectoryInfo(path);
            FileInfo[] files = dir.GetFiles();
            if(files.Length == 0)
            {
                return true;
            }
            bool empty = true;
            foreach(FileInfo f in files)
            {
                if(File.Exists(f.FullName))
                {
                    empty = false;
                    break;
                }
                else if(Directory.Exists(f.FullName))
                {
                    empty = isEmptyDir(f.FullName);
                    if(!empty)
                    {
                        break;
                    }
                }
            }
            return empty;
        }



        //
        // The CopyLocal property doesn't work consistently, as sometimes it is set to false
        // when the reference isn't found. This happens when project demos are fisrt 
        // opened, as at this point the reference path has not been fixed to use the
        // correct IceHome value. This method returns the private metadata of a 
        // Reference from the project file; this value doesn't change as does CopyLocal.
        //

#if VS2010
        //
        // This method requires .NET 4. Microsoft.Build.BuildEngine is deprecated 
        // in .NET 4, so this method uses the new API Microsoft.Build.Evaluation.
        //
        private static bool getCopyLocal(Project project, string name)
        {
            Microsoft.Build.Evaluation.ProjectItem referenceItem = null;

            Microsoft.Build.Evaluation.Project p =
                Microsoft.Build.Evaluation.ProjectCollection.GlobalProjectCollection.LoadProject(project.FullName);

            foreach(Microsoft.Build.Evaluation.ProjectItem item in p.Items)
            {
                if(!item.ItemType.Equals("Reference"))
                {
                    continue;
                }

                if(!item.HasMetadata("private"))
                {
                    continue;
                }

                string[] tokens = item.EvaluatedInclude.Split(',');
                if(tokens.Length <= 0)
                {
                    continue;
                }

                if(!tokens[0].Trim().Equals(name, StringComparison.CurrentCultureIgnoreCase))
                {
                    continue;
                }

                referenceItem = item;

                if(referenceItem != null)
                {
                    break;
                }
            }

            if(referenceItem != null)
            {
                return referenceItem.GetMetadata("Private").EvaluatedValue.Equals(true.ToString(), 
                    StringComparison.CurrentCultureIgnoreCase);
            }
            return true;
        }
#elif VS2008
        //
        // This method uses the .NET 3.5 API Microsoft.Build.BuildEngine. This API
        // should not be used with .NET 4 because it has been deprecated.
        //
        private static bool getCopyLocal(Project project, string name)
        {
            Microsoft.Build.BuildEngine.BuildItem referenceItem = null;
            Microsoft.Build.BuildEngine.Project p =
                Microsoft.Build.BuildEngine.Engine.GlobalEngine.GetLoadedProject(project.FileName);

            foreach(Microsoft.Build.BuildEngine.BuildItemGroup itemGroup in p.ItemGroups)
            {
                foreach(Microsoft.Build.BuildEngine.BuildItem item in itemGroup)
                {
                    if(!item.Name.Equals("Reference"))
                    {
                        continue;
                    }

                    string[] tokens = item.Include.Split(',');
                    if(tokens.Length <= 0)
                    {
                        continue;
                    }

                    if(!tokens[0].Trim().Equals(name, StringComparison.CurrentCultureIgnoreCase))
                    {
                        continue;
                    }

                    referenceItem = item;

                    if(referenceItem != null)
                    {
                        break;
                    }
                }
                if(referenceItem != null)
                {
                    break;
                }
            }
            if(referenceItem != null)
            {
                return referenceItem.GetMetadata("Private").Equals(true.ToString(),
                                                    StringComparison.CurrentCultureIgnoreCase);
            }
            return true;
        }
#endif
        private static void setCopyLocal(Project project, string name, bool copyLocal)
        {
            VSLangProj.VSProject vsProject = (VSLangProj.VSProject)project.Object;
            foreach(Reference r in vsProject.References)
            {
                if(r.Name.Equals(name))
                {
                    r.CopyLocal = copyLocal;
                    break;
                }
            }
        }

        public static int getProjectPropertyAsInt(Project project, string name)
        {
            int value = -1;
            try
            {
                value = Convert.ToInt32(Util.getProjectProperty(project, name), CultureInfo.InvariantCulture);
            }
            catch(FormatException)
            {
            }
            catch(OverflowException)
            {
            }
            return value;
        }

        public static bool getProjectPropertyAsBool(Project project, string name)
        {
            return Util.getProjectProperty(project, name).Equals(
                                        true.ToString(), StringComparison.CurrentCultureIgnoreCase);
        }

        public static string getProjectProperty(Project project, string name)
        {
            return Util.getProjectProperty(project, name, "", true);
        }

        public static string getProjectProperty(Project project, string name, string defaultValue)
        {
            return Util.getProjectProperty(project, name, defaultValue, true);
        }

        public static string getProjectProperty(Project project, string name, string defaultValue, bool update)
        {
            if(project == null || String.IsNullOrEmpty(name) || project.Globals == null)
            {
                return defaultValue;
            }

            if(project.Globals.get_VariableExists(name))
            {
                return project.Globals[name].ToString();
            }

            if(update)
            {
                if(!String.IsNullOrEmpty(defaultValue))
                {
                    project.Globals[name] = defaultValue;
                    if(!project.Globals.get_VariablePersists(name))
                    {
                        project.Globals.set_VariablePersists(name, true);
                    }
                }
            }
            return defaultValue;
        }

        public static void setProjectProperty(Project project, string name, string value)
        {
            if(project == null || String.IsNullOrEmpty(name) || project.Globals == null)
            {
                return;
            }
            if(String.IsNullOrEmpty(value))
            {
                if(project.Globals.get_VariableExists(name))
                {
                    project.Globals.set_VariablePersists(name, false);
                }
            }
            else if(!project.Globals.get_VariableExists(name) || (string)project.Globals[name] != value)
            {
                project.Globals[name] = value;
                if(!project.Globals.get_VariablePersists(name))
                {
                    project.Globals.set_VariablePersists(name, true);
                }
            }
        }

        //
        // As the add-in evolves the code changes, but users still have project files
        // created by previous versions. This method is called when projects are opened
        // to account for old project files and to move them to the current requirements.
        //
        public static void fix(Project p)
        {
            if(p == null || p.Globals == null)
            {
                return;
            }

            // This variable was removed for 3.4.1.2.
            if(p.Globals.get_VariableExists("ZerocIce_HomeExpanded"))
            {
                p.Globals.set_VariablePersists("ZerocIce_HomeExpanded", false);
            }

            // This variable was removed for 3.4.2
            if(p.Globals.get_VariableExists("ZerocIce_Home"))
            {
                string iceHome = expandEnvironmentVars(getProjectProperty(p, "ZerocIce_Home"));
                p.Globals.set_VariablePersists("ZerocIce_Home", false);

                if(Util.isCppProject(p))
                {
                    //
                    // Before 3.4.2 C++ project configurations don't use $(IceHome) macro,
                    // we remove these old style settings.
                    //
                    VCProject vcProject = (VCProject)p.Object;
                    IVCCollection configurations = (IVCCollection)vcProject.Configurations;

                    //
                    // Path to property sheet used by VS 2010 projects before 3.4.2,
                    // the property sheet is not longer needed and should be removed.
                    //
                    string sheetFile = Util.absolutePath(p, "ice.props");

                    foreach(VCConfiguration conf in configurations)
                    {
                        if(conf == null)
                        {
                            continue;
                        }

                        VCCLCompilerTool compilerTool =
                            (VCCLCompilerTool)(((IVCCollection)conf.Tools).Item("VCCLCompilerTool"));
                        bool staticLib = conf.ConfigurationType == Microsoft.VisualStudio.VCProjectEngine.ConfigurationTypes.typeStaticLibrary;
                        LinkerAdapter linkerAdapter;
                        if(staticLib)
                        {
                            linkerAdapter = new StaticLinkerAdapter((VCLibrarianTool)(((IVCCollection)conf.Tools).Item("VCLibrarianTool")));
                        }
                        else
                        {
                            linkerAdapter = new DynamicLinkerAdapter((VCLinkerTool)(((IVCCollection)conf.Tools).Item("VCLinkerTool")));
                        }

                        Util.removeIceCppEnvironment((VCDebugSettings)conf.DebugSettings, iceHome);
                        Util.removeIceCppLibraryDir(linkerAdapter, iceHome);
                        Util.removeCppIncludes(compilerTool, iceHome, Util.getProjectOutputDirRaw(p));

                        //
                        // Remove ice.props, old property sheet used by VS 2010
                        // from all project configurations.
                        //
#if VS2010
                        VCPropertySheet sheet = null;
                        IVCCollection sheets = (IVCCollection)conf.PropertySheets;
                        foreach(VCPropertySheet s in sheets)
                        {
                            if(Util.absolutePath(p, s.PropertySheetFile).Equals(sheetFile))
                            {
                                sheet = s;
                                break;
                            }
                        }

                        if(sheet != null)
                        {
                            conf.RemovePropertySheet(sheet);
                        }
#endif
                    }

                    //
                    // If the old property sheet exists delete it from disk.
                    //
                    if(File.Exists(sheetFile))
                    {
                        try
                        {
                            File.Delete(sheetFile);
                        }
                        catch(IOException)
                        {
                        }
                    }
                }
            }

            // This feature was made more general for 3.4.1.2.
            if(p.Globals.get_VariableExists("ZerocIce_ConsoleOutput"))
            {
                if(!p.Globals.get_VariableExists(PropertyVerboseLevel))
                {
                    setProjectProperty(p, PropertyVerboseLevel, "0");
                    if(getProjectPropertyAsBool(p, "ZerocIce_ConsoleOutput"))
                    {
                        setProjectProperty(p, PropertyVerboseLevel, "1");
                    }
                }
                p.Globals.set_VariablePersists("ZerocIce_ConsoleOutput", false);
            }
        }

        public static String getPrecompileHeader(Project project)
        {
            string preCompiledHeader = "";

            VCConfiguration configuration = getActiveVCConfiguration(project);
            if(configuration == null)
            {
                return preCompiledHeader;
            }

            VCCLCompilerTool compilerTool =
            (VCCLCompilerTool)(((IVCCollection)configuration.Tools).Item("VCCLCompilerTool"));
            if(compilerTool == null)
            {
                return preCompiledHeader;
            }

            if(compilerTool.UsePrecompiledHeader == pchOption.pchCreateUsingSpecific ||
               compilerTool.UsePrecompiledHeader == pchOption.pchUseUsingSpecific)
            {
                preCompiledHeader = compilerTool.PrecompiledHeaderThrough;
            }

            return preCompiledHeader;
        }

        public static ComponentList getIceCppComponents(Project project)
        {
            ComponentList components = new ComponentList();
            ConfigurationManager configManager = project.ConfigurationManager;
            Configuration activeConfig = (Configuration)configManager.ActiveConfiguration;

            VCProject vcProject = (VCProject)project.Object;
            IVCCollection configurations = (IVCCollection)vcProject.Configurations;
            foreach(VCConfiguration conf in configurations)
            {
                if(conf == null)
                {
                    continue;
                }
                if(String.IsNullOrEmpty(conf.Name))
                {
                    continue;
                }
                if(!conf.Name.Equals(activeConfig.ConfigurationName + "|" + activeConfig.PlatformName))
                {
                    continue;
                }

                VCCLCompilerTool compilerTool =
                    (VCCLCompilerTool)(((IVCCollection)conf.Tools).Item("VCCLCompilerTool"));
                VCLinkerTool linkerTool = (VCLinkerTool)(((IVCCollection)conf.Tools).Item("VCLinkerTool"));
                if(linkerTool == null || compilerTool == null)
                {
                    break;
                }

                if(String.IsNullOrEmpty(linkerTool.AdditionalDependencies))
                {
                    break;
                }

                bool debug = isDebug(compilerTool.RuntimeLibrary); 

                List<string> componentNames = new List<string>(linkerTool.AdditionalDependencies.Split(' '));
                foreach(string s in componentNames)
                {
                    if(String.IsNullOrEmpty(s))
                    {
                        continue;
                    }

                    int index = s.LastIndexOf('.');
                    if(index <= 0)
                    {
                        continue;
                    }

                    string libName = s.Substring(0, index);
                    if(libName.EndsWith("d"))
                    {
                        libName = libName.Substring(0, libName.Length - 1);
                    }
                    if(String.IsNullOrEmpty(libName))
                    {
                        continue;
                    }

                    if(Array.BinarySearch(Util.getCppNames(), libName, StringComparer.CurrentCultureIgnoreCase) < 0)
                    {
                        continue;
                    }
                    components.Add(libName.Trim());
                }
            }
            return components;
        }

        public static ComponentList getIceSilverlightComponents(Project project)
        {
            ComponentList components = new ComponentList();
            if(project == null)
            {
                return components;
            }

            VSLangProj.VSProject vsProject = (VSLangProj.VSProject)project.Object;
            foreach(Reference r in vsProject.References)
            {
                if(Array.BinarySearch(Util.getSilverlightNames(), r.Name, StringComparer.CurrentCultureIgnoreCase) < 0)
                {
                    continue;
                }

                components.Add(r.Name);
            }
            return components;
        }

        public static ComponentList getIceDotNetComponents(Project project)
        {
            ComponentList components = new ComponentList();
            if(project == null)
            {
                return components;
            }

            string[] componentNames = null;
            if(Util.isCSharpSmartDeviceProject(project) || Util.isVBSmartDeviceProject(project))
            {
                componentNames = getDotNetCompactNames();
            }
            else
            {
                componentNames = getDotNetNames();
            }
            VSLangProj.VSProject vsProject = (VSLangProj.VSProject)project.Object;
            foreach(Reference r in vsProject.References)
            {
                if(Array.BinarySearch(componentNames, r.Name, StringComparer.CurrentCultureIgnoreCase) < 0)
                {
                    continue;
                }

                components.Add(r.Name);
            }
            return components;
        }

        public static bool addIceCppConfigurations(Project project)
        {
            if(!isCppProject(project))
            {
                return false;
            }

            VCProject vcProject = (VCProject)project.Object;
            IVCCollection configurations = (IVCCollection)vcProject.Configurations;
            bool changed = false;
            
            Util.addIcePropertySheet(project);

            foreach(VCConfiguration conf in configurations)
            {
                if(addIceCppConfiguration(project, conf))
                {
                    changed = true;
                }
            }
            return changed;
        }

        public static bool addIceCppConfiguration(Project project, VCConfiguration conf)
        {
            if(!isCppProject(project))
            {
                return false;
            }

            if (conf.ConfigurationType == Microsoft.VisualStudio.VCProjectEngine.ConfigurationTypes.typeGeneric ||
               conf.ConfigurationType == Microsoft.VisualStudio.VCProjectEngine.ConfigurationTypes.typeUnknown)
            {
                string err = "Configuration Type: '" + conf.ConfigurationType.ToString() + "' not suported by Ice Visual Studio Add-in";
                Util.write(project, Util.msgLevel.msgError,
                    "------ Slice compilation failed: Project: " + Util.getTraceProjectName(project) + " ------\n\n" +
                    err);
                MessageBox.Show(err, "Ice Visual Studio Add-in", MessageBoxButtons.OK, MessageBoxIcon.Error, MessageBoxDefaultButton.Button1,
                                (MessageBoxOptions)0);

                Connect.getBuilder().addError(project, "", TaskErrorCategory.Error, 0, 0, err);

                return false;
            }

            bool x64 = false;

            if(conf == null || String.IsNullOrEmpty(conf.Name))
            {
                return false;
            }


            x64 = false;
            VCPlatform platform = (VCPlatform)conf.Platform;
            if(platform == null || String.IsNullOrEmpty(platform.Name))
            {
                return false;
            }

            if(platform.Name.Equals("x64", StringComparison.CurrentCultureIgnoreCase) ||
                platform.Name.Equals("Itanium", StringComparison.CurrentCultureIgnoreCase))
            {
                x64 = true;
            }
            VCCLCompilerTool compilerTool =
                (VCCLCompilerTool)(((IVCCollection)conf.Tools).Item("VCCLCompilerTool"));
            VCLinkerTool linkerTool = (VCLinkerTool)(((IVCCollection)conf.Tools).Item("VCLinkerTool"));

            Util.addIceCppEnvironment((VCDebugSettings)conf.DebugSettings, project, x64);
            bool staticLib = conf.ConfigurationType == Microsoft.VisualStudio.VCProjectEngine.ConfigurationTypes.typeStaticLibrary;
            LinkerAdapter linkerAdapter;
            if(staticLib)
            {
                linkerAdapter = new StaticLinkerAdapter((VCLibrarianTool)(((IVCCollection)conf.Tools).Item("VCLibrarianTool")));
            }
            else
            {
                linkerAdapter = new DynamicLinkerAdapter((VCLinkerTool)(((IVCCollection)conf.Tools).Item("VCLinkerTool")));
            }

            bool libChanged = Util.addIceCppLibraryDir(linkerAdapter, project, x64);
            bool includesChanged = Util.addCppIncludes(compilerTool, project);

            return libChanged | includesChanged;
        }

        public static void removeIceCppConfigurations(Project project)
        {
            if(!isCppProject(project))
            {
                return;
            }

            VCProject vcProject = (VCProject)project.Object;
            IVCCollection configurations = (IVCCollection)vcProject.Configurations;
            foreach(VCConfiguration conf in configurations)
            {
                if(conf == null)
                {
                    continue;
                }

                VCCLCompilerTool compilerTool =
                    (VCCLCompilerTool)(((IVCCollection)conf.Tools).Item("VCCLCompilerTool"));
                bool staticLib = conf.ConfigurationType == Microsoft.VisualStudio.VCProjectEngine.ConfigurationTypes.typeStaticLibrary;
                LinkerAdapter linkerAdapter;
                if(staticLib)
                {
                    linkerAdapter = new StaticLinkerAdapter((VCLibrarianTool)(((IVCCollection)conf.Tools).Item("VCLibrarianTool")));
                }
                else
                {
                    linkerAdapter = new DynamicLinkerAdapter((VCLinkerTool)(((IVCCollection)conf.Tools).Item("VCLinkerTool")));
                }


                Util.removeIceCppEnvironment((VCDebugSettings)conf.DebugSettings, "$(IceHome)");
                Util.removeIceCppLibraryDir(linkerAdapter, "$(IceHome)");
                Util.removeCppIncludes(compilerTool, "$(IceHome)", Util.getProjectOutputDirRaw(project));
                Util.removeIcePropertySheet(conf);
            }
            Util.removeIceCppLibs(project);
        }

        public static void addIceCppLibs(Project project, ComponentList components)
        {
            if(!isCppProject(project))
            {
                return;
            }

            VCProject vcProject = (VCProject)project.Object;
            IVCCollection configurations = (IVCCollection)vcProject.Configurations;

            foreach(VCConfiguration conf in configurations)
            {
                if(conf == null)
                {
                    continue;
                }
                VCCLCompilerTool compilerTool =
                    (VCCLCompilerTool)(((IVCCollection)conf.Tools).Item("VCCLCompilerTool"));
                bool staticLib = conf.ConfigurationType == Microsoft.VisualStudio.VCProjectEngine.ConfigurationTypes.typeStaticLibrary;
                LinkerAdapter linkerAdapter;
                if(staticLib)
                {
                   linkerAdapter = new StaticLinkerAdapter((VCLibrarianTool)(((IVCCollection)conf.Tools).Item("VCLibrarianTool")));
                }
                else
                { 
                    linkerAdapter = new DynamicLinkerAdapter((VCLinkerTool)(((IVCCollection)conf.Tools).Item("VCLinkerTool")));
                }

                if(compilerTool == null || linkerAdapter == null)
                {
                    continue;
                }

                bool debug = isDebug(compilerTool.RuntimeLibrary);

                foreach(string component in components)
                {
                    if(String.IsNullOrEmpty(component))
                    {
                        continue;
                    }
                    Util.addCppLib(linkerAdapter, component, debug);
                }
            }
        }

        public static ComponentList removeIceCppLibs(Project project)
        {
            return Util.removeIceCppLibs(project, new ComponentList(Util.getCppNames()));
        }

        public static ComponentList removeIceCppLibs(Project project, ComponentList components)
        {
            ComponentList removed = new ComponentList();
            if(!isCppProject(project))
            {
                return removed;
            }

            VCProject vcProject = (VCProject)project.Object;
            IVCCollection configurations = (IVCCollection)vcProject.Configurations;

            foreach(VCConfiguration conf in configurations)
            {
                if(conf == null)
                {
                    continue;
                }
                VCCLCompilerTool compilerTool =
                    (VCCLCompilerTool)(((IVCCollection)conf.Tools).Item("VCCLCompilerTool"));
                bool staticLib = conf.ConfigurationType == Microsoft.VisualStudio.VCProjectEngine.ConfigurationTypes.typeStaticLibrary;
                LinkerAdapter linkerAdapter;
                if(staticLib)
                {
                    linkerAdapter = new StaticLinkerAdapter((VCLibrarianTool)(((IVCCollection)conf.Tools).Item("VCLibrarianTool")));
                }
                else
                {
                    linkerAdapter = new DynamicLinkerAdapter((VCLinkerTool)(((IVCCollection)conf.Tools).Item("VCLinkerTool")));
                }

                if(compilerTool == null || linkerAdapter == null)
                {
                    continue;
                }

                bool debug = false;
                if(!String.IsNullOrEmpty(compilerTool.PreprocessorDefinitions))
                {
                    debug = (compilerTool.PreprocessorDefinitions.Contains("DEBUG") &&
                             !compilerTool.PreprocessorDefinitions.Contains("NDEBUG"));
                }
                if(!debug)
                {
                    debug = conf.Name.Contains("Debug");
                }

                foreach(string s in components)
                {
                    if(String.IsNullOrEmpty(s))
                    {
                        continue;
                    }

                    if(Util.removeCppLib(linkerAdapter, s, debug) && !removed.Contains(s))
                    {
                        removed.Add(s);
                    }
                }
            }
            return removed;
        }

        //
        // Return true if the .NET reference that corresponds to the Ice component 
        // is present in the given project.
        //
        public static bool hasDotNetReference(Project project, string component)
        {
            bool found = false;
            foreach(Reference r in ((VSProject)project.Object).References)
            {
                if(r.Name.Equals(component, StringComparison.OrdinalIgnoreCase))
                {
                    found = true;
                    break;
                }
            }
            return found;
        }

        //
        // Return true if at least one of the C++ project configurations contains
        // the library corresponding to the given component.
        //
        public static bool hasIceCppLib(Project project, string component)
        {
            if(!isCppProject(project))
            {
                return false;
            }

            if(Array.BinarySearch(Util.getCppNames(), component, StringComparer.CurrentCultureIgnoreCase) < 0)
            {
                return false;
            }

            VCProject vcProject = (VCProject)project.Object;
            IVCCollection configurations = (IVCCollection)vcProject.Configurations;
            bool found = false;

            foreach(VCConfiguration conf in configurations)
            {
                if(conf == null)
                {
                    continue;
                }

                VCCLCompilerTool compilerTool =
                    (VCCLCompilerTool)(((IVCCollection)conf.Tools).Item("VCCLCompilerTool"));
                bool staticLib = conf.ConfigurationType == Microsoft.VisualStudio.VCProjectEngine.ConfigurationTypes.typeStaticLibrary;
                LinkerAdapter linkerAdapter;
                if(staticLib)
                {
                    linkerAdapter = new StaticLinkerAdapter((VCLibrarianTool)(((IVCCollection)conf.Tools).Item("VCLibrarianTool")));
                }
                else
                {
                    linkerAdapter = new DynamicLinkerAdapter((VCLinkerTool)(((IVCCollection)conf.Tools).Item("VCLinkerTool")));
                }

                if(compilerTool == null || linkerAdapter == null)
                {
                    continue;
                }

                bool debug = false;
                if(!String.IsNullOrEmpty(compilerTool.PreprocessorDefinitions))
                {
                    debug = (compilerTool.PreprocessorDefinitions.Contains("DEBUG") &&
                             !compilerTool.PreprocessorDefinitions.Contains("NDEBUG"));
                }
                if(!debug)
                {
                    debug = conf.Name.Contains("Debug");
                }

                string libName = component;
                if(debug)
                {
                    libName += "d";
                }
                libName += ".lib";

                string additionalDependencies = linkerAdapter.AdditionalDependencies;
                if(String.IsNullOrEmpty(additionalDependencies))
                {
                    continue;
                }

                ComponentList components = new ComponentList(additionalDependencies.Split(' '));
                if(components.Contains(libName))
                {
                    found = true;
                    break;
                }
            }
            return found;
        }

        public static DTE getCurrentDTE()
        {
            return Connect.getCurrentDTE();
        }

        public static string expandEnvironmentVars(string s)
        {
            if(String.IsNullOrEmpty(s))
            {
                return s;
            }
            string result = s;
            int beg = 0;
            int end = 0;
            while(beg < result.Length && 
                  (beg = result.IndexOf("$(", beg, StringComparison.Ordinal)) != -1 &&
                  (end = result.IndexOf(")", beg, StringComparison.Ordinal)) != -1)
            {
                string variable = result.Substring(beg + "$(".Length, end - (beg + "$(".Length));
                string value = System.Environment.GetEnvironmentVariable(variable);
                if(value == null)
                {
                    // Skip undefined vars.
                    beg += "$(".Length + variable.Length + ")".Length;
                    continue;
                }
                result = result.Replace("$(" + variable + ")", value);
                beg += value.Length;
            }
            return result;
        }

        public static bool containsEnvironmentVars(string s)
        {
            if(String.IsNullOrEmpty(s))
            {
                return false;
            }
            return s.Contains("$(");
        }

        public static string quote(string arg)
        {
            if(String.IsNullOrEmpty(arg))
            {
                return "\"\"";
            }
            return "\"" + arg + "\"";
        }

        public static void verifyProjectSettings(Project project)
        {
            if(isCppProject(project))
            {
                addIceCppConfigurations(project); ;
            }
            else
            {
                string iceHome = getIceHome();
                string binDir = getCsBinDir(project);
                string sliceCompiler = Util.slice2cs;
                ComponentList components = Util.getIceDotNetComponents(project);

                if(isSilverlightProject(project))
                {
                    iceHome = getIceSlHome();
                    sliceCompiler = Util.slice2sl;
                    components = Util.getIceSilverlightComponents(project);
                }

                String version = Builder.getSliceCompilerVersion(project, sliceCompiler);

                string[] tokens = version.Split('.');
                //
                // Add patch version 0 if there isn't one
                //
                if(tokens.Length == 3)
                {
                    version += ".0";
                }

                foreach(string component in components)
                {
                    if (String.IsNullOrEmpty(component))
                    {
                        continue;
                    }

                    string reference = iceHome + binDir + component + ".dll";

                    //
                    // If Ice components are not from the current Ice home binary
                    // directory, we update the references to use the new value of
                    // Ice home.
                    //
                    bool development = developmentMode(project);
                    foreach(Reference r in ((VSProject)project.Object).References)
                    {
                        if(r.Name.Equals(component, StringComparison.OrdinalIgnoreCase))
                        {
                            if(!r.Path.Equals(reference) ||
                               !r.Version.Equals(version))
                            {
                                bool copyLocal = getCopyLocal(project, component);
                                Util.removeDotNetReference(project, component);

                                Util.addDotNetReference(project, component, iceHome, development);
                                setCopyLocal(project, component, copyLocal);
                            }
                            break;
                        }
                    }
                }
            }
        }

        public static bool addBuilderToProject(Project project, ComponentList components)
        {
            Builder builder = Connect.getBuilder();
            if(builder == null || project == null)
            {
                return false;
            }

            if(Util.isSliceBuilderEnabled(project))
            {
                return false; // Already enabled.
            }

            builder.addBuilderToProject(project, components);
            return true;
        }

        public static bool removeBuilderFromProject(Project project, ComponentList components)
        {
            Builder builder = Connect.getBuilder();
            if(builder == null || project == null)
            {
                return false;
            }

            if(!Util.isSliceBuilderEnabled(project))
            {
                return false; // Already disabled.
            }

            builder.removeBuilderFromProject(project, components);
            return true;
        }

        public static void cleanProject(Project project, bool remove)
        {
            Builder builder = Connect.getBuilder();
            builder.cleanProject(project, remove);
        }

        public static void rebuildProject(Project project)
        {
            Builder builder = Connect.getBuilder();
            builder.cleanProject(project, false);
            builder.buildProject(project, true, vsBuildScope.vsBuildScopeProject);
        }

        public static int getVerboseLevel(Project p)
        {
            int verboseLevel = (int)Util.msgLevel.msgInfo;
            if(p != null)
            {
                try
                {
                    verboseLevel = Int32.Parse(Util.getProjectProperty(p, Util.PropertyVerboseLevel),
                                               CultureInfo.InvariantCulture);
                }
                catch(ArgumentNullException)
                {
                }
                catch(FormatException)
                {
                }
                catch(OverflowException)
                {
                }
            }
            return verboseLevel;
        }

        public static void write(Project p, Util.msgLevel msgLevel, string message)
        {
            Builder builder = Connect.getBuilder();
            if(builder == null)
            {
                return;
            }

            int verboseLevel = getVerboseLevel(p);

            if((int)msgLevel <= verboseLevel)
            {
                if(!builder.commandLine)
                {
                    OutputWindowPane pane = builder.buildOutput();
                    if(pane == null)
                    {
                        return;
                    }
                    pane.Activate();
                    pane.OutputString(message);
                }
                else
                {
                    System.Console.Write(message);
                }
            }
        }

        //
        // Get the assembly name of a .NET project
        //
        public static string assemblyName(Project project)
        {
            if(project == null)
            {
                return "";
            }
            if(!isCSharpProject(project) && !isVBProject(project))
            {
                return "";
            }

            Property assemblyName = project.Properties.Item("AssemblyName");
            
            if(assemblyName.Value == null)
            {
                return "";
            }
            return assemblyName.Value.ToString();
        }

        //
        // Check if the Visual Studio hosting process is enabled in any of
        // the project configurations.
        //
        public static bool useVSHostingProcess(Project p)
        {
            bool enabled = false;
            foreach(Configuration config in p.ConfigurationManager)
            {
                if(config.Properties == null)
                {
                    continue;
                }
                Property property = config.Properties.Item("UseVSHostingProcess");
                if(property.Value.ToString().Equals(true.ToString(), StringComparison.CurrentCultureIgnoreCase))
                {
                    enabled = true;
                    break;
                }
            }
            return enabled;
        }

        //
        // Enable/Disable the Visual Studio hosting process. We use this
        // to force Visual Studio to restart the hosting process, which is
        // necessary for the process to run with updated environment variables.
        //
        public static void setVsHostingProcess(Project p, bool value)
        {
            foreach(Configuration config in p.ConfigurationManager)
            {
                if(config == null)
                {
                    continue;
                }

                if(config.Properties == null)
                {
                    continue;
                }

                Property property = config.Properties.Item("UseVSHostingProcess");
                if(property == null)
                {
                    continue;
                }

                if(property.Value != null)
                { 
                    if(property.Value.ToString().Equals(value.ToString(), StringComparison.CurrentCultureIgnoreCase))
                    {
                        continue;
                    }
                }
                property.Value = value;
            }
        }

        //
        // Check if the .NET development mode is enabled in the application config that resides in
        // the file <applicationName>.exe.config.
        //
        public static bool developmentMode(Project project)
        {
            string configPath = 
                        Path.Combine(Path.GetDirectoryName(project.FileName), assemblyName(project) + ".exe.config");
            if(!File.Exists(configPath))
            {
                return false;
            }

            System.Xml.XmlDocument dom = new System.Xml.XmlDocument();

            try
            {
                dom.Load(configPath);
                System.Xml.XmlNodeList nodes = dom.SelectNodes("/configuration/runtime/developmentMode");
                if(nodes.Count <= 0)
                {
                    return false;
                }
                System.Xml.XmlNode node = nodes[0];

                System.Xml.XmlAttribute attribute = node.Attributes["developerInstallation"];

                if(attribute == null)
                {
                    return false;
                }

                if(String.IsNullOrEmpty(attribute.Value))
                {
                    return false;
                }
                return attribute.Value.Equals("true", StringComparison.CurrentCultureIgnoreCase);
            }
            catch(System.Xml.XmlException)
            {
                return false; // There was an error parsing the XML
            }
        }

        public static bool parseSlice2slOptions(string args, bool showWarning, ref Options opts)
        {
            try
            {
                opts = null;

                if(String.IsNullOrEmpty(args))
                {
                    return true; //No options to parse
                }

                opts = new Options();
                opts.addOpt("h", "help");
                opts.addOpt("v", "version");
                opts.addOpt("D", "", Options.ArgType.NeedArg, "", Options.RepeatType.Repeat);
                opts.addOpt("U", "", Options.ArgType.NeedArg, "", Options.RepeatType.Repeat);
                opts.addOpt("I", "", Options.ArgType.NeedArg, "", Options.RepeatType.Repeat);
                opts.addOpt("E");
                opts.addOpt("", "output-dir", Options.ArgType.NeedArg);
                opts.addOpt("", "depend");
                opts.addOpt("d", "debug");
                opts.addOpt("", "ice");
                opts.addOpt("", "case-sensitive");

                opts.parse(Options.split(args));

                checkInvalidOptions(opts);
                
                return true;
            }
            catch(BadOptionException ex)
            {
                if(showWarning)
                {
                    showExtraOptionsWarning(ex);
                }
                return false;
            }
        }

        public static bool parseSlice2csOptions(string args, bool showWarning, ref Options opts)
        {
            try
            {
                opts = null;

                if(String.IsNullOrEmpty(args))
                {
                    return true; //No options to parse
                }

                opts = new Options();
                opts.addOpt("h", "help");
                opts.addOpt("v", "version");
                opts.addOpt("D", "", Options.ArgType.NeedArg, "", Options.RepeatType.Repeat);
                opts.addOpt("U", "", Options.ArgType.NeedArg, "", Options.RepeatType.Repeat);
                opts.addOpt("I", "", Options.ArgType.NeedArg, "", Options.RepeatType.Repeat);
                opts.addOpt("E");
                opts.addOpt("", "output-dir", Options.ArgType.NeedArg);
                opts.addOpt("", "tie");
                opts.addOpt("", "impl");
                opts.addOpt("", "impl-tie");
                opts.addOpt("", "depend");
                opts.addOpt("d", "debug");
                opts.addOpt("", "ice");
                opts.addOpt("", "underscore");
                opts.addOpt("", "checksum");
                opts.addOpt("", "stream");

                opts.parse(Options.split(args));

                checkInvalidOptions(opts);

                return true;
            }
            catch(BadOptionException ex)
            {
                if(showWarning)
                {
                    showExtraOptionsWarning(ex);
                }
                return false;
            }
        }

        public static bool parseSlice2cppOptions(string args, bool showWarning, ref Options opts,
                                                 ref string headerExt, ref string sourceExt)
        {
            try
            {
                opts = null;
                headerExt = "";
                sourceExt = "";

                if(String.IsNullOrEmpty(args))
                {
                    return true; //No options to parse
                }

                opts = new Options();
                opts.addOpt("h", "help");
                opts.addOpt("v", "version");
                opts.addOpt("", "header-ext", Options.ArgType.NeedArg, "h");
                opts.addOpt("", "source-ext", Options.ArgType.NeedArg, "cpp");
                opts.addOpt("", "add-header", Options.ArgType.NeedArg, "", Options.RepeatType.Repeat);
                opts.addOpt("D", "", Options.ArgType.NeedArg, "", Options.RepeatType.Repeat);
                opts.addOpt("U", "", Options.ArgType.NeedArg, "", Options.RepeatType.Repeat);
                opts.addOpt("I", "", Options.ArgType.NeedArg, "", Options.RepeatType.Repeat);
                opts.addOpt("E");
                opts.addOpt("", "include-dir", Options.ArgType.NeedArg);
                opts.addOpt("", "output-dir", Options.ArgType.NeedArg);
                opts.addOpt("", "dll-export", Options.ArgType.NeedArg);
                opts.addOpt("", "impl");
                opts.addOpt("", "depend");
                opts.addOpt("d", "debug");
                opts.addOpt("", "ice");
                opts.addOpt("", "underscore");
                opts.addOpt("", "checksum");
                opts.addOpt("", "stream");

                opts.parse(Options.split(args));

                checkInvalidOptions(opts);

                string hExt = opts.optArg("header-ext");
                string cppExt = opts.optArg("source-ext");

                if(Util.containsEnvironmentVars(hExt) ||
                   Util.containsEnvironmentVars(cppExt))
                {
                    throw new BadOptionException(
                                "You cannot use environment variables in `--header-ext' or `--source-ext' options.");
                }
                headerExt = hExt;
                sourceExt = cppExt;

                if(opts.isSet("dll-export"))
                {
                    throw new BadOptionException(
                                        "Use the `DLL Export Symbol' text box below to set the --dll-export option\n" +
                                        "instead of the `Extra Compiler Options' text box.");
                }

                return true;
            }
            catch(BadOptionException ex)
            {
                if(showWarning)
                {
                    showExtraOptionsWarning(ex);
                }
                return false;
            }
        }

        //
        // Check for common options that must not appear in Extra Options textbox.
        //
        private static void checkInvalidOptions(Options opts)
        {
            if(opts.isSet("output-dir"))
            {
                throw new BadOptionException(
                                    "Use the `Output Dir' text box above to set `--output-dir' option\n" +
                                    "instead of the `Extra Compiler Options' text box.");
            }

            if(opts.isSet("depend"))
            {
                throw new BadOptionException("Don't use `--depend' option in `Extra Options' text box.");
            }

            if(opts.isSet("help"))
            {
                throw new BadOptionException("Don't use `--help' or `-h' option in `Extra Options' text box.");
            }

            if(opts.isSet("version"))
            {
                throw new BadOptionException("Don't use `--version' or `-v' option in `Extra Options' text box.");
            }
        }

        private static void showRunTimeLibraryWarning(Project p)
        {
            string err = "Run Time library not supported with Ice, Ice requires /MD or /MDd Run Time Library.";
            MessageBox.Show(err, "Ice Visual Studio Add-in", MessageBoxButtons.OK, MessageBoxIcon.Error, MessageBoxDefaultButton.Button1,
                            (MessageBoxOptions)0);

            Util.write(p, msgLevel.msgError, err);
        }

        private static void showExtraOptionsWarning(BadOptionException ex)
        {
            MessageBox.Show("Extra Options field contains some errors:\n" +
                                                 ex.reason,
                                                 "Ice Visual Studio Add-in", MessageBoxButtons.OK,
                                                 MessageBoxIcon.Error,
                                                 MessageBoxDefaultButton.Button1,
                                                 (MessageBoxOptions)0);
        }


        public static void unexpectedExceptionWarning(Exception ex)
        {
            try
            {
                Builder builder = Connect.getBuilder();
                if(!builder.commandLine)
                {
                    MessageBox.Show("The Ice Visual Studio Add-in has raised an unexpected exception:\n" +
                                    ex.ToString(),
                                    "Ice Visual Studio Add-in", MessageBoxButtons.OK,
                                    MessageBoxIcon.Error,
                                    MessageBoxDefaultButton.Button1,
                                    (MessageBoxOptions)0);
                }
            }
            catch(Exception)
            { 
            }

            try
            {
                Util.write(null, Util.msgLevel.msgError, ex.ToString() + "\n");
            }
            catch(Exception)
            {
            }
        }

        public static string getHeaderExt(Project p)
        {
            String extension = getProjectProperty(p, Util.PropertyIceHeaderExt);
            if(String.IsNullOrEmpty(extension))
            {
                extension = "h";
            }
            return extension;
        }

        public static string getSourceExt(Project p)
        {
            String extension = getProjectProperty(p, Util.PropertyIceSourceExt);
            if(String.IsNullOrEmpty(extension))
            {
                extension = "cpp";
            }
            return extension;
        }

        //
        // Warn the user about unsaved changes. Returns true if the user 
        // wants to discard changes, otherwise returns false.
        //
        public static bool warnUnsavedChanges(IWin32Window owner)
        {
            if(MessageBox.Show(owner, "Are you sure you want to discard all changes?",
                               "Ice Visual Studio Add-in", MessageBoxButtons.YesNo,
                               MessageBoxIcon.Question, MessageBoxDefaultButton.Button1,
                               (MessageBoxOptions)0) == DialogResult.Yes)
            {
                return true;
            }
            return false;
        }

        public static List<ProjectItem> clone(ProjectItems items)
        {
            List<ProjectItem> list = new List<ProjectItem>();
            foreach(ProjectItem i in items)
            {
                list.Add(i);
            }
            return list;
        }

        public static List<Project> getProjects(Solution solution)
        {
            List<Project> projects = new List<Project>();
            foreach(Project p in solution.Projects)
            {
                getProjects(solution, p, ref projects);
            }
            return projects;
        }

        public static void getProjects(Solution solution, Project project, ref List<Project> projects)
        {
            if(project.Kind == EnvDTE80.ProjectKinds.vsProjectKindSolutionFolder)
            {
                foreach(ProjectItem item in project.ProjectItems)
                {
                    Project p = item.Object as Project;
                    if(p == null)
                    {
                        continue;
                    }
                    getProjects(solution, p, ref projects);
                }
            }
            if(projects.Find(
                delegate(Project p)
                {
                    return project.UniqueName.Equals(p.UniqueName);
                }) == null)
            {
                projects.Add(project);
            }
        }

        //
        // Rerturn a list of projects with match the build order
        // of the solution.
        //
        public static List<Project> buildOrder(Solution solution)
        {
            List<Project> projects = new List<Project>();
            foreach(Project p in solution.Projects)
            {
                buildOrder(solution, p, ref projects);
            }
            return projects;
        }


        //
        // This method helps to build the list of projects with the
        // right build order.
        //
        public static void buildOrder(Solution solution, Project project, ref List<Project> projects)
        {
            if(project.Kind == EnvDTE80.ProjectKinds.vsProjectKindSolutionFolder)
            {
                foreach(ProjectItem item in project.ProjectItems)
                {
                    Project p = item.Object as Project;
                    if(p == null)
                    {
                        continue;
                    }
                    buildOrder(solution, p, ref projects);
                }
            }
            BuildDependencies dependencies = solution.SolutionBuild.BuildDependencies;
            for(int i = 0; i < dependencies.Count; ++i)
            {
                BuildDependency dp = dependencies.Item(i + 1);
                if(dp.Project.Equals(project))
                {
                    System.Array requiredProjects = dp.RequiredProjects as System.Array;
                    if(requiredProjects == null)
                    {
                        continue;
                    }
                    foreach(Project p in requiredProjects)
                    {
                        Util.buildOrder(solution, p, ref projects);
                    }
                }
            }
            if(projects.Find(
                delegate(Project p)
                    {
                        return project.UniqueName.Equals(p.UniqueName);
                    }) == null)
            {
                projects.Add(project);
            }
        }

        public static List<Project> solutionFolderProjects(Project project)
        {
            List<Project> projects = new List<Project>();
            if(!project.Kind.Equals(EnvDTE80.ProjectKinds.vsProjectKindSolutionFolder))
            {
                return projects;
            }
            solutionFolderProjects(project, ref projects);
            return projects;
        }

        public static void solutionFolderProjects(Project project, ref List<Project> projects)
        {
            if(project.Kind.Equals(EnvDTE80.ProjectKinds.vsProjectKindSolutionFolder))
            {
                foreach(ProjectItem item in project.ProjectItems)
                {
                    Project p = item.Object as Project;
                    if(p == null)
                    {
                        continue;
                    }
                    solutionFolderProjects(p, ref projects);
                }
            }
            else
            {
                if(!projects.Contains(project))
                {
                    projects.Add(project);
                }
            }
        }

        //
        // This methods refresh the solution explorer if the command
        // is available.
        //
        public static void solutionExplorerRefresh()
        {
            if(_refreshCommand != null && _refreshCommand.IsAvailable)
            {
                DTE dte = getCurrentDTE();
                object objIn = null;
                object objOut = null;
                dte.Commands.Raise(refreshCommandGUID, refreshCommandID, ref objIn, ref objOut);
            }
        }

        public static void setRefreshCommand(Command command)
        {
            _refreshCommand = command;
        }

        static public bool hasProjecType(Project project, string type)
        {
            ComponentList types = new ComponentList(getProjectTypeGuids(project), ';');
            return types.Contains(type);
        }
        
        static public string getProjectTypeGuids(Project proj)
        {
            string guids = "";
            Microsoft.VisualStudio.Shell.Interop.IVsHierarchy hierarchy = null;
            Microsoft.VisualStudio.Shell.Interop.IVsAggregatableProject aggregatableProject = null;
            IVsSolution solution = getIVsSolution();

            int result = solution.GetProjectOfUniqueName(proj.UniqueName, out hierarchy);

            if(result == 0)
            {
                aggregatableProject = (Microsoft.VisualStudio.Shell.Interop.IVsAggregatableProject)hierarchy;
                result = aggregatableProject.GetAggregateProjectTypeGuids(out guids);
            }
            return guids;
        }

        static private IVsSolution getIVsSolution()
        {
            
            Guid serviceGuid = typeof(SVsSolution).GUID;
            Guid interfaceGuid = typeof(IVsSolution).GUID;
            IntPtr ppObject;

            Microsoft.VisualStudio.OLE.Interop.IServiceProvider serviceProvider = 
                (Microsoft.VisualStudio.OLE.Interop.IServiceProvider)getCurrentDTE();

            if(ErrorHandler.Failed(serviceProvider.QueryService(ref serviceGuid, ref interfaceGuid, out ppObject)))
            {
                return null;
            }

            IVsSolution service = null;
            if(!ppObject.Equals(IntPtr.Zero))
            {
                service = System.Runtime.InteropServices.Marshal.GetObjectForIUnknown(ppObject) as IVsSolution;
                System.Runtime.InteropServices.Marshal.Release(ppObject);
            }

            return service;
        }

        //
        // Check if we are using a debug run-time library.
        //
        static public bool isDebug(runtimeLibraryOption rt)
        {
            return rt == runtimeLibraryOption.rtMultiThreadedDebug || rt == runtimeLibraryOption.rtMultiThreadedDebugDLL;
        }

        //
        // Check if the run time library is supported with Ice projects
        //
        static public bool checkCppRunTimeLibrary(Project p, runtimeLibraryOption rt)
        {
            return rt == runtimeLibraryOption.rtMultiThreadedDebugDLL || rt == runtimeLibraryOption.rtMultiThreadedDLL;
        }

        static public bool checkCppRunTimeLibrary(Builder builder, Project project, VCCLCompilerTool compilerTool,
                                                  LinkerAdapter linkerTool)
        {
            //
            // Check the project run time library for the active configuration.
            //
            if(!Util.checkCppRunTimeLibrary(project, compilerTool.RuntimeLibrary))
            {
                builder.addError(project, "", TaskErrorCategory.Error, 0, 0,
                    "The selected C++ Runtime Library is not supported by Ice; Ice requires /MD or /MDd.");
                return false;
            }

            //
            // Ensure that linker settings match the Runtime Library settings.
            //
            ComponentList components = Util.getIceCppComponents(project);
            bool debug = Util.isDebug(compilerTool.RuntimeLibrary);
            string additionalDependencies = linkerTool.AdditionalDependencies;

            //
            // For each component we need to check that the correct 
            // library version (debug/release) is used.
            //
            foreach(string c in components)
            {
                string debugName = c + "d.lib";
                string releaseName = c + ".lib";

                if((debug && additionalDependencies.Contains(debugName)) ||
                   (!debug && additionalDependencies.Contains(releaseName)))
                {
                    continue;
                }

                if(debug)
                {
                    if(additionalDependencies.Contains(releaseName))
                    {
                        additionalDependencies = additionalDependencies.Replace(releaseName, debugName);
                    }
                    else
                    {
                        if(!String.IsNullOrEmpty(additionalDependencies) &&
                           !additionalDependencies.TrimEnd().EndsWith(";"))
                        {
                            additionalDependencies += ";";
                        }
                        additionalDependencies += debugName;
                    }
                }
                else
                {
                    if(additionalDependencies.Contains(debugName))
                    {
                        additionalDependencies = additionalDependencies.Replace(debugName, releaseName);
                    }
                    else
                    {
                        if(!String.IsNullOrEmpty(additionalDependencies) &&
                           !additionalDependencies.TrimEnd().EndsWith(";"))
                        {
                            additionalDependencies += ";";
                        }
                        additionalDependencies += releaseName;
                    }
                }
            }

            //
            // If the linker settings has changed we update it.
            //
            if(!additionalDependencies.Equals(linkerTool.AdditionalDependencies))
            {
                linkerTool.AdditionalDependencies = additionalDependencies;
            }
            return true;
        }

        static public VCConfiguration getActiveVCConfiguration(Project project)
        {
            if(!Util.isCppProject(project))
            {
                return null;
            }
            ConfigurationManager configManager = project.ConfigurationManager;
            if(configManager == null)
            {
                return null;
            }

            Configuration activeConfig;
            try
            {
                activeConfig = (Configuration)configManager.ActiveConfiguration;
            }
            catch(COMException)
            {
                return null;
            }

            if(activeConfig == null)
            {
                return null;
            }

            VCProject vcProject = (VCProject)project.Object;
            IVCCollection configurations = (IVCCollection)vcProject.Configurations;
            VCConfiguration configuration = null;
            foreach(VCConfiguration c in configurations)
            {
                if(c.Name != (activeConfig.ConfigurationName + "|" + activeConfig.PlatformName))
                {
                    continue;
                }
                configuration = c;
                break;
            }
            return configuration;
        }

        public static string projectFullName(Project p)
        {
            if(p.ParentProjectItem != null &&
               p.ParentProjectItem.ContainingProject != null)
            {
                return projectFullName(p.ParentProjectItem.ContainingProject) + "/" + p.Name;
            }
            return p.Name;
        }

        public static string getTraceProjectName(Project project)
        {
            string fullName = projectFullName(project);
            if(fullName.Equals(project.Name))
            {
                return fullName;
            }
            return project.Name + " (" + fullName + ")";
        }

        private static Command _refreshCommand;
        public const string refreshCommandGUID = "{1496A755-94DE-11D0-8C3F-00C04FC2AAE2}";
        public const string vsSmartDeviceCSharp = "{4D628B5B-2FBC-4AA6-8C16-197242AEB884}";
        public const string vsSmartDeviceVB = "{68B1623D-7FB9-47D8-8664-7ECEA3297D4F}";

        public const int refreshCommandID = 222;
    }
}
