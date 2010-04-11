/*
 * Copyright (c) All Rights Reserved. See details at the end of the file.
 */
package com.embedthis.ejs.ide.core;

import java.util.StringTokenizer;

/**
 * This class maintains the compiler options for compiling EJ files.
 * @author Gavin
 *
 */
public class EJScriptCompilerOptions {
	
	public static final String DEBUG_SWITCH = "--debug";
	public static final String OPTIMIZE_SWITCH = "--optimize";
	public static final String OUT_SWITCH = "--out";
	public static final String SEARCH_PATH_SWITCH = "--searchpath";
	public static final String STANDARD_SWITCH = "--standard";
	public static final String STRICT_SWITCH = "--strict";
	public static final String VERBOSE_SWITCH = "--verbose";
	public static final String WARN_SWITCH = "--warn";

	public static final String DEFAULT_COMPILER_OPTIONS = 
		"--optimize 5 --standard --debug --warn 0";
	
	private boolean debugSwitch = false;
	private boolean optimizeSwitch = false;
	private String optimizeValue;
	private boolean outSwitch = false;
	private String outValue;
	private boolean searchPathSwitch = false;
	private String searchPathValue;
	private boolean standardSwitch = false;
	private boolean strictSwitch = false;
	private boolean verboseSwitch = false;
	private boolean warnSwitch = false;
	private String warnValue;
	private boolean extraSwitch = false;
	private String extraOption;
	
	/**
	 * Create with default compiler options.
	 */
	public EJScriptCompilerOptions() {
		this(DEFAULT_COMPILER_OPTIONS);
	}
	
	/**
	 * Create with specified compiler options.
	 * @param commandString
	 */
	public EJScriptCompilerOptions(String commandString) {
		if (commandString == null || commandString == "") {
			commandString = DEFAULT_COMPILER_OPTIONS;
		}
		StringTokenizer tok = new StringTokenizer(commandString, " ");
		String extraString = null;
		while (tok.hasMoreTokens()) {
			String value = tok.nextToken();
			if (value.equals(DEBUG_SWITCH)) {
				enableDebug(true);
			} else if (value.equals(OPTIMIZE_SWITCH)) {
				enableOptimize(true, tok.nextToken());
			} else if (value.equals(OUT_SWITCH)) {
				enableOut(true, tok.nextToken());
			} else if (value.equals(SEARCH_PATH_SWITCH)) {
				enableSearchPath(true, tok.nextToken());
			} else if (value.equals(STANDARD_SWITCH)) {
				enableStandard(true);
			} else if (value.equals(STRICT_SWITCH)) {
				enableStrict(true);
			} else if (value.equals(VERBOSE_SWITCH)) {
				enableVerbose(true);
			} else if (value.equals(WARN_SWITCH)) {
				enableWarn(true, tok.nextToken());
			} else {
				if (extraString == null) {
					extraString = value;
				} else {
					extraString.concat(" " + value);
				}
			}
		}
		
		if (extraString != null) {
			enableExtraOption(true, extraString);
		}
	}
	
	/**
	 * Enable/Disable the debug option
	 * @param enabled
	 */
	public void enableDebug(boolean enabled) {
		debugSwitch = enabled;
	}
	
	/**
	 * 
	 * @return true if debug is enabled
	 */
	public boolean isDebugEnabled() {
		return debugSwitch;
	}
	
	/**
	 * Enable/Disable the optimize option
	 * @param enabled
	 * @param value [0-9] optimize level
	 */
	public void enableOptimize(boolean enabled, String value) {
		optimizeSwitch = enabled;
		if (enabled) {
			optimizeValue = value;
		} else {
			optimizeValue = null;
		}
	}
	
	/**
	 * 
	 * @return true if optimizations are enabled
	 */
	public boolean isOptimizeEnabled() {
		return optimizeSwitch;
	}
	
	/**
	 * 
	 * @return the currently configured optimize level
	 */
	public String getOptimizeValue() {
		return optimizeValue;
	}	
	
	/**
	 * Enable/Disable the out option for specifying the output file
	 * @param enabled
	 * @param value
	 */
	public void enableOut(boolean enabled, String value) {
		outSwitch = enabled;
		if (enabled) {
			outValue = value;
		} else {
			outValue = null;
		}
	}
	
	/**
	 * 
	 * @return true if out options is enabled
	 */
	public boolean isOutEnabled() {
		return outSwitch;
	}
	
	/**
	 * 
	 * @return the value for the out option
	 */
	public String getOutValue() {
		return outValue;
	}
	
	/**
	 * Enable/Disable the search path option
	 * @param enabled
	 * @param value
	 */
	public void enableSearchPath(boolean enabled, String value) {
		searchPathSwitch = enabled;
		if (enabled) {
			searchPathValue = value;
		} else {
			searchPathValue = null;
		}
	}
	
	/**
	 * 
	 * @return true if search path options is enabled
	 */
	public boolean isSearchPathEnabled() {
		return searchPathSwitch;
	}
	
	/**
	 * 
	 * @return the currently configured searchpath
	 * TODO GMK will this work cross-platform or will the search path need to be
	 * returned as an array of strings so that the OS separator can be added.
	 */
	public String getSearchPathValue() {
		return searchPathValue;
	}
	
	/**
	 * Enable/Disable the standard option
	 * @param enabled
	 */
	public void enableStandard(boolean enabled) {
		standardSwitch = enabled;
	}
	
	/**
	 * 
	 * @return
	 */
	public boolean isStandardEnabled() {
		return standardSwitch;
	}
	
	public void enableStrict(boolean enabled) {
		strictSwitch = enabled;
	}
	
