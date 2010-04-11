/*
 * Copyright (c) All Rights Reserved. See details at the end of the file.
 */
package com.embedthis.ejs.ide.views;

import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IFolder;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.jface.viewers.ArrayContentProvider;
import org.eclipse.jface.viewers.ITreeContentProvider;

import com.embedthis.ejs.ide.EJScriptPlugin;
import com.embedthis.ejs.ide.core.EJScriptTrace;

public class EJScriptProjectContentProvider extends ArrayContentProvider implements
		ITreeContentProvider {

	public EJScriptProjectContentProvider() {
		// TODO Auto-generated constructor stub
	}
	
	public Object[] getElements(Object parent) {
		return ResourcesPlugin.getWorkspace().getRoot().getProjects();
	}
	
	public Object[] getChildren(Object parentElement) {
		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.VIEWS_TRACE, 
				"+getChildren");
		Object[] children = null;
		if (parentElement instanceof IContainer) {
			IContainer container = (IContainer)parentElement;
			try {
				children = container.members();
			} catch (CoreException e) {
				children = null;
			}
		}
		return children;
	}

	public Object getParent(Object element) {
		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.VIEWS_TRACE, 
				"+getParent");
		IResource parent = null;
		if (element instanceof IResource) {
			IResource current = (IResource)element;
			parent = current.getParent();
		}
		return parent;
	}

	public boolean hasChildren(Object element) {
		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.VIEWS_TRACE, 
				"+hasChildren");
		boolean hasChildren = false;
		if (element instanceof IContainer) {
			IContainer container = (IContainer)element;
			try {
				if (container.members().length > 0){
					hasChildren = true;
				}
			} catch (CoreException e) {
				hasChildren = false;
			}
		} else {
			EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
					EJScriptTrace.VIEWS_TRACE, 
					"object not instanceof IContainer");
		}
		return hasChildren;
	}
}


/*
 *	@copy	default
 *	
 *	Copyright (c) Embedthis Software LLC, 2003-2010. All Rights Reserved.
 *	Copyright (c) Michael O'Brien, 1993-2010. All Rights Reserved.
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
 *	@end
 */
