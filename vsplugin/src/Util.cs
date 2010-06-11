// **********************************************************************
//
// Copyright (c) 2003-2010 ZeroC, Inc. All rights reserved.
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
using Microsoft.VisualStudio.CommandBars;
using Microsoft.VisualStudio.VCProjectEngine;
using Microsoft.VisualStudio.VCProject;
using Microsoft.VisualStudio.Shell;
using System.Resources;
using System.Reflection;
using VSLangProj;
using System.Globalization;

using System.Collections;
using System.Runtime.InteropServices.ComTypes;
using Microsoft.CSharp;

namespace Ice.VisualStudio
{
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

        public new void Add(string value)
        {
            value = value.Trim();
            if(!base.Contains(value))
            {
                base.Add(value);
            }
        }

        public new bool Contains(string value)
        {
            string found = base.Find(delegate(string s)
                                    {
                                        return s.Equals(value, StringComparison.CurrentCultureIgnoreCase);
                                    });
            return !String.IsNullOrEmpty(found);
        }

        public new void Remove(string value)
        {
            string found = base.Find(delegate(string s)
            {
                return s.Equals(value, StringComparison.CurrentCultureIgnoreCase);
            });

            if(!String.IsNullOrEmpty(found))
            {
                base.Remove(found);
            }
        }

        public ComponentList(string value)
        {
            init(value, ';');
        }

        public ComponentList(string value, char separator)
        {
            init(value, separator);
        }

        private void init(string value, char separator)
        {
            Array items = value.Split(separator);
            foreach(string s in items)
            {
                string trimmed = s.Trim();
                if(trimmed.Length > 0)
                {
                    Add(trimmed);
                }
            }
        }

        public override string ToString()
        {
            return ToString(';');
        }

        public string ToString(char separator)
        {
            StringBuilder sb = new StringBuilder();
            for(int cont = 0; cont < this.Count; cont++)
            {
                sb.Append(this[cont]);
                if(cont < this.Count - 1)
                {
                    if(!separator.Equals(' '))
                    {
                        sb.Append(' ');
                    }
                    sb.Append(separator);
                    if(!separator.Equals(' '))
                    {
                        sb.Append(' ');
                    }
                }
            }
            return sb.ToString();
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
    }

    public static class Util
    {
        public const string slice2cs = "slice2cs.exe";
        public const string slice2cpp = "slice2cpp.exe";
        public const string slice2sl = "slice2sl.exe";

        //
        // Property names used to persist project configuration.
        //
        public const string PropertyIce = "ZerocIce_Enabled";
        public const string PropertyIceHome = "ZerocIce_Home";
        public const string PropertyIceHomeExpanded = "ZerocIce_HomeExpanded";
        public const string PropertyIceComponents = "ZerocIce_Components";
        public const string PropertyIceExtraOptions = "ZerocIce_ExtraOptions";
        public const string PropertyIceIncludePath = "ZerocIce_IncludePath";
        public const string PropertyIceStreaming = "ZerocIce_Streaming";
        public const string PropertyIceChecksum = "ZerocIce_Checksum";
        public const string PropertyIceTie = "ZerocIce_Tie";
        public const string PropertyIcePrefix = "ZerocIce_Prefix";
        public const string PropertyIceDllExport = "ZerocIce_DllExport";
        public const string PropertyConsoleOutput = "ZerocIce_ConsoleOutput";

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

        public static string[] getDotNetNames()
        {
            return (string[])dotNetNames.Clone();
        }

        const string iceSilverlightHome = "C:\\IceSL-0.3.3";
        static string defaultIceHome;

        private static void setIceHomeDefault()
        {
            defaultIceHome = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);
            if(defaultIceHome.EndsWith("\\bin", StringComparison.Ordinal))
            {
                defaultIceHome = defaultIceHome.Substring(0, defaultIceHome.Length - 4);
            }
        }

        public static string getIceHomeRaw(Project project, bool update)
        {
            if(defaultIceHome == null)
            {
                setIceHomeDefault();
            }

            if(Util.isSilverlightProject(project))
            {
                return Util.getProjectProperty(project, Util.PropertyIceHome, iceSilverlightHome, update);
            }
            string iceHome = Util.getProjectProperty(project, Util.PropertyIceHome, "", update);
            if(iceHome.Length == 0)
            {
                iceHome = defaultIceHome;
            }
            return iceHome;
        }

