/*
 * Copyright (c) All Rights Reserved. See details at the end of the file.
 */
package com.embedthis.ejs.ide.core;

import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Platform;
import org.eclipse.core.runtime.Status;

import com.embedthis.ejs.ide.EJScriptPlugin;

public class EJScriptTrace {

	public static final String GENERAL_TRACE = 
		EJScriptPlugin.PLUGIN_ID + "/debug/general";
	public static final String EDITOR_OUTPUT_TRACE = 
		EJScriptPlugin.PLUGIN_ID + "/debug/editor";
	public static final String EJSCRIPT_TRACE_LEVEL = 
		EJScriptPlugin.PLUGIN_ID + "/debug/level";
	public static final String COMPILER_TRACE = 
		EJScriptPlugin.PLUGIN_ID + "/debug/compiler";
	public static final String COMPILER_OUTPUT_TRACE = 
		EJScriptPlugin.PLUGIN_ID + "/debug/compilerOutput";
	public static final String WIZARD_TRACE = 
		EJScriptPlugin.PLUGIN_ID + "/debug/wizards";
	public static final String VIEWS_TRACE = 
		EJScriptPlugin.PLUGIN_ID + "/debug/views";
	public static final String EJSCRIPT_LABEL = 
		"EJSCRIPT:\t";
	public static final String BUILDER_TRACE = 
		EJScriptPlugin.PLUGIN_ID + "/debug/builder";
	
	/**
	 * this trace will always print.
	 */
	public static final int TRACE_CRITICAL = 0;
	/**
	 * this trace will always print.
	 */
	public static final int TRACE_NORMAL = 1;
	/**
	 * additional debug information
	 */
	public static final int TRACE_DEBUG = 2;
	/**
	 * verbose trace information (i.e. method entry/exit)
	 */
	public static final int TRACE_VERBOSE = 3;
	
	/**
	 * Writes a trace message if the current trace level is defined 
	 * as at or higher than the trace message.
	 * 
	 * If system trace level is at 2 and trace is level 3, trace message
	 * will not print.
	 * 
	 * @param traceLevel
	 * @param option
	 * @param message
	 */
	public static final void trace(int traceLevel, String option, String message)
	{
		if (EJScriptPlugin.getDefault().isDebugging())
		{
			Integer currentLevel = new Integer(Platform.getDebugOption(EJSCRIPT_TRACE_LEVEL)); 
			if (currentLevel.intValue() >= traceLevel)
			{
				String optionString = Platform.getDebugOption(option);
				if (optionString != null && optionString.equals("true"))
				{
					System.out.println(EJSCRIPT_LABEL + traceLevel + "\t" + message);
				}
			}
		}
	}
	
	/**
	 * Writes an error message to both the Error Log and the trace.
	 * @param option
	 * @param message
	 */
	public static void traceError(String option, String message) {
		writeError(createStatus(IStatus.ERROR, IStatus.OK, message, null));
		trace(TRACE_CRITICAL, option, message);
	}
	
	public static void traceError(String option, Throwable exception) {
		traceError(option, exception.getMessage(), exception);
	}
	
	public static void traceError(String option, String message, Throwable exception) {
		writeError(createStatus(IStatus.ERROR, IStatus.OK, message, exception));
		trace(TRACE_CRITICAL, option, exception.getMessage());
	}
	
	/**
	 * writes an status to the Error Log.
	 * @param status
	 */
	public static void writeError(IStatus status) {
		if (EJScriptPlugin.getDefault() != null) {
			EJScriptPlugin.getDefault().getLog().log(status);
		}
	}
	
	/**
	 * creates the Status object for writing errors to the Error Log.
	 * @param severity
	 * @param code
	 * @param message
	 * @param exception
	 * @return
	 */
	private static IStatus createStatus(int severity, int code, String message, Throwable exception) {
		return new Status(severity, EJScriptPlugin.PLUGIN_ID, code, message, exception);
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
