/*
 * Copyright (c) All Rights Reserved. See details at the end of the file.
 */
package com.embedthis.ejs.ide.properties;

import org.eclipse.core.runtime.QualifiedName;

import com.embedthis.ejs.ide.EJScriptPlugin;

public class PropertyConstants {
	
	public static final int NUMBER_OF_COMPILER_OPTIONS = 9;

	public static final String DEBUG_DESCRIPTION = "Include line number debugging info";
	public static final String OPTIMIZE_DESCRIPTION = "Level is 0-9";
	public static final String OUT_DESCRIPTION = "Name the output package [default: default.pak]";
	public static final String SEARCH_PATH_DESCRIPTION = "Package search path (use '" + System.getProperty("path.separator") + 
															"' as the path separator)";
	public static final String STANDARD_DESCRIPTION = "MOB: TODO- Single description";
	public static final String STRICT_DESCRIPTION = "MOB: TODO- Strict description";
	public static final String VERBOSE_DESCRIPTION = "Trace progress to the console";
	public static final String WARN_DESCRIPTION = "Warning level";
	
	public static final String EXTRA_OPTION_DESCRIPTION = "Extra option appended to the compiler options.  No validation is performed, the string is added exactly as you enter it.";
	
	public static final String DEFAULT_OPTIMIZATION = "0";
	
	public static final QualifiedName EXCLUDE_FROM_BUILD_PROPERTY = 
		new QualifiedName(EJScriptPlugin.PLUGIN_ID, "excludeFromBuild");
	
	public static final QualifiedName GLOBAL_COMPILER_OPTIONS = 
		new QualifiedName(EJScriptPlugin.PLUGIN_ID, "globalCompilerOpts");
	
	public static final QualifiedName GLOBAL_SEARCH_PATH =
		new QualifiedName(EJScriptPlugin.PLUGIN_ID, "globalSearchPath");
	
	public static final QualifiedName BUILD_TYPE =
		new QualifiedName(EJScriptPlugin.PLUGIN_ID, "buildType");
	
	public static final String GLOBAL_BUILD = "globalBuild";
	public static final String BC_BUILD = "bcBuild";
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
