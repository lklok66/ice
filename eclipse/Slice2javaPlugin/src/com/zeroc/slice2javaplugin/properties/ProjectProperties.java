// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This plug-in is provided to you under the terms and conditions
// of the Eclipse Public License Version 1.0 ("EPL"). A copy of
// the EPL is available at http://www.eclipse.org/legal/epl-v10.html.
//
// **********************************************************************

package com.zeroc.slice2javaplugin.properties;

import java.io.IOException;
import java.util.Arrays;
import java.util.Iterator;

import org.eclipse.core.resources.IFolder;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IncrementalProjectBuilder;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;
import org.eclipse.jface.dialogs.ErrorDialog;
import org.eclipse.jface.preference.PreferencePage;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.FocusEvent;
import org.eclipse.swt.events.FocusListener;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.DirectoryDialog;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.List;
import org.eclipse.swt.widgets.TabFolder;
import org.eclipse.swt.widgets.TabItem;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.dialogs.ContainerSelectionDialog;
import org.eclipse.ui.dialogs.PropertyPage;

import com.zeroc.slice2javaplugin.Activator;
import com.zeroc.slice2javaplugin.builder.Slice2javaBuilder;

public class ProjectProperties extends PropertyPage
{
    public ProjectProperties()
    {
        super();
        setTitle("Slice2java settings");
        noDefaultAndApplyButton();
    }
    
    public void performApply()
    {
        super.performApply();
    }

    public boolean
    performOk()
    {
        final IProject project = (IProject)getElement();
        
        try
        {
            _config.setIceHome(getShell(), _iceHome.getText());
            _config.setGeneratedDir(_generatedDir.getText());
            _config.setSliceSourceDirs(Arrays.asList(_sourceDirectories.getItems()));
            _config.setIncludes(Arrays.asList(_includes.getItems()));
            _config.setDefines(Configuration.toList(_defines.getText()));
            _config.setMeta(Configuration.toList(_meta.getText()));
            _config.setStream(_stream.getSelection());
            _config.setTie(_tie.getSelection());
            _config.setConsole(_console.getSelection());

            if(_config.write(project))
            {
                // The configuration properties were changed. We need to rebuild
                // the slice files.
                Job job = new Job("Rebuild")
                {
                    protected IStatus run(IProgressMonitor monitor)
                    {
                        try
                        {
                            project.build(IncrementalProjectBuilder.FULL_BUILD, Slice2javaBuilder.BUILDER_ID, null,
                                    monitor);
                        }
                        catch(CoreException e)
                        {
                            return new Status(Status.ERROR, Activator.PLUGIN_ID, 0, "rebuild failed", e);
                        }
                        return Status.OK_STATUS;
                    }
                };
                job.setPriority(Job.BUILD);
                job.schedule(); // start as soon as possible
            }
        }
        catch(CoreException e)
        {
            return false;
        }
        catch(IOException e)
        {
            ErrorDialog.openError(getShell(), "Error", "Error saving preferences",
                    new Status(Status.ERROR, Activator.PLUGIN_ID, 0, null, e));
            return false;
        }
        return true;
    }
    
    /**
     * @see PreferencePage#createContents(Composite)
     */
    protected Control createContents(Composite parent)
    {
        // Composite composite = new Composite(parent, SWT.NONE);

        TabFolder tabFolder = new TabFolder(parent, SWT.NONE);
        {
            TabItem tabItem = new TabItem(tabFolder, SWT.NONE);
            tabItem.setText("Installation");
            Control source = createInstallation(tabFolder);
            tabItem.setControl(source);
        }
        {
            TabItem tabItem = new TabItem(tabFolder, SWT.NONE);
            tabItem.setText("Source");
            Control source = createSource(tabFolder);
            tabItem.setControl(source);
        }
        {
            TabItem tabItem = new TabItem(tabFolder, SWT.NONE);
            tabItem.setText("Options");
            Control source = createOptions(tabFolder);
            tabItem.setControl(source);
        }
        tabFolder.pack();

        loadPrefs();

        return tabFolder;
    }
    
