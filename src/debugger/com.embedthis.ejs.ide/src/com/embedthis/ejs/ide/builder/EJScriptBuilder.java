/*
 * Copyright (c) All Rights Reserved. See details at the end of the file.
 */
package com.embedthis.ejs.ide.builder;

import java.io.File;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.GregorianCalendar;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.Map;
import java.util.List;
import java.util.Set;
import java.util.StringTokenizer;

import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IResourceDelta;
import org.eclipse.core.resources.IResourceVisitor;
import org.eclipse.core.resources.IResourceDeltaVisitor;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.IncrementalProjectBuilder;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IConfigurationElement;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Preferences;
import org.eclipse.core.runtime.QualifiedName;
import org.eclipse.core.runtime.Preferences.IPropertyChangeListener;
import org.eclipse.core.runtime.Preferences.PropertyChangeEvent;
import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.IWorkbenchWindow;

import com.embedthis.ejs.ide.EJScriptPlugin;
import com.embedthis.ejs.ide.core.BCFile;
import com.embedthis.ejs.ide.core.CompilerExecutor;
import com.embedthis.ejs.ide.core.CompilerOutput;
import com.embedthis.ejs.ide.core.EJSBuildConfiguration;
import com.embedthis.ejs.ide.core.EJScriptConsoleDisplayMgr;
import com.embedthis.ejs.ide.core.EJScriptTrace;
import com.embedthis.ejs.ide.preferences.PreferenceConstants;

public class EJScriptBuilder extends IncrementalProjectBuilder {

	/* ID used in plugin.xml and to reference from other objects */
	public static final String BUILDER_ID = 
		"com.embedthis.ejs.ide.EJScriptBuilder";
	
	public static final String BUILD_START_MESSAGE = "Beginning Build: ";
	public static final String BUILD_END_MESSAGE = "End of Build: ";
	public static final String CLEAN_START_MESSAGE = "Beginning Clean: ";
	public static final String CLEAN_END_MESSAGE = "End of Clean: ";
	
	
	/* static value for monitor */
	private static final int TOTAL_WORK_UNITS = 5;
	
	/* separates BC names */
	private static final String SEPARATOR = "^";
	
	/* console object for writing trace information to USER */
	private EJScriptConsoleDisplayMgr console = 
		EJScriptConsoleDisplayMgr.getDefault();
	
	/* used for looking up property values */
	private QualifiedName qn = new QualifiedName(EJScriptPlugin.PLUGIN_ID,
			"EJScriptProperty");
	
	/*
	 * Main Lists for handing build configurations
	 */
	/*
	 *  List of Build Configurations by file name
	 *  
	 *  lookup 
	 */ 
	private static HashMap<String, BCFile> bcFilesList = 
		new HashMap<String, BCFile>();
	
	/*
	 * List of all Build configurations in the system
	 * based on build configuration name.
	 */
	private static HashMap<String, EJSBuildConfiguration> 
		allBuildConfigs = 
		new HashMap<String, EJSBuildConfiguration>();

	/*
	 * During the build, this list maintains the build configuratoins
	 * that need to be built after all files have been checked.
	 */
	private Set<String> dirtyBuildConfigs =	new HashSet<String>();
	
