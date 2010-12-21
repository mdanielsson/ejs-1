/*
 * Copyright (c) All Rights Reserved. See details at the end of the file.
 */
package com.embedthis.ejs.ide.properties;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IProjectDescription;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.SWT;

import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.dialogs.PropertyPage;

import com.embedthis.ejs.ide.builder.EJScriptNature;
import com.embedthis.ejs.ide.core.EJScriptTrace;

public class EJScriptProjectPropertyPage extends PropertyPage {

	private Button addNatureButton;
	private Button removeNatureButton;
	
	private static final String DESCRIPTION = 
		"Global options are applied to each EJScript file as it is compiled.";
	
	/**
	 * Constructor for SamplePropertyPage.
	 */
	public EJScriptProjectPropertyPage() {
		super();
		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.GENERAL_TRACE,
				"EJScriptProjectPropertyPage init");
	}

	/**
	 * Creates the Properties Page for the EJScript projects. 
	 * @param parent
	 */
	private void addFirstSection(Composite parent) {
		Composite composite = createDefaultComposite(parent);
		
		Label description = new Label(composite, SWT.NONE);
		description.setText(DESCRIPTION);
		
		//
		addNatureButton = new Button(composite, SWT.RADIO);
		addNatureButton.setText("Add EJScript Nature to project");
		GridData gd = new GridData();
		gd.grabExcessHorizontalSpace = true;
		gd.horizontalSpan = 3;
		addNatureButton.setLayoutData(gd);
		
		
		removeNatureButton = new Button(composite, SWT.RADIO);
		removeNatureButton.setText("Remove EJScript Nature from project");
		gd = new GridData();
		gd.grabExcessHorizontalSpace = true;
		gd.horizontalSpan = 3;
		removeNatureButton.setLayoutData(gd);
		
		if (doesCurrentProjectHaveNature() == true) {
			addNatureButton.setSelection(true);
		} else {
			removeNatureButton.setSelection(true);
		}
	}
	
	private boolean doesCurrentProjectHaveNature() {
		IResource resource = (IResource)getElement().getAdapter(IProject.class);
		if (resource instanceof IProject) {
			IProject project = (IProject)resource;
			if (project.isOpen()) {
				// Get the description.
				IProjectDescription description;
				try {
					description = project.getDescription();
				}
				catch (CoreException e) {
					return false;
				}
				
				// Toggle the nature.
				List<String> newIds = new ArrayList<String>();
				newIds.addAll(Arrays.asList(description.getNatureIds()));
				
				int index = newIds.indexOf(EJScriptNature.NATURE_ID);
				if (index == -1) {
					return false;
				} else {
					return true;
				}
			}
		} else {
			EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.GENERAL_TRACE,
				"current element is not a project");
		}
		return false;
	}
	
	/**
	 * Prompt the user that they have selected to add/remove the EJScript Nature and 
	 * confirm that they want to do this.
	 * @param isAddingNature true if selected action is 
	 * 			to add the nature, false otherwise 
	 * @return true if action can continue, false otherwise
	 */
	private boolean verify(boolean isAddingNature) {
		Shell shell = getShell();
		String message;
		boolean result;
		
		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.GENERAL_TRACE,
				"+ToggleProjectNature:verify");
		
		if (isAddingNature) {
			message = "This will add the EJScript Nature, do you want to continue?";
		} else {
			message = "This will remove the EJScript Nature, do you want to continue?";
		}
		result = MessageDialog.openConfirm(shell, "Confirm",
					message);
		
		return result;
	}
	
	
	private void addSeparator(Composite parent) {
		Label separator = new Label(parent, SWT.SEPARATOR | SWT.HORIZONTAL);
		GridData gridData = new GridData();
		gridData.horizontalAlignment = GridData.FILL;
		gridData.grabExcessHorizontalSpace = false;
		separator.setLayoutData(gridData);
	}

	/**
	 * @see PreferencePage#createContents(Composite)
	 */
	protected Control createContents(Composite parent) {
		Composite composite = new Composite(parent, SWT.NONE);
		// TODO GMK add tabbed pages here. 1) build configurations - 
		
//		TableLayout table = new TableLayout();
		GridLayout layout = new GridLayout();
		composite.setLayout(layout);
		GridData data = new GridData(GridData.FILL);
		data.grabExcessHorizontalSpace = true;
		composite.setLayoutData(data);

		addFirstSection(composite);
		addSeparator(composite);

		return composite;
	}

	private Composite createDefaultComposite(Composite parent) {
		Composite composite = new Composite(parent, SWT.NULL);
		GridLayout layout = new GridLayout();
		layout.numColumns = 3;
		composite.setLayout(layout);

		GridData data = new GridData();
		data.verticalAlignment = GridData.FILL;
		data.horizontalAlignment = GridData.FILL;
		composite.setLayoutData(data);

		return composite;
	}

	protected void performDefaults() {
		// Populate the owner text field with the default value
		
	}
	
	public boolean performOk() {
		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.GENERAL_TRACE,
				"+performOk");
		
		boolean isAddingNature;
		IResource resource = (IResource)getElement().getAdapter(IProject.class);
		if (!(resource instanceof IProject)) {
			return true;
		}
		IProject project = (IProject)resource;
		// Cannot modify closed projects.
		if (project.isOpen()) {
			// Get the description.
			IProjectDescription description;
			try {
				description = project.getDescription();
			}
			catch (CoreException e) {
				return true;
			}
			
			// Toggle the nature.
			List<String> newIds = new ArrayList<String>();
			newIds.addAll(Arrays.asList(description.getNatureIds()));
			
			int index = newIds.indexOf(EJScriptNature.NATURE_ID);
			if (index == -1) {
				newIds.add(EJScriptNature.NATURE_ID);
				isAddingNature = true;
			} else {
				newIds.remove(index);
				isAddingNature = false;
			}
				
			if (verify(isAddingNature)) {
				description.setNatureIds((String[]) 
						newIds.toArray(new String[newIds.size()]));
				
				// Save the description.
				try {
					project.setDescription(description, null);
				}
				catch (CoreException e) {
					EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
							EJScriptTrace.GENERAL_TRACE,
							e.getMessage());
				}
			}
		}

		return true;
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
    vim: sw=8 ts=8 expandtab

    @end
 */