    private void loadPrefs()
    {
        IProject project = (IProject)getElement();
        _config = new Configuration(project);

        _iceHome.setText(_config.getIceHome());
        _generatedDir.setText(_config.getGeneratedDir());
        for(Iterator<String> iter = _config.getSliceSourceDirs().iterator(); iter.hasNext();)
        {
            _sourceDirectories.add(iter.next());
        }
        for(Iterator<String> iter = _config.getIncludes().iterator(); iter.hasNext();)
        {
            _includes.add(iter.next());
        }
        _defines.setText(Configuration.fromList(_config.getDefines()));
        _meta.setText(Configuration.fromList(_config.getMeta()));
        _stream.setSelection(_config.getStream());
        _tie.setSelection(_config.getTie());
        _console.setSelection(_config.getConsole());

        checkValid();
    }

    private void checkValid()
    {
        if(!Configuration.verifyIceHome(_iceHome.getText()))
        {
            setErrorMessage("Cannot locate valid Ice installation");
            setValid(false);
            return;
        }
        IProject project = (IProject) getElement();
        IFolder folder = project.getFolder(_generatedDir.getText());
        if(!folder.exists())
        {
            setErrorMessage("Generated folder does not exist");
            setValid(false);
            return;
        }
        setValid(true);
        setErrorMessage(null);
    }
    
    private Control createInstallation(Composite parent)
    {
        Composite composite = new Composite(parent, SWT.NONE);
        
        GridLayout gridLayout = new GridLayout();
        gridLayout.numColumns = 1;
        composite.setLayout(gridLayout);
        
        Group iceHomeGroup = new Group(composite, SWT.NONE);
        iceHomeGroup.setText("Location of Ice installation");
        gridLayout = new GridLayout();
        gridLayout.numColumns = 1;
        iceHomeGroup.setLayout(gridLayout);
        iceHomeGroup.setLayoutData(new GridData(GridData.FILL_HORIZONTAL));

        Composite c = new Composite(iceHomeGroup, SWT.NONE);

        GridLayout gridLayout2 = new GridLayout();
        gridLayout2.numColumns = 2;
        gridLayout2.marginLeft = 0;
        gridLayout2.marginTop = 0;
        gridLayout2.marginBottom = 0;
        
        c.setLayout(gridLayout2);
        c.setLayoutData(new GridData(GridData.FILL_BOTH));

        _iceHome = new Text(c, SWT.BORDER | SWT.READ_ONLY);
        _iceHome.setLayoutData(new GridData(GridData.FILL_HORIZONTAL));

        Button but = new Button(c, SWT.PUSH);
        but.setText("Browse");
        but.addSelectionListener(new SelectionAdapter()
        {
            public void widgetSelected(SelectionEvent e)
            {
                DirectoryDialog chooser = new DirectoryDialog(getShell(), SWT.NONE);
                String dir = chooser.open();
                if(dir != null)
                {
                    _iceHome.setText(dir);
                    checkValid();
                }
            }
        });
        
        Group optionsGroup = new Group(composite, SWT.NONE);
        optionsGroup.setText("Builder Options");
        gridLayout = new GridLayout();
        gridLayout.numColumns = 2;
        optionsGroup.setLayout(gridLayout);
        optionsGroup.setLayoutData(new GridData(GridData.FILL_HORIZONTAL));

        _console = new Button(optionsGroup, SWT.CHECK);
        new Label(optionsGroup, SWT.NONE).setText("Enable console");
        
        return composite;
    }