	/*
	 * Constructor - listen for changes to project properties.
	 */
	public EJScriptBuilder() {
		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.COMPILER_TRACE,
				"+EJScriptBuilder");
		EJScriptPlugin.getDefault().getPluginPreferences().
			addPropertyChangeListener(compilerChangeListener);
		
		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.COMPILER_TRACE,
				"-EJScriptBuilder");
	}
	
	@Override
	protected IProject[] build(int kind, Map args, IProgressMonitor monitor)
			throws CoreException {
		/*
		 * TODO GMK potential javadoc if @Override doesn't provide better.
		 * Called by Eclipse as an incremental builder.  This is the entry for this class
		 * when performing a build.
		 * @param kind - type of build defined by IncrementalProjectBuilder
		 * @param args
		 * @param monitor
		 */
		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.COMPILER_TRACE,
				"+EJScriptBuilder:build");
		logBuildType(kind);
		traceBuilderStart(BUILD_START_MESSAGE);
		
		monitor.beginTask("Compiling EJScript", TOTAL_WORK_UNITS);
		
		if (kind == FULL_BUILD) {
			fullBuild(monitor);
			// TODO GMK: Add all build configs to the dirty list.
		} else {
			IResourceDelta delta = getDelta(getProject());
			if (delta == null) {
				EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
						EJScriptTrace.COMPILER_TRACE,
						"delta == null");
				fullBuild(monitor);
			} else {
				incrementalBuild(delta, monitor);
			}
		}
		
		/*
		 * For all Build Configurations that need to be built, do that now.
		 */
		executeBuildConfigs(monitor);
		
 		monitor.done();
 		traceBuilderEnd(BUILD_END_MESSAGE);
 		
 		try {
 			// Refresh the project so that newly compiled files are viewed in the Package Browser
 			getProject().refreshLocal(IResource.DEPTH_INFINITE, null);
 		} catch (CoreException e) {
		
 		}
 		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.COMPILER_TRACE,
				"-EJScriptBuilder:build");
		return null;
	}
	
	@Override
	protected void clean(IProgressMonitor monitor)
	{
		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.COMPILER_TRACE,
				"build clean");
		traceBuilderStart(CLEAN_START_MESSAGE);
		// TODO GMK: should clear all storage for BC's and resources.
		// Do we need to iterate through the project and remove resource properties?
		cleanResources(this.getProject());
		bcFilesList.clear();
		allBuildConfigs.clear();
		dirtyBuildConfigs.clear();
		traceBuilderEnd(CLEAN_END_MESSAGE);
	}
	
	/**
	 * Handles a full build of the project
	 * @param monitor
	 * @throws CoreException
	 */
	protected void fullBuild(final IProgressMonitor monitor)
								throws CoreException {
		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.COMPILER_TRACE,
				"+EJScriptBuilder:fullBuild");
		try {
			getProject().accept(new EJScriptResourceVisitor(monitor));
		} catch (CoreException e) {
			EJScriptTrace.traceError(EJScriptTrace.COMPILER_TRACE,e);
		}
		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.COMPILER_TRACE,
				"-EJScriptBuilder:fullBuild");
	}
	
	/**
	 * Handles an incremental build of the project
	 * @param delta
	 * @param monitor
	 * @throws CoreException
	 */
	protected void incrementalBuild(IResourceDelta delta,
			IProgressMonitor monitor) throws CoreException {
		// the visitor does the work.
		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
			EJScriptTrace.COMPILER_TRACE,
			"+EJScriptBuilder:incrementalBuild");
		monitor.worked(1);
		delta.accept(new EJScriptDeltaVisitor(monitor));
		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
			EJScriptTrace.COMPILER_TRACE,
			"-EJScriptBuilder:incrementalBuild");
	}

	/**
	 * Visitor hits every resource in the project.
	 * @author Gavin
	 *
	 */
	private class EJScriptResourceVisitor implements IResourceVisitor {
		
		/* TODO GMK: Set monitor based on the number of files to compute */
		IProgressMonitor monitor;
		
		EJScriptResourceVisitor(IProgressMonitor monitor)
		{
			this.monitor = monitor;
		}
		
		public boolean visit(IResource resource) {
			if (resource == null) {
				EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
						EJScriptTrace.COMPILER_TRACE,
						"resource is null");
			}
			EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
					EJScriptTrace.COMPILER_TRACE,
					"+++++++++++++++++++++Inside EJScriptResourceVisitor:visit " +
					resource.getName());
			boolean result = true;
			if (!monitor.isCanceled()) {
				monitor.worked(1);
				if (resource instanceof IFile) {
					IFile file = (IFile)resource;
					String extension = file.getFileExtension();
					if (extension != null && extension.equals("es")) {
						checkResource(file);
					} else if (extension != null && extension.equals("bc")) {
						addBCFile(file);
					} else {
						// Other files
					}
				}
			}
			EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
					EJScriptTrace.COMPILER_TRACE,
					resource.getName() + " finished visiting");
 			return result;
		}
	}
	
	/**
	 * Determines if the resource is controlled by a BC
	 * 
	 * TODO GMK: This currently only works if the resource is added
	 * to the BuildConfiguration.  Folders are not handled.
	 * 
	 * @param file
	 */
	private void checkResource(IFile file) {
		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.COMPILER_TRACE,
				"+checkResource: " + file.getName());
		String property;
		try {
			if ((property = file.getPersistentProperty(qn)) != null) {
				if (property.indexOf(SEPARATOR) == -1) {
					EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
							EJScriptTrace.COMPILER_TRACE, 
							"Adding single BC to dirty list: " + property);
					dirtyBuildConfigs.add(property);
				} else {
					StringTokenizer tok = new StringTokenizer(property,
							SEPARATOR, false);
					while (tok.hasMoreElements()) {
						property = tok.nextToken();
						EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
							EJScriptTrace.COMPILER_TRACE, 
							"Adding multiple BCs to dirty list: " + property);
						dirtyBuildConfigs.add(property);
					}
				}
			}
		} catch (CoreException e) {
			EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.COMPILER_TRACE, e.getMessage());
		}
		
 		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.COMPILER_TRACE,
				"-checkResource: " + file.getName());
	}
	
	/**
	 * Remove the persistent property from all resources from startingPath down.
	 */
	private void cleanResources(IResource startingResource) {
		if (startingResource instanceof IContainer) {
			IContainer container = (IContainer)startingResource;
			try {
				IResource[] resources = container.members();
				for (int i = 0; i < resources.length; i++) {
					cleanResources(resources[i]);
				}
			} catch (CoreException e) {
				EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
						EJScriptTrace.COMPILER_TRACE,
						"cleanResources: error getting members for " + startingResource.getName());
			}
		} else if (startingResource instanceof IFile &&
				startingResource.getFileExtension() != null &&
				startingResource.getFileExtension().equals("es")				
		) {
			removeAllProperties(startingResource);
		}
	}
	
	/**
	 * Add BC property to all resources referenced in the build configuration
	 * 
	 * This allows us to know, during a build, which resources are referenced
	 * in a BC and which BC that is.
	 * @param bc
	 */
	private void addResources(EJSBuildConfiguration bc) {
		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.COMPILER_TRACE,
				"+addResources: " + bc.getName());
		
		String value = bc.getName();

		Iterator<IResource> iter = bc.getResourceList().iterator();
		while (iter.hasNext()) {
			addProperty(iter.next(), value);
		}
		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
			EJScriptTrace.COMPILER_TRACE,
			"-addResources: " + bc.getName());
	}
	
	/*
	 * Add the property to the file. If property is already set, then update
	 * with the new BC reference.
	 */
	private void addProperty(IResource resource, String value) {
		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
			EJScriptTrace.COMPILER_TRACE, 
			"####################### Adding pProp " + value + " to resource " + 
			resource.getName());
		
		try {
			String property;
			if ((property = resource.getPersistentProperty(qn)) == null) {
				EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
					EJScriptTrace.COMPILER_TRACE, 
					"Adding first pProp " + value + " to resource " + 
					resource.getName());
				resource.setPersistentProperty(qn, value);
			} else {
				EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
					EJScriptTrace.COMPILER_TRACE, 
					"Adding another pProp " + value + " to resource " + 
					resource.getName() + " which already has " + property);
				StringTokenizer tok = new StringTokenizer(property, SEPARATOR);
				boolean found = false;
				while (tok.hasMoreTokens()) {
					if (tok.nextToken().equals(property)) {
						found = true;
					}
				}
				// only add a property once, do not add if already present.
				if (found == false) {
					resource.setPersistentProperty(qn, value + SEPARATOR + property);
				} else {
					EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
						EJScriptTrace.COMPILER_TRACE, 
						"Resource already had property");
				}
			}
		} catch (CoreException e) {
			EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.COMPILER_TRACE, 
				"-addProperty: " + e.getMessage());
		}
	}

	/*
	 * Remove the BC property from this file.  If this file is referenced by more 
	 * than one BC then only remove the BC being referenced.
	 */
	private void removeProperty(IResource resource, String value) {
		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
			EJScriptTrace.COMPILER_TRACE, 
			"####################### REMOVING pProp " + value + " to resource " + 
			resource.getName());
		
		try {
			String property;
			if ((property = resource.getPersistentProperty(qn)) != null) {
				if (property.contains(value)){
//					String[] tokens = property.split(SEPARATOR);
					StringTokenizer tok = new StringTokenizer(property, SEPARATOR);
					String newProperty = null;
					while (tok.hasMoreTokens())  {
//					for (int i = 0; i < tokens.length; i++) {
						String token = tok.nextToken();
						if (!token.equals(value)) {
							if (newProperty == null) {
								newProperty = token;
							} else {
								newProperty = newProperty.concat(SEPARATOR + token);
							}
						}
					}
					resource.setPersistentProperty(qn, newProperty);
				}
			}
		} catch (CoreException e) {
			EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.COMPILER_TRACE, 
				"-removeProperty Exctption: " + e.getMessage());
		}
	}
	
	/*
	 * Remove the BC property from this file.  If this file is referenced by more 
	 * than one BC then only remove the BC being referenced.
	 */
	private void removeAllProperties(IResource resource) {
		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
			EJScriptTrace.COMPILER_TRACE, 
			"removing all build properties from resource " + 
			resource.getName());
		
		try {
			resource.setPersistentProperty(qn, null);
		} catch (CoreException e) {
			EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.COMPILER_TRACE, 
				"-removeProperty Exctption: " + e.getMessage());
		}
	}
	
	/**
	 * 
	 * @param bs
	 */
	/*
	 * TODO GMK Need to:
	 * 1. Remove any resources that were removed from the bc
	 * 2. Add any resources that were added to the bc
	 * 
	 */
	private void updateResources(EJSBuildConfiguration bc) {
		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
			EJScriptTrace.COMPILER_TRACE,
			"+updateResources: " + bc.getName());
		String value = bc.getName();
		synchronized (bc) {
			HashSet<String> newFiles = bc.getNewResources();
			HashSet<String> removedFiles = bc.getRemovedResources();
			IWorkspaceRoot root = ResourcesPlugin.getWorkspace().getRoot();
			Iterator<String> iter = newFiles.iterator();
			while (iter.hasNext()) {
				IResource resource = root.findMember(iter.next());
				EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
					EJScriptTrace.COMPILER_TRACE,
					"+updateResources: adding " + resource.getName());
				addProperty(resource, value);
			}
			
			iter = removedFiles.iterator();
			while (iter.hasNext()) {
				IResource resource = root.findMember(iter.next());
				EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
					EJScriptTrace.COMPILER_TRACE,
					"+updateResources: removing " + resource.getName());
				removeProperty(resource, value);
			}
			

		}
		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.COMPILER_TRACE,
				"-updateResources: " + bc.getName());
	}
	
	/*
	 * Remove a resource from the resource list.
	 * 
	 * 1. Resource only belongs to one BuildConfiguration - remove from
	 * 		the resources list.
	 * 2. Resource belongs to more than one BuildConfiguration - remove
	 * 		the BC from the BC array for that resource.
	 */
	private void removeResources(IResource resource, EJSBuildConfiguration bc) {
		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.COMPILER_TRACE,
				"+removeResources: " + bc.getName());
		
		String value = bc.getName();
		Iterator<IResource> iter = bc.getResourceList().iterator();
		while (iter.hasNext()) {
			removeProperty(iter.next(), value);
		}
		
		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.COMPILER_TRACE,
				"-removeResources: " + bc.getName());
	}
	
	/**
	 * Invoked when the builder recognizes a BC file has been added
	 * to the project.
	 * @param file
	 */
	private void addBCFile(IFile file) {
		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.COMPILER_TRACE,
				"+addBCFile: " + file.getName());
		BCFile bcFile = new BCFile(file);
		bcFilesList.put(file.getProjectRelativePath().toPortableString(),
				bcFile);
		EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
				EJScriptTrace.COMPILER_TRACE,
				"ADDING BC: " + bcFile.getBCName());
		
			EJSBuildConfiguration bs = bcFile.getBuildConfig();
			addResources(bs);
			allBuildConfigs.put(bs.getName(), bs);
			dirtyBuildConfigs.add(bs.getName());

		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.COMPILER_TRACE,
				"-addBCFile: " + file.getName());
	}
	
	/**
	 * Invoked when the builder recognizes a BC file has been removed
	 * from the project.
	 */
	private void removeBCFile(IFile file) {
		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.COMPILER_TRACE,
				"+removeBCFile: " + file.getName());
		BCFile pdFile = bcFilesList.remove(file.getProjectRelativePath().
				toPortableString());

		EJSBuildConfiguration bc = pdFile.getBuildConfig();
		List<IResource> resourceList = bc.getResourceList();
		Iterator<IResource> rIter = resourceList.iterator();
		while (rIter.hasNext()) {
			IResource resource = rIter.next();
			removeResources(resource, bc);
		}

		allBuildConfigs.remove(bc.getName());
			
		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.COMPILER_TRACE,
				"-removeBCFile: " + file.getName());
	}
	
	/**
	 * Invoked when the builder recognizes a BC file has been changed
	 * in the project.
	 * @param file
	 */
	private void updateBCFile(IFile file) {
		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.COMPILER_TRACE,
				"+updateBCFile: " + file.getName());
		/*
		 * Since the bcfile is now added to the global list after it is saved, no need to compare new and old.
		 */
		BCFile bcFile = 
			bcFilesList.get(file.getProjectRelativePath().toPortableString());
		
		if (bcFile == null) {
			EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
					EJScriptTrace.COMPILER_TRACE,
					"+updateBCFile: addBCFile" + file.getName() + " THIS IS PROBABLY AN ERROR");
			addBCFile(file);
		} else {
			EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
					EJScriptTrace.COMPILER_TRACE,
					"+updateBCFile: oldPdFile not null " + file.getName());


			IWorkspaceRoot root = ResourcesPlugin.getWorkspace().getRoot();
			
			// Need to look up the bcFile to get a list of removed and Added BCs in this files.
			
		
			EJSBuildConfiguration bc = bcFile.getBuildConfig();
			HashSet<String> removedSet = bc.getRemovedResources();
			HashSet<String> addedSet = bc.getNewResources();
			String value = bc.getName();
			boolean bcChanged = false;

			if (bc.isNew()) {
				bc.setNew(false);
				bcChanged = true;
			}
			
			if (bc.isDirty()) {
				bcChanged = true;
			}
			
			if (removedSet != null && !removedSet.isEmpty()) {
				Iterator<String> removed = removedSet.iterator();
				while (removed.hasNext()) {
					IResource resource = root.findMember(removed.next());
					removeProperty(resource, value);
				}
				EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
						EJScriptTrace.COMPILER_TRACE,
						"bcChanged == true for removedSet");
				bcChanged = true;
			}
			
			if (addedSet != null && !addedSet.isEmpty()) {
				Iterator<String> added = addedSet.iterator();
				while (added.hasNext()) {
					IResource resource = root.findMember(added.next());
					addProperty(resource, value);
				}
				EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
						EJScriptTrace.COMPILER_TRACE,
						"bcChanged == true for addedSet");
				bcChanged = true;
			}
			bc.clearHistory();
			if (bcChanged == true) {
				dirtyBuildConfigs.add(bc.getName());
			}
		

			EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.COMPILER_TRACE,
				"-updateBCFile: " + file.getName());
		}
	}

	/**
	 * Class implements the IResourceDeltaVisitor which is used when some 
	 * files in a project have changed. This is an incremental build.
	 * @author Gavin 
	 *
	 */
	class EJScriptDeltaVisitor implements IResourceDeltaVisitor {
	
		IProgressMonitor monitor;
		
		EJScriptDeltaVisitor(IProgressMonitor newMonitor)
		{
			this.monitor = newMonitor;
		}
		
		public boolean visit(IResourceDelta delta) {
			boolean result = false;
			IResource resource = delta.getResource();

			EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
					EJScriptTrace.COMPILER_TRACE,
					"+++++++++++++++++++++++++++EJScriptDeltaVisitor:visit " + resource.getName());
			switch (delta.getKind()) {
				case IResourceDelta.ADDED :
					EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
							EJScriptTrace.COMPILER_TRACE,
							resource.getName() + " has been added to the project");
					if (!monitor.isCanceled()) {
						if (resource instanceof IFile) {
							IFile file = (IFile)resource;
							if (file.getFileExtension().equals("bc")) {
								addBCFile(file);
							} else if (file.getFileExtension().equals("es")) {
								checkResource(file);
							}
						}
					}
					break;
				case IResourceDelta.REMOVED :
					EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
							EJScriptTrace.COMPILER_TRACE,
							"Removed " + resource.getName());
					if (!monitor.isCanceled()) {
						if (resource instanceof IFile) {
							IFile file = (IFile)resource;
							if (file.getFileExtension().equals("bc")) {
								removeBCFile(file);
							} else if (file.getFileExtension().equals("es")) {
							/*
							 * Should we remove source file from any
							 * BC that references it, or 
							 */
							}
						}
					}
					break;
				case IResourceDelta.CHANGED :
					EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
							EJScriptTrace.COMPILER_TRACE,
							resource.getName() + " has changed");
					if (!monitor.isCanceled()) {
						if (resource instanceof IFile) {
							IFile file = (IFile)resource;
							EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
									EJScriptTrace.COMPILER_TRACE,
									resource.getName() + " is a file: extension " +
									file.getFileExtension());
							
							if (file.getFileExtension().equals("bc")) {
								updateBCFile(file);
							} else if (file.getFileExtension().equals("es")) {
								checkResource(file);
							}
							result = true;
						} else {
							EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
									EJScriptTrace.COMPILER_TRACE,
									resource.getName() + " is not a file");
							result = true;
						}
					} else {
						EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
								EJScriptTrace.COMPILER_TRACE,
								"monitor has been canceled");
					}
					break;
			}
			
			EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
					EJScriptTrace.COMPILER_TRACE,
					"-EJScriptDeltaVisitor:visit " + resource.getName());
			return result;
 		}
	}

	/**
	 * 
	 */
	@Override
	public void setInitializationData(IConfigurationElement config,
										String propertyName, Object data) {
		
	}
	
	/**
	 * 
	 * @param monitor
	 */
	private void executeBuildConfigs(IProgressMonitor monitor) {
		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.COMPILER_TRACE,
				"+executeBuildConfigs");
		boolean compiledSomething = false;
		
		// TODO GMK print BC name here.
		Iterator<String> iter = dirtyBuildConfigs.iterator();
		while (iter.hasNext()) {
			String bcFile = iter.next();
			EJSBuildConfiguration bc = allBuildConfigs.get(bcFile);
			if (bc == null){
				/*
				 * probably the first time this BC has been touched during this Eclipse session.
				 * Add it to the list.
				 */ 
				IResource resource = getProject().findMember(new Path(bcFile));
				if (resource != null && resource instanceof IFile) {
					IFile file = (IFile)resource;
					addBCFile(file);
				} else {
//					console.println("Unknown Build Configuration File '" + bcFile + "'");
					EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
							EJScriptTrace.COMPILER_TRACE,
							"Unknown Build Configuration File " + bcFile);
					continue;
				}
			}
			
			console.println("Compiling Build Configuration: " + bc.getName());
			compile(bc, monitor);
			compiledSomething = true;
			dirtyBuildConfigs.remove(bc.getName());
		}
		
		if (compiledSomething == false) {
			if (projectContainsABCFile() == false) {
//				String dialogTitle = "Ejscript Build File not found in project";
				String dialogMessage = "You must have an Ejscript Build file in your project in order for it to be compiled.";
				
				
//				MessageDialog dialog = new MessageDialog(shell, 
//						dialogTitle, null, dialogMessage, MessageDialog.ERROR, new String[] { "Save", "Discard", "Cancel" }, 0);
//				dialog.open();
				console.println(dialogMessage);
				console.println("Create one by going to File/New/Ejscript Build File");
			}
		}
		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.COMPILER_TRACE,
				"-executeBuildConfigs");
	}
	
	private boolean projectContainsABCFile() {
		boolean result = false;
		IProject project = this.getProject();
		if (memberContainsBC(project) == true) {
			result = true;
		}
		
		return result; 
	}
	
	private boolean memberContainsBC(IContainer container) {
		boolean result = false;
		try {
			IResource[] members = container.members();
			for (int i = 0; i < members.length && result == false; i++) {
				if (members[i] instanceof IContainer) {
					result = memberContainsBC((IContainer)members[i]);
				} else if (members[i] instanceof IFile) {
					IFile file = (IFile)members[i];
					if (file.getFileExtension().equals("bc")) {
						result = true;
					}
				}
			}
		} catch (CoreException e) {
			
		}
	
		return result;
	}
	
	private boolean compile(EJSBuildConfiguration bc, IProgressMonitor monitor) {
		
		Preferences prefs = EJScriptPlugin.getDefault().getPluginPreferences();
		boolean result = true;
		
		String compilerLocation =
			prefs.getString(PreferenceConstants.PATH_TO_EJS_COMPILER);
		
		if (compilerLocation == null) {
			return true;
		}
		if (!bc.isFileByFileEnabled()) {

			List<IResource> list = bc.getResourceList();
			IResource[] files = new IResource[list.size()];
			Iterator<IResource> iter = list.iterator();
			int i = 0;
			while (iter.hasNext()) {
				files[i] = iter.next();
				deleteMarkers(files[i]);
				i++;
			}
			
			
			CompilerExecutor compiler = new CompilerExecutor(compilerLocation, files,
					bc.getCommandArgsAsString());
			
			// TODO GMK: need to remove all markers first
			
			List<CompilerOutput> errors = compiler.compile();
			if (errors != null) {
				Iterator<CompilerOutput> iter2 = errors.iterator();
				while(iter2.hasNext()) {
					CompilerOutput output = iter2.next();
					if (output.isValid()) {
						EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
				 				EJScriptTrace.COMPILER_TRACE,
				 				"Adding Marker to " + 
								output.getFileName() + " at line number " + 
								output.getLineNumber());
						addMarker(output.getFile(), output);
					}
				}
//				monitor.worked(1);
			} else {
				// Compilation was successful!
			}
		} else {
			List<IResource> list = bc.getResourceList();
			Iterator<IResource> iter = list.iterator();
			while (iter.hasNext()) {
				IResource file = iter.next();
				deleteMarkers(file);
				compile(file, monitor, bc.getCommandArgsAsString());
			}
		}
		
		return result;
	}
	
	/*
	 * Compile a resource.  This is only used when the global build is enabled.
	 * 
	 */
	private boolean compile(IResource file, IProgressMonitor monitor, String compilerOptions) {
		
		Preferences prefs = EJScriptPlugin.getDefault().getPluginPreferences();
		boolean result = true;

		String compilerLocation =
			prefs.getString(PreferenceConstants.PATH_TO_EJS_COMPILER);
		EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
				EJScriptTrace.COMPILER_TRACE,
				"compileLocation is: " + compilerLocation);
		if (compilerLocation != null) {
			IResource[] files = new IResource[1];
			files[0] = file;
			EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
					EJScriptTrace.COMPILER_TRACE,
					"Initializing CompilerExecutor");
			CompilerExecutor compiler = new CompilerExecutor(compilerLocation, files, 
					compilerOptions);
			deleteMarkers(file);
			List<CompilerOutput> errors = compiler.compile();
			if (errors != null) {
				Iterator<CompilerOutput> iter = errors.iterator();
				while(iter.hasNext()) {
					CompilerOutput output = iter.next();
					if (output.isValid()) {
						EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
				 				EJScriptTrace.COMPILER_TRACE,
				 				"Adding Marker to " + 
								output.getFileName() + " at line number " + 
								output.getLineNumber());
						addMarker(output.getFile(), output);
					}
				}
