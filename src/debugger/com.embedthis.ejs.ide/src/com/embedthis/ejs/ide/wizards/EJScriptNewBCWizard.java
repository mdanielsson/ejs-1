/*
 * Copyright (c) All Rights Reserved. See details at the end of the file.
 */
package com.embedthis.ejs.ide.wizards;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.lang.reflect.InvocationTargetException;

import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Status;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.operation.IRunnableWithProgress;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.wizard.Wizard;
import org.eclipse.jface.wizard.WizardDialog;
import org.eclipse.ui.INewWizard;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.IWorkbenchWindowActionDelegate;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.ide.IDE;

import com.embedthis.ejs.ide.core.EJScriptCompilerOptions;

public class EJScriptNewBCWizard extends Wizard implements
		INewWizard, IWorkbenchWindowActionDelegate {

	private EJScriptNewBCWizardPage page;
	private ISelection selection;
	private IWorkbenchWindow window;
	
	public EJScriptNewBCWizard() {
		super();
		setNeedsProgressMonitor(true);
	}
	
	
	
	public void addPages() {
		page = new EJScriptNewBCWizardPage(selection);
		addPage(page);
	}

	@Override
	public boolean performFinish() {
		final String containerName = page.getContainerName();
		final String fileName = page.getFileName();
		IRunnableWithProgress op = new IRunnableWithProgress() {
			public void run(IProgressMonitor monitor) throws InvocationTargetException {
				try {
					doFinish(containerName, fileName, monitor);
				} catch (CoreException e) {
					throw new InvocationTargetException(e);
				} finally {
					monitor.done();
				}
			}
		};
		try {
			getContainer().run(true, false, op);
		} catch (InterruptedException e) {
			return false;
		} catch (InvocationTargetException e) {
			Throwable realException = e.getTargetException();
			MessageDialog.openError(getShell(),
			"Error", realException.getMessage());
			return false;
		}
		return true;
	}
	
	/**
	 * The worker method. It will find the container, create the
	 * file if missing or just replace its contents, and open
	 * the editor on the newly created file.
	 */
	private void doFinish(
		String containerName,
		String fileName,
		IProgressMonitor monitor)
		throws CoreException {
		// create a sample file
		monitor.beginTask("Creating " + fileName, 2);
		IWorkspaceRoot root = ResourcesPlugin.getWorkspace().getRoot();
		IResource resource = root.findMember(new Path(containerName));
		if (!resource.exists() || !(resource instanceof IContainer)) {
			throwCoreException("Container \"" + containerName + "\" does not exist.");
		}
		IContainer container = (IContainer) resource;
		final IFile file = container.getFile(new Path(fileName));
		try {
			InputStream stream = openContentStream(fileName);
			if (file.exists()) {
				file.setContents(stream, true, true, monitor);
			} else {
				file.create(stream, true, monitor);
			}
			stream.close();
		} catch (IOException e) {
		}
		monitor.worked(1);
		monitor.setTaskName("Opening file for editing...");
		getShell().getDisplay().asyncExec(new Runnable() {
			public void run() {
				IWorkbenchPage page =
					PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage();
				try {
					IDE.openEditor(page, file, true);
				} catch (PartInitException e) {
				}
			}
		});
		monitor.worked(1);
	}
	
	/**
	 * We will initialize file contents with a sample text.
	 */
	private InputStream openContentStream(String fileName) {
		String contents =
			"<?xm version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n" +
			"<bc>\n" +
			"<buildConfiguration name=\"" + fileName + "\">\n" +
			"<resource>ALL</resource>" +
			"<compilerOptions> " + EJScriptCompilerOptions.DEFAULT_COMPILER_OPTIONS + "</compilerOptions>\n" +
			"</buildConfiguration>\n</bc>\n";
		return new ByteArrayInputStream(contents.getBytes());
	}

	private void throwCoreException(String message) throws CoreException {
		IStatus status =
			new Status(IStatus.ERROR, "com.embedthis.ejs.ide", IStatus.OK, message, null);
		throw new CoreException(status);
	}
	
	/*
	 * The following methods are used to invoke the wizard directly from
	 * the New.. menu instead of having to go into the Other.. section. This
	 * is done by implemention IWorkbenchWindowActionDelegate
	 * (non-Javadoc)
	 * @see org.eclipse.ui.IWorkbenchWizard#init(org.eclipse.ui.IWorkbench, org.eclipse.jface.viewers.IStructuredSelection)
	 */
	
	public void init(IWorkbench workbench, IStructuredSelection selection) {
		this.selection = selection;
	}

	public void init(IWorkbenchWindow window) {
		this.window = window;
	}
	
	public void selectionChanged(IAction action,
            ISelection selection) {
	
	}
	
	public void run(IAction action) {
		Wizard wizard = new EJScriptNewBCWizard();
		WizardDialog dialog = new WizardDialog(window.getShell(), wizard);
		dialog.open();
	}
	/* END IWorkbenchWindowActionDelegate methods */
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
