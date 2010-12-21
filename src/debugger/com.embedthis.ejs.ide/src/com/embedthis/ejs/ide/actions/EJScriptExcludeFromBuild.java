/*
 * Copyright (c) All Rights Reserved. See details at the end of the file.
 */
package com.embedthis.ejs.ide.actions;


import java.util.ArrayList;
import java.util.Arrays;
import java.util.Comparator;
import java.util.Iterator;
import java.util.List;
import java.util.Set;
import java.util.TreeSet;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IFolder;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IProjectDescription;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.core.runtime.QualifiedName;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.IWorkbenchWindowActionDelegate;
import org.eclipse.ui.actions.ActionDelegate;

import com.embedthis.ejs.ide.EJSConstants;
import com.embedthis.ejs.ide.EJScriptPlugin;
import com.embedthis.ejs.ide.builder.EJScriptBuilder;
import com.embedthis.ejs.ide.builder.EJScriptNature;
import com.embedthis.ejs.ide.core.EJScriptTrace;
import com.embedthis.ejs.ide.properties.PropertyConstants;

/**
 * 
 * @author Gavin
 *
 */
public class EJScriptExcludeFromBuild extends ActionDelegate implements
		IWorkbenchWindowActionDelegate {

	/**
	 * this string allows us to know which action was pressed.
	 */
	private static String EXCLUDE_FROM_BUILD = "com.embedthis.ejs.ide.excludeFromBuild";
	private static String ADD_TO_BUILD = "com.embedthis.ejs.ide.addToBuild";
	
	private static String EXCLUDED_VALUE = "excluded";
	private static String ADDED_VALUE = "added";
	
	/*
	 * Temporary list used to store the selected items 
	 * (if multiple items selected before right clicking).
	 */
	private ArrayList<IResource> resources = null;
	
	private IWorkbenchWindow window;
	
	/**
	 * Initializes the lists
	 */
	public EJScriptExcludeFromBuild() {
		
	}

	/**
	 * 
	 */
	public void init(IWorkbenchWindow window) {
		this.window = window;
	}

	/**
	 * This method is run when a new selection is made by the user in
	 * the package explorer.
	 * 
	 * Sets "Exclude From Build" active if single resource selected and
	 * both it and it's parent is not already excluded from the build.
	 * 
	 * Sets "Add To Build" active if single resource selected and both
	 *  it and it's parent are not already excluded from the build.
	 *  
	 * Sets "Exlude From Build" and "Add To Build" active if multiple
	 *  resources selected.
	 */
	public void selectionChanged(IAction action, ISelection selection) {
		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.GENERAL_TRACE,
				"+selectionChanged for " + action.getId());
		resources = new ArrayList<IResource>();
		setSelectionIfFileOrFolder(selection);
		int numSelected = ((IStructuredSelection) selection).size();
		if (numSelected == 1) {
			if (action.getId().equals(EXCLUDE_FROM_BUILD)) {
				action.setEnabled(!isCurrentSelectionExcluded(selection));
			} else {
				if (!parentResourceIsExcluded(selection)) {
					action.setEnabled(isCurrentSelectionExcluded(selection));
				} else {
					action.setEnabled(false);
				}
			}
		} else {
			int numExcluded = getNumberExcluded(selection);
			EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
					EJScriptTrace.GENERAL_TRACE,
					"+selectionChanged numExcluded is " + numExcluded + " numSelected is " + numSelected);
			if (numExcluded == numSelected) {
				if (action.getId().equals(EXCLUDE_FROM_BUILD)) {
					action.setEnabled(true);
				} else {
					action.setEnabled(false);
				}
			} else {
				if (action.getId().equals(EXCLUDE_FROM_BUILD)) {
					if (numExcluded == 0) {
						EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
								EJScriptTrace.GENERAL_TRACE,
								"MULTI EXCLUDE_FROM_BUILD setting false");
						action.setEnabled(false);
					} else {
						EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
								EJScriptTrace.GENERAL_TRACE,
								"MULTI EXCLUDE_FROM_BUILD setting true");
						action.setEnabled(true);
					}
				} else {
					action.setEnabled(true);
				}
			}
		}
	}
	
	/**
	 * Returns true if at least one file has a property value that matches value.
	 * @param selection
	 * @return
	 */
	private boolean isCurrentSelectionExcluded(ISelection selection) {
		
		boolean result = false;
		Iterator<IStructuredSelection> iter = 
			((IStructuredSelection) selection).iterator();
		
		Object elem = iter.next();
		IResource resource = (IResource)elem;
		result = isResourceExcluded(resource);
		
		return result;
	}
	
	/**
	 * When multiple selections are made in the package explorer (to exclude or
	 * add to build in mass) this method determines how many of the selected
	 * files are already excluded.
	 * @param selection
	 * @return
	 */
	private int getNumberExcluded(ISelection selection) {
		int result = 0;
		Iterator<IStructuredSelection> iter = 
			((IStructuredSelection) selection).iterator();
		
		while (iter.hasNext()) {
			Object elem = iter.next();
			IResource resource = (IResource)elem;
			if (isResourceExcluded(resource) == true) {
				result++;
			}
		}
		return result;
	}
	
	/**
	 * Checks to see if a given resource is currently excluded.
	 * @param resource
	 * @return true if file explicitly marked as exclude, or if the parent
	 * directory is marked as exclude.
	 */
	private boolean isResourceExcluded(IResource resource) {
		if (resource instanceof IFile || resource instanceof IFolder) {
			try {
				String excluded = resource.getPersistentProperty(
						PropertyConstants.EXCLUDE_FROM_BUILD_PROPERTY);
				String parentExcluded = resource.getParent().getPersistentProperty(
						PropertyConstants.EXCLUDE_FROM_BUILD_PROPERTY);
				EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
						EJScriptTrace.COMPILER_TRACE,
						"excluded is " + excluded + " and parentExcluded is " + parentExcluded);
				if (excluded != null && excluded.equals(EXCLUDED_VALUE)) {
					EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
							EJScriptTrace.COMPILER_TRACE,
							"value is " + EXCLUDED_VALUE + " and excluded is " + excluded);
					return true;
				}
				if (parentExcluded != null && parentExcluded.equals(EXCLUDED_VALUE)) {
					EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
							EJScriptTrace.COMPILER_TRACE,
							"--fileIsExcluded");
					return true;
				}
			} catch (CoreException e) {
				EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
						EJScriptTrace.COMPILER_TRACE,
						"Inside catch for fileIsExcluded");
			}
		}
		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.COMPILER_TRACE,
				"--fileIs NOT Excluded");
		return false;
	}
	
	/**
	 * Determines if the parent resource of the currently selected object is
	 * excluded.
	 * @param selection
	 * @return
	 */
	private boolean parentResourceIsExcluded(ISelection selection) {
		Iterator<IStructuredSelection> iter = 
			((IStructuredSelection) selection).iterator();
		Object elem = iter.next();
		IResource resource = (IResource)elem;
		if (resource instanceof IFile || resource instanceof IFolder) {
			try {
				String parentExcluded = resource.getParent().getPersistentProperty(
					PropertyConstants.EXCLUDE_FROM_BUILD_PROPERTY);
				if (parentExcluded != null && parentExcluded.equals(EXCLUDED_VALUE)) {
					EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
							EJScriptTrace.COMPILER_TRACE,
							"parentResourceIsExcluded:fileIsExcluded");
					return true;
				}
			} catch (CoreException e) {
				EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
						EJScriptTrace.COMPILER_TRACE,
						"Insidef catch for fileIsExcluded");
			}
		}
		return false;
	}
	
	/**
	 * Checks that the selected items in the tree are of the correct
	 * type. Any folder can be excluded from the build.  Only .es files
	 * can be excluded from the build.
	 * @param selection 
	 */
	private void setSelectionIfFileOrFolder(ISelection selection) {
		
		// an IStructuredSelection is expected in this action
		if (!(selection instanceof IStructuredSelection)) {
			EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
					EJScriptTrace.GENERAL_TRACE,
					"! IStructuredSelection");
			return;
		}
		
		/*
		 *  Check every item that has been selected.  Only add to the
		 *  temporary list those resources that are the correct type.
		 */ 
		
		for (
			Iterator<IStructuredSelection> iter = ((IStructuredSelection) selection).iterator();
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
			if (!(elem instanceof IFile) && !(elem instanceof IFolder)) {
					continue;
			}
			
			resources.add((IResource)elem);
			break;
		}
	}
	
	/**
	 * this method runs when the action is actually clicked.
	 * Adds or removes selected items as necessary to the exclude
	 * lists.
	 */
	public void run(IAction action) {
		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.GENERAL_TRACE,
				"+ExcludeFromBuild:run");
	
		// Cannot modify closed projects.
		Iterator<IResource> iter = resources.iterator();
		while (iter.hasNext()) {
			// Get the description.
			IResource resource = iter.next();
			EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
					EJScriptTrace.GENERAL_TRACE,
					"run: iterator");
			
			if (!(resource instanceof IFile) && !(resource instanceof IFolder)) {
				EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
						EJScriptTrace.GENERAL_TRACE,
						"Not correct type of Object");
					continue;
			}
			if (action.getId().equals(EXCLUDE_FROM_BUILD)) {
				EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
						EJScriptTrace.GENERAL_TRACE,
						"excluding " + resource.getLocation().toOSString() +
						" from build");
				if (resource instanceof IFile) {
					IFile file = (IFile)resource;
					if (!file.getName().endsWith(EJSConstants.EJSCRIPT_FILE_EXTENSION)) {
						continue;
					}
					applyExcludeProperty(resource, EXCLUDED_VALUE);
				} else {
					applyExcludedPropertyToAll((IFolder)resource, EXCLUDED_VALUE);
				}
			} else {
				EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
						EJScriptTrace.GENERAL_TRACE,
						"adding " + resource.getLocation().toOSString() + " to build");
				if (resource instanceof IFile) {
					applyExcludeProperty(resource, ADDED_VALUE);
				} else {
					applyExcludedPropertyToAll((IFolder)resource, ADDED_VALUE);
				}
			}
		}
	}
	
	/**
	 * Comparator class used by the TreeSets to determine
	 * if an object has already ben added ot the set or not.
	 * @author Gavin
	 *
	 */
	class stringComparator implements Comparator<IResource> {
		 public int compare(IResource o1, IResource o2) {
			 return o1.getLocation().toOSString().compareTo(o2.getLocation().toOSString());
		 }
	}
	
	/**
	 * Apply the EXCLUDE_FROM_BUILD_PROPERTY to the resource. 
	 * @param resource
	 * @param value
	 */
	private void applyExcludeProperty(IResource resource, String value) {
		try {
			resource.setPersistentProperty(
				PropertyConstants.EXCLUDE_FROM_BUILD_PROPERTY,
				value);
		} catch (CoreException e) {
			
		}
		/*
		 * Special case to get rid of any potential Problem markers before the file is excluded.
		 */
		if (resource instanceof IFile) {
			EJScriptBuilder.deleteMarkers(resource);
		}
	}
	
	/**
	 * Apply the EXCLUDE_FROM_BUILD_PROPERTY to the resource and all
	 * contianer resources below it.
	 * @param folder
	 * @param value
	 */
	private void applyExcludedPropertyToAll(IFolder folder, String value) {
		applyExcludeProperty(folder, value);
		try {
			IResource[] members = folder.members();
			for (int i = 0; i < members.length; i++) {
				IResource member = members[i];
				if (member instanceof IFolder) {
					applyExcludedPropertyToAll((IFolder)member, value);
				}
			}
		} catch (CoreException e) {
			
		}
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