//				monitor.worked(1);
			} else {
				// Compilation was successful!
			}
		}
	 	
		return result;
	}

	/**
	 * 
	 * @param resource - The resource for the compiler to execute on.
	 * @return true if compiler executed correctly.
	 */
/*	private boolean compile(IResource resource, IProgressMonitor monitor) {
		
		Preferences prefs = EJScriptPlugin.getDefault().getPluginPreferences();
		
		boolean result = true;
		EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
 				EJScriptTrace.COMPILER_TRACE,
 				"Checking");
		if (resource instanceof IFile && 
				resource.getName().endsWith(EJSConstants.EJSCRIPT_FILE_EXTENSION)) {
			String compilerLocation = 
				prefs.getString(PreferenceConstants.PATH_TO_EJS_COMPILER);
			if (compilerLocation != null) {
			
				EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
		 				EJScriptTrace.COMPILER_TRACE,
		 				"Compiling file: " + resource.getName());
				IFile file = (IFile) resource;
				deleteMarkers(file);
				monitor.worked(1);
				CompilerExecutor compiler = new CompilerExecutor(
						compilerLocation);
				List<CompilerOutput> errors = compiler.compile(file);
				monitor.worked(1);
				if (errors != null) {
					Iterator<CompilerOutput> iter = errors.iterator();
					while(iter.hasNext()) {
						CompilerOutput output = iter.next();
						if (output.isValid()) {
							EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
					 				EJScriptTrace.COMPILER_TRACE,
					 				"Adding Marker to " + 
									file + " at line number " + output.getLineNumber());
							addMarker(file, output);
						}
					}
//					monitor.worked(1);
				} else {
					// Compilation was successful!
				}
			} else {
				EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
						EJScriptTrace.COMPILER_TRACE,
						"Can not find compiler: " + compilerLocation);
			}
		} else {
			EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.COMPILER_TRACE,
				"Skipping file: " + resource.getName());
		}
		
//		monitor.worked(1);
		return result;
	}
 */	
	private void addMarker(IFile file, CompilerOutput output) {
		EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG, 
				EJScriptTrace.COMPILER_OUTPUT_TRACE, 
				"Setting Severity to " + output.getSeverity());
		try {
			IMarker marker = file.createMarker(IMarker.PROBLEM);
			
			marker.setAttribute(IMarker.MESSAGE, output.getDescription());
			if (output.getSeverity().equals("error")) {
				marker.setAttribute(IMarker.SEVERITY, IMarker.SEVERITY_ERROR);
			} else {
				marker.setAttribute(IMarker.SEVERITY, IMarker.SEVERITY_WARNING);
			}
			marker.setAttribute(IMarker.LINE_NUMBER, 
					output.getLineNumber() < 0 ? 1 : output.getLineNumber());
/*			if (output.hasLocation()) {
				EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
						EJScriptTrace.COMPILER_OUTPUT_TRACE,
						"Setting char_start to " + output.getCharStart());
				EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
						EJScriptTrace.COMPILER_OUTPUT_TRACE,
						"Setting char_end to " + output.getCharEnd());
				marker.setAttribute(IMarker.CHAR_START, output.getCharStart());
				marker.setAttribute(IMarker.CHAR_START, output.getCharEnd());
				EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
						EJScriptTrace.COMPILER_OUTPUT_TRACE,
						"Done");
			}
 */		} catch (CoreException e) {
			EJScriptTrace.traceError(EJScriptTrace.COMPILER_TRACE, e);
		}
	}
	
	/**
	 * delete Markers from a resource - should be called prior to compiling
	 * the resource.
	 * This is now public static to allow the exclude from build to clear all problems on the resource before compiling.
	 * @param file
	 */
	public static void deleteMarkers(IResource resource) {
		if (resource instanceof IFile) {
			IFile file = (IFile)resource;
			EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
					EJScriptTrace.COMPILER_TRACE,
					"+deleteMarkers on " + file);
			try {
				IMarker[] markers = file.findMarkers(IMarker.PROBLEM, true, 
						IResource.DEPTH_INFINITE);
				EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
						EJScriptTrace.COMPILER_TRACE,
						"found " + markers.length + " markers on " + file);
				file.deleteMarkers(IMarker.PROBLEM, false, IResource.DEPTH_ZERO);
			} catch (CoreException ce) {
				EJScriptTrace.traceError(EJScriptTrace.COMPILER_TRACE, ce);
			}
		}
	}

	/**
	 * Prints a trace message for the type of build being performed.
	 * (This message is not displayed to the User.)
	 * @param kind
	 */
	private void logBuildType(int kind) {
		if (kind == FULL_BUILD) {
			EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
					EJScriptTrace.COMPILER_TRACE,
					"Building FULL_BUILD");
		} else if (kind == AUTO_BUILD) {
			EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
					EJScriptTrace.COMPILER_TRACE,
					"Building AUTO_BUILD");
		} else if (kind == CLEAN_BUILD) {
			EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
					EJScriptTrace.COMPILER_TRACE,
					"Building CLEAN_BUILD");
		} else if (kind == INCREMENTAL_BUILD) {
			EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
					EJScriptTrace.COMPILER_TRACE,
					"Building INCREMENTAL_BUILD");
		} else {
			EJScriptTrace.traceError(EJScriptTrace.COMPILER_TRACE,
					"Building unknown build: " + kind);
		}
	}
	
	private final IPropertyChangeListener compilerChangeListener = 
		new IPropertyChangeListener() {
		public void propertyChange(PropertyChangeEvent event) {
			if (event.getProperty().equals(PreferenceConstants.PATH_TO_EJS_COMPILER)) {
				
			}
		}
	};
	
	private void traceBuilderStart(String message) {
		console.println("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" +
				"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
		
		Calendar calendar = GregorianCalendar.getInstance();
		calendar.setTimeInMillis(System.currentTimeMillis());
		
		console.println(message + calendar.getTime());
	}
	
	private void traceBuilderEnd(String message) {
		Calendar calendar = GregorianCalendar.getInstance();
		calendar.setTimeInMillis(System.currentTimeMillis());
		console.println(message + calendar.getTime());
	}
	
	public static void updateBuildConfigurationInGlobalList(EJSBuildConfiguration bc) {
		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.COMPILER_TRACE,
				"+updateBuildConfigurationInGlobalLlist: allBuildConfigs has " + allBuildConfigs.size() + " items");
		if (allBuildConfigs != null) {
			EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
					EJScriptTrace.COMPILER_TRACE,
					"updateBuildConfigurationInGlobalLlist - adding " + bc.getName());
			allBuildConfigs.put(bc.getName(), bc);
		}
		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.COMPILER_TRACE,
				"-updateBuildConfigurationInGlobalLlist: allBuildConfigs has " + allBuildConfigs.size() + " items");
	}
	
	public static void updateBCFileInGlobalList(String bcFileName, BCFile newBcFile) {
		if (bcFilesList != null) {
			bcFilesList.put(bcFileName, newBcFile);
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
