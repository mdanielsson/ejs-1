/*
 * Copyright (c) All Rights Reserved. See details at the end of the file.
 */
package com.embedthis.ejs.ide.editors;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Iterator;
import java.util.List;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.jface.viewers.CheckStateChangedEvent;
import org.eclipse.jface.viewers.ICheckStateListener;
import org.eclipse.jface.window.Window;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Tree;
import org.eclipse.ui.dialogs.ContainerCheckedTreeViewer;
import org.eclipse.ui.forms.IManagedForm;
import org.eclipse.ui.forms.editor.FormEditor;
import org.eclipse.ui.forms.editor.FormPage;
import org.eclipse.ui.forms.events.ExpansionAdapter;
import org.eclipse.ui.forms.events.ExpansionEvent;
import org.eclipse.ui.forms.widgets.FormToolkit;
import org.eclipse.ui.forms.widgets.ScrolledForm;
import org.eclipse.ui.forms.widgets.Section;

import com.embedthis.ejs.ide.builder.EJScriptBuilder;
import com.embedthis.ejs.ide.core.EJSBuildConfiguration;
import com.embedthis.ejs.ide.core.EJScriptTrace;
import com.embedthis.ejs.ide.dialogs.CompilerOptionsDialog;

public class EJSBuildConfigPage extends FormPage {
	
	private FormToolkit toolkit;
	private ScrolledForm form;
	private List<IResource> resources;
	private IProject project = null;
	private int num;
	private BCEditor editor;
	private ContainerCheckedTreeViewer treeViewer;
	private EJSBuildConfiguration buildConfig;
	private Label compilerOptString;
	private GridData labelData = null;
	private Button buildTypeButton;
	
	private static final String EDIT_COMPILER_OPTIONS_TEXT = "Edit Compiler Options";
	
