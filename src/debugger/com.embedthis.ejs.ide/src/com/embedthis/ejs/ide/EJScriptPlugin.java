/**
 * Copyright (c) All Rights Reserved. See details at the end of the file.
 */
package com.embedthis.ejs.ide;

import java.net.URL;
import java.io.File;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.Properties;

import org.eclipse.core.resources.ISaveParticipant;
import org.eclipse.core.resources.ISavedState;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.osgi.framework.BundleContext;

import com.embedthis.ejs.ide.core.EJScriptTrace;
import com.embedthis.ejs.ide.core.PakDefFile;

/**
 * The activator class controls the plug-in life cycle
 */
public class EJScriptPlugin extends AbstractUIPlugin {

	// The plug-in ID
	public static final String PLUGIN_ID = "com.embedthis.ejs.ide";

	// The shared instance
	private static EJScriptPlugin plugin;
	
	// logger properties
	private static final String LOG_PROPERTIES_FILE = "logger.properties";
	
	private static ArrayList<PakDefFile> pakDefFiles = null;
	
	/**
	 * The constructor
	 */
	public EJScriptPlugin() {
		if (plugin != null)
			throw new IllegalStateException("Plug-in class already exists");
		plugin = this;
	}

	/*
	 * (non-Javadoc)
	 * @see org.eclipse.ui.plugin.AbstractUIPlugin#start(org.osgi.framework.BundleContext)
	 */
	public void start(BundleContext context) throws Exception {
		super.start(context);
		configure_logger();
		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
			EJScriptTrace.COMPILER_TRACE,
			"-EJScriptPlugin:start");
		
		ISaveParticipant saveParticipant = new EJScriptSaveParticipant();
		ISavedState lastState = ResourcesPlugin.getWorkspace().
			addSaveParticipant(this, saveParticipant);

		if (lastState == null) {
			return;
		}
		
		lastState.processResourceChangeEvents(new EJScriptResourceChangeListener());
		
		IPath location = lastState.lookup(new Path("EJSCRIPT_SAVE_PATH"));
		if (location != null) {
			File f = getStateLocation().append(location).toFile();
			restoreState(f);
		}
	}

	/*
	 * (non-Javadoc)
	 * @see org.eclipse.ui.plugin.AbstractUIPlugin#stop(org.osgi.framework.BundleContext)
	 */
	public void stop(BundleContext context) throws Exception {
		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.COMPILER_TRACE,
				"+EJScriptPlugin:stop");
		plugin = null;
		super.stop(context);
	}

	/**
	 * Returns the shared instance
	 *
	 * @return the shared instance
	 */
	public static EJScriptPlugin getDefault() {
		return plugin;
	}

	/**
	 * Returns an image descriptor for the image file at the given
	 * plug-in relative path
	 *
	 * @param path the path
	 * @return the image descriptor
	 */
	public static ImageDescriptor getImageDescriptor(String path) {
		return imageDescriptorFromPlugin(PLUGIN_ID, path);
	}
	
	private void configure_logger() {
		try {
			URL url = getBundle().getEntry("/" + LOG_PROPERTIES_FILE);
			InputStream propertiesInputStream = url.openStream();
			if (propertiesInputStream != null) {
				Properties props = new Properties();
				props.load(propertiesInputStream);
				propertiesInputStream.close();
//				this.logManager = new JScriptLogManager(this, props);
			}
		} catch (Exception e) {
			
		}
	}
	
	private void restoreState(File file) {
		
	}
	
	public static ArrayList<PakDefFile> getPakDefFiles() {
		return pakDefFiles;
	}
	
	public static void setPakDefFiles(ArrayList<PakDefFile> files) {
		EJScriptPlugin.pakDefFiles = files;
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