    private Control createSource(Composite parent)
    {
        Composite composite = new Composite(parent, SWT.NONE);

        GridLayout gridLayout = new GridLayout();
        gridLayout.numColumns = 1;
        composite.setLayout(gridLayout);

        Group sourceGroup = new Group(composite, SWT.NONE);
        sourceGroup.setText("Location of Slice Source Files");
        gridLayout = new GridLayout();
        gridLayout.numColumns = 1;
        sourceGroup.setLayout(gridLayout);
        sourceGroup.setLayoutData(new GridData(GridData.FILL_BOTH));
        
        Composite c1 = new Composite(sourceGroup, SWT.NONE);

        gridLayout = new GridLayout();
        gridLayout.numColumns = 2;
        c1.setLayout(gridLayout);
        c1.setLayoutData(new GridData(GridData.FILL_BOTH));

        _sourceDirectories = new List(c1, SWT.H_SCROLL | SWT.V_SCROLL | SWT.MULTI | SWT.BORDER);
        _sourceDirectories.setLayoutData(new GridData(GridData.FILL_BOTH));

        Composite c2 = new Composite(c1, SWT.NONE);

        gridLayout = new GridLayout();
        gridLayout.numColumns = 1;
        c2.setLayout(gridLayout);

        Button but1 = new Button(c2, SWT.PUSH);
        but1.setLayoutData(new GridData(GridData.FILL_HORIZONTAL));
        but1.setText("Add Folder");
        but1.addSelectionListener(new SelectionAdapter()
        {
            public void widgetSelected(SelectionEvent e)
            {
                IProject project = (IProject) getElement();

                SourceSelectionDialog dialog = new SourceSelectionDialog(getShell(), project, "Select Source Location");
                String[] items = _sourceDirectories.getItems();
                IFolder[] resources = new IFolder[items.length];
                for(int i = 0; i < items.length; ++i)
                {
                    resources[i] = project.getFolder(items[i]);
                }
                dialog.setInitialSelections(resources);
                if(dialog.open() == ContainerSelectionDialog.OK)
                {
                    Object[] selection = dialog.getResult();
                    for(int i = 0; i < selection.length; ++i)
                    {
                        IFolder path = (IFolder) selection[i];
                        _sourceDirectories.add(path.getProjectRelativePath().toString());
                    }
                }
            }
        });
        
        Button but2 = new Button(c2, SWT.PUSH);
        but2.setLayoutData(new GridData(GridData.FILL_HORIZONTAL));
        but2.setText("Remove");
        but2.addSelectionListener(new SelectionAdapter()
        {
            public void widgetSelected(SelectionEvent e)
            {
                _sourceDirectories.remove(_sourceDirectories.getSelectionIndices());
            }
        });

        Group gclGroup = new Group(composite, SWT.NONE);
        gclGroup.setText("Generated Code Location");
        gridLayout = new GridLayout();
        gridLayout.numColumns = 1;
        gclGroup.setLayout(gridLayout);
        gclGroup.setLayoutData(new GridData(GridData.FILL_HORIZONTAL));
        
        Composite tc = new Composite(gclGroup, SWT.NONE);
        gridLayout = new GridLayout();
        gridLayout.numColumns = 1;
        tc.setLayout(gridLayout);
        tc.setLayoutData(new GridData(GridData.FILL_HORIZONTAL));
        
        Label l = new Label(tc, SWT.WRAP);
        l.setText("This subdirectory is used by the plugin to manage the source files generated from your Slice definitions. It should not be used for any other purpose.");
        GridData gridData = new GridData(GridData.FILL_BOTH);
        gridData.widthHint = 400;
        l.setLayoutData(gridData);
        
        Composite c = new Composite(tc, SWT.NONE);

        GridLayout gridLayout2 = new GridLayout();
        gridLayout2.numColumns = 2;
        gridLayout2.marginLeft = 0;
        gridLayout2.marginTop = 0;
        gridLayout2.marginBottom = 0;
        c.setLayout(gridLayout2);

        c.setLayoutData(new GridData(GridData.FILL_BOTH));

        _generatedDir = new Text(c, SWT.BORDER | SWT.READ_ONLY);
        gridData = new GridData(GridData.FILL_HORIZONTAL);
        // gridData.horizontalSpan = 2;
        _generatedDir.setLayoutData(gridData);

        Button but3 = new Button(c, SWT.PUSH);
        but3.setText("Browse");
        but3.addSelectionListener(new SelectionAdapter()
        {
            public void widgetSelected(SelectionEvent e)
            {
                IProject project = (IProject) getElement();

                SourceSelectionDialog dialog = new SourceSelectionDialog(getShell(), project,
                        "Select Generated Code Location");
                dialog.setMultiple(false);
                if(dialog.open() == ContainerSelectionDialog.OK)
                {
                    Object[] selection = dialog.getResult();
                    if(selection.length == 1)
                    {
                        IFolder path = (IFolder) selection[0];
                        _generatedDir.setText(path.getProjectRelativePath().toString());
                    }
                }
            }
        });

        return composite;
    }

