/*
 * Copyright (c) All Rights Reserved. See details at the end of the file.
 */
package com.embedthis.ejs.ide.builder;

import java.util.logging.Logger;

import org.eclipse.core.resources.ICommand;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IFolder;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IProjectDescription;
import org.eclipse.core.resources.IProjectNature;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.runtime.CoreException;

import com.embedthis.ejs.ide.EJSConstants;
import com.embedthis.ejs.ide.core.EJScriptConsoleDisplayMgr;
import com.embedthis.ejs.ide.core.EJScriptTrace;


/**
 * @author Gavin
 *
 */
public class EJScriptNature implements IProjectNature {

	/**
	 * ID of this project nature
	 */
	public static final String NATURE_ID = "com.embedthis.ejs.ide.EJScriptNature";
	
	public IProject project;

	private Logger logger = Logger.getLogger("com.embedthis.ejs.ide");
	
	private EJScriptConsoleDisplayMgr console =
		EJScriptConsoleDisplayMgr.getDefault();
	
	/* (non-Javadoc)
	 * @see org.eclipse.core.resources.IProjectNature#configure()
	 */
	public void configure() throws CoreException {
		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.GENERAL_TRACE,
				"Inside EJScriptNature.configure()");
		IProjectDescription desc = project.getDescription();
		ICommand[] commands = desc.getBuildSpec();
		boolean found = false;
		
		for (int i = 0; i < commands.length; ++i) {
			if (commands[i].getBuilderName().equals(EJScriptBuilder.BUILDER_ID)) {
				found = true;
				break;
			}
		}
		if (!found) {
			ICommand[] newCommands = new ICommand[commands.length + 1];
			System.arraycopy(commands, 0, newCommands, 0, commands.length);
			ICommand command = desc.newCommand();
			command.setBuilderName(EJScriptBuilder.BUILDER_ID);
			EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
					EJScriptTrace.GENERAL_TRACE,
					"Adding " + EJScriptBuilder.BUILDER_ID + " as a builder\n");
			newCommands[newCommands.length - 1] = command;
			desc.setBuildSpec(newCommands);
			project.setDescription(desc, null);
		}
	}

	/* (non-Javadoc)
	 * @see org.eclipse.core.resources.IProjectNature#deconfigure()
	 */
	public void deconfigure() throws CoreException {
		console.println("Removing EJScript Nature");
		IProjectDescription description = getProject().getDescription();
		ICommand[] commands = description.getBuildSpec();
		for (int i = 0; i < commands.length; ++i) {
			if (commands[i].getBuilderName().equals(EJScriptBuilder.BUILDER_ID)) {
				ICommand[] newCommands = new ICommand[commands.length - 1];
				System.arraycopy(commands, 0, newCommands, 0, i);
				System.arraycopy(commands, i + 1, newCommands, i,
						commands.length - i - 1);
				description.setBuildSpec(newCommands);
			
				deleteProjectMarkers();
				return;
			}
		}
	}

	private void deleteProjectMarkers() {
		console.println("Removing EJScript Markers");
		try {
			deleteMarkers(project.members());
		} catch (CoreException ce) {
			EJScriptTrace.traceError(EJScriptTrace.COMPILER_TRACE, ce);
		}
	}
	
	private void deleteMarkers(IResource[] resources) {
		try {
			for (int i = 0; i < resources.length; i++) {
				IResource resource = resources[i];
				if (resource instanceof IFile &&
						resource.getName().endsWith(EJSConstants.EJSCRIPT_FILE_EXTENSION)) {
					IFile file = (IFile) resource;
					file.deleteMarkers(IMarker.PROBLEM, false, IResource.DEPTH_ZERO);
				} else if (resource instanceof IFolder) {
					IFolder folder = (IFolder)resource;
					deleteMarkers(folder.members());
				}
			}
		} catch (CoreException ce) {
			EJScriptTrace.traceError(EJScriptTrace.COMPILER_TRACE, ce);
		}
	}
		
	/* (non-Javadoc)
	 * @see org.eclipse.core.resources.IProjectNature#getProject()
	 */
	public IProject getProject() {
		return project;
	}

	/* (non-Javadoc)
	 * @see org.eclipse.core.resources.IProjectNature#setProject(org.eclipse.core.resources.IProject)
	 */
	public void setProject(IProject project) {
		this.project = project;

	}

	/**
	 * 
	 * @param project
	 * @throws CoreException
	 */
	public void addJavaScriptNature(IProject project) throws CoreException {
		if (!project.hasNature(EJScriptNature.NATURE_ID)) {
			try {
				IProjectDescription description = project.getDescription();
				String[] natures = description.getNatureIds();
				String[] newNatures = new String[natures.length + 1];
				System.arraycopy(natures, 0, newNatures, 0, natures.length);
				newNatures[natures.length] = EJScriptNature.NATURE_ID;
				description.setNatureIds(newNatures);
				project.setDescription(description, null);
				// Confirm nature add.
				logger.info("SampleNature added to the " + project.getName() + " project.");
			} catch (CoreException e) {
				logger.severe("Could not add SampleNature to the " + project.getName() + " project.");
			}
		} else {
			logger.warning("The SampleNature is already associated with the " + 
					project.getName() + " project.");
		}
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