	/*
	 * Construct a new BC page.
	 */
	public EJSBuildConfigPage(FormEditor editor, EJSBuildConfiguration buildConfig,
			IProject project, int numPages) {
		super(editor, buildConfig.getName(), buildConfig.getName());
		EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
				EJScriptTrace.GENERAL_TRACE,
				"+EJSBuildConfigPage:");
		this.editor = (BCEditor)editor;
		this.project = project;
		this.num = numPages;
		this.buildConfig = buildConfig;
		resources = buildConfig.getResourceList();
		EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
				EJScriptTrace.GENERAL_TRACE,
				"-EJSBuildConfigPage:");
	}
	
	/**
	 * Create the content for the page.
	 */
	public void createFormContent(IManagedForm managedForm) {
		EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
				EJScriptTrace.GENERAL_TRACE,
				"+createFormContent");
		toolkit = managedForm.getToolkit();
		form = managedForm.getForm();
		
		form.setText("Build Configuration in " + editor.getBCName());
		GridLayout layout = new GridLayout();
		layout.numColumns = 1;
		form.getBody().setLayout(layout);
		GridData gd = new GridData(GridData.FILL);
		gd.heightHint = 20;
		gd.widthHint = 100;
		EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
				EJScriptTrace.GENERAL_TRACE,
				"+createFormContent:createTreeViewer");
		
		/* CREATE TREE VIEWER */
		Section section = toolkit.createSection(form.getBody(),
				Section.DESCRIPTION | Section.TWISTIE | Section.EXPANDED |
				Section.TITLE_BAR);
		gd = new GridData(GridData.FILL);
		gd.widthHint = 500;
		section.setLayoutData(gd);
		section.addExpansionListener(new ExpansionAdapter() {
			public void expansionStateChanged(ExpansionEvent e) {
				form.reflow(true);
			}
		});
		section.setText("Resources");
		section.setDescription("Select those resources to be members of this" +
				" build configuration.");
		EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
				EJScriptTrace.GENERAL_TRACE,
				"+createFormContent:createSectionclient");
		Composite sectionClient = toolkit.createComposite(section);
		createResourceTreeSection(sectionClient);
        section.setClient(sectionClient);
        EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
				EJScriptTrace.GENERAL_TRACE,
				"+createFormContent:createSectionclientA");
        /* CREATE COMPILER OPTIONS SECTION */
        
        Section optSection = toolkit.createSection(form.getBody(),
        		Section.DESCRIPTION | Section.TWISTIE | Section.EXPANDED |
        		Section.TITLE_BAR);
        EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
				EJScriptTrace.GENERAL_TRACE,
				"+createFormContent:createSectionclientB");
        gd = new GridData(GridData.FILL);
		optSection.setLayoutData(gd);
		gd.widthHint = 500;
		optSection.addExpansionListener(new ExpansionAdapter() {
			public void expansionStateChanged(ExpansionEvent e) {
				form.reflow(true);
			}
		});
		EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
				EJScriptTrace.GENERAL_TRACE,
				"+createFormContent:createSectionclientC");
		optSection.setText("Compiler Options");
        optSection.setDescription("Compiler options for this build configuration.  " +
        		"To change these options, click the '" + EDIT_COMPILER_OPTIONS_TEXT + 
        		"' button below.");
		Composite optSectionClient = toolkit.createComposite(optSection);
		EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
				EJScriptTrace.GENERAL_TRACE,
				"+createFormContent:createSectionclientD");
		createCompilerSettingsSection(optSectionClient, num);
		EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
				EJScriptTrace.GENERAL_TRACE,
				"+createFormContent:createSectionclientE");

        optSection.setClient(optSectionClient);
        EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
				EJScriptTrace.GENERAL_TRACE,
				"+createFormContent:createSectionclientF");
        
        /* Create section for build type */
		Section typeSection = toolkit.createSection(form.getBody(),
				Section.DESCRIPTION | Section.TWISTIE | Section.EXPANDED |
				Section.TITLE_BAR);
		gd = new GridData(GridData.FILL);
		gd.widthHint = 500;
		typeSection.setLayoutData(gd);
		typeSection.addExpansionListener(new ExpansionAdapter() {
			public void expansionStateChanged(ExpansionEvent e) {
				form.reflow(true);
			}
		});
		typeSection.setText("Enable File-By-File Compile");
		typeSection.setDescription("A File-By-File Compile will build each file " +
				"selected in the project individually. The selected compiler options will " +
				"be applied to each file.");
		Composite typeComposite = toolkit.createComposite(typeSection);
		createBuildTypeSection(typeComposite);
		typeSection.setClient(typeComposite);
 	}
	
	/**
	 * Create the section for enabling the File-by-File compile.
	 * @param typeComposite
	 */
	private void createBuildTypeSection(Composite typeComposite) {
		GridLayout layout = new GridLayout();
		layout.numColumns = 2;
		typeComposite.setLayout(layout);
		GridData gd = new GridData(GridData.FILL);
		gd.heightHint = 500;
		typeComposite.setData(gd);
		buildTypeButton = toolkit.createButton(typeComposite, 
				"Enable File-By-File Compile", SWT.CHECK);
		if (buildConfig.isFileByFileEnabled() == true) {
			buildTypeButton.setSelection(true);
		}
		buildTypeButton.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				setIsDirty(true);
			}
		});
	}
	
	/**
	 * Create the section for enabling 
	 * @param sectionClient
	 */
	private void createResourceTreeSection(Composite sectionClient) {
		GridLayout layout = new GridLayout();
		layout.numColumns = 1;
		sectionClient.setLayout(layout);
		GridData gd = new GridData(GridData.FILL);
		gd.heightHint = 500;
		sectionClient.setData(gd);
		toolkit.createLabel(sectionClient, 
				"Select the files to include in this Build Configuration");

		Tree tree = toolkit.createTree(sectionClient, SWT.CHECK);
		treeViewer = new ContainerCheckedTreeViewer(tree);
        treeViewer.setContentProvider(new FileContentProvider());
        treeViewer.setLabelProvider(new FileLabelProvider());
        treeViewer.addFilter(new EJScriptFileFilter(project));

        treeViewer.setInput(project.getParent());
//        treeViewer.setFilters()
        
        if (resources != null) {
        	/*
        	 * Set elements that were already selected.
        	 */
        	EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
    			EJScriptTrace.GENERAL_TRACE,
    			"setting " + resources.size() + 
    			" resources as checked");
        	treeViewer.setCheckedElements(resources.toArray());
        } else {
        	EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
    				EJScriptTrace.GENERAL_TRACE,
    				"resources is null");
        }
        treeViewer.addCheckStateListener(new ICheckStateListener() {
        	public void checkStateChanged(CheckStateChangedEvent event) {
        		setIsDirty(true);
		       	}
        	});
        
        gd = new GridData(GridData.FILL);
        gd.heightHint = 180;
        gd.widthHint = 500;
        Tree treeWidget = treeViewer.getTree();
        treeWidget.setLayoutData(gd);

	}

	
	/**
	 * Create the compiler settings section of the editor page.
	 * @param parent parent Composite created in createBuildConfigurationsPage
	 * @param index -
	 */
	private void createCompilerSettingsSection(Composite composite, int index){
		GridLayout layout = new GridLayout();
		layout.numColumns = 1;
		composite.setLayout(layout);
		GridData data = new GridData(GridData.FILL);
		composite.setLayoutData(data);

		labelData = new GridData(GridData.FILL);
		labelData.widthHint = 500;
		compilerOptString = toolkit.createLabel(composite, 
				buildConfig.getCompilerOptions().convertToString(),
				SWT.BORDER | SWT.WRAP);
		compilerOptString.setLayoutData(labelData);
		
		Button editCompilerOptions = toolkit.createButton(composite,
				EDIT_COMPILER_OPTIONS_TEXT, SWT.NONE);

		editCompilerOptions.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				CompilerOptionsDialog dialog = new CompilerOptionsDialog(
						Display.getCurrent().getActiveShell(), 
						buildConfig.getCompilerOptions());
				if (dialog.open() == Window.OK) {
					EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
							EJScriptTrace.GENERAL_TRACE,
							"+dialogOpen: " + dialog.getCompilerOptions().convertToString());
					
					/*
					 * TODO GMK: need to figure out how to resize this in cases where 
					 * the compiler options are long. 
					 */
					compilerOptString.setText(dialog.getCompilerOptions().convertToString());
					compilerOptString.setLayoutData(labelData);
					EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
							EJScriptTrace.GENERAL_TRACE,
							"setting text to " + dialog.getCompilerOptions().convertToString());
					setIsDirty(true);
				}
			}
		});
		
		EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
				EJScriptTrace.GENERAL_TRACE,
				"Adding compilerOptString to " + index);
	}
	protected void setIsDirty(boolean isDirty) {
		EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
				EJScriptTrace.EDITOR_OUTPUT_TRACE,
				"setIsDirty(" + isDirty + ")");
		editor.setIsDirty(isDirty);
	}
	
	/**
	 * 
	 */
	public void doSave(IProgressMonitor monitor) {
		EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
				EJScriptTrace.GENERAL_TRACE,
				"+doSave1");
		try {
			saveResourceList(monitor);
		} catch (NullPointerException e) {
			EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
					EJScriptTrace.GENERAL_TRACE,
					"saveResourceList returned "+ e.getMessage());
			
			StackTraceElement[] elements = e.getStackTrace();
			for (int i = 0; i < elements.length; i++) {
				EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
						EJScriptTrace.GENERAL_TRACE,
						elements[i].toString());
			}
		}
		EJScriptBuilder.updateBuildConfigurationInGlobalList(buildConfig);
		EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
				EJScriptTrace.GENERAL_TRACE,
				"-doSave:saveCompilerOptions next");
		saveCompilerOptions(monitor);
		EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
				EJScriptTrace.GENERAL_TRACE,
				"-doSave");
		saveBuildType(monitor);
	}
	
	/**
	 * Save the selected resource list.
	 * @param bc - Build configuration being saved.
	 * @param index - tab number of the current editor
	 * @param monitor
	 */
	private void saveResourceList(IProgressMonitor monitor) {
		EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
				EJScriptTrace.GENERAL_TRACE,
				"+saveResourceList");
		boolean found;
		if (treeViewer == null) {
			/**
			 * If treeViewer is null, then this page was never touched 
			 * (and not edited) continue.
			 */
			EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
					EJScriptTrace.EDITOR_OUTPUT_TRACE,
					"treeViewer is null");
			return;
		}
		Object[] checked = treeViewer.getCheckedElements();
		Object[] grayed = treeViewer.getGrayedElements();
		
		List<Object> grayedList = Arrays.asList(grayed);
		EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
				EJScriptTrace.GENERAL_TRACE,
				"checked is " + checked.length + " and grayed is " + grayed.length);
