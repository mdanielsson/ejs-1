/*
 * Copyright (c) All Rights Reserved. See details at the end of the file.
 */
package com.embedthis.ejs.ide.core;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.Path;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

public class EJSBuildConfiguration {

	// Contains the list of files or folders included in this
	// build configuration.
	private List<IResource> resourceList = new ArrayList<IResource>();
	
	// Name representing Build Configuration
	private String buildConfigName;

	//
	private EJScriptCompilerOptions commandArgs;
	
	private HashSet<String> removedResources = new HashSet<String>();
	private HashSet<String> newResources = new HashSet<String>();
	
	private boolean isNew; // allows the builder to recognize a new BC and execute it the first time.
	private boolean isDirty = false;
	
	private boolean fileByFileEnabled = false;
	
	public EJSBuildConfiguration(String bcName,
			String pakDefFile) {
		EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
				EJScriptTrace.GENERAL_TRACE,
				"EJSBuildConfig name is \"" + bcName + "\"");

		// TODO GMK: would be a good idea to confirm this is the project path to the file.
		buildConfigName = pakDefFile;
		
		commandArgs = new EJScriptCompilerOptions();
		isNew = true;
		EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
				EJScriptTrace.COMPILER_TRACE,
				"-EJSBuildConfiguration:");
	}
	
	public void setCommandArgs(String command) {
		EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
				EJScriptTrace.COMPILER_TRACE,
				"+setCommandArgs '" + command + "'");
		commandArgs = new EJScriptCompilerOptions(command);
		EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
				EJScriptTrace.COMPILER_TRACE,
				"-setCommandArgs '" + commandArgs.convertToString() + "'");
	}
	
	public EJScriptCompilerOptions getCompilerOptions() {
		return commandArgs;
	}
	
	public void setCommandArgs(EJScriptCompilerOptions commandArgs) {
		EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
				EJScriptTrace.COMPILER_TRACE,
				"+setCommandArgs ");
		if (!this.commandArgs.equals(commandArgs)) {
			isDirty = true;
		}
		this.commandArgs = commandArgs;
	}
	
	public String getBuildConfigName() {
		return buildConfigName;
	}
	
	public List<IResource> getResourceList() {
		return resourceList;
	}
	
	public String[] getCommandArgs() {
		return commandArgs.convertToArgs();
	}
	
	public String getCommandArgsAsString() {
		return commandArgs.convertToString();
	}
	
	public String getName() {
		return buildConfigName;
	}
	
	public String getResourceString() {
		Iterator<IResource> iter = resourceList.iterator();
		String resourceString = new String("");
		while(iter.hasNext()) {
			IResource resource = iter.next();
			if (resource instanceof IFile) {
				resourceString = resourceString.concat(resource.getFullPath().toOSString() + " ");
			}
		}
		return resourceString;
	}
	
	public void setResourceList(ArrayList<IResource> resources) {
		EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
				EJScriptTrace.COMPILER_TRACE,
				"EJSBuildConfiguration: setResourceList");
		resourceList = resources;
	}
	
	/*
	 * Using the resourceString locate the resource in the project and add it
	 * to the resource list.
	 */
	public void addResource(String resourceString) {
		IWorkspaceRoot root = ResourcesPlugin.getWorkspace().getRoot();
		
		if (resourceString == null) {
			EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
				EJScriptTrace.COMPILER_TRACE,
				"resourceString is null");
		}
		IResource resource = root.findMember(new Path(resourceString));
		
		if (resource != null) {
			EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
					EJScriptTrace.COMPILER_TRACE,
					"FOUND resource: " + resourceString);
			resourceList.add(resource);
		} else {
			EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
					EJScriptTrace.COMPILER_TRACE,
					"Could not find resource " + resourceString + 
					" in current project.");
		}
	}
	
	/**
	 * Outputs the Build Configuration to the document.
	 * @param doc
	 */
	public void addToDoc(Document doc, Element element) {
		EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
				EJScriptTrace.COMPILER_TRACE,
				"+addToDoc");
		Element bc = doc.createElement(BCFile.BUILD_CONFIGURATION_NODE);
		bc.setAttribute(BCFile.ATTRIBUTE_NAME, buildConfigName);
		EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
				EJScriptTrace.COMPILER_TRACE,
				"+addToDoc:A");
		if (resourceList == null) {
			return;			
		}
		Iterator<IResource> iter = resourceList.iterator();
		while (iter.hasNext()) {
			EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
					EJScriptTrace.COMPILER_TRACE,
					"+addToDoc:B");
			Element fileNode = doc.createElement(BCFile.RESOURCE_LIST_NODE);
			bc.appendChild(fileNode);
			IResource resourcenode = iter.next();
			if (resourcenode == null) {
				EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
						EJScriptTrace.COMPILER_TRACE,
						"iter.next() returned null");
				continue;
			}
			String resource = resourcenode.getFullPath().toPortableString();
			EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
					EJScriptTrace.COMPILER_TRACE,
					"Saving: " + resource);
			Node resourceNode =  doc.createTextNode(resource);
			fileNode.appendChild(resourceNode);
		}
		Element compOptsElemet = doc.createElement(BCFile.COMPILER_OPTIONS_NODE);
		Node compOpts = doc.createTextNode(commandArgs.convertToString());
		EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
			EJScriptTrace.COMPILER_TRACE,
			commandArgs.convertToString());

		bc.appendChild(compOptsElemet);
		compOptsElemet.appendChild(compOpts);
		
		Element buildTypeElement = doc.createElement(BCFile.BUILD_TYPE_NODE);
		String buildType;
		if (fileByFileEnabled == true) {
			buildType = BCFile.BUILD_TYPE_FILE_BY_FILE;
		} else {
			buildType = BCFile.BUILD_TYPE_NORMAL;
		}
		Node buildNode = doc.createTextNode(buildType);
		bc.appendChild(buildTypeElement);
		buildTypeElement.appendChild(buildNode);
		
		//Add the BC node to the doc.
		element.appendChild(bc);
		
		EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
				EJScriptTrace.COMPILER_TRACE,
				"-addToDoc");
	}
	
	public IFile getFileByName(String fileName) {
		IFile file = null;
		Iterator<IResource> iter = resourceList.iterator();
		while (iter.hasNext()) {
			IResource resource = iter.next();
			if (resource instanceof IFile) {
				IFile tmpFile = (IFile)resource;
				if (tmpFile.getName().equals(fileName)) {
					file = tmpFile;
					break;
				}
			}
		}
		
		return file;
	}
	
	public void clearHistory() {
		EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
			EJScriptTrace.COMPILER_TRACE,
			"Clearing removed and new Resources List");
		removedResources.clear();
		newResources.clear();
	}
	
	public void addRemovedResource(IResource resource) {
		if (resource != null) {
			removedResources.add(resource.getFullPath().toPortableString());
		}
	}
	
	public void addNewResource(IResource resource) {
		if (resource != null) {
			if (resource instanceof IFile) {
				newResources.add(resource.getFullPath().toPortableString());
			}
		}
	}

	public HashSet<String> getRemovedResources() {
		if (!removedResources.isEmpty()) {
			EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
					EJScriptTrace.COMPILER_TRACE,
					"REMOVED RESOURCES IS NOT EMPTY !!!");
		} else {
			EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
					EJScriptTrace.COMPILER_TRACE,
					"REMOVED RESOURCES IS EMPTY");
		}
		return removedResources;
	}
	
	public HashSet<String> getNewResources() {
		return newResources;
	}
	
	public boolean isNew() {
		return isNew;
	}
	
	public void setNew(boolean isNew) {
		this.isNew = isNew;
	}
	
	public void setBuildType(boolean enabled) {
		fileByFileEnabled = enabled;
	}
	
	public boolean isFileByFileEnabled() {
		return fileByFileEnabled;
	}
	
	public boolean isDirty() {
		return isDirty;
	}
	
	// Necessary for BC config page to set build type has changed and a rebuild of the BC is necessary
	public void setIsDirty(boolean dirty) {
		isDirty = dirty;
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
