/*
 * Copyright (c) All Rights Reserved. See details at the end of the file.
 */
package com.embedthis.ejs.ide.view;

import javax.swing.*;

import org.eclipse.ui.views.contentoutline.ContentOutlinePage;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.ui.part.IPageSite;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.jface.viewers.TreeViewer;

import com.embedthis.ejs.ide.EJScriptPlugin;
import com.embedthis.ejs.ide.editors.EJScriptEditor;
import com.embedthis.ejs.ide.views.EJScriptProjectContentProvider;

public class EJScriptOutlinePage extends ContentOutlinePage {

	private IPageSite pageSite;
	private EJScriptEditor editor;
		
	public EJScriptOutlinePage() {
		// TODO Auto-generated constructor stub
	}
	
	public EJScriptOutlinePage(EJScriptEditor editor) {
		this.editor = editor;
	}
	
	public void init(IPageSite pageSite) {
		this.pageSite = pageSite;
	}
	
	public void createControl(Composite parent) {
		super.createControl(parent);
		TreeViewer viewer = getTreeViewer();
		
		viewer.setContentProvider(new EJScriptProjectContentProvider());
//		viewer.setLabelProvider(new EJScriptLabelProvider());
		viewer.addSelectionChangedListener(this);
		viewer.setInput(editor.getEditorInput());
	}
	
	public void inputChanged(Viewer view, Object oldInput, Object newInput){
		
	}
}

/*
 *	@copy	default
 *	
 *	Copyright (c) Embedthis Software LLC, 2003-2012. All Rights Reserved.
 *	Copyright (c) Michael O'Brien, 1993-2012. All Rights Reserved.
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