        public static string getIceHome(Project project)
        {
            if(defaultIceHome == null)
            {
                setIceHomeDefault();
            }

            if(Util.isSilverlightProject(project))
            {
                return Util.getProjectProperty(project, Util.PropertyIceHomeExpanded, iceSilverlightHome);
            }
            string iceHome = Util.getProjectProperty(project, Util.PropertyIceHomeExpanded, defaultIceHome);
            Environment.SetEnvironmentVariable("IceHome", iceHome);
            return iceHome;
        }

        public static string getAbsoluteIceHome(Project project)
        {
            string iceHome = Util.getIceHome(project);
            if(!Path.IsPathRooted(iceHome))
            {
                iceHome = Path.Combine(Path.GetDirectoryName(project.FileName), iceHome);
                iceHome = Path.GetFullPath(iceHome);
            }
            return iceHome;
        }
        
        public static string getPathRelativeToProject(ProjectItem item)
        {
            StringBuilder path = new StringBuilder();
            if(item != null)
            {
                path.Append(Util.getPathRelativeToProject(item, item.ContainingProject.ProjectItems));
            }
            return Util.normalizePath(path.ToString());
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

        public static void addCppIncludes(VCCLCompilerTool tool, Project project)
        {
            if(tool == null || project == null)
            {
                return;
            }

            string iceHome = Util.getAbsoluteIceHome(project);
            string iceIncludeDir = "";
            if(Directory.Exists(iceHome + "\\cpp\\include"))
            {
                iceIncludeDir = Util.getIceHome(project) + "\\cpp\\include";
            }
            else
            {
                iceIncludeDir = Util.getIceHome(project) + "\\include";
            }

            string additionalIncludeDirectories = tool.AdditionalIncludeDirectories;
            if(String.IsNullOrEmpty(additionalIncludeDirectories))
            {
                tool.AdditionalIncludeDirectories = iceIncludeDir + ";.";
                return;
            }

            ComponentList includes = new ComponentList(additionalIncludeDirectories);
            bool changed = false;
            if(!includes.Contains(iceIncludeDir))
            {
                changed = true;
                includes.Add(iceIncludeDir);
            }

            if(!includes.Contains("."))
            {
                changed = true;
                includes.Add(".");
            }

            if(changed)
            {
                tool.AdditionalIncludeDirectories = includes.ToString();
            }
        }

        private static readonly string[] _cppIncludeDirs =
        {
            "\\include",
            "\\cpp\\include",
        };

        public static void removeCppIncludes(VCCLCompilerTool tool, Project project)
        {
            if(tool == null || project == null)
            {
                return;
            }

            string additionalIncludeDirectories = tool.AdditionalIncludeDirectories;
            if(String.IsNullOrEmpty(additionalIncludeDirectories))
            {
                return;
            }
            ComponentList includes = new ComponentList(additionalIncludeDirectories);

            string iceHome = Util.getIceHome(project);
            foreach(string dir in _cppIncludeDirs)
            {
                string includeDir = iceHome + dir;
                if(includes.Contains(includeDir))
                {
                    includes.Remove(includeDir);
                    tool.AdditionalIncludeDirectories = includes.ToString();
                    break;
                }
            }
        }

        private static readonly string[] _csBinDirs =
        {
            "\\bin\\",
            "\\cs\\bin\\",
            "\\sl\\bin\\",
        };

        public static void addDotNetReference(Project project, string component)
        {
            if(project == null || String.IsNullOrEmpty(component))
            {
                return;
            }

            string iceHome = Util.getAbsoluteIceHome(project);
            foreach(string dir in _csBinDirs)
            {
                if(Directory.Exists(iceHome + dir))
                {
                    string reference = iceHome + dir + component + ".dll";                
                    if(File.Exists(reference))
                    {
                        VSLangProj.VSProject vsProject = (VSLangProj.VSProject)project.Object;
                        try
                        {
                            vsProject.References.Add(reference);
                            return;
                        }
                        catch(COMException ex)
                        {
                            Console.WriteLine(ex);
                        }
                    }
                }
            }

            System.Windows.Forms.MessageBox.Show("Could not locate '" + component + 
                                                 ".dll'. Review you 'Ice Home' setting.",
                                                 "Ice Visual Studio Extension", MessageBoxButtons.OK,
                                                 MessageBoxIcon.Error,
                                                 System.Windows.Forms.MessageBoxDefaultButton.Button1,
                                                 System.Windows.Forms.MessageBoxOptions.RightAlign);
        }

        public static bool removeDotNetReference(Project project, string component)
        {
            if(project == null || String.IsNullOrEmpty(component))
            {
                return false;
            }

            foreach(Reference r in ((VSProject)project.Object).References)
            {
                if(r.Identity.Equals(component, StringComparison.OrdinalIgnoreCase))
                {
                    r.Remove();
                    return true;
                }
            }
            return false;
        }

        public static void addCppLib(VCLinkerTool tool, string component, bool debug)
        {
            if(tool == null || String.IsNullOrEmpty(component))
            {
                return;
            }

            if(Array.BinarySearch(Util.getCppNames(), component) < 0)
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

        public static bool removeCppLib(VCLinkerTool tool, string component, bool debug)
        {
            if(tool == null)
            {
                return false;
            }

            if(String.IsNullOrEmpty(tool.AdditionalDependencies))
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

        public static void addIceCppEnviroment(VCDebugSettings debugSettings, Project project, bool x64)
        {
            if(debugSettings == null || project == null)
            {
                return;
            }
            string iceHome = Util.getAbsoluteIceHome(project);
            string iceBinDir = "";
            if(Directory.Exists(iceHome + "\\cpp\\bin"))
            {
                iceBinDir = Util.getIceHome(project) + "\\cpp\\bin";
            }
            else
            {
                iceBinDir = Util.getIceHome(project) + "\\bin";
#if VS2010
                iceBinDir += "\\vc100";
#endif
                if(x64)
                {
                    iceBinDir += "\\x64";
                }
            }
            string icePath = "PATH=" + iceBinDir;

            string enviroment = debugSettings.Environment;
            if(String.IsNullOrEmpty(enviroment))
            {
                debugSettings.Environment = "PATH=" + iceBinDir;
                return;
            }

            ComponentList envs = new ComponentList(enviroment, '\n');
            if(!envs.Contains(icePath))
            {
                envs.Add(icePath);
                debugSettings.Environment = envs.ToString('\n');
                return;
            }
        
        }

        private static readonly string[] _cppBinDirs =
        {
            "\\bin",
            "\\bin\\x64",
            "\\bin\\vc100",
            "\\bin\\vc100\\x64",
            "\\cpp\\bin",
        };

        public static void removeIceCppEnviroment(VCDebugSettings debugSettings,
                                                  Project project)
        {
            if(debugSettings == null || project == null)
            {
                return;
            }

            string iceHome = Util.getIceHome(project);
            foreach(string dir in _cppBinDirs)
            {
                string enviroment = debugSettings.Environment;
                if(String.IsNullOrEmpty(enviroment))
                {
                    return;
                }

                ComponentList envs = new ComponentList(enviroment, '\n');
                string binDir = "PATH=" + iceHome + dir;
                if(envs.Contains(binDir))
                {
                    envs.Remove(binDir);
                    debugSettings.Environment = envs.ToString('\n');
                    return;
                }
            }
        }

        public static void addIceCppLibraryDir(VCLinkerTool tool, Project project, bool x64)
        {
            if(tool == null || project == null)
            {
                return;
            }

            string iceHome = Util.getAbsoluteIceHome(project);
            string iceLibDir = "";
            if(Directory.Exists(iceHome + "\\cpp\\lib"))
            {
                iceLibDir = Util.getIceHome(project) + "\\cpp\\lib";
            }
            else
            {
                iceLibDir = Util.getIceHome(project) + "\\lib";
#if VS2010
                iceLibDir += "\\vc100";
#endif
                if(x64)
                {
                    iceLibDir += "\\x64";
                }
            }

            string additionalLibraryDirectories = tool.AdditionalLibraryDirectories;
            if(String.IsNullOrEmpty(additionalLibraryDirectories))
            {
                tool.AdditionalLibraryDirectories = iceLibDir;
                return;
            }

            ComponentList libs = new ComponentList(additionalLibraryDirectories);
            if(!libs.Contains(iceLibDir))
            {
                libs.Add(iceLibDir);
                tool.AdditionalLibraryDirectories = libs.ToString();
                return;
            }
        }

        private static readonly string[] _cppLibDirs =
        {
            "\\lib",
            "\\lib\\x64",
            "\\lib\\vc100",
            "\\lib\\vc100\\x64",
            "\\cpp\\lib",
        };

        public static void removeIceCppLibraryDir(VCLinkerTool tool, Project project)
        {
            if(tool == null || project == null)
            {
                return;
            }

            string iceHome = Util.getIceHome(project);
            foreach(string dir in _cppLibDirs)
            {
                string additionalLibraryDirectories = tool.AdditionalLibraryDirectories;
                if(String.IsNullOrEmpty(additionalLibraryDirectories))
                {
                    return;
                }

                ComponentList libs = new ComponentList(additionalLibraryDirectories);
                string libDir = iceHome + dir;
                if(libs.Contains(libDir))
                {
                    libs.Remove(libDir);
                    tool.AdditionalLibraryDirectories = libs.ToString();
                    return;
                }
            }
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
                    if(Path.GetFullPath(fullPath).Equals(
                           Path.GetFullPath(path), StringComparison.CurrentCultureIgnoreCase))
                    {
                        item = i;
                        break;
                    }
                }
                else if(Util.isProjectItemFolder(i))
                {
                    string p = Path.GetDirectoryName(i.Properties.Item("FullPath").Value.ToString());
                    if(p.Equals(Path.GetFullPath(path), StringComparison.CurrentCultureIgnoreCase))
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
                                            Util.normalizePath(Util.getPathRelativeToProject(i))));

                    if(p.Equals(Path.GetFullPath(path), StringComparison.CurrentCultureIgnoreCase))
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
                    if(file.FullPath != fullPath)
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

        public static string normalizePath(string path)
        {
            path = path.Replace('/', '\\');
            path = path.Replace(".\\", "");
            if(path.IndexOf("\\", StringComparison.Ordinal) == 0)
            {
                path = path.Substring(1, path.Length - 1);
            }
            if(path.EndsWith("\\.", StringComparison.Ordinal))
            {
                path = path.Substring(0, path.Length - "\\.".Length);
            }
            return path;
        }

        public static string relativePath(string mainDirPath, string absoluteFilePath)
        {
            if(absoluteFilePath != null)
            {
                absoluteFilePath = absoluteFilePath.Trim();
            }
            if(mainDirPath != null)
            {
                mainDirPath = mainDirPath.Trim();
            }
            if(String.IsNullOrEmpty(absoluteFilePath) || String.IsNullOrEmpty(mainDirPath))
            {
                return "";
            }
            string[] firstPathParts = 
                mainDirPath.Trim(Path.DirectorySeparatorChar).Split(Path.DirectorySeparatorChar);
            string[] secondPathParts =
                absoluteFilePath.Trim(Path.DirectorySeparatorChar).Split(Path.DirectorySeparatorChar);

            int sameCounter = 0;
            for(int i = 0; i < Math.Min(firstPathParts.Length, secondPathParts.Length); i++)
            {
                if(!firstPathParts[i].Equals(secondPathParts[i], StringComparison.CurrentCultureIgnoreCase))
                {
                    break;
                }
                sameCounter++;
            }

            if(sameCounter == 0)
            {
                return absoluteFilePath;
            }

            string newPath = String.Empty;
            for(int i = sameCounter; i < firstPathParts.Length; i++)
            {
                if(i > sameCounter)
                {
                    newPath += Path.DirectorySeparatorChar;
                }
                newPath += "..";
            }

            if(newPath.Length == 0)
            {
                newPath = ".";
            }

            for(int i = sameCounter; i < secondPathParts.Length; i++)
            {
                newPath += Path.DirectorySeparatorChar;
                newPath += secondPathParts[i];
            }
            return newPath;
        }

        public static ProjectItem getSelectedProjectItem(_DTE dte)
        {
            UIHierarchyItem uiItem = getSelectedUIHierearchyItem(dte);
            if(uiItem == null)
            {
                return null;
            }
            return uiItem.Object as ProjectItem;
        }

        public static Project getSelectedProject()
        {
            return Util.getSelectedProject(Util.getCurrentDTE());
        }

        public static Project getSelectedProject(_DTE dte)
        {
            UIHierarchyItem uiItem = getSelectedUIHierearchyItem(dte);
            if(uiItem == null)
            {
                return null;
            }
            return uiItem.Object as Project;
        }

        public static UIHierarchyItem getSelectedUIHierearchyItem(_DTE dte)
        {
            if(dte == null)
            {
                return null;
            }

            UIHierarchy uiHierarchy = 
                (EnvDTE.UIHierarchy)dte.Windows.Item(EnvDTE.Constants.vsWindowKindSolutionExplorer).Object;
            if(uiHierarchy == null)
            {
                return null;
            }

            if(uiHierarchy.SelectedItems == null)
            {
                return null;
            }

            if(((Array)uiHierarchy.SelectedItems).Length <= 0)
            {
                return null;
            }
            return (UIHierarchyItem)((Array)uiHierarchy.SelectedItems).GetValue(0);
        }

        public static void updateIceHome(Project project, string iceHome, bool force)
        {
            if(project == null || String.IsNullOrEmpty(iceHome))
            {
                return;
            }

            if(!force)
            {
                string oldIceHome = Util.getIceHomeRaw(project, true).ToUpper(CultureInfo.InvariantCulture);
                if(oldIceHome.Equals(iceHome, StringComparison.CurrentCultureIgnoreCase))
                {
                    return;
                }
            }

            if(Util.isCSharpProject(project) || Util.isVBProject(project))
            {
                updateIceHomeDotNetProject(project, iceHome);
            }
            else if(Util.isCppProject(project))
            {
                updateIceHomeCppProject(project, iceHome);
            }
        }

        private static void updateIceHomeCppProject(Project project, string iceHome)
        {
            Util.removeIceCppConfigurations(project);
            Util.setIceHome(project, iceHome);
            Util.addIceCppConfigurations(project);
        }

        private static bool getCopyLocal(Project project, string name)
        {
            VSLangProj.VSProject vsProject = (VSLangProj.VSProject)project.Object;
            foreach(Reference r in vsProject.References)
            {
                if(r.Name.Equals(name))
                {
                    return r.CopyLocal;
                }
            }
            return true;
        }

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

        private static void updateIceHomeDotNetProject(Project project, string iceHome)
        {
            Util.setIceHome(project, iceHome);

            ComponentList components = Util.getIceDotNetComponents(project);
            foreach(string s in components)
            {
                if(String.IsNullOrEmpty(s))
                {
                    continue;
                }

                bool copyLocal = getCopyLocal(project, s);
                Util.removeDotNetReference(project, s);

                Util.addDotNetReference(project, s);
                setCopyLocal(project, s, copyLocal);
            }
        }

        public static void setIceHome(Project project, string value)
        {
            string expanded = subEnvironmentVars(value);

            string fullPath = expanded;
            if(!Path.IsPathRooted(fullPath))
            {
                fullPath = Path.GetFullPath(Path.Combine(Path.GetDirectoryName(project.FileName), fullPath));
            }

            if(Util.isSilverlightProject(project))
            {
                if(!File.Exists(fullPath + "\\bin\\slice2sl.exe") || !Directory.Exists(fullPath + "\\slice\\Ice"))
                {
                    if(!File.Exists(fullPath + "\\cpp\\bin\\slice2sl.exe") ||
                       !Directory.Exists(fullPath + "\\sl\\slice\\Ice"))
                    {
                        System.Windows.Forms.MessageBox.Show("Could not locate Ice for Silverlight installation in '"
                                                             + expanded + "' directory.\n",
                                                             "Ice Visual Studio Extension", MessageBoxButtons.OK,
                                                             MessageBoxIcon.Error,
                                                             System.Windows.Forms.MessageBoxDefaultButton.Button1,
                                                             System.Windows.Forms.MessageBoxOptions.RightAlign);
                        return;
                    }
                }
            }
            else if(Util.isCppProject(project))
            {
                if(!Directory.Exists(fullPath + "\\slice\\Ice") || 
                   (!File.Exists(fullPath + "\\bin\\slice2cpp.exe") && 
                    !File.Exists(fullPath + "\\bin\\x64\\slice2cpp.exe") &&  
                    !File.Exists(fullPath + "\\cpp\\bin\\slice2cpp.exe")))
                {
                    System.Windows.Forms.MessageBox.Show("Could not locate Ice installation in '"
                                                         + expanded + "' directory.\n",
                                                         "Ice Visual Studio Extension", MessageBoxButtons.OK,
                                                         MessageBoxIcon.Error,
                                                         System.Windows.Forms.MessageBoxDefaultButton.Button1,
                                                         System.Windows.Forms.MessageBoxOptions.RightAlign);

                    return;
                }
            }
            else if(Util.isCSharpProject(project))
            {
                if(!Directory.Exists(fullPath + "\\slice\\Ice") || 
                   (!File.Exists(fullPath + "\\bin\\slice2cs.exe") && 
                    !File.Exists(fullPath + "\\bin\\x64\\slice2cs.exe") &&  
                    !File.Exists(fullPath + "\\cpp\\bin\\slice2cs.exe")))
                {
                    System.Windows.Forms.MessageBox.Show("Could not locate Ice installation in '"
                                                         + expanded + "' directory.\n",
                                                         "Ice Visual Studio Extension", MessageBoxButtons.OK,
                                                         MessageBoxIcon.Error,
                                                         System.Windows.Forms.MessageBoxDefaultButton.Button1,
                                                         System.Windows.Forms.MessageBoxOptions.RightAlign);

                    return;
                }
            }
            else if(Util.isVBProject(project))
            {
                if(!File.Exists(fullPath + "\\bin\\Ice.dll") && !File.Exists(fullPath + "\\cs\\bin\\Ice.dll"))
                {
                    System.Windows.Forms.MessageBox.Show("Could not locate Ice installation in '"
                                                         + expanded + "' directory.\n",
                                                         "Ice Visual Studio Extension", MessageBoxButtons.OK,
                                                         MessageBoxIcon.Error,
                                                         System.Windows.Forms.MessageBoxDefaultButton.Button1,
                                                         System.Windows.Forms.MessageBoxOptions.RightAlign);

                    return;
                }
            }

            if(value.Equals(defaultIceHome))
            {
                setProjectProperty(project, Util.PropertyIceHome, "");
            }
            else
            {
                setProjectProperty(project, Util.PropertyIceHome, value);
            }
            setProjectProperty(project, Util.PropertyIceHomeExpanded, expanded);
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
            if(project == null || String.IsNullOrEmpty(name))
            {
                return defaultValue;
            }

            if(project.Globals == null)
            {
                return defaultValue;
            }

            if(project.Globals.get_VariableExists(name))
            {
                return project.Globals[name].ToString();
            }

            if(update && !String.IsNullOrEmpty(defaultValue))
            {
                project.Globals[name] = defaultValue;
                if(!project.Globals.get_VariablePersists(name))
                {
                    project.Globals.set_VariablePersists(name, true);
                }
            }
            return defaultValue;
        }

        public static void setProjectProperty(Project project, string name, string value)
        {
            if(project == null || String.IsNullOrEmpty(name))
            {
                return;
            }

            if(project.Globals == null)
            {
                return;
            }
	    
	    project.Globals[name] = value;
	    if(!project.Globals.get_VariablePersists(name))
	    {
                project.Globals.set_VariablePersists(name, true);
	    }
        }
        
        public static String getPrecompileHeader(Project project)
        {
            if(!Util.isCppProject(project))
            {
                return "";
            }
            ConfigurationManager configManager = project.ConfigurationManager;
            Configuration activeConfig = (Configuration)configManager.ActiveConfiguration;

            VCProject vcProject = (VCProject)project.Object;
            IVCCollection configurations = (IVCCollection)vcProject.Configurations;
            String preCompiledHeader = "";
            foreach(VCConfiguration conf in configurations)
            {
                if(conf.Name != (activeConfig.ConfigurationName + "|" + activeConfig.PlatformName))
                {
                    continue;
                }
                VCCLCompilerTool compilerTool = 
                    (VCCLCompilerTool)(((IVCCollection)conf.Tools).Item("VCCLCompilerTool"));
                if(compilerTool == null)
                {
                    break;
                }
                if(compilerTool.UsePrecompiledHeader == pchOption.pchCreateUsingSpecific ||
                   compilerTool.UsePrecompiledHeader == pchOption.pchUseUsingSpecific)
                {
                    preCompiledHeader = compilerTool.PrecompiledHeaderThrough;
                }
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
                if(conf.Name != (activeConfig.ConfigurationName + "|" + activeConfig.PlatformName))
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
                    if(debug)
                    {
                        libName = libName.Substring(0, libName.Length - 1);
                    }
                    if(String.IsNullOrEmpty(libName))
                    {
                        continue;
                    }

                    if(Array.BinarySearch(Util.getCppNames(), libName) < 0)
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
                if(Array.BinarySearch(Util.getSilverlightNames(), r.Name) < 0)
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

            VSLangProj.VSProject vsProject = (VSLangProj.VSProject)project.Object;
            foreach(Reference r in vsProject.References)
            {
                if(Array.BinarySearch(Util.getDotNetNames(), r.Name) < 0)
                {
                    continue;
                }

                components.Add(r.Name);
            }
            return components;
        }

        public static void addIceCppConfigurations(Project project)
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
                    bool x64 = false;
                    VCPlatform platform = (VCPlatform)conf.Platform;
                    String platformName = platform.Name;
                    if(platformName.Equals("x64", StringComparison.CurrentCultureIgnoreCase) ||
                       platformName.Equals("Itanium", StringComparison.CurrentCultureIgnoreCase))
                    {
                        x64 = true;
                    }
                    VCCLCompilerTool compilerTool =
                        (VCCLCompilerTool)(((IVCCollection)conf.Tools).Item("VCCLCompilerTool"));
                    VCLinkerTool linkerTool = (VCLinkerTool)(((IVCCollection)conf.Tools).Item("VCLinkerTool"));

                    Util.addIceCppEnviroment((VCDebugSettings)conf.DebugSettings, project, x64);
                    Util.addIceCppLibraryDir(linkerTool, project, x64);
                    Util.addCppIncludes(compilerTool, project);
                }
            }
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
                if(conf != null)
                {
                    VCCLCompilerTool compilerTool =
                        (VCCLCompilerTool)(((IVCCollection)conf.Tools).Item("VCCLCompilerTool"));
                    VCLinkerTool linkerTool = (VCLinkerTool)(((IVCCollection)conf.Tools).Item("VCLinkerTool"));

                    Util.removeIceCppEnviroment((VCDebugSettings)conf.DebugSettings, project);
                    Util.removeIceCppLibraryDir(linkerTool, project);
                    Util.removeCppIncludes(compilerTool, project);
                }
            }
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
                if(conf != null)
                {
                    VCCLCompilerTool compilerTool = 
                        (VCCLCompilerTool)(((IVCCollection)conf.Tools).Item("VCCLCompilerTool"));
                    VCLinkerTool linkerTool = (VCLinkerTool)(((IVCCollection)conf.Tools).Item("VCLinkerTool"));

                    if(compilerTool == null || linkerTool == null)
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
                    foreach(string component in components)
                    {
                        if(String.IsNullOrEmpty(component))
                        {
                            continue;
                        }
                        Util.addCppLib(linkerTool, component, debug);
                    }
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
                if(conf != null)
                {
                    VCCLCompilerTool compilerTool = 
                        (VCCLCompilerTool)(((IVCCollection)conf.Tools).Item("VCCLCompilerTool"));
                    VCLinkerTool linkerTool = (VCLinkerTool)(((IVCCollection)conf.Tools).Item("VCLinkerTool"));

                    if(compilerTool == null || linkerTool == null)
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
                        if(s != null)
                        {
                            if(Util.removeCppLib(linkerTool, s, debug)  && !removed.Contains(s))
                            {
                                removed.Add(s);
                            }
                        }
                    }
                }
            }
            return removed;
        }

        public static DTE getCurrentDTE()
        {
           return Connect.getCurrentDTE();
        }

        public static string subEnvironmentVars(string s)
        {
            string result = s;
            int beg = 0;
            int end;
            while((beg = result.IndexOf("$(", beg, StringComparison.Ordinal)) != -1 && beg < result.Length -1)
            {
                end = result.IndexOf(")", beg + 1, StringComparison.Ordinal);
                if(end == -1)
                {
                    break;
                }
                string variable = result.Substring(beg + 2, end - beg - 2);
                string value = System.Environment.GetEnvironmentVariable(variable);
                if(value == null)
                {
                    value = "";
                }
                result = result.Replace("$(" + variable + ")", value);
                beg += value.Length;
            }
            return result;
        }

        public static bool containsEnvironmentVars(string s)
        {
            int pos = s.IndexOf("$(", StringComparison.Ordinal);
            if(pos != -1)
            {
                return s.IndexOf(')', pos) != -1;
            }
            return false;
        }
    }
}
