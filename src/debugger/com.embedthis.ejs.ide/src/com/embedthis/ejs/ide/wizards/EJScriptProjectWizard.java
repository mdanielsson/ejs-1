/*
 * Copyright (c) All Rights Reserved. See details at the end of the file.
 */
package com.embedthis.ejs.ide.wizards;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.URL;
import java.net.MalformedURLException;


import org.eclipse.jface.action.IAction;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.wizard.Wizard;
import org.eclipse.jface.wizard.WizardDialog;
import org.eclipse.ui.dialogs.WizardNewProjectCreationPage;
import org.eclipse.ui.ide.IDE;
import org.eclipse.ui.INewWizard;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.IWorkbenchWindowActionDelegate;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IProjectDescription;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;

import com.embedthis.ejs.ide.EJScriptPlugin;
import com.embedthis.ejs.ide.builder.EJScriptNature;
import com.embedthis.ejs.ide.core.EJScriptCompilerOptions;
import com.embedthis.ejs.ide.core.EJScriptTrace;

/**
 * @author Gavin
 *
 */
public class EJScriptProjectWizard extends Wizard implements INewWizard, 
					IWorkbenchWindowActionDelegate {

	private WizardNewProjectCreationPage newProjectPage;
	private EJScriptNewBCWizardPage newBCFilePage;
	private static IPath location = null;
	
	private IWorkbenchWindow window;
	
	/**
	 * 
	 */
	public EJScriptProjectWizard() {
		super();
	}
	
	/* (non-Javadoc)
	 * @see org.eclipse.jface.wizard.Wizard#performFinish()
	 */
	@Override
	public boolean performFinish() {
		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.WIZARD_TRACE,
				"Creating Project " + newProjectPage.getProjectName());
		IProject project = newProjectPage.getProjectHandle();
		IProjectDescription description = ResourcesPlugin.getWorkspace().
						newProjectDescription(project.getName());
		
		try {
			if (newProjectPage.useDefaults() == false) {
				location = newProjectPage.getLocationPath();
				EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
						EJScriptTrace.WIZARD_TRACE,
						"Not using default location setting location to " + 
						location.toFile().getAbsolutePath());
				description.setLocation(location);
			} else {
				
			}
			project.create(description, null);
			project.open(null);
			/**
			 *  TODO GMK This is where we would set additional natures if necessary.
			 *  For instance mobs 'javadoc'-like builder. Unless this builder
			 *  is included in the JScriptNature
			 */
			
			String[] natures = description.getNatureIds();
			String[] newNatures = new String[natures.length + 1];
			System.arraycopy(natures, 0, newNatures, 0, natures.length);
			newNatures[natures.length] = EJScriptNature.NATURE_ID;
			description.setNatureIds(newNatures);
			project.setDescription(description, null);
			// TODO GMK: Need to set values for Builder. i.e. output folder
			
			final String fileName = newBCFilePage.getFileName();
			doFinish(project, fileName);

		} catch (CoreException e) {
			EJScriptTrace.traceError(EJScriptTrace.WIZARD_TRACE,
					"Failed to create project " + newProjectPage.getProjectName(), e);
			try {
				project.delete(true, null);
			} catch (CoreException e2) {
				
			}
		}
		
		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.WIZARD_TRACE,
				"Successfully Created JScriptEditor project " + project.getName());
		return true;
	}

	/* (non-Javadoc)
	 * @see org.eclipse.ui.IWorkbenchWizard#init(org.eclipse.ui.IWorkbench, org.eclipse.jface.viewers.IStructuredSelection)
	 */
	public void init(IWorkbench workbench, IStructuredSelection selection) {
		setWindowTitle("mbedThis JavaScript Project Wizard");
		setDefaultPageImageDescriptor(getImageDescriptor("newjprj_wiz.gif"));
		setNeedsProgressMonitor(true);
	}

	private ImageDescriptor getImageDescriptor(String relativePath) {
		
		String iconPath = "icons/";
		try {
			EJScriptPlugin plugin = EJScriptPlugin.getDefault();
			if (plugin == null) {
				EJScriptTrace.traceError(EJScriptTrace.WIZARD_TRACE,
						"plugin is null", null);
			}
			if (plugin.getBundle() == null) {
				EJScriptTrace.traceError(EJScriptTrace.WIZARD_TRACE,
						"getBundle returnd null", null);
			}
			URL installURL = plugin.getBundle().getEntry("/");
			URL url = new URL(installURL, iconPath + relativePath);
			return ImageDescriptor.createFromURL(url);
		} catch (MalformedURLException e) {
			return null;
		}
	}
	
	/**
	 * Add pages to the Wizard
	 * @see org.eclipse.jface.wizard.Wizard#addPages()
	 */
	public void addPages() {
		newProjectPage = new WizardNewProjectCreationPage("New Project Creation");
		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.WIZARD_TRACE,
				"Adding page");
		newProjectPage.setTitle("Create new EJS Project");
		newProjectPage.setDescription("Description for New Project Creation");
		addPage(newProjectPage);

		newBCFilePage = new EJScriptNewBCWizardPage(null);
		newBCFilePage.setTitle("Create a new Build Configuration File");
		newBCFilePage.setDescription("The Build Configuration File determines how your project " +
				"will be built by the compiler.  You can build the full project as one application " +
				"(default) or you can build a subset of the files by selecting only those which you " +
				"would like compiled together.");
		addPage(newBCFilePage);
	}

	
	/**
	 * The worker method. It will find the container, create the
	 * file if missing or just replace its contents, and open
	 * the editor on the newly created file.
	 */
	private void doFinish(
		IContainer container,
		String fileName)
		throws CoreException {

		final IFile file = container.getFile(new Path(fileName));
		try {
			InputStream stream = openContentStream(fileName);
			if (file.exists()) {
				file.setContents(stream, true, true, null);
			} else {
				file.create(stream, true, null);
			}
			stream.close();
		} catch (IOException e) {
		}

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

	}
	
	/**
	 * We will initialize file contents with a sample text.
	 */
	private InputStream openContentStream(String fileName) {
		String contents =
			"<?xm version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n" +
			"<bc>\n" +
			"<buildConfiguration name=\"" + fileName + "|myConfig\">\n" +
			"<resource>ALL</resource>" +
			"<compilerOptions> " + EJScriptCompilerOptions.DEFAULT_COMPILER_OPTIONS + "</compilerOptions>\n" +
			"</buildConfiguration>\n</bc>\n";
		return new ByteArrayInputStream(contents.getBytes());
	}

	/*
	 * The following methods are used to invoke the wizard directly from
	 * the New.. menu instead of having to go into the Other.. section. This
	 * is done by implemention IWorkbenchWindowActionDelegate
	 * (non-Javadoc)
	 * @see org.eclipse.ui.IWorkbenchWizard#init(org.eclipse.ui.IWorkbench, org.eclipse.jface.viewers.IStructuredSelection)
	 */
	public void init(IWorkbenchWindow window) {
		this.window = window;
	}
	
	public void selectionChanged(IAction action,
            ISelection selection) {
	
	}
	
	public void run(IAction action) {
		Wizard wizard = new EJScriptProjectWizard();
		WizardDialog dialog = new WizardDialog(window.getShell(), wizard);
		dialog.open();
	}
	
	/* END IWorkbenchWindowActionDelegate methods */
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