    private Control createIncludes(Composite parent)
    {
        Composite composite = new Composite(parent, SWT.NONE);

        GridLayout gridLayout = new GridLayout();
        gridLayout.numColumns = 2;
        composite.setLayout(gridLayout);
        composite.setLayoutData(new GridData(GridData.FILL_BOTH));

        _includes = new List(composite, SWT.H_SCROLL | SWT.V_SCROLL | SWT.MULTI | SWT.BORDER);
        _includes.setLayoutData(new GridData(GridData.FILL_BOTH));

        Composite c2 = new Composite(composite, SWT.NONE);

        gridLayout = new GridLayout();
        gridLayout.numColumns = 1;
        c2.setLayout(gridLayout);

        Button but1 = new Button(c2, SWT.PUSH);
        but1.setLayoutData(new GridData(GridData.FILL_HORIZONTAL));
        but1.setText("Add");
        but1.addSelectionListener(new SelectionAdapter()
        {
            public void widgetSelected(SelectionEvent e)
            {
                IProject project = (IProject) getElement();
                DirectoryDialog dialog = new DirectoryDialog(getShell());
                String dir = dialog.open();
                if(dir != null)
                {
                    String projectDir = project.getLocation().toOSString();

                    if(dir.startsWith(projectDir)) 
                    {
                        dir = dir.substring(projectDir.length()+1, dir.length());
                    }
                    if(dir.length() > 0)
                    {
                        _includes.add(dir);
                    }
                }
            }
        });
        Button but2 = new Button(c2, SWT.PUSH);
        but2.setLayoutData(new GridData(GridData.FILL_HORIZONTAL));
        but2.setText("Remove");
        but2.addSelectionListener(new SelectionAdapter()
        {
            public void widgetSelected(SelectionEvent e)
            {
                _includes.remove(_includes.getSelectionIndices());
            }
        });
        Button but3 = new Button(c2, SWT.PUSH);
        but3.setLayoutData(new GridData(GridData.FILL_HORIZONTAL));
        but3.setText("Up");
        but3.addSelectionListener(new SelectionAdapter()
        {
            public void widgetSelected(SelectionEvent e)
            {
                int index = _includes.getSelectionIndex();
                if(index > 0)
                {
                    String[] items = _includes.getItems();
                    String tmp = items[index-1];
                    items[index-1] = items[index];
                    items[index] = tmp;
                    _includes.setItems(items);
                    _includes.setSelection(index-1);
                }
            }
        });
        Button but4 = new Button(c2, SWT.PUSH);
        but4.setLayoutData(new GridData(GridData.FILL_HORIZONTAL));
        but4.setText("Down");
        but4.addSelectionListener(new SelectionAdapter()
        {
            public void widgetSelected(SelectionEvent e)
            {
                int index = _includes.getSelectionIndex();
                if(index != -1)
                {
                    String[] items = _includes.getItems();
                    if(index != items.length-1)
                    {
                        String tmp = items[index+1];
                        items[index+1] = items[index];
                        items[index] = tmp;
                        _includes.setItems(items);
                        _includes.setSelection(index+1);
                    }
                }   
            }
        });
     
        return composite;
    }
    
    private String
    semiFilter(String text)
    {
        java.util.List<String> l = Arrays.asList(text.split(";"));
        StringBuffer sb = new StringBuffer();
        for(Iterator<String> p = l.iterator(); p.hasNext();)
        {
            String n = p.next().trim();
            if(n.length() > 0)
            {
                if(sb.length() != 0)
                {
                    sb.append(';');
                }
                sb.append(n);
            }
        }
        return sb.toString();
    }

