/*
 * Copyright (c) All Rights Reserved. See details at the end of the file.
 */
package com.embedthis.ejs.ide.editors;

import java.util.ArrayList;
import java.util.Iterator;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorSite;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.editors.text.TextEditor;
import org.eclipse.ui.forms.editor.FormEditor;

import com.embedthis.ejs.ide.core.EJSBuildConfiguration;
import com.embedthis.ejs.ide.core.EJScriptTrace;
import com.embedthis.ejs.ide.core.PakDefFile;

public class PakDefEditor extends FormEditor {

	private TextEditor editor;
	private int editorIndex;
	private boolean isDirty = false;
	private IEditorInput input = null;
	private PakDefFile pakDefFile = null;
	private ArrayList<EJSBuildConfiguration> buildConfigs = null;
	
	private ArrayList<EJSBuildConfigPage> configPages = 
		new ArrayList<EJSBuildConfigPage>();
	
	private int numPages = 0;
	
	
	
	public PakDefEditor() {
		super();
		EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
				EJScriptTrace.GENERAL_TRACE,
				"+PakDefEditor()");
		
	}

	@Override
	public boolean isDirty() {
		return isDirty;
	}
	
	public String getPakDefName() {
		return pakDefFile.getPakDefName();
	}

	public void setIsDirty(boolean isDirty) {
		EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
				EJScriptTrace.EDITOR_OUTPUT_TRACE,
				"setIsDirty(" + isDirty + ")");
		this.isDirty = isDirty;
		firePropertyChange(PROP_DIRTY);
	}
	
	@Override
	protected void addPages() {
		input = this.getEditorInput();
		EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
			EJScriptTrace.GENERAL_TRACE,
			"+PakDefEditor:addPages");
		IFile file;
		
		if (input instanceof IFileEditorInput) {
			EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
				EJScriptTrace.GENERAL_TRACE,
				"+PakDefEditor:createPages - IS IFileEditorInput");
			file = ((IFileEditorInput)input).getFile();
			pakDefFile = new PakDefFile(file);
			buildConfigs = pakDefFile.getBuildConfigs();
			Iterator<EJSBuildConfiguration> iter = buildConfigs.iterator();
			while (iter.hasNext()) {
				EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
						EJScriptTrace.GENERAL_TRACE,
						"+PakDefEditor:addPages: adding " + numPages);
				EJSBuildConfiguration buildConfig = iter.next();
				EJSBuildConfigPage page = new EJSBuildConfigPage(this, buildConfig, 
						getCurrentProject(), numPages);
				configPages.add(page);
				try {
					addPage(page);
				} catch (PartInitException e) {
					
				}
				numPages++;
			}
			// new file, create on build config
			if (numPages == 0) {
				EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
						EJScriptTrace.GENERAL_TRACE,
						"NUMPAGES is 0");
				// Need to add one page
				numPages++;
				createTextEditorPage();
			}
		} else {
			EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
					EJScriptTrace.GENERAL_TRACE,
					"+PakDefEditor:createPages - not IFileEditorInput");
		}
		EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
			EJScriptTrace.GENERAL_TRACE,
			"-PakDefEditor:addPages");
	}
	
	public IResource getProject() {
		return getCurrentProject();
	}

	@Override
	public void doSave(IProgressMonitor monitor) {
		EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
				EJScriptTrace.EDITOR_OUTPUT_TRACE,
				"+doSave");
		monitor.beginTask("Saving " + pakDefFile.getPakDefName(), getPageCount());
		EJSBuildConfigPage page;
		Iterator<EJSBuildConfigPage> iter = configPages.iterator();
		while (iter.hasNext()) {
			page = iter.next();
			page.doSave(monitor);
		}
		pakDefFile.save(monitor);
		setIsDirty(false);
		monitor.done();
	}
	
	@Override
	public void doSaveAs() {
		// TODO GMK SaveAs still needs to be implemented.
		EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
				EJScriptTrace.GENERAL_TRACE,
				"+doSaveAs");
		// Use File browser dialog to get new file name.
		// save can take an additional parameter, newName.
		pakDefFile.save(null);
		setIsDirty(false);
	}

	@Override
	public boolean isSaveAsAllowed() {
		// TODO Auto-generated method stub
		return false;
	}
	
	/**
	 * 
	 * @param name
	 */
	public void addNewBuildConfiguration(String name) {
		EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
				EJScriptTrace.GENERAL_TRACE,
				"+addNewBuildConfiguration:");
		EJSBuildConfiguration buildConfig =
			new EJSBuildConfiguration(name, pakDefFile.getPakDefName());
		try {
			EJSBuildConfigPage newPage = new EJSBuildConfigPage(this, buildConfig, 
					getCurrentProject(), numPages); 
			int index = addPage(newPage);
//			setActivePage(index);
			newPage.setActive(true);
			
		} catch (PartInitException e) {
			
		}
		numPages++;
		buildConfigs.add(buildConfig);
		EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
				EJScriptTrace.GENERAL_TRACE,
				"-addNewBuildConfiguration:");
	}
	
	/**
	 * 
	 * @return
	 */
	private IProject getCurrentProject() {
		IEditorInput input = this.getEditorInput();
		if (input instanceof IFileEditorInput) {
			
			IFile file = ((IFileEditorInput)input).getFile();
			EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
					EJScriptTrace.COMPILER_TRACE,
					"current project is " + file.getProject().getName());
			return file.getProject();
		} else {
			EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
				EJScriptTrace.COMPILER_TRACE,
				"resource not IFile");
		}
		return null;
	}
	
	/**
	 * 
	 */
	private void createTextEditorPage() {
		EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
				EJScriptTrace.COMPILER_TRACE,
				"createTextEditorPage");
		try {
			editor = new TextEditor();
			editor.doRevertToSaved();
			editorIndex = addPage(editor, getEditorInput());
			setPageText(editorIndex, "*.pakdef");
			EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
					EJScriptTrace.COMPILER_TRACE,
					"-createTextEditorPage");
		} catch (PartInitException e) {
			EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
					EJScriptTrace.GENERAL_TRACE,
					"ERROR: " + e.getMessage());
		}
	}
	
	/**
     * The <code>MultiPageEditorExample</code> implementation of this method
     * checks that the input is an instance of <code>IFileEditorInput</code>.
     */
    public void init(IEditorSite site, IEditorInput editorInput)
            throws PartInitException {
        if (!(editorInput instanceof IFileEditorInput))
            throw new PartInitException("InvalidInput"); //$NON-NLS-1$
        super.init(site, editorInput);
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