	public boolean isStrictEnabled() {
		return strictSwitch;
	}
	
	public void enableVerbose(boolean enabled) {
		verboseSwitch = enabled;
	}
	
	public boolean isVerboseEnabled() {
		return verboseSwitch;
	}
	
	public void enableWarn(boolean enabled, String value) {
		warnSwitch = enabled;
		if (enabled) {
			warnValue = value;
		} else {
			warnValue = null;
		}
	}
	
	public boolean isWarnEnabled() {
		return warnSwitch;
	}
	
	public String getWarnValue() {
		return warnValue;
	}
	
	public void enableExtraOption(boolean enabled, String value) {
		extraSwitch = enabled;
		if(enabled) {
			extraOption = value;
		} else {
			extraOption = null;
		}
	}
	
	public boolean isExtraOptionEnabled() {
		return extraSwitch;
	}
	
	public String getExtraOption() {
		return extraOption;
	}
	
	/**
	 * Convert the compiler options to a string for easy viewing.
	 * This is generally used for display purposes only.
	 * @return String representation of the compiler options.
	 */
	public String convertToString() {
		String finalString = new String("");
		EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
				EJScriptTrace.COMPILER_TRACE,
				"+convertToString");
		if (debugSwitch) {
			finalString = finalString.concat(" " + DEBUG_SWITCH);
		}
		if (optimizeSwitch) {
			finalString = finalString.concat(" " + OPTIMIZE_SWITCH + " " + 
					optimizeValue);
		}
		if (outSwitch) {
			finalString = finalString.concat(" " + OUT_SWITCH + " " + outValue);
		}
		if (searchPathSwitch) {
			finalString = finalString.concat(" " + SEARCH_PATH_SWITCH + " " + 
					searchPathValue);
		}
		if (standardSwitch) {
			finalString = finalString.concat(" " + STANDARD_SWITCH);
		}
		if (strictSwitch) {
			finalString = finalString.concat(" " + STRICT_SWITCH);
		}
		if (verboseSwitch) {
			finalString = finalString.concat(" " + VERBOSE_SWITCH);
		}
		if (warnSwitch) {
			finalString = finalString.concat(" " + WARN_SWITCH + " " + 
					warnValue);
		}
		if (extraSwitch) {
			finalString = finalString.concat(" " + extraOption);
		}
		EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
				EJScriptTrace.COMPILER_TRACE,
				"+convertToString: final '" + finalString + "'");
		return finalString;
	}
	
	/**
	 * Convert the current compiler options to an argument list.
	 * @param executable
	 * @param resource
	 * @return String array of arguments for passing to the compiler.
	 */
	public String[] convertToArgs() {
		
		String args[] = new String[1];

		if (debugSwitch) {
			args = appendArg(args, DEBUG_SWITCH);
		}
		if (optimizeSwitch) {
			args = appendArg(args, OPTIMIZE_SWITCH);
			args = appendArg(args, optimizeValue);
		}
		if (outSwitch) {
			args = appendArg(args, OUT_SWITCH);
			args = appendArg(args, outValue);
		}
		if (searchPathSwitch) {
			args = appendArg(args, SEARCH_PATH_SWITCH);
			args = appendArg(args, searchPathValue);
		}
		if (standardSwitch) {
			args = appendArg(args, STANDARD_SWITCH);
		}
		if (strictSwitch) {
			args = appendArg(args, STRICT_SWITCH);
		}
		if (verboseSwitch) {
			args = appendArg(args, VERBOSE_SWITCH);
		}
		if (warnSwitch) {
			args = appendArg(args, WARN_SWITCH);
			args = appendArg(args, warnValue);
		}
		/*
		 *  For the extra options, split each by the space and pass as it's own arg.
		 */
		
		if (extraSwitch) {
			StringTokenizer tok = new StringTokenizer(extraOption, " ");
			while (tok.hasMoreTokens()) {
				args = appendArg(args, tok.nextToken());
			}
		}
		return args;
	}
	
	/**
	 * 
	 * @param currentArgs
	 * @param newArg
	 * @return
	 */
	private String[] appendArg(String[] currentArgs, String newArg) {
		String[] newArgs = new String[currentArgs.length + 1];
		System.arraycopy(currentArgs, 0, newArgs, 0, currentArgs.length);
		newArgs[currentArgs.length] = newArg;
		
		return newArgs;
	}
	
	public boolean equals(EJScriptCompilerOptions bc) {
		boolean result = true;
		if (this.debugSwitch != bc.debugSwitch) {
			result = false;
		} else if (this.optimizeSwitch != bc.optimizeSwitch) {
			result = false;
		} else if (!this.optimizeValue.equals(bc.optimizeValue)) {
			result = false;
		} else if (!this.outSwitch != bc.outSwitch) {
			result = false;
		} else if (!this.outValue.equals(bc.outValue)) {
			result = false;
		} else if (!(this.searchPathSwitch != bc.searchPathSwitch)) {
			result = false;
		} else if (!(this.searchPathValue.equals(bc.searchPathValue))) {
			result = false;
		} else if (!(this.standardSwitch != bc.standardSwitch)) {
			result = false;
		} else if (!(this.strictSwitch != bc.strictSwitch)) {
			result = false;
		} else if (!(this.verboseSwitch != bc.verboseSwitch)) {
			result = false;
		} else if (!(this.warnSwitch != bc.warnSwitch)) {
			result = false;
		} else if (!(this.warnValue != bc.warnValue)) {
			result = false;
		} else if (!(this.extraSwitch != bc.extraSwitch)) {
			result = false;
		} else if (!(this.extraOption.equals(bc.extraOption))) {
			result = false;			
		}
		return result;
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