//		IResource[] resources = new IResource[checked.length - grayed.length];
		ArrayList<IResource> resources = new ArrayList<IResource>();
		List<IResource> oldResourceList = buildConfig.getResourceList();
		int j = 0;
		for (int i = 0; i < checked.length; i++) {
			/*
			 * resources array will represent only those elements checked
			 * and not those elements which are gray-checked.
			 * The grayedList represents those elements that were not
			 * explicitly selected, but which were gray-selected (i.e.
			 * a folder who has at least one, but not all members selected).
			 */
		
			if (grayedList != null) {
				/*
				 * Some things are not selected, but have partial selection.
				 */
				if (!grayedList.contains(checked[i]) && checked[i] instanceof IFile && checked[i] != null) {
					resources.add((IResource)checked[i]);
					EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
							EJScriptTrace.GENERAL_TRACE,
							"Adding: " + resources.get(j).getName());
					
					/*
					 * if oldResourceList does not contain resources[j]
					 * then it is a new resource.
					 * if oldResourceList == null, found will remain false
					 * and the resource is added.
					 */
					found = false;
					if (oldResourceList != null) {
						Iterator<IResource> iter = oldResourceList.iterator();
						found = false;
						while (iter.hasNext()) {
							IResource resource = iter.next();
							if (resource != null && resource.equals(resources.get(j))) {
								found = true;
								break;
							}
						}
					} else {
						EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
								EJScriptTrace.GENERAL_TRACE,
								"Old Resource List is NULL");
					}
					
					if (found == false) {
						EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
								EJScriptTrace.GENERAL_TRACE,
								"Adding NEW RESOURCE: " + resources.get(j).getName());
						buildConfig.addNewResource(resources.get(j));
					}
					j++;
				}
			} else {
				/*
				 * greyed list is empty so everything in the selected list is good.
				 */
				found = false;
				if (oldResourceList != null) {
					Iterator<IResource> iter = oldResourceList.iterator();
					found = false;
					while (iter.hasNext()) {
						if (iter.next().equals(resources.get(j))) {
							found = true;
							break;
						}
					}
				} else {
					EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
							EJScriptTrace.GENERAL_TRACE,
							"Old Resource List is NULL 2");
				}
				if (found == false) {
					EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
						EJScriptTrace.GENERAL_TRACE,
						"Adding NEW RESOURCE2: " + resources.get(j).getName());
				
					buildConfig.addNewResource((IResource)checked[i]);
				}
			}
			EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
					EJScriptTrace.GENERAL_TRACE,
					"-saveResourceList1 i is " + i);
		}
		
		/*
		 * Now check through the old list of resources and see
		 * if any of them have been removed.  This populates the REMOVED
		 * resources list.
		 */
		if (oldResourceList == null) {
			EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
					EJScriptTrace.GENERAL_TRACE,
					"oldResourceList is null");
		} else {
			EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
				EJScriptTrace.GENERAL_TRACE,
				"oldResourceList: oldResourceList.size = " + oldResourceList.size());
			/*
			 * If oldResourceList[x] is not found in resources then
			 * it has been removed from this buildConfig.
			 */
			
			for (int i = 0; i < oldResourceList.size(); i++) {
				found = false;
				for (int g = 0; g < resources.size(); g++){
					EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
							EJScriptTrace.GENERAL_TRACE,
							"comparing " + resources.get(g) + " and " + 
							oldResourceList.get(i));
					if (resources.get(g).equals(oldResourceList.get(i))) {
						found = true;
						break;
					}
				}
				EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
						EJScriptTrace.GENERAL_TRACE,
						"found is " + found);
				if (found == false) {
					EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
							EJScriptTrace.GENERAL_TRACE,
							"Adding REMOVED RESOURCE: " + oldResourceList.get(i));
					buildConfig.addRemovedResource(oldResourceList.get(i));
				}
			}
		}
		
		buildConfig.setResourceList(resources);
		monitor.worked(1);
		EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
				EJScriptTrace.GENERAL_TRACE,
				"--saveResourceList");
	}
	
	/**
	 * Save the current editor tab's compiler settings.
	 * @param bc
	 * @param i
	 * @param monitor
	 */
	private void saveCompilerOptions(IProgressMonitor monitor) {
		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.EDITOR_OUTPUT_TRACE,
				"+saveCompilerOptions");
		/*
		 * If compilerOptString is null, that means this page was 
		 * not touched, don't do anything.
		 */
		if (compilerOptString != null) {
			buildConfig.setCommandArgs(compilerOptString.getText());
		}
		monitor.worked(1);
		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.EDITOR_OUTPUT_TRACE,
				"-saveCompilerOptions");
	}
	
	private void saveBuildType(IProgressMonitor monitor) {
		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.EDITOR_OUTPUT_TRACE,
				"+saveBuildType");

		if (compilerOptString != null) {
			if (buildConfig.isFileByFileEnabled() == buildTypeButton.getSelection()) {
				//no change
			} else {
				buildConfig.setBuildType(buildTypeButton.getSelection());
				buildConfig.setCommandArgs(compilerOptString.getText());
				buildConfig.setIsDirty(true);
			}
		}
		monitor.worked(1);
		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.EDITOR_OUTPUT_TRACE,
				"-saveBuildType");
	}
	
	public void setFocus() {
		super.setFocus();
	}
	
	public void dispose() {
		toolkit.dispose();
		super.dispose();
	}
}

