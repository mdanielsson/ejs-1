/*
 * Copyright (c) All Rights Reserved. See details at the end of the file.
 */
package com.embedthis.ejs.ide.editors;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IFolder;
import org.eclipse.core.resources.IProject;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.jface.viewers.ViewerFilter;

import com.embedthis.ejs.ide.EJSConstants;

public class EJScriptFileFilter extends ViewerFilter {

	IProject project;
	
	public EJScriptFileFilter(IProject project) {
		this.project = project;
	}
	
	@Override
	public boolean select(Viewer viewer, Object parentElement, Object element) {
		boolean result = true;

		if (element instanceof IProject) {
			/*
			 * We only care about the current project.  This is how we get a single node
			 * at the top of the tree viewer to select/deselect the whole project.
			 */
			IProject testProject = (IProject)element;
			if (!testProject.getName().equals(project.getName())) {
				result = false;
			}
		} else if (element instanceof IFile) {
		
			IFile file = (IFile)element;
			if (file.getName().endsWith(EJSConstants.EJSCRIPT_FILE_EXTENSION)) {
				result = true;
			} else {
				result = false;
			}
		} else if (element instanceof IFolder){
			// TODO GMK Need to only include folders which contain .es files?
			
			IFolder folder = (IFolder)element;
			if (folder.getName().startsWith(".")) {
				result = false;
			} else {
				result = true;
			}
		}
		return result;
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
