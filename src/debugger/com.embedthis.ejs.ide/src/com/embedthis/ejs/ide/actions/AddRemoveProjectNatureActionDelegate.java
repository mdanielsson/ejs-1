/**
 * Copyright (c) All Rights Reserved. See details at the end of the file.
 */
package com.embedthis.ejs.ide.actions;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Iterator;
import java.util.List;

import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IProjectDescription;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.IWorkbenchWindowActionDelegate;
import org.eclipse.ui.actions.ActionDelegate;

import com.embedthis.ejs.ide.builder.EJScriptNature;
import com.embedthis.ejs.ide.core.EJScriptTrace;

/**
 * @author Gavin
 *
 */
public class AddRemoveProjectNatureActionDelegate extends ActionDelegate implements
		IWorkbenchWindowActionDelegate {
	
	private IWorkbenchWindow window;

	private IProject project = null;
	
	private static String REMOVE_ACTION_ID = "com.embedthis.ejs.ide.removeNature";
	private static String APPLY_ACTION_ID = "com.embedthis.ejs.ide.applyNature";
	
	/* (non-Javadoc)
	 * @see org.eclipse.ui.IWorkbenchWindowActionDelegate#init(org.eclipse.ui.IWorkbenchWindow)
	 */
	public void init(IWorkbenchWindow window) {
		this.window = window;
	}
	
	public void selectionChanged(IAction action, ISelection selection) {
		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.GENERAL_TRACE,
				"+selectionChanged");
		setSelectionIfProject(selection);
		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.GENERAL_TRACE,
				"getId returned: " + action.getId());
		if (action.getId().equals(APPLY_ACTION_ID)) {
			if (doesCurrentProjectHaveNature()) {
				action.setEnabled(false);
			} else {
				action.setEnabled(true);
			}
		} else {
			if (!doesCurrentProjectHaveNature()) {
				action.setEnabled(false);
			} else {
				action.setEnabled(true);
			}
		}
	}

	private void setSelectionIfProject(ISelection selection) {

		if (!(selection instanceof IStructuredSelection)) {
			EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
					EJScriptTrace.GENERAL_TRACE,
					"! IStructuredSelection");
			return;
		}
		
		for (
			Iterator iter = ((IStructuredSelection) selection).iterator();
			iter.hasNext();
		) {
			Object elem = iter.next();
			if (!(elem instanceof IResource)) {
				if (!(elem instanceof IAdaptable)) {
					EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
							EJScriptTrace.GENERAL_TRACE,
							"+elem ! IAdaptable");
					continue;
				}
				elem = ((IAdaptable) elem).getAdapter(IResource.class);
				if (!(elem instanceof IResource)) {
					EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
							EJScriptTrace.GENERAL_TRACE,
							"elem ! IResource");
					continue;
				}
			}
			if (!(elem instanceof IProject)) {
				elem = ((IResource) elem).getProject();
				if (!(elem instanceof IProject)) {
					continue;
				}
			}
			project = (IProject)elem;
			break;
		}
	}
	
	private boolean doesCurrentProjectHaveNature() {
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
		return false;
	}

	public void run(IAction action) {
		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.GENERAL_TRACE,
				"+ToggleProjectNature:run");
		
		boolean isAddingNature;
	
		// Cannot modify closed projects.
		if (project.isOpen()) {
			// Get the description.
			IProjectDescription description;
			try {
				description = project.getDescription();
			}
			catch (CoreException e) {
				return;
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
	}
	
	/**
	 * Prompt the user that they have selected to add/remove the EJScript Nature and 
	 * confirm that they want to do this.
	 * @param isAddingNature true if selected action is 
	 * 			to add the nature, false otherwise 
	 * @return true if action can continue, false otherwise
	 */
	private boolean verify(boolean isAddingNature) {
/*		Shell shell = window.getShell();
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
 */

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