    private Control createDefines(Composite parent)
    {
        Composite composite = new Composite(parent, SWT.NONE);

        GridLayout gridLayout = new GridLayout();
        gridLayout.numColumns = 1;
        gridLayout.marginLeft = 0;
        gridLayout.marginTop = 0;
        gridLayout.marginBottom = 0;
        composite.setLayout(gridLayout);
        composite.setLayoutData(new GridData(GridData.FILL_BOTH));
        
        Label l = new Label(composite, SWT.WRAP);
        l.setText("Enter macros (';' separated). For example, enter FOO;BAR to define -DFOO -DBAR.");
        GridData gridData = new GridData(GridData.FILL_BOTH);
        gridData.widthHint = 400;
        l.setLayoutData(gridData);
         
        _defines = new Text(composite, SWT.BORDER);
        _defines.setLayoutData(new GridData(GridData.FILL_HORIZONTAL));
        _defines.addFocusListener(new FocusListener()
        {
            public void focusGained(FocusEvent e)
            {   
            }

            public void focusLost(FocusEvent e)
            {
                Text t = (Text)e.widget;
                String f = t.getText();
                String filtered = semiFilter(f);
                if(!f.equals(filtered))
                {
                    t.setText(filtered);
                }
            }
        
        });
        
        return composite;
    }

    private Control createMeta(Composite parent)
    {
        Composite composite = new Composite(parent, SWT.NONE);

        GridLayout gridLayout = new GridLayout();
        gridLayout.numColumns = 1;
        gridLayout.marginLeft = 0;
        gridLayout.marginTop = 0;
        gridLayout.marginBottom = 0;
        composite.setLayout(gridLayout);
        composite.setLayoutData(new GridData(GridData.FILL_BOTH));
        
        Label l = new Label(composite, SWT.WRAP);
        l.setText("Enter metadata (';' separated). For example, enter java2 to define --meta=java2.");
        GridData gridData = new GridData(GridData.FILL_BOTH);
        gridData.widthHint = 400;
        l.setLayoutData(gridData);
        
        _meta = new Text(composite, SWT.BORDER);
        _meta.setLayoutData(new GridData(GridData.FILL_HORIZONTAL));
        _meta.addFocusListener(new FocusListener()
        {
            public void focusGained(FocusEvent e)
            {   
            }

            public void focusLost(FocusEvent e)
            {
                Text t = (Text)e.widget;
                String f = t.getText();
                String filtered = semiFilter(f);
                if(!f.equals(filtered))
                {
                    t.setText(filtered);
                }
            }
        });
        
        return composite;
    }

    private Control createOptions(Composite parent)
    {
        Composite composite = new Composite(parent, SWT.NONE);
        
        GridLayout gridLayout = new GridLayout();
        gridLayout.numColumns = 1;
        composite.setLayout(gridLayout);

        Group includesGroup = new Group(composite, SWT.NONE);
        includesGroup.setText("Location of Include Files");
        gridLayout = new GridLayout();
        gridLayout.numColumns = 1;
        includesGroup.setLayout(gridLayout);
        includesGroup.setLayoutData(new GridData(GridData.FILL_BOTH));
        
        createIncludes(includesGroup);

        Group definesGroup = new Group(composite, SWT.NONE);
        definesGroup.setText("Preprocessor Definitions");
        gridLayout = new GridLayout();
        gridLayout.numColumns = 1;
        definesGroup.setLayout(gridLayout);
        definesGroup.setLayoutData(new GridData(GridData.FILL_BOTH));

        createDefines(definesGroup);

        Group metaGroup = new Group(composite, SWT.NONE);
        metaGroup.setText("Metadata Definitions");
        gridLayout = new GridLayout();
        gridLayout.numColumns = 1;
        metaGroup.setLayout(gridLayout);
        metaGroup.setLayoutData(new GridData(GridData.FILL_BOTH));

        createMeta(metaGroup);

        Group optionsGroup = new Group(composite, SWT.NONE);

        gridLayout = new GridLayout();
        gridLayout.numColumns = 4;
        optionsGroup.setText("Options");
        optionsGroup.setLayout(gridLayout);
        optionsGroup.setLayoutData(new GridData(GridData.FILL_BOTH));
        
        _stream = new Button(optionsGroup, SWT.CHECK);
        new Label(optionsGroup, SWT.NONE).setText("Enable streaming");
        _tie = new Button(optionsGroup, SWT.CHECK);
        new Label(optionsGroup, SWT.NONE).setText("Enable tie");

        return composite;
    }


    
    private Configuration _config;
    private Text _iceHome;
    private Button _console;
    private Text _generatedDir;
    private List _sourceDirectories;
    private List _includes;
    private Text _defines;
    private Button _stream;
    private Button _tie;
    private Text _meta;   
}
