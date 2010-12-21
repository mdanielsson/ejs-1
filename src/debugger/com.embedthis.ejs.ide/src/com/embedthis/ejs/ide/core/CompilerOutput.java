/*
 * Copyright (c) All Rights Reserved. See details at the end of the file.
 */
package com.embedthis.ejs.ide.core;

import java.io.File;
import java.io.IOException;
import java.util.StringTokenizer;
import javax.imageio.stream.FileImageInputStream;

import org.eclipse.core.resources.IFile;

import com.embedthis.ejs.ide.core.EJScriptTrace;

/**
 * The constructor for this class only creates a valid object if the 
 * provided errorString matches the expected format.  Current format is as follows:
 * 
 * program: FILE: LINE: CODE: SEVERITY: MSG
 * 
 * @author Gavin
 * 
 * TODO GMK: Special compiler output cases.
 * ec.exe: Error: Can't load ejs.bun
 *
 */
public class CompilerOutput {
	
	private static int NUMBER_OF_EXPECTED_TOKENS = 6;
	
	private String program;
	private String fileName;
	private Integer lineNumber;
	private Integer errorCode;
	private String description;
	private String severity;
	private IFile file;
	
	private int char_start = 0;
	private int char_end = 0;
	
	private boolean valid = false;
	
	public CompilerOutput(String errorString, IFile file)
	{
		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.COMPILER_OUTPUT_TRACE, 
				"+CompilerOutput(" + errorString + ")");
		/*
		 *  replace ': ' with '|' to allow the StringTokenizer to
		 *  properly separate tokens without confusing spaces in
		 *	the error/warning description.
		 */  
		
		this.file = file;

		setErrorString(errorString);
	}
	
	public CompilerOutput(String errorString) {
		setErrorString(errorString);
		this.file = null;
	}
	
	private void setErrorString(String errorString) {
		
		String convertedString = errorString.replace(": ", "|");
		StringTokenizer tok = new StringTokenizer(convertedString, "|");
		
		if (tok.countTokens() == NUMBER_OF_EXPECTED_TOKENS) {
			program = tok.nextToken();
			fileName = tok.nextToken();
			lineNumber = new Integer(tok.nextToken());
			errorCode = new Integer(tok.nextToken());
			severity = tok.nextToken();
			description = tok.nextToken();
			valid = true;
			EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
					EJScriptTrace.COMPILER_OUTPUT_TRACE,
					"'" + errorString + 
					"' is a valid compiler error return: " + 
					tok.countTokens());
		} else {
			EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
					EJScriptTrace.COMPILER_OUTPUT_TRACE, 
					"'" + errorString + 
					"' is not a valid compiler error return: " + 
					tok.countTokens());
			valid = false;
		}
	}
	
	public boolean setTokenLocation(String location, String carrot)
	{
		try
		{
			EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
					EJScriptTrace.COMPILER_OUTPUT_TRACE,
					"Trying to open " + file.getLocation().toOSString());
			FileImageInputStream in = new FileImageInputStream(new File(file.getLocation().toOSString()));
			
			EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
					EJScriptTrace.COMPILER_OUTPUT_TRACE, 
					"location line is '" + location + "'");
			
			EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
					EJScriptTrace.COMPILER_OUTPUT_TRACE, 
					"carrot line is '" + carrot + "'");
			char_start = 0;
			EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
					EJScriptTrace.COMPILER_OUTPUT_TRACE, 
					"getLineNumber is " + getLineNumber());
			for (int i = 0; i < getLineNumber() - 1; i++)
			{
				EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
						EJScriptTrace.COMPILER_OUTPUT_TRACE, 
						"Reading now char_start is: " + char_start);
				String line = in.readLine(); // ignore everything up to LineNumer
				char_start += line.length();
				
			}
			char_start = char_start + carrot.indexOf('^') - 3;
			char_end = char_start;
			int i = carrot.indexOf('^') - 2;
			String fileLine = in.readLine();
			EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
					EJScriptTrace.COMPILER_OUTPUT_TRACE, 
					"i is: " + i);
			EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
					EJScriptTrace.COMPILER_OUTPUT_TRACE, 
					"fileLine.charAt(" + i + "): " + fileLine.charAt(i));
			while(i < fileLine.length() && Character.isLetterOrDigit(fileLine.charAt(i)))
			{
				EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
						EJScriptTrace.COMPILER_OUTPUT_TRACE, 
						"fileLine.charAt(" + i + "): " + fileLine.charAt(i));
				i++;
				char_end++;
			}
			char_end++;
			EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
					EJScriptTrace.COMPILER_OUTPUT_TRACE, 
					"char_start is " + char_start + " char_end is " + char_end);
		} catch (IOException e) {
			EJScriptTrace.traceError(EJScriptTrace.COMPILER_OUTPUT_TRACE, e);
		}
		
		return true;
	}
	
	
	public String getProgram()
	{
		return program;
	}
	
	public String getFileName()
	{
		return fileName;
	}
	
	public Integer getLineNumber()
	{
		return lineNumber;
	}
	
	public Integer getErrorCode()
	{
		return errorCode;
	}
	
	public String getDescription()
	{
		return description;
	}
	
	public String getSeverity()
	{
		return severity;
	}
	
	public void setFile(IFile file) {
		this.file = file;
	}
	
	public IFile getFile() {
		return file;
	}
	
	public String toString()
	{
		if (valid) {
			return fileName + ":" + lineNumber.toString() + ":" + errorCode.toString() +
						":" + severity + " " + description;
		} else {
			return "";
		}
	}
	
	public boolean isValid(){
		return valid;
	}
	
	public boolean hasLocation() {
		if (char_start != 0 || char_end != 0)
			return true;
		else
			return false;
	}
	
	public int getCharStart() {
		return char_start;
	}
	
	public int getCharEnd() {
		return char_end;
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
    vim: sw=8 ts=8 expandtab

    @end
 */