/*
 *	@copy	default
 *	
 *	Copyright (c) Embedthis Software LLC, 2003-2011. All Rights Reserved.
 *	Copyright (c) Michael O'Brien, 1993-2011. All Rights Reserved.
 *	
 *	This software is distributed under commercial and open source licenses.
 *	You may use the GPL open source license described below or you may acquire 
 *	a commercial license from Embedthis Software. You agree to be fully bound 
 *	by the terms of either license. Consult the LICENSE.TXT distributed with 
 *	this software for full details.
 *	
 *	This software is open source; you can redistribute it and/or modify it 
 *	under the terms of the GNU General Public License as published by the 
 *	Free Software Foundation; either version 2 of the License, or (at your 
 *	option) any later version. See the GNU General Public License for more 
 *	details at: http://www.embedthis.com/downloads/gplLicense.html
 *	
 *	This program is distributed WITHOUT ANY WARRANTY; without even the 
 *	implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 *	
 *	This GPL license does NOT permit incorporating this software into 
 *	proprietary programs. If you are unable to comply with the GPL, you must
 *	acquire a commercial license to use this software. Commercial licenses 
 *	for this software and support services are available from Embedthis 
 *	Software at http://www.embedthis.com 
 *	
 *	Local variables:
 *	tab-width: 4
 *	c-basic-offset: 4
 *	End:
 *	vim: sw=4 ts=4 
 *
 *	Local variables:
    tab-width: 4
    c-basic-offset: 4
    End:
    vim: sw=4 ts=4 expandtab

    @end
 */
